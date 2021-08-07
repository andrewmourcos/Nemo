#include <errno.h>

#define FISH_LOG(f_, ...) printf((f_), __VA_ARGS__)


typedef enum
{
    FISH_EPERM = EPERM,     /* Operation not permitted */
    FISH_EIO = EIO,         /* I/O error */
    FISH_EINVAL = EINVAL,    /* Invalid argument */
    FISH_EOK = EOK
} fish_error_t;



