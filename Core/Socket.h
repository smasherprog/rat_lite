#pragma once
#include <memory>

#include <boost/asio.hpp>
#include<functional>

namespace SL {
	namespace Remote_Access_Library {
		namespace Network {
			struct SocketImpl;
			class Socket: public std::enable_shared_from_this<Socket> {
			public:
				Socket(boost::asio::io_service& io_service, 
					boost::asio::ip::tcp::socket& socket,
					std::function<void(const Socket*, const char*, size_t)>& onreceive,
					std::function<void(const Socket*)>& onclose
					);
				Socket(boost::asio::io_service& io_service, 
					tcp::resolver::iterator endpoint_iterator,
					std::function<void(const Socket*, const char*, size_t)>& onreceive,
					std::function<void(const Socket*)>& onclose
					);
			
				void send(const char* data, size_t num_bytes);
				void close();

	
			private:
				std::unique_ptr<SocketImpl> _SocketImpl;

			};
		}
	}
}