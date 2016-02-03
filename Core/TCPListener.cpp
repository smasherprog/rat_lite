#include "stdafx.h"
#include "TCPListener.h"
#include <boost/asio.hpp>
#include <functional>

namespace SL {
	namespace Remote_Access_Library {
		namespace Network {

			//this namespace should only be accessed by the Core library and NEVER by a consumer of this library
			namespace INTERNAL {
				class ListinerImpl {
				public:
					ListinerImpl(const boost::asio::ip::tcp::endpoint& endpoint, boost::asio::io_service& io_service, std::function<void(void*)>& func) :
						onaccept(func),
						acceptor_(io_service, endpoint),
						socket_(io_service) {}
					~ListinerImpl() {
						socket_.close();
						acceptor_.close();
					}
					boost::asio::ip::tcp::acceptor acceptor_;
					boost::asio::ip::tcp::socket socket_;
					std::function<void(void*)>& onaccept;
				};
				class Listiner_DataImpl {
				public:
					Listiner_DataImpl(unsigned short port, boost::asio::io_service& ioserv, std::function<void(void*)> onac) : _ListenPort(port), _io_service(ioserv), onaccept(onac) { }
					unsigned short _ListenPort;
					boost::asio::io_service& _io_service;
					std::function<void(void*)> onaccept;
				};
			}
		}
	}
}



void do_accept(std::shared_ptr<SL::Remote_Access_Library::Network::TCPListener> ptr, SL::Remote_Access_Library::Network::INTERNAL::ListinerImpl* impl) {

	impl->acceptor_.async_accept(impl->socket_, [ptr, impl](boost::system::error_code ec)
	{
		if (!ec)
		{
			impl->onaccept((void*)&(impl->socket_));
		}
		do_accept(ptr, impl);
	});
}

std::shared_ptr<SL::Remote_Access_Library::Network::TCPListener> SL::Remote_Access_Library::Network::TCPListener::Create(unsigned short port, boost::asio::io_service& io_service, std::function<void(void*)> onaccept)
{
	return std::make_shared<TCPListener>(new INTERNAL::Listiner_DataImpl(port, io_service, onaccept));
}
SL::Remote_Access_Library::Network::TCPListener::TCPListener(INTERNAL::Listiner_DataImpl* data) : _Listiner_DataImpl(data), _ListinerImpl(nullptr)
{

}
SL::Remote_Access_Library::Network::TCPListener::~TCPListener() {
	delete _Listiner_DataImpl;
	Stop();
}
void SL::Remote_Access_Library::Network::TCPListener::Start() {
	if (!_ListinerImpl) _ListinerImpl = new INTERNAL::ListinerImpl(boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), _Listiner_DataImpl->_ListenPort), _Listiner_DataImpl->_io_service, _Listiner_DataImpl->onaccept);
	do_accept(shared_from_this(), _ListinerImpl);
}

void SL::Remote_Access_Library::Network::TCPListener::Stop()
{
	delete _ListinerImpl;
}


