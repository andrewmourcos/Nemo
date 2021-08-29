/*
    Author: AndrewMourcos
    Date: Aug 24 2021
    License: Not for commercial use outside LamperLabs
*/

#ifndef __FISHIO_H__
#define __FISHIO_H__

#include <termios.h>
#include <iostream>
#include <stdio.h>

#include "../common/fish_types.h"

#define SERIAL_MSG_LEN 10 // All servo messages are 8 characters long + 2 chars for whitespace
#define VMINX          1
#define BAUDRATE       B38400

typedef struct {
    struct termios port_options;
    int fid;
} serial_handle_t;

/* Description: Establishes serial connection to target device provided as serial device file */
fish_error_t setupSerial(serial_handle_t * serial, const char * uart_target);

/* Description: Sends message to serial device to turn servo to specified angle (0-180 deg) at 
 *              the specified speed (0-100). Does not wait or even check if the rotation is
 *              complete before returning.
 */
fish_error_t moveServoAsync(serial_handle_t serial, uint8_t angle, uint8_t speed, bool left_servo);

/* Description: Sends message to serial device to turn servo to specified angle (0-180 deg) at 
 *              the specified speed (0-100). Blocks until it receives a message back from the 
 *              serial device.
 */
fish_error_t moveServoSync(serial_handle_t serial, uint8_t angle, uint8_t speed, bool left_servo);

/* Description: Sends desired speed (as percentage) to the UART device. Blocks until
 *              it receives a message back from the device.
 */
fish_error_t setCaudalFinSpeed(serial_handle_t serial, uint8_t speed_percentage);

#endif /* __FISHIO_H__ */