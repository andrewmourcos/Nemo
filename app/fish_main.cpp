/*
    Author: AndrewMourcos
    Date: Aug 24 2021
    Not for commercial use.
*/

#include "streaming/gst-streamer.hpp"
#include "actuators/serial-actuators.hpp"
#include "socks/boost-sock.hpp"
#include "common/fish_types.h"
#include <gst/gst.h>
#include "fishStream/fishGST.hpp" // TODO: remove dep

#include <thread>
#include <iostream>
#include <signal.h>

std::mutex fish_handle_mtx;
fish_handle_t handle = {90, 90, 90, 90, 0, 0};

void sigint_handler(int s) {
	cleanupBroadcaster(handle.server_url, handle.room_id, handle.token, handle.broadcaster_id);
	exit(0);
}

int main(int argc, char *argv[])
{
	signal (SIGINT, sigint_handler);

	if (argc != 7) {
        std::cout << "Usage: ./nemo <server url> <room id> <username> <password> <host> <port>\n";
        std::cout << "Example:\n";
        std::cout << "  ./nemo https://192.168.0.142:4443 FISH username@gmail.com password123 192.168.0.142 4443\n";
        return EXIT_FAILURE; 
    }

	const char* server_url = argv[1];
    const char* room_id = argv[2];
    const char* username = argv[3];
    const char* password = argv[4];
    const char* host = argv[5];
    const char* port = argv[6];

    handle.server_url = server_url;
    handle.room_id = room_id;
    handle.username = username;
    handle.password = password;
    handle.host = host;
    handle.port = port;
	
	gst_init(&argc, &argv);

	// Spawn 3 main services
	std::thread video_thread(runVideoService, &handle);
	std::thread websocket_thread(runWebsocketService, &handle);
	std::thread motor_controller_thread(runActuatorService, &handle);

	video_thread.join();
	websocket_thread.join();
	motor_controller_thread.join();

	return 0;
}