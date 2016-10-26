#include "error.h"

#define GRAV_ERROR_TO_STRING(error) [GRAV_ERROR_##error] = #error,

static char const *grav_errmsg_[] = {
    GRAV_ERROR_LIST(GRAV_ERROR_TO_STRING)
};

char const *grav_errmsg(int error_code)
{
    return grav_errmsg_[error_code > 0 ? error_code : -error_code];
}
