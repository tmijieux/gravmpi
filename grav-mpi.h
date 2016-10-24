#ifndef GRAV_MPI_H
#define GRAV_MPI_H

#include "star.h"

void grav_mpi_create_mpi_star_struct(void);
void grav_mpi_init_comm(int rank, int group_size, grav_site *remote, grav_site *input);
void grav_mpi_init_star_transfer(grav_site *remote, grav_site *input, int size);
void grav_mpi_finalize_star_transfer(grav_site *remote, grav_site *input, int size);
double grav_mpi_reduce_step(int rank, int size, double local_step);

#endif // GRAV_MPI_H
