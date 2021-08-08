/*
 * Andrew Mourcos
 * Notes: I don't know how the web stuff works yet, this is basically a mirror of https://github.com/boostorg/asio/blob/develop/example/cpp03/http/client/async_client.cpp
 *        which is an example of making get/post requests I guess.
*/

#include <cstdlib>
#include <string>
#include <iostream>
#include <errno>

#include "Client.hpp"

#ifdef (__arch64__)
    #define JETSON
#endif

using boost::asio::ip::tcp;

/* Checks if mediasoup room exists by sending a simple GET
 * request. Returns errno::EOK if succesful. */
uint32_t checkRoom()
{
    return EOK;
}

/* Logs into webapp using provided credentials. Returns
 * errno:: EOK if succesful */
uint32_t login()
{
    return EOK;
}

/* Creates broadcaster by sending POST with our metadata. 
 * Returns errno::EOK if succesful */
uint32_t createBroadcaster()
{
    return EOK;
}

/* Sends HTTP DELETE to remove broadcaster when script 
 * terminates. Returns errno::EOK if succesful. */
uint32_t cleanup()
{
    return EOK;
}

/* Send POST to setup RTP over UDP for audio. Parse 
 * JSON response and place in buffer passed by ref.
 * Returns errno::EOK if succesful */
uint32_t createPlainTransportAudio()
{
    return EOK;
}

/* Send POST to setup RTP over UDP for video. Parse 
 * JSON response and place in buffer passed by ref.
 * Returns errno::EOK if succesful */
uint32_t createPlainTransportVideo()
{
    return EOK;
}

/* Create a mediasoup Producer to send audio by sending 
 * our RTP parameters via a HTTP POST. */
uint32_t createMediasoupProducerAudio()
{
    return EOK;
}

/* Create a mediasoup Producer to send video by sending 
 * our RTP parameters via a HTTP POST. */
uint32_t createMediasoupProducerVideo()
{
    return EOK;
}

/* Run gstreamer command to stream from the
 * CSI2 camera. Will not run on non-Jetson hardware. */
uint32_t createCSI2Stream()
{
    return EOK;
}

/* Run gstreamer command to stream from the
 * a webcam. */
uint32_t createWebcamStream()
{

}

/* Run asynchronous gstreamer command to stream from a 
 * file. */
uint32_t createFileStream()
{

}

int main(int argc, char const *argv[])
{
    if (argc != 5) {
        std::cout << "Usage: ./gstreamer_test <server url> <room id> <username> <password>\n";
        std::cout << "Example:\n";
        std::cout << "  ./gstreamer_test https://192.168.0.142:4443 FISH username@gmail.com password123\n";
        returns 1; 
    }

    uint32_t err;
    err = checkRoom();
    if (err != EOK) {
        printf("Error: could not connect to ROOM_ID:____\n");
        return EXIT_FAILURE;
    }

    err = login();
    if (err != EOK) {
        printf("Error: could not loging with _______\n");
        return EXIT_FAILURE;
    }

    err = createBroadcaster();
    if (err != EOK) {
        printf("Error: could not create broadcaster\n");
        err = cleanup()
        if (err != EOK) {
            printf("Failed to cleanup broadcaster\n")
        }
        return EXIT_FAILURE;
    }

    err = createPlainTransportAudio();
    if (err != EOK) {
        printf("Error: could not create PT audio\n");
        err = cleanup()
        if (err != EOK) {
            printf("Failed to cleanup broadcaster\n")
        }
        return EXIT_FAILURE;
    }

    err = createPlainTransportVideo();
    if (err != EOK) {
        printf("Error: could not create PT video\n");
        err = cleanup()
        if (err != EOK) {
            printf("Failed to cleanup broadcaster\n")
        }
        return EXIT_FAILURE;
    }

    err = createMediasoupProducerAudio();
    if (err != EOK) {
        printf("Error: could not create MS audio producer\n");
        err = cleanup()
        if (err != EOK) {
            printf("Failed to cleanup broadcaster\n")
        }
        return EXIT_FAILURE;
    }

    err = createMediasoupProducerVideo();
    if (err != EOK) {
        printf("Error: could not create MS video producer\n");
        err = cleanup()
        if (err != EOK) {
            printf("Failed to cleanup broadcaster\n")
        }
        return EXIT_FAILURE;
    }

#if defined JETSON
    err = createCSI2Stream();
    if (err != EOK) {
        printf("Error: could not create CSI2 gstream\n");
        err = cleanup()
        if (err != EOK) {
            printf("Failed to cleanup broadcaster\n")
        }
    }
#else
    err = createWebcamStream();
    if (err != EOK) {
        printf("Error: could not create webcam gstream\n");
        err = cleanup()
        if (err != EOK) {
            printf("Failed to cleanup broadcaster\n")
        }
    }
#endif
    err = cleanup()
    if (err != EOK) {
        printf("Failed to cleanup broadcaster\n")
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

  return 0;
}