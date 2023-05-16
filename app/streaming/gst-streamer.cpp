#include "gst-streamer.hpp"
#include "../fishStream/fishGST.hpp"

// TODO: remove
#if defined(__aarch64__)
	#define JETSON_TARGET
#endif 

void runVideoService(fish_handle_t * handle) {
	fish_error_t err;
    std::string video_transport_id;
    std::string video_transport_ip;
    std::string video_transport_port;
    std::string video_transport_rtcp_port;
    std::string audio_transport_id;
    std::string audio_transport_ip;
    std::string audio_transport_port;
    std::string audio_transport_rtcp_port;

    const char* server_url = handle->server_url;
    const char* room_id = handle->room_id;
    const char* username = handle->username;
    const char* password = handle->password;

    err = checkRoom(server_url, room_id);
    if (err != FISH_EOK) {
        printf("Error: could not connect to ROOM_ID:%s\n", room_id);
        return ;
    }

    err = login(server_url, username, password, handle->token);
    if (err != FISH_EOK) {
        printf("Error: could not login with %s, %s\n", username, password);
        return ;
    }

    err = createBroadcaster(server_url, room_id, handle->token, handle->broadcaster_id);
    if (err != FISH_EOK) {
        printf("Error: could not create broadcaster\n");
        err = cleanupBroadcaster(server_url, room_id, handle->token, handle->broadcaster_id);
        if (err != FISH_EOK) {
            printf("Failed to cleanup broadcaster\n");
        }
        return ;
    }

    err = createPlainTransportAudio(server_url, room_id, handle->token, handle->broadcaster_id,
                                    audio_transport_id, audio_transport_ip,
                                    audio_transport_port, audio_transport_rtcp_port);
    if (err != FISH_EOK) {
        printf("Error: could not create PT audio\n");
        err = cleanupBroadcaster(server_url, room_id, handle->token, handle->broadcaster_id);
        if (err != FISH_EOK) {
            printf("Failed to cleanup broadcaster\n");
        }
        return ;
    }

    err = createPlainTransportVideo(server_url, room_id, handle->token, handle->broadcaster_id,
                                    video_transport_id, video_transport_ip,
                                    video_transport_port, video_transport_rtcp_port);
    if (err != FISH_EOK) {
        printf("Error: could not create PT video\n");
        err = cleanupBroadcaster(server_url, room_id, handle->token, handle->broadcaster_id);
        if (err != FISH_EOK) {
            printf("Failed to cleanup broadcaster\n");
        }
        return ;
    }

    err = createMediasoupProducerAudio(server_url, room_id, handle->token, handle->broadcaster_id,
                                       audio_transport_id);
    if (err != FISH_EOK) {
        printf("Error: could not create MS audio producer\n");
        err = cleanupBroadcaster(server_url, room_id, handle->token, handle->broadcaster_id);
        if (err != FISH_EOK) {
            printf("Failed to cleanup broadcaster\n");
        }
        return ;
    }

    err = createMediasoupProducerVideo(server_url, room_id, handle->token, handle->broadcaster_id,
                                       video_transport_id);
    if (err != FISH_EOK) {
        printf("Error: could not create MS video producer\n");
        err = cleanupBroadcaster(server_url, room_id, handle->token, handle->broadcaster_id);
        if (err != FISH_EOK) {
            printf("Failed to cleanup broadcaster\n");
        }
        return ;
    }

// Only use csi2 function if running on Jetson, otherwise use regular webcam
#if defined(JETSON_TARGET)

    err = createCSI2Stream(video_transport_ip, video_transport_port, video_transport_rtcp_port);
    if (err != FISH_EOK) {
        printf("Error: could not create CSI2 gstream\n");
        err = cleanupBroadcaster(server_url, room_id, handle->token, handle->broadcaster_id);
        if (err != FISH_EOK) {
            printf("Failed to cleanup broadcaster\n");
        }
    }

#else
    // TODO: remove
    // This is just used for testing purposes
    err = videoStreamFile(video_transport_ip, video_transport_port, video_transport_rtcp_port, "/media/test.mp4");
    if (err != FISH_EOK) {
        printf("Error: could not create webcam gstream\n");
        err = cleanupBroadcaster(server_url, room_id, handle->token, handle->broadcaster_id);
        if (err != FISH_EOK) {
            printf("Failed to cleanup broadcaster\n");
        }
    }
#endif

    err = cleanupBroadcaster(server_url, room_id, handle->token, handle->broadcaster_id);
    if (err != FISH_EOK) {
        printf("Failed to cleanup broadcaster\n");
    }
}