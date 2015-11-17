#include "stdafx.h"
#include "Packet.h"
#include "Socket.h"
#include "Listener.h"
#include <boost/asio.hpp>
#include "Internal_Impls.h"


////////////////////////SERVER HERE

struct SL::Remote_Access_Library::Network::ListinerImpl : SL::Remote_Access_Library::INTERNAL::io_runner {
	ListinerImpl(const boost::asio::ip::tcp::endpoint& endpoint, NetworkEvents& netevents) :
		NetworkEvents_(netevents),
		acceptor_(io_service, endpoint),
		socket_(io_service) {}

	boost::asio::ip::tcp::acceptor acceptor_;
	boost::asio::ip::tcp::socket socket_;
	NetworkEvents NetworkEvents_;
};

void do_accept(std::shared_ptr<SL::Remote_Access_Library::Network::Listener> ptr, SL::Remote_Access_Library::Network::ListinerImpl* impl) {
	impl->acceptor_.async_accept(impl->socket_, [ptr, impl](boost::system::error_code ec)
	{
		if (!ec)
		{
			auto socketimpl = SL::Remote_Access_Library::INTERNAL::Create(impl->io_service, impl->NetworkEvents_, impl->ThreadPool, std::move(impl->socket_));
			auto sock = std::make_shared<SL::Remote_Access_Library::Network::Socket>(socketimpl);
			sock->connect(nullptr, nullptr);
		}
		do_accept(ptr, impl);
	});
}

SL::Remote_Access_Library::Network::Listener::Listener(ListinerImpl* impl) : _ListinerImpl(impl)
{

}
SL::Remote_Access_Library::Network::Listener::~Listener() {
	Stop();
	delete _ListinerImpl;
}
void SL::Remote_Access_Library::Network::Listener::Start() {
	do_accept(shared_from_this(), _ListinerImpl);
}
void SL::Remote_Access_Library::Network::Listener::Stop() {
	_ListinerImpl->socket_.close();
	_ListinerImpl->acceptor_.close();
	_ListinerImpl->io_service.stop();
}

std::shared_ptr<SL::Remote_Access_Library::Network::Listener> SL::Remote_Access_Library::Network::Listener::CreateListener(unsigned short port, NetworkEvents& netevents) {
	return std::make_shared<SL::Remote_Access_Library::Network::Listener>(new SL::Remote_Access_Library::Network::ListinerImpl(boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port), netevents));
}
