#include "stdafx.h"
#include "Internal_Impls.h"
#include <iostream>
#include "Socket.h"

SL::Remote_Access_Library::Utilities::BufferManager SL::Remote_Access_Library::INTERNAL::_PacketBuffer;
SL::Remote_Access_Library::Utilities::BufferManager SL::Remote_Access_Library::INTERNAL::_ImageBuffer;
SL::Remote_Access_Library::INTERNAL::io_runner::io_runner(int numofthreads)
	:work(io_service), ThreadPool(numofthreads) {
	io_servicethread = std::thread([this]() {
		std::cout << "Starting io_service for Connecto Factory" << std::endl;
		io_service.run();
		std::cout << "stopping io_service for Connecto Factory" << std::endl;
	});
}

SL::Remote_Access_Library::INTERNAL::io_runner::~io_runner()
{
	io_service.stop();
	io_servicethread.detach();//it will stop eventually, but we dont need to wait for that...
}
