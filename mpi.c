#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <glib.h>

#include "grav-mpi.h"
#include "star.h"
#include "error.h"

#define BUFSIZE 2048

MPI_Datatype star_type;

#define GET_ADRESS(struct_, field_)             \
    ({                                          \
        MPI_Aint addr1_, addr2_;                \
        struct_ a_ ;                            \
        MPI_Get_address(&a_, &addr1);           \
        MPI_Get_address(&a->field_, &addr2);    \
        addr2 - addr1;                          \
    })

int grav_get_rank(void)
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
        grav_debug("%3d: %s\n", grav_get_rank(), error_string);
        exit(EXIT_FAILURE);
    }
}

void grav_mpi_create_mpi_star_struct(void)
{
    const int count = 5;
    int blen[6] = { 1, 1, 1, 1, 1 };
    MPI_Aint offsets[6] = {
        G_STRUCT_OFFSET(grav_star, mass),
        G_STRUCT_OFFSET(grav_star, x), G_STRUCT_OFFSET(grav_star, y),
        G_STRUCT_OFFSET(grav_star, vx),G_STRUCT_OFFSET(grav_star, vy),
    };
    MPI_Datatype types[5] = {
        MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE
    };

    int err;
    MPI_Datatype tmp;
    err = MPI_Type_create_struct(count, blen, offsets, types, &tmp);
    handle_mpi_error(err);

    err = MPI_Type_create_resized(tmp, 0, sizeof(grav_star), &star_type);
    handle_mpi_error(err);

    err = MPI_Type_commit(&star_type);
    handle_mpi_error(err);

    MPI_Type_free(&tmp);
}

/**
 * Initialiser les communications persistantes avec le processus suivants
 * et le processus precedant;
 */
void grav_mpi_init_comm(int rank, int group_size,
                        grav_site *remote, grav_site *input)
{
    if (group_size == 1)
        return;

    MPI_Bsend_init(remote->stars, remote->star_count,
                   star_type, (rank+1)%group_size, 1,
                   MPI_COMM_WORLD, &remote->mpi_req_send);
    grav_debug("remote_send count=%d\n", remote->star_count);

    MPI_Recv_init(remote->stars, remote->star_count,
                  star_type, rank-1 < 0 ? group_size-1 : rank-1, MPI_ANY_TAG,
                  MPI_COMM_WORLD, &remote->mpi_req_recv);


    MPI_Bsend_init(input->stars, input->star_count,
                   star_type, (rank+1)%group_size, 1,
                   MPI_COMM_WORLD, &input->mpi_req_send);

    MPI_Recv_init(input->stars, input->star_count,
                  star_type, rank-1 < 0 ? group_size-1 : rank-1, MPI_ANY_TAG,
                  MPI_COMM_WORLD, &input->mpi_req_recv);
    grav_debug("input count=%d\n", input->star_count);
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

    int err;
    err = MPI_Start(&remote->mpi_req_send);
    handle_mpi_error(err);

    err = MPI_Start(&input->mpi_req_recv);
    handle_mpi_error(err);
}

/**
 * Finaliser les opération démarrer dans la fonction
 *  'grav_mpi_init_star_transfer'
 */
void grav_mpi_finalize_star_transfer(
    grav_site *remote, grav_site *input, int group_size)
{
    if (group_size <= 1)
        return;

    int err;
    MPI_Status status;
    err = MPI_Wait(&remote->mpi_req_send, &status);
    handle_mpi_error(err);

    int count;
    MPI_Get_count(&status, star_type, &count);
    grav_debug("count=%d\n", count);

    err = MPI_Wait(&input->mpi_req_recv, &status);
    handle_mpi_error(err);
}

/**
 * Calculer avec les autres processus la valeur du pas de temps à adopter
 * avec une réduction MPI
 */
double grav_mpi_reduce_step(int rank, int group_size, double local_step)
{
    int err;
    double result;
    err = MPI_Allreduce(&local_step, &result, 1, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD);
    handle_mpi_error(err);
    return result;
}
