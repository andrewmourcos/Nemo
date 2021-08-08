/*
 * Andrew Mourcos
 * Notes: This won't run until all functions below are filled out.
*/

#include <cstdlib>
#include <string>
#include <iostream>
#include <errno.h>

#include "Client.hpp"

#if defined(__arch64__)
    #define JETSON
#endif

using boost::asio::ip::tcp;

typedef enum
{
    FISH_EPERM = EPERM,     /* Operation not permitted */
    FISH_EIO = EIO,         /* I/O error */
    FISH_EINVAL = EINVAL,   /* Invalid argument */
    FISH_EOK = 0          /* No error */
} fish_error_t;


/* Checks if mediasoup room exists by sending a simple GET
 * request. Returns errno::EOK if succesful. */
fish_error_t checkRoom()
{
    return FISH_EOK;
}

/* Logs into webapp using provided credentials. Returns
 * FISH_EOK if succesful */
fish_error_t login()
{
    return FISH_EOK;
}

/* Creates broadcaster by sending POST with our metadata. 
 * Returns errno::EOK if succesful */
fish_error_t createBroadcaster()
{
    return FISH_EOK;
}

/* Sends HTTP DELETE to remove broadcaster when script 
 * terminates. Returns errno::EOK if succesful. */
fish_error_t cleanup()
{
    return FISH_EOK;
}

/* Send POST to setup RTP over UDP for audio. Parse 
 * JSON response and place in buffer passed by ref.
 * Returns errno::EOK if succesful */
fish_error_t createPlainTransportAudio()
{
    return FISH_EOK;
}

/* Send POST to setup RTP over UDP for video. Parse 
 * JSON response and place in buffer passed by ref.
 * Returns errno::EOK if succesful */
fish_error_t createPlainTransportVideo()
{
    return FISH_EOK;
}

/* Create a mediasoup Producer to send audio by sending 
 * our RTP parameters via a HTTP POST. */
fish_error_t createMediasoupProducerAudio()
{
    return FISH_EOK;
}

/* Create a mediasoup Producer to send video by sending 
 * our RTP parameters via a HTTP POST. */
fish_error_t createMediasoupProducerVideo()
{
    return FISH_EOK;
}

#if defined(JETSON)
/* Run gstreamer command to stream from the
 * CSI2 camera. Will not run on non-Jetson hardware. */
fish_error_t createCSI2Stream()
{
    return FISH_EOK;
}
#endif

/* Run gstreamer command to stream from the
 * a webcam. */
fish_error_t createWebcamStream()
{
    return FISH_EOK;
}

/* Run asynchronous gstreamer command to stream from a 
 * file. */
fish_error_t createFileStream()
{
    return FISH_EOK;
}

int main(int argc, char const *argv[])
{
    if (argc != 5) {
        std::cout << "Usage: ./gstreamer_test <server url> <room id> <username> <password>\n";
        std::cout << "Example:\n";
        std::cout << "  ./gstreamer_test https://192.168.0.142:4443 FISH username@gmail.com password123\n";
        return EXIT_FAILURE; 
    }

    uint32_t err;
    err = checkRoom();
    if (err != FISH_EOK) {
        printf("Error: could not connect to ROOM_ID:____\n");
        return EXIT_FAILURE;
    }

    err = login();
    if (err != FISH_EOK) {
        printf("Error: could not loging with _______\n");
        return EXIT_FAILURE;
    }

    err = createBroadcaster();
    if (err != FISH_EOK) {
        printf("Error: could not create broadcaster\n");
        err = cleanup();
        if (err != FISH_EOK) {
            printf("Failed to cleanup broadcaster\n");
        }
        return EXIT_FAILURE;
    }

    err = createPlainTransportAudio();
    if (err != FISH_EOK) {
        printf("Error: could not create PT audio\n");
        err = cleanup();
        if (err != FISH_EOK) {
            printf("Failed to cleanup broadcaster\n");
        }
        return EXIT_FAILURE;
    }

    err = createPlainTransportVideo();
    if (err != FISH_EOK) {
        printf("Error: could not create PT video\n");
        err = cleanup();
        if (err != FISH_EOK) {
            printf("Failed to cleanup broadcaster\n");
        }
        return EXIT_FAILURE;
    }

    err = createMediasoupProducerAudio();
    if (err != FISH_EOK) {
        printf("Error: could not create MS audio producer\n");
        err = cleanup();
        if (err != FISH_EOK) {
            printf("Failed to cleanup broadcaster\n");
        }
        return EXIT_FAILURE;
    }

    err = createMediasoupProducerVideo();
    if (err != FISH_EOK) {
        printf("Error: could not create MS video producer\n");
        err = cleanup();
        if (err != FISH_EOK) {
            printf("Failed to cleanup broadcaster\n");
        }
        return EXIT_FAILURE;
    }

// Only use csi2 function if running on Jetson, otherwise use regular webcam
#if defined(JETSON)
    err = createCSI2Stream();
    if (err != FISH_EOK) {
        printf("Error: could not create CSI2 gstream\n");
        err = cleanup();
        if (err != FISH_EOK) {
            printf("Failed to cleanup broadcaster\n");
        }
    }
#else
    err = createWebcamStream();
    if (err != FISH_EOK) {
        printf("Error: could not create webcam gstream\n");
        err = cleanup();
        if (err != FISH_EOK) {
            printf("Failed to cleanup broadcaster\n");
        }
    }
#endif
    err = cleanup();
    if (err != FISH_EOK) {
        printf("Failed to cleanup broadcaster\n");
    }

    // ****** Code from boost::asio HTTP request example: *****
    // try
    // {
    //  if (argc != 3)
    //  {
    //      std::cout << "Usage: gstreamer_test <server> <path>\n";
    //      std::cout << "Example:\n";
    //      std::cout << "  gstreamer_test www.boost.org /LICENSE_1_0.txt\n";
    //      return 1;
    //  }

    //  boost::asio::io_context io_context;
    //  client c(io_context, argv[1], argv[2]);
    //  io_context.run();
    // }
    // catch (std::exception& e)
    // {
    //  std::cout << "Exception: " << e.what() << "\n";
    // }
    
    printf("Execution finished normally\n");
    return 0;
}