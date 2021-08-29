/*
    Author: AndrewMourcos
    Date: Aug 24 2021
    License: Not for commercial use outside LamperLabs
*/

#ifndef __FISH_ERR_H__
#define __FISH_ERR_H__

#include <errno.h>
#include <stdint.h>
#include <mutex>

/* Type definition for error codes. All functions should return one of these. */
typedef enum
{
    FISH_ETIMEDOUT = ETIMEDOUT, /* Function took too long and timedout */
    FISH_EPERM = EPERM,         /* Operation not permitted */
    FISH_EIO = EIO,             /* I/O error */
    FISH_EINVAL = EINVAL,       /* Invalid argument */
    FISH_EOK = 0                /* No error */
} fish_error_t;

/* State structure that gets passed to all threads */
typedef struct {
    uint8_t curr_left_angle;
    uint8_t curr_right_angle;
    uint8_t next_left_angle;
    uint8_t next_right_angle;
    uint8_t curr_speed;
    uint8_t next_speed;
} fish_handle_t;

extern std::mutex fish_handle_mtx;

#endif /* __FISH_ERR_H__ */