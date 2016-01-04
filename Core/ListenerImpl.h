#pragma once

#include <boost/asio.hpp>
#include "IO_Runner.h"


namespace SL {
	namespace Remote_Access_Library {
		namespace Network {
			class BaseNetworkDriver;
		}
		//this namespace should only be accessed by the Core library and NEVER by a consumer of this library
		namespace INTERNAL {
			class ListinerImpl: public Network::IO_Runner {
			public:
				ListinerImpl(const boost::asio::ip::tcp::endpoint& endpoint, Network::BaseNetworkDriver* netevents) :
					NetworkEvents_(netevents),
					acceptor_(io_service, endpoint),
					socket_(io_service) {}
				~ListinerImpl() {
					socket_.close();
					acceptor_.close();
				}
		
				boost::asio::ip::tcp::acceptor acceptor_;
				boost::asio::ip::tcp::socket socket_;
				Network::BaseNetworkDriver* NetworkEvents_;
		};
	

		}
	}
}



