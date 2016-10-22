#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#include "cmdline.h"
#include "star.h"
#include "input.h"
#include "grav-mpi.h"
#include "local.h"

#define SWAP_POINTER(P1_, P2_)                  \
    do {                                        \
        void *_ptr_tmp_MAXCRO_ = (P1_);         \
        (P1_) = (P2_);                          \
        (P2_) = _ptr_tmp_MAXCRO_;               \
    }while(0)


static void
main_loop(grav_site *local, grav_site *remote, grav_site *input, int group_size)
{
    const double tmax = 1000.0;
    double t = 0.0;
    while (t < tmax) {
        int n = group_size;
        while (n--) {
            grav_mpi_init_star_transfer(remote, input, group_size);
            grav_site_local_compute_force(local, remote);
            grav_mpi_finalize_star_transfer(remote, input, group_size);
            SWAP_POINTER(remote, input);
        }

        // à partir d'ici la valeur de la force est "complète"
        grav_site_local_compute_position(local, t);

        // MPI reduce sur les distances minimales
        double step;
        step = grav_site_local_compute_step(local);
        step = grav_mpi_reduce_step(local->rank, group_size, step);
        t += step;
    }
}

int main(int argc, char *argv[])
{
    int group_size, rank;
    grav_site remote_buf[2];
    grav_site local_stars;
    struct gengetopt_args_info opt;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &group_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    printf("rank: %d; group_size: %d\n", rank, group_size);

    cmdline_parser(argc, argv, &opt);
    grav_mpi_create_mpi_star_struct();
    grav_read_file(opt.input_file_arg, rank,
                   group_size, &local_stars, remote_buf);

    grav_mpi_init_comm(remote_buf, group_size);
    main_loop(&local_stars, remote_buf, remote_buf+1, group_size);

    MPI_Finalize();
    return EXIT_SUCCESS;
}
