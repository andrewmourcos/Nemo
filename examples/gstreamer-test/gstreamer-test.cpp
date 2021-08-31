/*
 * TODO:
 *      - implement gstreamer function(s)
 *      - create a handle structure to pass in functions instead of having a ton of parameters
*/

#include <cstdlib>
#include <string>
#include <iostream>
#include <errno.h>

#include "jsoncpp/json/json.h"
#include <opencv2/opencv.hpp>

// Note: macro needs to be defined before including httplib
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/random_generator.hpp>


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
    token = root["token"].asString();

    printf(">> Logged in succesfully, token:%s\n", token.c_str());

    return FISH_EOK;
}

/* Creates broadcaster by sending POST with our metadata. 
 * Returns errno::EOK if succesful */
fish_error_t createBroadcaster(const char* server_url, const char* room_id, std::string token, std::string &broadcaster_id)
{
    boost::uuids::uuid uuid = boost::uuids::random_generator()();
    broadcaster_id = boost::lexical_cast<std::string>(uuid);

    httplib::Client cli(server_url);
    cli.enable_server_certificate_verification(false);
    cli.set_bearer_token_auth(token.c_str());

    char json_msg[512];
    sprintf(json_msg, "                             \
        {                                           \
          \"id\": \"%s\",                           \
          \"displayName\": \"Broadcaster\",         \
          \"device\": {\"name\": \"GStreamer\"}     \
        }", broadcaster_id.c_str());

    std::string extension("/rooms/");
    extension += room_id;
    extension += "/broadcasters";
    
    auto res = cli.Post(extension.c_str(), json_msg, "application/json");
    if (res->status != 200) {
        printf("Failed to create broadcaster");
        return FISH_EIO;
    }

    printf(">> Created Broadcaster\n");

    return FISH_EOK;
}

/* Sends HTTP DELETE to remove broadcaster when script 
 * terminates. Returns errno::EOK if succesful. */
fish_error_t cleanup(const char * server_url, const char * room_id, std::string token, std::string broadcaster_id)
{
    std::string extension("/rooms/");
    extension += room_id;
    extension += "/broadcasters/";
    extension += broadcaster_id;

    httplib::Client cli(server_url);
    cli.enable_server_certificate_verification(false);
    cli.set_bearer_token_auth(token.c_str());

    auto res = cli.Delete(extension.c_str());
    if (res->status != 200) {
        printf("Failed to delete broadcaster");
        return FISH_EIO;
    }
    printf(">> Deleted Broadcaster\n");
    return FISH_EOK;
}

/* Send POST to setup RTP over UDP for audio. Parse 
 * JSON response and place in buffer passed by ref.
 * Returns errno::EOK if succesful */
fish_error_t createPlainTransportAudio(const char * server_url, const char * room_id, 
                                        std::string token, std::string broadcaster_id,
                                        std::string &audio_transport_id, 
                                        std::string &audio_transport_ip,
                                        std::string &audio_transport_port,
                                        std::string &audio_transport_rtcp_port)
{
    std::string extension("/rooms/");
    extension += room_id;
    extension += "/broadcasters/";
    extension += broadcaster_id;
    extension += "/transports";

    httplib::Client cli(server_url);
    cli.enable_server_certificate_verification(false);
    cli.set_bearer_token_auth(token.c_str());

    char json_msg[512] = "          \
        {                           \
          \"type\": \"plain\",      \
          \"comedia\": true,        \
          \"rtcpMux\": false        \
        }";

    auto res = cli.Post(extension.c_str(), json_msg, "application/json");
    if (res->status != 200) {
        printf("Failed to create plain transport audio");
        return FISH_EIO;
    }

    // Parse the JSON response to get the ID, IP, port and RTCP port
    // TODO: add error checking to the JSON indexing below
    Json::Reader reader;
    Json::Value root;
    reader.parse(res->body, root);
    audio_transport_id          = root["id"].asString();
    audio_transport_ip          = root["ip"].asString();
    audio_transport_port        = root["port"].asString();
    audio_transport_rtcp_port   = root["rtcpPort"].asString();

    printf(">> Created audio plain transport\n");

    return FISH_EOK;
}

/* Send POST to setup RTP over UDP for video. Parse 
 * JSON response and place in buffer passed by ref.
 * Returns errno::EOK if succesful */
fish_error_t createPlainTransportVideo(const char * server_url, const char * room_id, 
                                        std::string token, std::string broadcaster_id,
                                        std::string &video_transport_id, 
                                        std::string &video_transport_ip,
                                        std::string &video_transport_port,
                                        std::string &video_transport_rtcp_port)
{
    std::string extension("/rooms/");
    extension += room_id;
    extension += "/broadcasters/";
    extension += broadcaster_id;
    extension += "/transports";

    httplib::Client cli(server_url);
    cli.enable_server_certificate_verification(false);
    cli.set_bearer_token_auth(token.c_str());

    char json_msg[512] = "          \
        {                           \
          \"type\": \"plain\",      \
          \"comedia\": true,        \
          \"rtcpMux\": false        \
        }";

    auto res = cli.Post(extension.c_str(), json_msg, "application/json");
    if (res->status != 200) {
        printf("Failed to create plain transport audio");
        return FISH_EIO;
    }

    // Parse the JSON response to get the ID, IP, port and RTCP port
    // TODO: add error checking to the JSON indexing below
    Json::Reader reader;
    Json::Value root;
    reader.parse(res->body, root);
    video_transport_id          = root["id"].asString();
    video_transport_ip          = root["ip"].asString();
    video_transport_port        = root["port"].asString();
    video_transport_rtcp_port   = root["rtcpPort"].asString();

    printf(">> Created video plain transport\n");
    return FISH_EOK;
}

/* Create a mediasoup Producer to send audio by sending 
 * our RTP parameters via a HTTP POST. */
fish_error_t createMediasoupProducerAudio(const char * server_url, const char * room_id, 
                                          std::string token, std::string broadcaster_id,
                                          std::string audio_transport_id)
{
    std::string extension("/rooms/");
    extension += room_id;
    extension += "/broadcasters/";
    extension += broadcaster_id;
    extension += "/transports/";
    extension += audio_transport_id;
    extension += "/producers";

    httplib::Client cli(server_url);
    cli.enable_server_certificate_verification(false);
    cli.set_bearer_token_auth(token.c_str());

    char json_msg[1024] = "\
        {\
            \"kind\": \"audio\",\
            \"rtpParameters\": {\
                \"codecs\": [\
                    {\
                        \"mimeType\": \"audio/opus\",\
                        \"payloadType\": 100,\
                        \"clockRate\": 48000,\
                        \"channels\": 2,\
                        \"parameters\": {\
                            \"sprop-stereo\": 1\
                        }\
                    }\
                ],\
                \"encodings\": [\
                    {\
                        \"ssrc\": 1111\
                    }\
                ]\
            }\
        }";

    auto res = cli.Post(extension.c_str(), json_msg, "application/json");
    if (res->status != 200) {
        printf("Failed to create mediasoup audio producer");
        return FISH_EIO;
    }

    printf(">> Created audio producer\n");
    return FISH_EOK;
}

/* Create a mediasoup Producer to send video by sending 
 * our RTP parameters via a HTTP POST. */
fish_error_t createMediasoupProducerVideo(const char * server_url, const char * room_id, 
                                          std::string token, std::string broadcaster_id,
                                          std::string video_transport_id)
{
    std::string extension("/rooms/");
    extension += room_id;
    extension += "/broadcasters/";
    extension += broadcaster_id;
    extension += "/transports/";
    extension += video_transport_id;
    extension += "/producers";

    httplib::Client cli(server_url);
    cli.enable_server_certificate_verification(false);
    cli.set_bearer_token_auth(token.c_str());

    char json_msg[1024] = "\
        {\
            \"kind\": \"video\",\
            \"rtpParameters\": {\
                \"codecs\": [\
                    {\
                        \"mimeType\": \"video/h264\",\
                        \"payloadType\": 100,\
                        \"clockRate\": 90000,\
                        \"parameters\": {\
                            \"packetization-mode\":1,\
                            \"profile-level-id\": \"42e01f\",\
                            \"level-asymmetry-allowed\":1,\
                            \"x-google-start-bitrate\":1000\
                        }\
                    }\
                ],\
                \"encodings\": [\
                    {\
                        \"ssrc\": 2222\
                    }\
                ]\
            }\
        }";

    auto res = cli.Post(extension.c_str(), json_msg, "application/json");
    if (res->status != 200) {
        printf("Failed to create mediasoup audio producer");
        return FISH_EIO;
    }

    printf(">> Created video producer\n");
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

/*
gst-launch-1.0 \
    rtpbin name=rtpbin \
    filesrc location=${MEDIA_FILE} \
    ! qtdemux name=demux \
    demux.video_0 \
    ! queue \
    ! decodebin \
    ! videoconvert \
    ! vp8enc target-bitrate=1000000 deadline=1 cpu-used=4 \
    ! rtpvp8pay pt=${VIDEO_PT} ssrc=${VIDEO_SSRC} picture-id-mode=2 \
    ! rtpbin.send_rtp_sink_0 \
    rtpbin.send_rtp_src_0 ! udpsink host=${videoTransportIp} port=${videoTransportPort} \
    rtpbin.send_rtcp_src_0 ! udpsink host=${videoTransportIp} port=${videoTransportRtcpPort} sync=false async=false \
    
    demux.audio_0 \
    ! queue \
    ! decodebin \
    ! audioresample \
    ! audioconvert \
    ! opusenc \
    ! rtpopuspay pt=${AUDIO_PT} ssrc=${AUDIO_SSRC} \
    ! rtpbin.send_rtp_sink_1 \
    rtpbin.send_rtp_src_1 ! udpsink host=${audioTransportIp} port=${audioTransportPort} \
    rtpbin.send_rtcp_src_1 ! udpsink host=${audioTransportIp} port=${audioTransportRtcpPort} sync=false async=false

*/



/* Run gstreamer command to stream from the
 * a webcam. */
fish_error_t videoStreamFile(std::string video_transport_ip, std::string video_transport_port, std::string video_transport_rtcp_port)
{
    cv::VideoCapture cap("/mnt/c/Users/Andrew/Videos/InVision-Website-Home-Video-4-5-21.mp4");
    if (!cap.isOpened()) {
        std::cerr <<"VideoCapture not opened" << std::endl;
        return FISH_EINVAL;
    }

    int width = cap.get(cv::CAP_PROP_FRAME_WIDTH);
    int height = cap.get(cv::CAP_PROP_FRAME_HEIGHT);

    char gstcmd_buf[512];
    sprintf(gstcmd_buf, " \
            rtpbin name=rtpbin rtp-profile=avpf \
            appsrc \
            ! videoconvert \
            ! video/x-raw,format=I420,framerate=30/1 \
            ! x264enc tune=zerolatency speed-preset=1 dct8x8=true quantizer=23 pass=qual \
            ! rtph264pay pt=100 ssrc=2222 \
            ! rtpbin.send_rtp_sink_0 \
            rtpbin.send_rtp_src_0 ! udpsink host=%s port=%s \
            rtpbin.send_rtcp_src_0 ! udpsink host=%s port=%s sync=false async=false \
        ", video_transport_ip.c_str(), video_transport_port.c_str(), video_transport_ip.c_str(), video_transport_rtcp_port.c_str());

    cv::VideoWriter writer(
        gstcmd_buf, 
        0,      // fourcc 
        30,     // fps
        cv::Size(width, height), 
        true);  // isColor

    if (!writer.isOpened()) {
        std::cerr <<"VideoWriter not opened"<<std::endl;
        return FISH_EIO;
    }
    printf(">> Streaming video from file\n");
    
    bool incoming_frame;
    while (true) {
        cv::Mat frame;
        incoming_frame = cap.read(frame);
        if (!incoming_frame) {
            break;
        }
        writer.write(frame);
    }

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
    std::string broadcaster_id;
    std::string video_transport_id;
    std::string video_transport_ip;
    std::string video_transport_port;
    std::string video_transport_rtcp_port;
    std::string audio_transport_id;
    std::string audio_transport_ip;
    std::string audio_transport_port;
    std::string audio_transport_rtcp_port;

    const char* server_url = argv[1];
    const char* room_id = argv[2];
    const char* username = argv[3];
    const char* password = argv[4];

    err = checkRoom(server_url, room_id);
    if (err != FISH_EOK) {
        printf("Error: could not connect to ROOM_ID:%s\n", room_id);
        return EXIT_FAILURE;
    }

    err = login(server_url, username, password, token);
    if (err != FISH_EOK) {
        printf("Error: could not login with %s, %s\n", username, password);
        return EXIT_FAILURE;
    }

    err = createBroadcaster(server_url, room_id, token, broadcaster_id);
    if (err != FISH_EOK) {
        printf("Error: could not create broadcaster\n");
        err = cleanup(server_url, room_id, token, broadcaster_id);
        if (err != FISH_EOK) {
            printf("Failed to cleanup broadcaster\n");
        }
        return EXIT_FAILURE;
    }

    err = createPlainTransportAudio(server_url, room_id, token, broadcaster_id,
                                    audio_transport_id, audio_transport_ip,
                                    audio_transport_port, audio_transport_rtcp_port);
    if (err != FISH_EOK) {
        printf("Error: could not create PT audio\n");
        err = cleanup(server_url, room_id, token, broadcaster_id);
        if (err != FISH_EOK) {
            printf("Failed to cleanup broadcaster\n");
        }
        return EXIT_FAILURE;
    }

    err = createPlainTransportVideo(server_url, room_id, token, broadcaster_id,
                                    video_transport_id, video_transport_ip,
                                    video_transport_port, video_transport_rtcp_port);
    if (err != FISH_EOK) {
        printf("Error: could not create PT video\n");
        err = cleanup(server_url, room_id, token, broadcaster_id);
        if (err != FISH_EOK) {
            printf("Failed to cleanup broadcaster\n");
        }
        return EXIT_FAILURE;
    }

    err = createMediasoupProducerAudio(server_url, room_id, token, broadcaster_id,
                                       audio_transport_id);
    if (err != FISH_EOK) {
        printf("Error: could not create MS audio producer\n");
        err = cleanup(server_url, room_id, token, broadcaster_id);
        if (err != FISH_EOK) {
            printf("Failed to cleanup broadcaster\n");
        }
        return EXIT_FAILURE;
    }

    err = createMediasoupProducerVideo(server_url, room_id, token, broadcaster_id,
                                       video_transport_id);
    if (err != FISH_EOK) {
        printf("Error: could not create MS video producer\n");
        err = cleanup(server_url, room_id, token, broadcaster_id);
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
        err = cleanup(server_url, room_id, token, broadcaster_id);
        if (err != FISH_EOK) {
            printf("Failed to cleanup broadcaster\n");
        }
    }
#else
    err = videoStreamFile(video_transport_ip, video_transport_port, video_transport_rtcp_port);
    if (err != FISH_EOK) {
        printf("Error: could not create webcam gstream\n");
        err = cleanup(server_url, room_id, token, broadcaster_id);
        if (err != FISH_EOK) {
            printf("Failed to cleanup broadcaster\n");
        }
    }
#endif

    err = cleanup(server_url, room_id, token, broadcaster_id);
    if (err != FISH_EOK) {
        printf("Failed to cleanup broadcaster\n");
    }
    
    printf("Execution finished normally\n");
    return 0;
}