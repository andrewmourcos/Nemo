/*
 * Andrew Mourcos
 * Notes: I don't know how the web stuff works yet, this is basically a mirror of https://github.com/boostorg/asio/blob/develop/example/cpp03/http/client/async_client.cpp
 *		  which is an example of making get/post requests I guess.
*/

#include <cstdlib>
#include <string>
#include <iostream>

#include "Client.hpp"

using boost::asio::ip::tcp;

int main(int argc, char const *argv[])
{
	try
	{
		if (argc != 3)
		{
			std::cout << "Usage: gstreamer_test <server> <path>\n";
			std::cout << "Example:\n";
			std::cout << "  gstreamer_test www.boost.org /LICENSE_1_0.txt\n";
			return 1;
		}

		boost::asio::io_context io_context;
		client c(io_context, argv[1], argv[2]);
		io_context.run();
	}
	catch (std::exception& e)
	{
		std::cout << "Exception: " << e.what() << "\n";
	}

  return 0;
}