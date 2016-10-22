#ifndef GRAV_LOCAL_H
#define GRAV_LOCAL_H

#include "star.h"

#define SQUARE(X_)  ((X_)*(X_))

void grav_site_local_compute_force(grav_site *local, grav_site *remote);
void grav_site_local_compute_position(grav_site *local, double t);
double grav_site_local_compute_step(grav_site *local);

#endif // GRAV_LOCAL_H
