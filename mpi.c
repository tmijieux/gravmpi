#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
/* #include <glib.h> */

#include "grav-mpi.h"
#include "star.h"
#include "error.h"
#include "input.h"

#define BUFSIZE 2048

MPI_Datatype star_type;

#define GET_ADDRESS(struct_, field_)            \
    ({                                          \
        MPI_Aint addr1_, addr2_;                \
        struct_ a_;                             \
        MPI_Get_address(&a_, &addr1_);          \
        MPI_Get_address(&(a_.field_), &addr2_); \
        addr2_ - addr1_;                        \
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
        grav_error("%3d: %s\n", grav_get_rank(), error_string);
        exit(EXIT_FAILURE);
    }
}

void grav_mpi_create_mpi_star_struct(void)
{
    const int count = 5;
    int blen[6] = { 1, 1, 1, 1, 1 };
    MPI_Aint offsets[6] = {
        #ifdef HAVE_GLIB_H
        G_STRUCT_OFFSET(grav_star, mass),
        G_STRUCT_OFFSET(grav_star, x), G_STRUCT_OFFSET(grav_star, y),
        G_STRUCT_OFFSET(grav_star, vx),G_STRUCT_OFFSET(grav_star, vy),
        #else
        GET_ADDRESS(grav_star, mass),
        GET_ADDRESS(grav_star, x), GET_ADDRESS(grav_star, y),
        GET_ADDRESS(grav_star, vx),GET_ADDRESS(grav_star, vy),
        #endif

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
void grav_mpi_init_comm(
    int rank, int group_size, int star_count, grav_site *buf1, grav_site *buf2)
{
    if (group_size <= 1)
        return;

    const int tag = 1;
    int next = (rank+1) % group_size;
    int prev = ((rank-1) < 0) ? (group_size-1) : (rank-1);
    int len = STAR_BUF_SIZE(group_size, star_count);

    MPI_Bsend_init(buf1->stars, len, star_type, next, tag,
                   MPI_COMM_WORLD, &buf1->mpi_req_send);
    MPI_Bsend_init(buf2->stars, len, star_type, next, tag,
                   MPI_COMM_WORLD, &buf2->mpi_req_send);

    MPI_Recv_init(buf1->stars, len, star_type, prev, MPI_ANY_TAG,
                  MPI_COMM_WORLD, &buf1->mpi_req_recv);
    MPI_Recv_init(buf2->stars, len, star_type, prev, MPI_ANY_TAG,
                  MPI_COMM_WORLD, &buf2->mpi_req_recv);
}

/**
 * Démarrer le transfert des données de 'remote' au processus suivant
 * et la reception des données du processus précédant dans le buffer de 'input'
 */
void grav_mpi_init_star_transfer(int group_size,
                                 grav_site *remote, grav_site *input)
{
    // MPI_Start
    if (group_size <= 1)
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
    int group_size, grav_site *remote, grav_site *input)
{
    if (group_size <= 1)
        return;

    int err;
    MPI_Status status;
    err = MPI_Wait(&remote->mpi_req_send, &status);
    handle_mpi_error(err);

    int count;
    MPI_Get_count(&status, star_type, &count);
    err = MPI_Wait(&input->mpi_req_recv, &status);
    handle_mpi_error(err);
}

/**
 * Calculer avec les autres processus la valeur du pas de temps à adopter
 * avec une réduction MPI
 */
double grav_mpi_reduce_step(double local_step)
{
    double result;
    int err = MPI_Allreduce(&local_step, &result, 1,
                            MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD);
    handle_mpi_error(err);
    return result;
}
