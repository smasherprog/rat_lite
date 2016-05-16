#include "stdafx.h"
#include "IO_Runner.h"
#include <boost/asio.hpp>
#include <thread>
#include "Logging.h"

namespace SL {
	namespace Remote_Access_Library {
		namespace Network {
			class IO_RunnerImpl {
			public:
				boost::asio::io_service io_service;
				IO_RunnerImpl() :work(io_service) { 
					io_servicethread = std::thread([this]() {
						SL_RAT_LOG(Utilities::Logging_Levels::INFO_log_level, "Starting io_service Factory");
						boost::system::error_code ec;
						io_service.run(ec);
						if (ec) {
							SL_RAT_LOG(Utilities::Logging_Levels::ERROR_log_level,ec.message());
						}
						SL_RAT_LOG(Utilities::Logging_Levels::INFO_log_level, "stopping io_service Factory" );
					});
				}
				~IO_RunnerImpl()
				{
					Stop();
				}
				void Stop() {
					io_service.stop();
					if(io_servicethread.joinable()) io_servicethread.join();
				}
				std::thread io_servicethread;
				boost::asio::io_service::work work;
			};
		}
	}
}

SL::Remote_Access_Library::Network::IO_Runner::IO_Runner() {
	Start();
}


SL::Remote_Access_Library::Network::IO_Runner::~IO_Runner()
{
	Stop();
}

void SL::Remote_Access_Library::Network::IO_Runner::Start()
{
	if (_IO_RunnerImpl) _IO_RunnerImpl->Stop();
	_IO_RunnerImpl = std::make_unique<IO_RunnerImpl>();
}

void SL::Remote_Access_Library::Network::IO_Runner::Stop()
{
	if (_IO_RunnerImpl) _IO_RunnerImpl->Stop();
	_IO_RunnerImpl.reset();
}

boost::asio::io_service& SL::Remote_Access_Library::Network::IO_Runner::get_io_service()
{
	return _IO_RunnerImpl->io_service;
}


