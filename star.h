#ifndef GRAV_STAR_H
#define GRAV_STAR_H

#include <mpi.h>

typedef struct grav_star grav_star;
typedef struct grav_site grav_site;

struct grav_star {
    double mass;
    double x, y;
    double vx, vy;
    double fx, fy;
};

struct grav_site {
    int star_count;
    grav_star *stars;
    MPI_Request *mpi_req;
    int rank;
};

#endif // GRAV_STAR_H
