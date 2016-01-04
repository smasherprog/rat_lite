#include "stdafx.h"
#include "IO_Runner.h"

SL::Remote_Access_Library::Network::IO_Runner::IO_Runner() :work(io_service) {
	io_servicethread = std::thread([this]() {
		std::cout << "Starting io_service for Connecto Factory" << std::endl;
		io_service.run();
		std::cout << "stopping io_service for Connecto Factory" << std::endl;
	});
}

SL::Remote_Access_Library::Network::IO_Runner::~IO_Runner()
{
	io_service.stop();
	io_servicethread.join();
}
