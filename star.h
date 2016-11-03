#ifndef GRAV_STAR_H
#define GRAV_STAR_H

#include <mpi.h>
#include <stdbool.h>

typedef struct grav_star grav_star;
typedef struct grav_site grav_site;

struct grav_star {
    double mass;
    double x, y;
    double vx, vy;
    double ax, ay;
    double fx, fy;
    double min_displacement;
};

struct grav_site {
    int star_count;
    grav_star *stars;
    MPI_Request mpi_req_send;
    MPI_Request mpi_req_recv;
    int rank;
};

void grav_star_dump(grav_star *s);
void grav_site_dump(grav_site *site, bool print_data);
void grav_site_print(grav_site *site);
void grav_site_copy(grav_site *dest, grav_site *src);

#endif // GRAV_STAR_H
