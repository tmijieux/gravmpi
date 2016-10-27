#ifndef GRAV_INPUT_H
#define GRAV_INPUT_H

#include "star.h"


#define EXTRA_STAR(TOTAL_STAR_COUNT_, GROUP_SIZE_)      \
    ((TOTAL_STAR_COUNT_) % (GROUP_SIZE_))

#define STAR_COUNT(RANK_, GROUP_SIZE_, TOTAL_STAR_COUNT_)               \
    ((TOTAL_STAR_COUNT_) / (GROUP_SIZE_)                                \
     + ((EXTRA_STAR(TOTAL_STAR_COUNT_, GROUP_SIZE_) != 0)               \
        * ((RANK_) < EXTRA_STAR(TOTAL_STAR_COUNT_, GROUP_SIZE_))))

#define STAR_BUF_SIZE(GROUP_SIZE_, TOTAL_STAR_COUNT_)                   \
    ((TOTAL_STAR_COUNT_) / (GROUP_SIZE_)                                \
     + (EXTRA_STAR(TOTAL_STAR_COUNT_, GROUP_SIZE_) != 0))


int grav_read_file(char const *filename, int rank, int size,
                   grav_site *local_stars, grav_site remote_buf[2]);

#endif // GRAV_INPUT_H
