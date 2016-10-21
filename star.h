#ifndef STAR_H
#define STAR_H

struct star {
    double mass;
    double x, y;
    double vx, vy;
    double ax, ay;
    double force;
};

struct remote_stars {
    struct star *stars;
    MPI_Request *mpi_req;
};

#endif // STAR_H
