#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <glib/gstdio.h>

#include "input.h"
#include "star.h"
#include "error.h"

#define LINESIZE 4096

static void
die_file_bad_format(char const *filename, int line)
{
    grav_fatal("%s: line %d invalid file format\n", filename, line);
}

static int
scan_line(FILE *f, char const *format, ...)
{
    va_list ap;
    size_t len = 0;
    char *line = NULL;

    if (getline(&line, &len, f) < 0) {
        free(line);
        return -1;
    }
    va_start(ap, format);
    int n = vsscanf(line, format, ap);
    va_end(ap);
    free(line);
    return n;
}

static void
parse_star(FILE *f, grav_star *s, const char *filename, int line_count)
{
    int n;
    n = scan_line(f, "%lf %lf %lf %lf %lf",
                  &s->mass, &s->x, &s->y, &s->vx, &s->vy);
    if (n != 5)
        die_file_bad_format(filename, line_count);
}

static void skip_line(FILE *f)
{
    char buf[LINESIZE];
    assert( fgets(buf, LINESIZE, f) != NULL );
}

static void
init_buffer(int rank, grav_site *buf, int len)
{
    size_t buf_size = len * sizeof(grav_star);

    buf->rank = rank;
    buf->star_count = 0;
    buf->stars = g_malloc0(buf_size);
}

static void
copy_buffer(grav_site *dest, grav_site *src, int len)
{
    size_t buf_size = len * sizeof(grav_star);
    memcpy(dest, src, sizeof*dest);
    dest->stars = g_memdup(src->stars, buf_size);
}

/**
 * return total star count
 */
int grav_read_file(char const *filename, int rank, int size,
                   grav_site *local_stars, grav_site remote_buf[2])
{
    FILE *f = g_fopen(filename, "rb");
    if (f == NULL) {
        fprintf(stderr, "Cannot open `%s´: %s\n", filename, strerror(errno));
        exit(EXIT_FAILURE);
    }

    int star_count;
    int c = scan_line(f, "%d", &star_count);
    if (c != 1)
        die_file_bad_format(filename, 0);
    if (star_count < size)
        grav_fatal("More process than stars!\n");

    int len = (star_count / size) + (star_count % size != 0);
    init_buffer(rank, local_stars, len);

    for (int i = 0; i < star_count; ++i) {
        if (i % size == rank) {
            int k = local_stars->star_count;
            parse_star(f, local_stars->stars+k, filename, i+1);
            ++ local_stars->star_count;
        }
        else
            skip_line(f);
    }
    fclose(f);

    // let 'remote' buffer be a copy of 'local' buffer for the first stage:
    copy_buffer(&remote_buf[0], local_stars, len);
    int input_buffer_rank = (rank-1) < 0 ? (size-1) : (rank-1);
    init_buffer(input_buffer_rank, remote_buf+1, len);

    return star_count;
}
