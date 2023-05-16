#ifndef __FISHGST_HPP__
#define __FISHGST_HPP__

#include "../common/fish_types.h"

#if defined(__aarch64__)
    #define JETSON_TARGET
#endif

/* Checks if mediasoup room exists by sending a simple GET
 * request and checking for 200. Returns errno::EOK if succesful. */
fish_error_t checkRoom(const char *server_url, const char *room_id);

/* Logs into webapp using provided credentials. Sets token for
 * future calls. Returns FISH_EOK if succesful */
fish_error_t login(const char* server_url, const char* username, const char* password, std::string &token);

/* Creates broadcaster by sending POST with our metadata. 
 * Returns errno::EOK if succesful */
fish_error_t createBroadcaster(const char* server_url, const char* room_id, std::string token, std::string &broadcaster_id);

/* Sends HTTP DELETE to remove broadcaster when script 
 * terminates. Returns errno::EOK if succesful. */
fish_error_t cleanupBroadcaster(const char * server_url, const char * room_id, std::string token, std::string broadcaster_id);

/* Send POST to setup RTP over UDP for audio. Parse 
 * JSON response and place in buffer passed by ref.
 * Returns errno::EOK if succesful */
fish_error_t createPlainTransportAudio(const char * server_url, const char * room_id, 
                                        std::string token, std::string broadcaster_id,
                                        std::string &audio_transport_id, 
                                        std::string &audio_transport_ip,
                                        std::string &audio_transport_port,
                                        std::string &audio_transport_rtcp_port);

/* Send POST to setup RTP over UDP for video. Parse 
 * JSON response and place in buffer passed by ref.
 * Returns errno::EOK if succesful */
fish_error_t createPlainTransportVideo(const char * server_url, const char * room_id, 
                                        std::string token, std::string broadcaster_id,
                                        std::string &video_transport_id, 
                                        std::string &video_transport_ip,
                                        std::string &video_transport_port,
                                        std::string &video_transport_rtcp_port);

/* Create a mediasoup Producer to send audio by sending 
 * our RTP parameters via a HTTP POST. */
fish_error_t createMediasoupProducerAudio(const char * server_url, const char * room_id, 
                                          std::string token, std::string broadcaster_id,
                                          std::string audio_transport_id);

/* Create a mediasoup Producer to send video by sending 
 * our RTP parameters via a HTTP POST. */
fish_error_t createMediasoupProducerVideo(const char * server_url, const char * room_id, 
                                          std::string token, std::string broadcaster_id,
                                          std::string video_transport_id);

#if defined(JETSON_TARGET)
/* Run gstreamer command to stream from the
 * CSI2 camera. Will not run on non-Jetson hardware. */
fish_error_t createCSI2Stream(std::string video_transport_ip, 
                              std::string video_transport_port, 
                              std::string video_transport_rtcp_port);

/* Uses OpenCV to allow for processing. Worse performance due to memory copying to CPU
*/
fish_error_t createCSI2ProcessedStream(std::string video_transport_ip, 
                                       std::string video_transport_port, 
                                       std::string video_transport_rtcp_port);
#endif

/* Run gstreamer command to stream from the
 * a file. */
fish_error_t videoStreamFile(std::string video_transport_ip, std::string video_transport_port, std::string video_transport_rtcp_port, std::string file_name);

#endif /* __FISHGST_HPP__ */