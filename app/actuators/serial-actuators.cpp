#include "../fishIO/fishIO.h"
#include "serial-actuators.hpp"

#if defined(__arch64__)
    #define JETSON_TARGET
#endif

void runActuatorService(fish_handle_t * handle) {
	fish_error_t err;

	#if defined	(JETSON_TARGET)
		const char * uart_target = "/dev/ttyTHS1";
	    serial_handle_t serial;
	    err = setupSerial(&serial, uart_target);
	    if (err != FISH_EOK){
	    	std::cout << "Failed to instantiate serial connection" << std::endl;
	        return;
	    }
	#endif

	while (1) {
		
		if (handle->next_speed != handle->curr_speed) {
			fish_handle_mtx.lock();

			#if defined	(JETSON_TARGET) 
				err = setCaudalFinSpeed(serial, handle->next_speed);
				if (err != FISH_EOK) {
					std::cout << "Failed to set speed" << std::endl;
				}
			#else
				std::cout << "speed: " << unsigned(handle->next_speed) << std::endl;
			#endif

			handle->curr_speed = handle->next_speed;
			fish_handle_mtx.unlock();
		}

		if (handle->next_right_angle != handle->curr_right_angle) {
			fish_handle_mtx.lock();
			#if defined	(JETSON_TARGET)
				err = moveServoSync(serial, handle->next_right_angle, 100, false);
				if (err != FISH_EOK) {
					std::cout << "Failed to set right servo angle" << std::endl;
				}
			#else
				std::cout << "right: " << unsigned(handle->next_right_angle) << std::endl;
			#endif
			handle->curr_right_angle = handle->next_right_angle;
			fish_handle_mtx.unlock();
		}
		
		if (handle->next_left_angle != handle->curr_left_angle) {
			fish_handle_mtx.lock();
			#if defined (JETSON_TARGET)
				err = moveServoSync(serial, handle->next_right_angle, 100, true);
				if (err != FISH_EOK) {
					std::cout << "Failed to set right servo angle" << std::endl;
				}
			#else
				std::cout << "left: " << unsigned(handle->next_left_angle) << std::endl;
			#endif
			handle->curr_left_angle = handle->next_left_angle;
			fish_handle_mtx.unlock();
		}
	}
}
