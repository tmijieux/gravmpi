#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#include "cmdline.h"
#include "star.h"
#include "input.h"
#include "grav-mpi.h"
#include "local.h"
#include "error.h"

#define SWAP_POINTER(P1_, P2_)                  \
    do {                                        \
        if (group_size > 1) {                   \
            void *_ptr_tmp_MAXCRO_ = (P1_);     \
            (P1_) = (P2_);                      \
            (P2_) = _ptr_tmp_MAXCRO_;           \
        }                                       \
    }while(0)

static void
main_loop(int rank, int group_size, int star_count,
          grav_site *local, grav_site *remote, grav_site *input)
{
    const double tmax = 3.154e7;
    grav_site_print(local);

    double t = 0.0;
    while (t < tmax) {

        grav_site_local_init(local);
        for (int i = 0; i < group_size; ++i) {
            grav_mpi_init_star_transfer(group_size, remote, input);

            int remote_rank = (rank+group_size-1+i)%group_size;
            remote->rank = remote_rank;
            remote->star_count = STAR_COUNT(remote_rank, group_size, star_count);

            grav_site_local_compute_force(local, remote);

            grav_mpi_finalize_star_transfer(group_size, remote, input);
            SWAP_POINTER(remote, input);
        }

        //grav_site_dump(local, true);
        double step = grav_site_local_compute_step(local);
        step = grav_mpi_reduce_step(step);
        grav_site_local_compute_position(local, step);
        grav_site_print(local);
         t += step;
    }
}

int main(int argc, char *argv[])
{
    int group_size, rank, star_count;
    grav_site remote_buf[2];
    grav_site local_stars;
    struct gengetopt_args_info opt;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &group_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    printf("#rank: %d; group_size: %d\n", rank, group_size);

    cmdline_parser(argc, argv, &opt);
    grav_mpi_create_mpi_star_struct();
    star_count = grav_read_file(opt.input_file_arg, rank, group_size,
                                &local_stars, remote_buf);
    grav_mpi_init_comm(rank, group_size, star_count, remote_buf, remote_buf+1);
    main_loop(rank, group_size, star_count,
              &local_stars, remote_buf, remote_buf+1);

    MPI_Finalize();
    return EXIT_SUCCESS;
}
