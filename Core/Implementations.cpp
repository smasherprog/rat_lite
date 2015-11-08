#include "stdafx.h"
#include <deque>
#include <thread>
#include "Packet.h"
#include "Socket.h"
#include "Listener.h"
#include <boost/asio.hpp>
#include <mutex>
#include "CommonNetwork.h"
#include <atomic>
#include <iostream>
#include "zstd.h"

void SL::Remote_Access_Library::Network::DefaultOnConnect(const std::shared_ptr<SL::Remote_Access_Library::Network::Socket> ptr) { 
	std::cout << "DEFAULT OnConnected Called!" << std::endl; 
};
void SL::Remote_Access_Library::Network::DefaultOnReceive(const std::shared_ptr<SL::Remote_Access_Library::Network::Socket> ptr, std::shared_ptr<SL::Remote_Access_Library::Network::Packet>& pac) { 
	std::cout << "DEFAULT OnReceive Called! PacketType: " <<(int)pac->header()->Packet_Type<<" Size: "<< pac->header()->PayloadLen<< std::endl;
};
void SL::Remote_Access_Library::Network::DefaultOnClose(const std::shared_ptr<SL::Remote_Access_Library::Network::Socket> ptr) { 
	std::cout << "DEFAULT OnClose Called!" << std::endl; 
};

struct SL::Remote_Access_Library::Network::SocketImpl {
	SocketImpl(SocketImpl&& impl) : io_service(std::move(impl.io_service)), socket(std::move(impl.socket)), NetworkEvents_(std::move(impl.NetworkEvents_)) {}
	SocketImpl(boost::asio::io_service& io, boost::asio::ip::tcp::socket& s, NetworkEvents& netevents) : io_service(io), socket(std::move(s)), NetworkEvents_(netevents) {}
	SocketImpl(boost::asio::io_service& io, NetworkEvents& netevents) : io_service(io), socket(io), NetworkEvents_(netevents) {}

	boost::asio::io_service& io_service;
	boost::asio::ip::tcp::socket socket;

	NetworkEvents NetworkEvents_;
	PacketHeader PacketHeader;
	std::deque<	std::shared_ptr<Packet>> OutgoingPackets;
	std::shared_ptr<Packet> IncomingPacket;


};

class io_runner {
public:
	boost::asio::io_service io_service;

	io_runner():work(io_service) {
		io_servicethread = std::thread([this]() {
			std::cout << "Starting io_service for Connecto Factory" << std::endl;
			io_service.run();
			std::cout << "stopping io_service for Connecto Factory" << std::endl;
		});
	}
	~io_runner() {
		io_service.stop();
		io_servicethread.detach();//it will stop eventually, but we dont need to wait for that...
	}
private:
	std::thread io_servicethread;
	boost::asio::io_service::work work;
};
void do_connect(std::shared_ptr<SL::Remote_Access_Library::Network::Socket> ptr, SL::Remote_Access_Library::Network::SocketImpl* impl, boost::asio::ip::tcp::resolver::iterator endpoint_iterator);
void do_read_header(std::shared_ptr<SL::Remote_Access_Library::Network::Socket> ptr, SL::Remote_Access_Library::Network::SocketImpl* impl);
void do_write(std::shared_ptr<SL::Remote_Access_Library::Network::Socket> ptr, SL::Remote_Access_Library::Network::SocketImpl* impl);
void do_write_header(std::shared_ptr<SL::Remote_Access_Library::Network::Socket> ptr, SL::Remote_Access_Library::Network::SocketImpl* impl);
void do_read_body(std::shared_ptr<SL::Remote_Access_Library::Network::Socket> ptr, SL::Remote_Access_Library::Network::SocketImpl* impl);

void compress_payload(std::shared_ptr<SL::Remote_Access_Library::Network::Packet>& pack)
{
	if (pack->header()->UnCompressedlen > 0) return;//allready compressed
	pack->header()->UnCompressedlen = pack->header()->PayloadLen;
	pack->header()->PayloadLen = static_cast<unsigned int>(ZSTD_compress(pack->data(), ZSTD_compressBound(pack->header()->UnCompressedlen), pack->data(), pack->header()->PayloadLen));
}

SL::Remote_Access_Library::Network::Socket::Socket(SocketImpl* impl) : _SocketImpl(impl) {
	std::cout << "Socket()" << std::endl;
}
SL::Remote_Access_Library::Network::Socket::~Socket() {
	std::cout << "~Socket()" << std::endl;
	delete _SocketImpl;
}

std::shared_ptr<SL::Remote_Access_Library::Network::Socket> SL::Remote_Access_Library::Network::Socket::ConnectTo(const char* host, const char* port, NetworkEvents& netevents) {
	//first thread in will initialize this and start the io_service
	static io_runner _io_runner;

	boost::asio::ip::tcp::resolver resolver(_io_runner.io_service);
	auto socketimpl = new SocketImpl(_io_runner.io_service, netevents);
	auto socket = std::make_shared<SL::Remote_Access_Library::Network::Socket>(socketimpl);

	do_connect(socket, socketimpl, resolver.resolve({ host, port }));
	return socket;
}

void SL::Remote_Access_Library::Network::Socket::send(std::shared_ptr<Packet>& pack)
{
	auto self(shared_from_this());
	compress_payload(pack);
	std::cout << "Sending " << pack->header()->PayloadLen << " bytes" << std::endl;
	_SocketImpl->io_service.post([this, pack, self]()
	{
		bool write_in_progress = !_SocketImpl->OutgoingPackets.empty();
		_SocketImpl->OutgoingPackets.push_back(pack);
		if (!write_in_progress)
		{
			do_write_header(self, _SocketImpl);
		}
	});
}

void SL::Remote_Access_Library::Network::Socket::close()
{
	auto self(shared_from_this());
	_SocketImpl->NetworkEvents_.OnClose(self);
	_SocketImpl->io_service.post([self, this]() {
		_SocketImpl->socket.close();
	});
}


void do_connect(std::shared_ptr<SL::Remote_Access_Library::Network::Socket> ptr, SL::Remote_Access_Library::Network::SocketImpl* impl, boost::asio::ip::tcp::resolver::iterator endpoint_iterator) {
	boost::asio::async_connect(impl->socket, endpoint_iterator, [ptr, impl](boost::system::error_code ec, boost::asio::ip::tcp::resolver::iterator)
	{
		if (!impl->socket.is_open()) return;
		
		if (!ec) {
			impl->NetworkEvents_.OnConnect(ptr);
			do_read_header(ptr, impl);
		}
		else ptr->close();
	});
}


void do_read_header(std::shared_ptr<SL::Remote_Access_Library::Network::Socket> ptr, SL::Remote_Access_Library::Network::SocketImpl* impl) {

	boost::asio::async_read(impl->socket, boost::asio::buffer(&impl->PacketHeader, sizeof(impl->PacketHeader)), [ptr, impl](boost::system::error_code ec, std::size_t /*length*/)
	{
		if (!ec) do_read_body(ptr, impl);
		else ptr->close();
	});
}
void do_read_body(std::shared_ptr<SL::Remote_Access_Library::Network::Socket> ptr, SL::Remote_Access_Library::Network::SocketImpl* impl) {
	impl->IncomingPacket = SL::Remote_Access_Library::Network::Packet::CreatePacket(impl->PacketHeader);
	auto p(impl->IncomingPacket->data());
	auto size(impl->PacketHeader.PayloadLen);
	boost::asio::async_read(impl->socket, boost::asio::buffer(p, size), [ptr, impl](boost::system::error_code ec, std::size_t /*length*/)
	{
		if (!ec) {
			impl->NetworkEvents_.OnReceive(ptr, impl->IncomingPacket);
			do_read_header(ptr, impl);
		}
		else ptr->close();
	});
}

void do_write_header(std::shared_ptr<SL::Remote_Access_Library::Network::Socket> ptr, SL::Remote_Access_Library::Network::SocketImpl* impl) {
	auto blk(impl->OutgoingPackets.front().get());
	boost::asio::async_write(impl->socket, boost::asio::buffer(blk->header(), sizeof(SL::Remote_Access_Library::Network::PacketHeader)), [ptr, impl](boost::system::error_code ec, std::size_t byteswritten)
	{
		if (!ec)
		{
			if (!impl->OutgoingPackets.empty())
			{
				do_write(ptr, impl);
			}
		}
		else ptr->close();
	});
}

void do_write(std::shared_ptr<SL::Remote_Access_Library::Network::Socket> ptr, SL::Remote_Access_Library::Network::SocketImpl* impl) {
	auto blk(impl->OutgoingPackets.front());
	boost::asio::async_write(impl->socket, boost::asio::buffer(blk->data(), blk->header()->PayloadLen), [ptr, impl](boost::system::error_code ec, std::size_t byteswritten)
	{
		if (!ec)
		{
			impl->OutgoingPackets.pop_front();
			if (!impl->OutgoingPackets.empty())
			{
				do_write_header(ptr, impl);
			}
		}
		else ptr->close();
	});
}


////////////////////////SERVER HERE

struct SL::Remote_Access_Library::Network::ListinerImpl {
	ListinerImpl(const boost::asio::ip::tcp::endpoint& endpoint, NetworkEvents& netevents) :
		NetworkEvents_(netevents),
		acceptor_(io_service, endpoint),
		socket_(io_service) {
		io_servicethread = std::thread([this]() {
			std::cout << "Starting io_service for listener" << std::endl;
			io_service.run();
			std::cout << "stopping io_service for listener" << std::endl;
		});
	}

	boost::asio::io_service io_service;
	boost::asio::ip::tcp::acceptor acceptor_;
	boost::asio::ip::tcp::socket socket_;
	NetworkEvents NetworkEvents_;
	std::thread io_servicethread;
};

void do_accept(std::shared_ptr<SL::Remote_Access_Library::Network::Listener> ptr, SL::Remote_Access_Library::Network::ListinerImpl* impl) {
	impl->acceptor_.async_accept(impl->socket_, [ptr, impl](boost::system::error_code ec)
	{
		if (!ec)
		{
			auto socketimpl = new SL::Remote_Access_Library::Network::SocketImpl(impl->io_service, std::move(impl->socket_), impl->NetworkEvents_);
			auto sock = std::make_shared<SL::Remote_Access_Library::Network::Socket>(socketimpl);
			impl->NetworkEvents_.OnConnect(sock);
			do_read_header(sock, socketimpl);
		}
		do_accept(ptr, impl);
	});
}

SL::Remote_Access_Library::Network::Listener::Listener(ListinerImpl* impl) : _ListinerImpl(impl)
{

}
SL::Remote_Access_Library::Network::Listener::~Listener() {
	delete _ListinerImpl;
}
void SL::Remote_Access_Library::Network::Listener::Start() {
	do_accept(shared_from_this(), _ListinerImpl);
}
void SL::Remote_Access_Library::Network::Listener::Stop() {
	_ListinerImpl->io_service.stop();
}

std::shared_ptr<SL::Remote_Access_Library::Network::Listener> SL::Remote_Access_Library::Network::Listener::CreateListener(unsigned short port, NetworkEvents& netevents) {
	return std::make_shared<SL::Remote_Access_Library::Network::Listener>(new SL::Remote_Access_Library::Network::ListinerImpl(boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port), netevents));
}
