#include "common.h"
#include <iostream>
#include <stdio.h>

int main(int argc, char const *argv[])
{
	// Spawn threads (camera processing, )
	fish_err_t err = fish_startup();
	if (err != FISH_EOK) {
		FISH_LOG("Startup code failed");
		return EXIT_FAILURE;
	}

	printf("Process exited\n");
	return EXIT_SUCCESS;
}

