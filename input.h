#ifndef GRAV_INPUT_H
#define GRAV_INPUT_H

#include "star.h"

void grav_read_file(char const *filename, int rank, int size,
                    grav_site *local_stars, grav_site remote_buf[2]);

#endif // GRAV_INPUT_H
