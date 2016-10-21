#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#include "cmdline.h"

static double
drand(double min, double max)
{
    return ((double) rand() / RAND_MAX) * (max - min) + min;
}

int main(int argc, char *argv[])
{
    uint64_t n;
    struct gengetopt_args_info conf;

    srand(time(NULL)+(long)&argc);
    cmdline_parser(argc, argv, &conf);
    int star_count = conf.star_arg;
    n = star_count + 0.1*(((double)rand() / RAND_MAX) * star_count);

    while (n--) {
        printf("%g %g %g %g %g\n",
               drand(conf.mass_min_arg, conf.mass_max_arg),
               drand(conf.xmin_arg, conf.xmax_arg),
               drand(conf.ymin_arg, conf.ymax_arg),
               drand(conf.vxmin_arg, conf.vxmax_arg),
               drand(conf.vymin_arg, conf.vymax_arg)
        );
    }

    return EXIT_SUCCESS;
}
