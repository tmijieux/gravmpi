#ifndef GRAV_MPI_H
#define GRAV_MPI_H

#include "star.h"

void grav_mpi_create_mpi_star_struct(void);
void grav_mpi_init_comm(int rank, int group_size, int star_count,
                        grav_site *remote, grav_site *input);
void grav_mpi_init_star_transfer(
    int group_size, grav_site *remote, grav_site *input);
void grav_mpi_finalize_star_transfer(
    int group_size, grav_site *remote, grav_site *input);
double grav_mpi_reduce_step(double local_step);

#endif // GRAV_MPI_H
