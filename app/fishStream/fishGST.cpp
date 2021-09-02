/*
    Author: AndrewMourcos
    Date: Sep 1 2021
    License: Not for commercial use outside LamperLabs
*/

#include <cstdlib>
#include <string>
#include <iostream>
#include <errno.h>

#include "jsoncpp/json/json.h"
#include <opencv2/opencv.hpp>
#include <gst/gst.h>

// Note: macro needs to be defined before including httplib
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/random_generator.hpp>

#include "../common/fish_types.h"

#if defined(__aarch64__)
    #define JETSON_TARGET
#endif

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
fish_error_t cleanupBroadcaster(const char * server_url, const char * room_id, std::string token, std::string broadcaster_id)
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

#if defined(JETSON_TARGET)
/* Run gstreamer command to stream from the
 * CSI2 camera. Will not run on non-Jetson hardware. */
fish_error_t createCSI2Stream(std::string video_transport_ip, std::string video_transport_port, std::string video_transport_rtcp_port)
{
    GstElement *pipeline;
    GstBus *bus;
    GstMessage *msg;

    char gstcmd_buf[1024];

    // Works with delay:
    sprintf(gstcmd_buf, "rtpbin name=rtpbin rtp-profile=avpf \
                         nvarguscamerasrc ! video/x-raw(memory:NVMM), \
                         format=NV12, width=852, height=480 \
                         ! nvv4l2h264enc insert-sps-pps=true ! h264parse \
                         ! rtph264pay ssrc=2222 pt=100 \
                         ! rtprtxqueue max-size-time=2000 max-size-packets=0 \
                         ! rtpbin.send_rtp_sink_0 \
                         rtpbin.send_rtp_src_0 ! udpsink  host=%s port=%s \
                         rtpbin.send_rtcp_src_0 ! udpsink  host=%s port=%s sync=false async=false \
                    ", video_transport_ip.c_str(), video_transport_port.c_str(), 
                       video_transport_ip.c_str(), video_transport_rtcp_port.c_str());

    /* Build the pipeline */
    pipeline = gst_parse_launch(gstcmd_buf, NULL);

    /* Start playing */
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    /* Wait until error or EOS */
    bus = gst_element_get_bus(pipeline);
    msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE,
      (GstMessageType) (GST_MESSAGE_ERROR | GST_MESSAGE_EOS));

    /* Free resources */
    if (msg != NULL) {
        gst_message_unref (msg);
    }

    gst_object_unref (bus);
    gst_element_set_state (pipeline, GST_STATE_NULL);
    gst_object_unref (pipeline);

    return FISH_EOK;
}

fish_error_t createCSI2ProcessedStream(std::string video_transport_ip, std::string video_transport_port, std::string video_transport_rtcp_port)
{
    cv::VideoCapture cap("nvarguscamerasrc ! video/x-raw(memory:NVMM), width=(int)1280, height=(int)720, format=(string)NV12, framerate=(fraction)120/1 \
                        ! nvvidconv ! video/x-raw,format=(string)BGRx \
                        ! videoconvert ! video/x-raw, format=(string)BGR ! appsink");

    if (!cap.isOpened()) {
        std::cerr <<"VideoCapture not opened" << std::endl;
        return FISH_EINVAL;
    }

    int width = cap.get(cv::CAP_PROP_FRAME_WIDTH);
    int height = cap.get(cv::CAP_PROP_FRAME_HEIGHT);

    char gstcmd_buf[1024];

    // Works with delay:
    sprintf(gstcmd_buf, "rtpbin name=rtpbin rtp-profile=avpf \
                         appsrc \
                         ! videoconvert \
                         ! video/x-raw,format=I420,framerate=120/1 \
                         ! x264enc tune=zerolatency speed-preset=1 dct8x8=true quantizer=23 pass=qual \
                         ! rtph264pay pt=100 ssrc=2222 \
                         ! rtpbin.send_rtp_sink_0 \
                         rtpbin.send_rtp_src_0 ! udpsink host=%s port=%s \
                         rtpbin.send_rtcp_src_0 ! udpsink host=%s port=%s sync=false async=false \
                    ", video_transport_ip.c_str(), video_transport_port.c_str(), 
                       video_transport_ip.c_str(), video_transport_rtcp_port.c_str());

    cv::VideoWriter writer( gstcmd_buf, 
                            0,      // fourcc 
                            120,     // fps
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
        /* Process image here if desired */
        writer.write(frame);
    }

    return FISH_EOK;

}
#endif


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

    char gstcmd_buf[600];
    sprintf(gstcmd_buf, "rtpbin name=rtpbin rtp-profile=avpf \
                         appsrc \
                         ! videoconvert \
                         ! video/x-raw,format=I420,framerate=30/1 \
                         ! x264enc tune=zerolatency speed-preset=1 dct8x8=true quantizer=23 pass=qual \
                         ! rtph264pay pt=100 ssrc=2222 \
                         ! rtpbin.send_rtp_sink_0 \
                         rtpbin.send_rtp_src_0 ! udpsink host=%s port=%s \
                         rtpbin.send_rtcp_src_0 ! udpsink host=%s port=%s sync=false async=false \
                    ", video_transport_ip.c_str(), video_transport_port.c_str(), 
                       video_transport_ip.c_str(), video_transport_rtcp_port.c_str());

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
