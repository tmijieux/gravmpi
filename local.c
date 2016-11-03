#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <float.h>

#include "star.h"
#include "local.h"
#include "error.h"

#define GRAVITATIONAL_CONSTANT (6.67384e-11)

static void
compute_local_force(grav_site *local)
{
    for (int i = 0; i < local->star_count; ++i) {
        grav_star *s = local->stars+i;

        for (int j = 0; j < i; ++j) {
            grav_star *r = local->stars+j;
            double f_n, v_x, v_y, u_n;

            u_n = hypot(s->x - r->x, s->y - r->y);
            f_n = ((GRAVITATIONAL_CONSTANT * s->mass * r->mass)
                   / SQUARE(u_n));
            v_x = (r->x - s->x) / u_n;
            v_y = (r->y - s->y) / u_n;

            s->fx += f_n * v_x;
            s->fy += f_n * v_y;
            s->min_displacement = min(s->min_displacement, 0.1 * u_n);
        }
        for (int j = i+1; j < local->star_count; ++j) {
            grav_star *r = local->stars+j;
            double f_n, v_x, v_y, u_n;

            u_n = hypot(s->x - r->x, s->y - r->y);
            f_n = ((GRAVITATIONAL_CONSTANT * s->mass * r->mass)
                   / SQUARE(u_n));
            v_x = (r->x - s->x) / u_n;
            v_y = (r->y - s->y) / u_n;

            s->fx += f_n * v_x;
            s->fy += f_n * v_y;
            s->min_displacement = min(s->min_displacement, 0.1 * u_n);
        }
    }
}

static void
compute_remote_force(grav_site *local, grav_site *remote)
{
    assert(local->rank != remote->rank);

    for (int i = 0; i < local->star_count; ++i) {
        grav_star *s = local->stars+i;
        for (int j = 0; j < remote->star_count; ++j) {
            grav_star *r = remote->stars+j;
            double f_n, u_x, u_y, u_n;

            u_n = hypot(s->x - r->x, s->y - r->y);
            f_n = ((GRAVITATIONAL_CONSTANT * s->mass * r->mass)
                   / SQUARE(u_n));
            u_x = (r->x - s->x) / u_n;
            u_y = (r->y - s->y) / u_n;

            s->fx += f_n * u_x;
            s->fy += f_n * u_y;
            s->min_displacement = min(s->min_displacement, 0.1 * u_n);
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

static double
greatest_root(double a, double b, double c)
{
    if (DEQUAL(a, 0)) {
        if (DEQUAL(b, 0)) {
            return 31536.0; // number of seconds in 1/1000e of a year
        } else {
            return -c / b;
        }
    }
    assert(c < 0);
    return (-b + sqrt(b*b-4*a*c)) / (2*a);
}

double grav_site_local_compute_step(grav_site *local, double min_step)
{
    double step = DBL_MAX;
    for (int i = 0; i < local->star_count; ++i) {
        double newstep;
        grav_star *s = local->stars+i;
        s->ax = s->fx / s->mass; // compute acceleration
        s->ay = s->fy / s->mass;
        newstep = greatest_root(hypot(s->ax, s->ay),
                                hypot(s->vx, s->vy),
                                - s->min_displacement);
        step = min(step, newstep);
    }
    return max(step, min_step);
}

void grav_site_local_compute_position(grav_site *local, double step)
{
    for (int i = 0; i < local->star_count; ++i) {
        grav_star *s = local->stars+i;
        s->vx += s->ax * step; // compute speed
        s->vy += s->ay * step;
        s->x += s->vx * step;  // compute position
        s->y += s->vy * step;
    }
}

void grav_site_local_init(grav_site *local)
{
    for (int i = 0; i < local->star_count; ++i) {
        grav_star *s = local->stars+i;

        s->min_displacement = DBL_MAX;
        s->fx = 0.0;
        s->fy = 0.0;
    }
}
