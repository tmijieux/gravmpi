#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "star.h"

void grav_star_dump(grav_star *s)
{
    fprintf(stderr, "M=%g x=%g y=%g vx=%g vy=%g, ax=%g,"
            " ay=%g, fx=%g fy=%g, mindisp=%g\n",
            s->mass, s->x, s->y, s->vx, s->vy, s->ax,
            s->ay, s->fx, s->fy, s->min_displacement);
}

void grav_site_dump(grav_site *site, bool print_data)
{
    fprintf(stderr, "star count: %d\n", site->star_count);
    fprintf(stderr, "rank: %d\n", site->rank);
    if (print_data) {
        for (int i = 0; i < site->star_count; ++i)
            grav_star_dump(site->stars+i);
    }
    fprintf(stderr, "\n");
}

void grav_site_print(grav_site *site)
{
    for (int i = 0; i < site->star_count; ++i)
        printf("%g %g ", site->stars[i].x, site->stars[i].y);
    printf("\n");
}


void grav_site_copy(grav_site *dest, grav_site *src)
{
    dest->star_count = src->star_count;
    dest->rank = src->rank;
    memcpy(dest->stars, src->stars, src->star_count*sizeof(grav_star));
}
