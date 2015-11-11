#pragma once
#include "BufferManager.h"
#include <boost/asio.hpp>
#include <thread>
#include "ThreadPool.h"

namespace SL {
	namespace Remote_Access_Library {
		namespace Utilities {
			class ThreadPool;
		}
	
		//this namespace should only be accessed by the Core library and NEVER by a consumer of this library
		namespace INTERNAL {
			extern Utilities::BufferManager _Buffer;

			class io_runner {
			public:
				boost::asio::io_service io_service;
				Utilities::ThreadPool ThreadPool;

				io_runner(int numofthreads=2);
				~io_runner();
			private:
				std::thread io_servicethread;
				boost::asio::io_service::work work;
			
			};
	
		}
	}
}