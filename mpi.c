#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <glib.h>

#include "grav-mpi.h"
#include "star.h"

#define BUFSIZE 2048

MPI_Datatype star_type;

int grav_get_rank()
{
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    return rank;
}

static void
handle_mpi_error(int code)
{
    if (code != MPI_SUCCESS) {
        char error_string[BUFSIZE];
        int len = BUFSIZE;
        MPI_Error_string(code, error_string, &len);
        fprintf(stderr, "%3d: %s\n", grav_get_rank(), error_string);
        exit(EXIT_FAILURE);
    }
}

void grav_mpi_create_mpi_star_struct(void)
{
    const int count = 5;
    int blen[6] = { 1, 1, 1, 1, 1, 1 };
    MPI_Aint offsets[6] = {
        G_STRUCT_OFFSET(grav_star, mass),
        G_STRUCT_OFFSET(grav_star, x), G_STRUCT_OFFSET(grav_star, y),
        G_STRUCT_OFFSET(grav_star, vx),G_STRUCT_OFFSET(grav_star, vy),
        sizeof(grav_star),
    };
    MPI_Datatype types[6] = {
        MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_UB };

    int err;
    err = MPI_Type_create_struct(count, blen, offsets, types, &star_type);
    handle_mpi_error(err);
    err = MPI_Type_commit(&star_type);
    handle_mpi_error(err);
}

/**
 * Initialiser les communications persistantes avec le processus suivants
 * et le processus precedant;
 */
void grav_mpi_init_comm(int rank, int group_size, grav_site *remote, grav_site *input)
{
  if (group_size == 1)
    return;
  
  MPI_Bsend_init(remote->stars, remote->star_count,
		 star_type, (rank+1)%group_size, MPI_ANY_TAG,
		 MPI_COMM_WORLD, &remote->mpi_req_send);

  MPI_Recv_init(remote->stars, remote->star_count,
		star_type, rank-1 < 0 ? group_size-1 : rank-1, MPI_ANY_TAG,
		MPI_COMM_WORLD, &remote->mpi_req_recv);


  MPI_Bsend_init(input->stars, input->star_count,
		 star_type, (rank+1)%group_size, MPI_ANY_TAG,
		 MPI_COMM_WORLD, &input->mpi_req_send);
  MPI_Recv_init(input->stars, input->star_count,
		star_type, rank-1 < 0 ? group_size-1 : rank-1, MPI_ANY_TAG,
		MPI_COMM_WORLD, &input->mpi_req_recv);
}
  
/**
 * Démarrer le transfert des données de 'remote' au processus suivant
 * et la reception des données du processus précédant dans le buffer de 'input'
 */
void grav_mpi_init_star_transfer(grav_site *remote, grav_site *input, int group_size)
{
  // MPI_Start
  if (group_size == 1)
    return;

    MPI_Start(&remote->mpi_req_send);
    MPI_Start(&input->mpi_req_recv);

}

/**
 * Finaliser les opération démarrer dans la fonction
 *  'grav_mpi_init_star_transfer'
 */
void grav_mpi_finalize_star_transfer(
    grav_site *remote, grav_site *input, int group_size)
{
  if (group_size == 1)
    return;

    MPI_Status status;
    MPI_Wait(&remote->mpi_req_send, &status);
    MPI_Wait(&input->mpi_req_recv, &status);

}

/**
 * Calculer avec les autres processus la valeur du pas de temps à adopter
 * avec une réduction MPI
 */
double grav_mpi_reduce_step(int rank, int group_size, double local_step)
{
  // MPI_Reduce
  double result;
  MPI_Allreduce(&local_step,&result,1,MPI_DOUBLE,MPI_MIN, MPI_COMM_WORLD);
  return result;
}
