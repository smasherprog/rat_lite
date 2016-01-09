#pragma once
#include <boost/asio.hpp>

namespace SL {
	namespace Remote_Access_Library {
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