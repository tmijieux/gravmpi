#include <math.h>
#include <assert.h>

#include "star.h"
#include "local.h"

#define GRAVITATIONAL_CONSTANT (6.67384e-11)

static void
compute_local_force(grav_site *local)
{
    for (int i = 0; i < local->star_count; ++i) {
        grav_star *s = local->stars+i;

        for (int j = 0; j < i; ++j) {
            grav_star *r = local->stars+j;
            double f_n, v_x, v_y, v_n;

            v_n = hypot(s->x - r->x, s->y - r->y);
            f_n = ((GRAVITATIONAL_CONSTANT * s->mass * r->mass)
                   / SQUARE(v_n));
            v_x = (r->x - s->x) / v_n;
            v_y = (r->y - s->y) / v_n;

            s->fx += f_n * v_x;
            s->fy += f_n * v_y;
        }
        for (int j = i+1; j < local->star_count; ++j) {
            grav_star *r = local->stars+j;
            double f_n, v_x, v_y, v_n;

            v_n = hypot(s->x - r->x, s->y - r->y);
            f_n = ((GRAVITATIONAL_CONSTANT * s->mass * r->mass)
                   / SQUARE(v_n));
            v_x = (r->x - s->x) / v_n;
            v_y = (r->y - s->y) / v_n;

            s->fx += f_n * v_x;
            s->fy += f_n * v_y;
        }
    }
}

static void
compute_remote_force(grav_site *local, grav_site *remote)
{
    assert(local->rank != remote->rank);

    for (int i = 0; i < local->star_count; ++i) {
        grav_star *s = local->stars+i;

        for (int j = 0; j < i; ++j) {
            grav_star *r = remote->stars+j;
            double f_n, v_x, v_y, v_n;

            v_n = hypot(s->x - r->x, s->y - r->y);
            f_n = ((GRAVITATIONAL_CONSTANT * s->mass * r->mass)
                   / SQUARE(v_n));
            v_x = (r->x - s->x) / v_n;
            v_y = (r->y - s->y) / v_n;

            s->fx += f_n * v_x;
            s->fy += f_n * v_y;
        }
    }
}

void grav_site_local_compute_force(grav_site *local, grav_site *remote)
{
    if (local->rank == remote->rank)
        compute_local_force(local);
    else
        compute_remote_force(local, remote);
}

void grav_site_local_compute_position(grav_site *local, double t)
{
    for (int i = 0; i < local->star_count; ++i) {
        double ax, ay;
        grav_star *s = local->stars+i;

        ax = s->fx / s->mass;
        ay = s->fy / s->mass;
        s->vx += ax * t;
        s->vy += ay * t;

        s->x += s->vx * t;
        s->y += s->vy * t;

        s->fx = s->fy = 0;
    }
}

double grav_site_local_compute_step(grav_site *local)
{
    return 1.0;
}
