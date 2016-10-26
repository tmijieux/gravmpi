#ifndef GRAV_ERROR_H
#define GRAV_ERROR_H

#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

G_BEGIN_DECLS

#define GRAV_ERROR_LIST(ERROR)                  \
    ERROR(SUCCESS)                              \


#define GRAV_ERROR_TO_ENUM(error) GRAV_ERROR_##error,

enum grav_error_code {
    GRAV_ERROR_LIST(GRAV_ERROR_TO_ENUM)
};

char const *grav_errmsg(int errcode);

#ifndef __GNUC__
#define __PRETTY_FUNCTION__    __FUNCDNAME__
#endif


#define __FILENAME__ (strrchr(__FILE__, '/') ?                  \
                      strrchr(__FILE__, '/') + 1 : __FILE__)

#define grav_error(format_, ...)                                \
    do {                                                        \
        fprintf(stderr, "ERROR: %s:%d|%s: ",  __FILENAME__ ,    \
                __LINE__, __PRETTY_FUNCTION__);                 \
        fprintf(stderr, (format_), ##__VA_ARGS__);              \
    } while(0)

#define grav_fatal(format_, ...)                                        \
    do {                                                                \
        fprintf(stderr, "FATAL ERROR: %s:%d|%s: ",  __FILENAME__ ,      \
                __LINE__, __PRETTY_FUNCTION__);                         \
        fprintf(stderr, (format_), ##__VA_ARGS__);                      \
        exit(EXIT_FAILURE);                                             \
    } while(0)

#define grav_warning(format_, ...)                              \
    do {                                                        \
        fprintf(stderr, "WARNING: %s:%d|%s: ",  __FILENAME__ ,  \
                __LINE__, __PRETTY_FUNCTION__);                 \
        fprintf(stderr, (format_), ##__VA_ARGS__);              \
    } while(0)


#ifdef DEBUG
#define grav_debug(format_, ...)                                        \
    do {                                                                \
        fprintf(stderr, "DEBUG: %s:%d|%s: " format_, __FILENAME__ ,     \
                __LINE__, __PRETTY_FUNCTION__, ##__VA_ARGS__);          \
    } while(0)

#else // DEBUG
#define grav_debug(format_, ...) ((void) (format_))
#endif // DEBUG

G_END_DECLS

#endif // GRAV_ERROR_H
