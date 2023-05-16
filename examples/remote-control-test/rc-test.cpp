/*
	Disclaimer, this may only work on linux. It also uses system calls to
	get key presses from the terminal which is sketch, so take care.
*/

#include "common/fish_err.h" // For the fish error types
#include "fishIO/fishIO.h"	 // For the motor functions
#include <iostream>

char getKeyPress()
{
	char c;
	system("stty raw");
	c = getchar();
	system("stty cooked");
	system("clear");
	return c;
}

int main()
{
	char c;
	fish_error_t err;

	// Connect to UART device
	const char *uart_target = "/dev/ttyTHS1";
	serial_handle_t serial;
	if (setupSerial(&serial, uart_target) != FISH_EOK)
	{
		return EXIT_FAILURE;
	}

	while (1)
	{
		c = getKeyPress();

		switch (c)
		{
		case 'w':
			// Move forward
			std::cout << c << " was pressed. Starting" << std::endl;
			err = setCaudalFinSpeed(serial, 50);
			break;
		case 's':
			// stop
			std::cout << c << " was pressed. Stopping" << std::endl;
			err = setCaudalFinSpeed(serial, 0);
			break;
		case 'D':
			// move left
			std::cout << c << " was pressed. Turning left" << std::endl;
			err = moveServoSync(serial, 90, 100, true);
			err = moveServoSync(serial, 60, 100, false);
			break;
		case 'C':
			// move right
			std::cout << c << " was pressed. Turning right" << std::endl;
			err = moveServoSync(serial, 60, 100, true);
			err = moveServoSync(serial, 90, 100, false);
			break;
		case 'A':
			// servos straight
			std::cout << c << " was pressed. Going straight" << std::endl;
			err = moveServoSync(serial, 90, 100, true);
			err = moveServoSync(serial, 90, 100, false);
			break;
		case '.':
			system("stty cooked");
			exit(0);
			break;
		default:
			std::cout << c << " was pressed. No effect" << std::endl;
			break;
		}

		std::cout << "Press period (.) to end program" << std::endl;
	}
}