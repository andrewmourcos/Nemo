/*
 * Andrew Mourcos
 * Notes: This won't run until all functions below are filled out.
*/

#include <cstdlib>
#include <string>
#include <iostream>
#include <errno.h>

#include "jsoncpp/json/json.h"

// Note: macro needs to be defined before including httplib
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"

#if defined(__arch64__)
    #define JETSON
#endif

/* Type definition for error codes. All functions should return one of these. */
typedef enum
{
    FISH_EPERM = EPERM,     /* Operation not permitted */
    FISH_EIO = EIO,         /* I/O error */
    FISH_EINVAL = EINVAL,   /* Invalid argument */
    FISH_EOK = 0            /* No error */
} fish_error_t;

/* Checks if mediasoup room exists by sending a simple GET
 * request and checking for 200. Returns errno::EOK if succesful. */
fish_error_t checkRoom(const char *server_url, const char *room_id)
{
    std::string room("/rooms/");
    room += room_id;

    httplib::Client cli(server_url);
    cli.enable_server_certificate_verification(false);

    auto res = cli.Get(room.c_str());

    if (res->status != 200) {
        printf("Room not found: %s\n", room.c_str());
        return FISH_EIO;
    } else {
        printf(">> Room found\n");
    }

    return FISH_EOK;
}

/* Logs into webapp using provided credentials. Sets token for
 * future calls. Returns FISH_EOK if succesful */
fish_error_t login(const char* server_url, const char* username, const char* password, std::string &token)
{
    // Set items to send in POST request
    httplib::Params params;
    params.emplace("email", username);
    params.emplace("password", password);

    httplib::Client cli(server_url);
    cli.enable_server_certificate_verification(false);

    auto res = cli.Post("/api/users/login", params);

    if (res->status != 200) {
        printf("Failed to login with usr:%s, psw:%s", username, password);
        return FISH_EIO;
    }

    Json::Reader reader;
    Json::Value root;
    reader.parse(res->body, root);
    token = root["token"].toStyledString();

    printf(">> Logged in succesfully, token:%s\n", token.c_str());

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
    
    fish_error_t err;
    std::string token;

    const char* server_url = argv[1];
    const char* room_id = argv[2];
    const char* username = argv[3];
    const char* password = argv[4];

    err = checkRoom(server_url, room_id);
    if (err != FISH_EOK) {
        printf("Error: could not connect to ROOM_ID:____\n");
        return EXIT_FAILURE;
    }

    err = login(server_url, username, password, token);
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
    
    printf("Execution finished normally\n");
    return 0;
}