#pragma once
#include <boost/asio.hpp>

namespace SL {
	namespace Remote_Access_Library {
		//this namespace should only be accessed by the Core library and NEVER by a consumer of this library
		namespace Network {

			class IO_Runner {

			public:
				boost::asio::io_service io_service;

				IO_Runner();
				~IO_Runner();
			private:
				std::thread io_servicethread;
				boost::asio::io_service::work work;

			};
		}
	}
}