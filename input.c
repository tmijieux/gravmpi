#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <string.h>
#include <errno.h>

#include "input.h"
#include "star.h"

void grav_read_file(char const *filename, int rank, int size,
                    grav_site *local_stars, grav_site remote_buf[2])
{
    FILE *f = fopen(filename, "r");
    if (f == NULL) {
        fprintf(stderr, "Cannot open `%s´: %s\n", filename, strerror(errno));
        exit(EXIT_FAILURE);
    }

    int star_count;
    fscanf(f, "%d", &star_count);

    int len, max_len;
    len = max_len = star_count / size;
    max_len += star_count % size;
    if (rank == size-1) len = max_len;

    int bufsize = len * sizeof(grav_star);
    int maxbufsize = max_len * sizeof(grav_star);
    local_stars->stars = g_malloc0(bufsize);
    remote_buf[0].stars = g_malloc0(maxbufsize);
    remote_buf[1].stars = g_malloc0(maxbufsize);

    int i = 0;
    while (star_count--) {
        grav_star *star = local_stars->stars+i;
        if (star_count % size == rank) {
            fscanf(f, "%lf %lf %lf %lf %lf",
                   &star->mass, &star->x, &star->y, &star->vx, &star->vy);
            ++i;
        }
    }

    memcpy(remote_buf[0].stars, local_stars->stars, len * sizeof(grav_star));
    local_stars->rank = rank;
    remote_buf[0].rank = rank;
    remote_buf[1].rank = (rank-1 < 0) ? (size-1) : (rank-1);

    fclose(f);
}
