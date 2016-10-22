#include <stdio.h>
#include <stdlib.h>

#include "star.h"

void grav_star_dump(grav_star *s)
{
    fprintf(stderr, "M=%g x=%g y=%g vx=%g vy=%g\n",
            s->mass, s->x, s->y, s->vx, s->vy);
}

void grav_site_dump(grav_site *site, bool print_data)
{
    fprintf(stderr, "star count: %d\n", site->star_count);
    fprintf(stderr, "rank: %d\n", site->rank);
    if (print_data) {
        for (int i = 0; i < site->star_count; ++i)
            grav_star_dump(site->stars+i);
    }
}
