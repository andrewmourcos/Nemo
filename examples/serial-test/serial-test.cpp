#include <iostream>
#include <stdio.h>
#include <unistd.h>       // Used for UART
#include <sys/fcntl.h>    // Used for UART
#include <termios.h>      // Used for UART
#include <cstring>
#include <time.h>         // Used for timeouts

#if defined(__arch64__)
    #define JETSON
#endif

#define     SERIAL_MSG_LEN 10 // All servo messages are 8 characters long + 2 chars for whitespace
#define     VMINX          1
#define     BAUDRATE       B115200

/* Type definition for error codes. All functions should return one of these. */
typedef enum
{
    FISH_ETIMEDOUT = ETIMEDOUT, /* Function took too long and timedout */
    FISH_EPERM = EPERM,         /* Operation not permitted */
    FISH_EIO = EIO,             /* I/O error */
    FISH_EINVAL = EINVAL,       /* Invalid argument */
    FISH_EOK = 0                /* No error */
} fish_error_t;

typedef struct
{
    struct termios port_options;
    int fid;
} serial_handle_t;

fish_error_t setupSerial(serial_handle_t * serial, const char * uart_target);

/* Doesn't check response from UART device before returning */
fish_error_t moveServoAsync(serial_handle_t serial, uint8_t angle, uint8_t speed, bool left_servo);

/* Waits for response from UART device before returning -- checks for servo errors */
fish_error_t moveServoSync(serial_handle_t serial, uint8_t angle, uint8_t speed, bool left_servo);

int main(int argc, char const *argv[])
{
    printf(">> Starting serial example ...\n");

    const char * uart_target = "/dev/ttyTHS1";
    serial_handle_t serial;
    if ( setupSerial(&serial, uart_target) != FISH_EOK){
        return EXIT_FAILURE;
    }

    fish_error_t err = FISH_EOK;

    // Example using Asynchronous servo moving. Note: you should probably sleep or do other stuff
    // for a certain amount of time such that the motor reaches it's desired angle.
    err = moveServoAsync(serial, 0, 100, true);
    usleep(1000000);
    err = moveServoAsync(serial, 25, 100, false);
    usleep(1000000);
    err = moveServoAsync(serial, 30, 100, true);
    usleep(1000000);
    err = moveServoAsync(serial, 100, 100, false);
    usleep(1000000);

    // Example using Synchronous -- it blocks until it reaches the desired angle, timesout or fails
    // Use this wherever possible
    err = moveServoSync(serial, 0, 100, true);
    err = moveServoSync(serial, 180, 100, true);
    err = moveServoSync(serial, 0, 100, false);
    err = moveServoSync(serial, 180, 100, false);

    /* Here we aren't checking the return values. Outside of this quick example, we should always 
     * check these for errors.
    */

    printf("Execution finished normally\n");
        return 0;
}

fish_error_t setupSerial(serial_handle_t * serial, const char * uart_target) {
    // Establish connection
    serial->fid = -1;
    tcgetattr(serial->fid, &(serial->port_options));
    serial->fid = open(uart_target, O_RDWR | O_NOCTTY );
    tcflush(serial->fid, TCIFLUSH);
    tcflush(serial->fid, TCIOFLUSH);
    usleep(1000000); // Wait a second for connection
    if (serial->fid == -1) {
        printf("Failed to open serial port\n");
        return FISH_EIO;
    }

    // Apply settings (attributes) to bitmask
    serial->port_options.c_cflag &= ~PARENB;            // Disables the Parity Enable bit(PARENB),So No Parity
    serial->port_options.c_cflag &= ~CSTOPB;            // CSTOPB = 2 Stop bits,here it is cleared so 1 Stop bit
    serial->port_options.c_cflag &= ~CSIZE;             // Clears the mask for setting the data size
    serial->port_options.c_cflag |=  CS8;               // Set the data bits = 8
    serial->port_options.c_cflag &= ~CRTSCTS;           // No Hardware flow Control
    serial->port_options.c_cflag |=  CREAD | CLOCAL;                  // Enable receiver,Ignore Modem Control lines
    serial->port_options.c_iflag &= ~(IXON | IXOFF | IXANY);          // Disable XON/XOFF flow control both input & output
    serial->port_options.c_iflag &= ~(ICANON | ECHO | ECHOE | ISIG);  // Non Cannonical mode
    serial->port_options.c_oflag &= ~OPOST;                           // No Output Processing
    serial->port_options.c_lflag = 0;               //  enable raw input instead of canonical,
    serial->port_options.c_cc[VMIN]  = VMINX;       // Read at least 1 character
    serial->port_options.c_cc[VTIME] = 0;          // Wait forever
    cfsetispeed(&(serial->port_options),BAUDRATE);  // Set Read  Speed
    cfsetospeed(&(serial->port_options),BAUDRATE);  // Set Write Speed

    // Set attributes in termios structure
    int att = tcsetattr(serial->fid, TCSANOW, &(serial->port_options));
    if (att != 0 ) {
        printf("Failed to set serial port attributes\n");
        return FISH_EIO;
    }

    // Flush Buffers
    tcflush(serial->fid, TCIFLUSH);
    tcflush(serial->fid, TCIOFLUSH);
    usleep(500000);

    return FISH_EOK;
}

/* Description: Sends message to serial device to turn servo to specified angle (0-180 deg) at 
 *              the specified speed (0-100). Does not wait or even check if the rotation is
 *              complete before returning.
 */
fish_error_t moveServoAsync(serial_handle_t serial, uint8_t angle, uint8_t speed, bool left_servo) {
    if (angle < 0 || angle > 180 || speed < 0 || speed > 100) {
        printf("Invalid angle or speed requested");
        return FISH_EINVAL;
    }

    char tx_buffer[SERIAL_MSG_LEN] = {0};
    if (left_servo) {
        snprintf(tx_buffer, SERIAL_MSG_LEN, "SL%03d%03d\n", angle, speed);
    } else {
        snprintf(tx_buffer, SERIAL_MSG_LEN, "SR%03d%03d\n", angle, speed);
    }

    int count = write(serial.fid, &tx_buffer, SERIAL_MSG_LEN);
    if (count < 0) {
        printf("Failed to send command to UART device\n");
        return FISH_EIO;
    }
    return FISH_EOK;
}

/* Description: Sends message to serial device to turn servo to specified angle (0-180 deg) at 
 *              the specified speed (0-100). Blocks until it receives a message back from the 
 *              serial device.
 */
fish_error_t moveServoSync(serial_handle_t serial, uint8_t angle, uint8_t speed, bool left_servo) {
    char rx_buffer[3]               = {0};
    char tx_buffer[SERIAL_MSG_LEN]  = {0};
    int num_chars                   = 0;
    int retries                     = 0;

    if (angle < 0 || angle > 180 || speed < 0 || speed > 100) {
        printf("Invalid angle or speed requested");
        return FISH_EINVAL;
    }

    if (left_servo) {
        snprintf(tx_buffer, SERIAL_MSG_LEN, "SL%03d%03d\n", angle, speed);
    } else {
        snprintf(tx_buffer, SERIAL_MSG_LEN, "SR%03d%03d\n", angle, speed);
    }
    // Clear Rx buffer so we don't read old messages
    tcflush(serial.fid, TCIFLUSH);
    num_chars = write(serial.fid, &tx_buffer, SERIAL_MSG_LEN);
    if (num_chars < 0) {
        printf("Failed to send command to UART device\n");
        return FISH_EIO;
    }
    sleep(0.001); // Needed to workaround Termios read()/tcflush() bug.

    // Allow up to 20 retries on reading message from device
    num_chars = 0;
    while (num_chars < 2 && retries < 20) {
        num_chars = read (serial.fid, rx_buffer, sizeof rx_buffer);
    }

    if (num_chars < 0) {
        printf("UART buffer read failed -- likely timeout\n");
        return FISH_ETIMEDOUT;
    } else if (!strcmp(rx_buffer, "S")) {
        printf("Serial device did not respond with success\n");
        return FISH_EIO;
    }

    return FISH_EOK;
}