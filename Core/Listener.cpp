#include "stdafx.h"
#include "Packet.h"
#include "Socket.h"
#include "Listener.h"
#include "BaseNetworkDriver.h"
#include "ListenerImpl.h"
#include "SocketImpl.h"


void do_accept(std::shared_ptr<SL::Remote_Access_Library::Network::Listener> ptr, SL::Remote_Access_Library::INTERNAL::ListinerImpl* impl) {
	impl->acceptor_.async_accept(impl->socket_, [ptr, impl](boost::system::error_code ec)
	{
		if (!ec)
		{
			auto sock = std::make_shared<SL::Remote_Access_Library::Network::Socket>(std::make_shared<SL::Remote_Access_Library::INTERNAL::SocketImpl>(impl->io_service, impl->NetworkEvents_, std::move(impl->socket_)));
			sock->connect(nullptr, nullptr);
		}
		do_accept(ptr, impl);
	});
}

SL::Remote_Access_Library::Network::Listener::Listener(INTERNAL::ListinerImpl* impl) : _ListinerImpl(impl)
{

}
SL::Remote_Access_Library::Network::Listener::~Listener() {
	delete _ListinerImpl;
}
void SL::Remote_Access_Library::Network::Listener::Start() {
	do_accept(shared_from_this(), _ListinerImpl);
}


std::shared_ptr<SL::Remote_Access_Library::Network::Listener> SL::Remote_Access_Library::Network::Listener::CreateListener(unsigned short port, Network::BaseNetworkDriver* netevents) {
	return std::make_shared<SL::Remote_Access_Library::Network::Listener>(new SL::Remote_Access_Library::INTERNAL::ListinerImpl(boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port), netevents));
}
