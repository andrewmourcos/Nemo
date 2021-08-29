/*
    Author: AndrewMourcos
    Date: Aug 24 2021
    License: Not for commercial use outside LamperLabs
*/

#include "actuators/serial-actuators.hpp"
#include "socks/boost-sock.hpp"
#include "common/fish_types.h"

#include <thread>
#include <iostream> // cout

std::mutex fish_handle_mtx;

int main(int argc, char const *argv[])
{
	fish_handle_t handle = {90, 90, 90, 90, 0, 0};

	// Spawn camera thread

	// Spawn websocket listener
	std::thread websocket_thread(runWebsocketService, &handle);

	// Spawn motor controller
	std::thread motor_controller_thread(runActuatorService, &handle);

	// Warning: There is no guarantee on the join order, so it may never end
	websocket_thread.join();
	motor_controller_thread.join();

	return 0;
}