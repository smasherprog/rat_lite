#include "stdafx.h"
#include "Socket.h"

#include "Internal_Impls.h"
#include <boost/asio.hpp>
#include "Packet.h"
#include <boost/asio.hpp>

void SL::Remote_Access_Library::Network::DefaultOnConnect(const std::shared_ptr<SL::Remote_Access_Library::Network::Socket> ptr) {
	std::cout << "DEFAULT OnConnected Called!" << std::endl;
};
void SL::Remote_Access_Library::Network::DefaultOnReceive(const Socket* ptr, std::shared_ptr<SL::Remote_Access_Library::Network::Packet>& pac) {
	std::cout << "DEFAULT OnReceive Called! PacketType: " << (int)pac->header()->Packet_Type << " Size: " << pac->header()->PayloadLen << std::endl;
};
void SL::Remote_Access_Library::Network::DefaultOnClose(const Socket* ptr) {
	std::cout << "DEFAULT OnClose Called!" << std::endl;
};

struct PacketSendCommand {
	std::shared_ptr<SL::Remote_Access_Library::Network::Packet> Packet;
	std::function<void()> OnSentCallback;
};
struct SL::Remote_Access_Library::INTERNAL::SocketImpl {
	SocketImpl(boost::asio::io_service& io,
		Network::NetworkEvents& netevents,
		Utilities::ThreadPool& t,
		boost::asio::ip::tcp::socket&& s) : io_service(io), socket(std::move(s)), NetworkEvents_(netevents), ThreadPool(t) {}
	SocketImpl(boost::asio::io_service& io,
		Network::NetworkEvents& netevents,
		Utilities::ThreadPool& t) : io_service(io), socket(io), NetworkEvents_(netevents), ThreadPool(t) {}

	boost::asio::io_service& io_service;
	Utilities::ThreadPool& ThreadPool;
	boost::asio::ip::tcp::socket socket;

	Network::NetworkEvents NetworkEvents_;
	Network::PacketHeader PacketHeader;
	std::deque<PacketSendCommand> OutgoingPackets;
	std::shared_ptr<Network::Packet> IncomingPacket;

	unsigned int OutGoingPacketCount = 0;
	unsigned int OutGoingByteCount = 0;
};
std::shared_ptr<SL::Remote_Access_Library::INTERNAL::SocketImpl> SL::Remote_Access_Library::INTERNAL::Create(boost::asio::io_service& io, Network::NetworkEvents& netevents, Utilities::ThreadPool& t, boost::asio::ip::tcp::socket&& s) {
	return std::make_shared<SocketImpl>(io, netevents, t, std::move(s));
}
std::shared_ptr<SL::Remote_Access_Library::INTERNAL::SocketImpl> SL::Remote_Access_Library::INTERNAL::Create(boost::asio::io_service& io, Network::NetworkEvents& netevents, Utilities::ThreadPool& t) {
	return std::make_shared<SocketImpl>(io, netevents, t);
}

SL::Remote_Access_Library::Network::Socket::Socket(std::shared_ptr<INTERNAL::SocketImpl>& impl) : _SocketImpl(impl) {
	std::cout << "Socket()" << std::endl;

}

SL::Remote_Access_Library::Network::Socket::~Socket() {
	std::cout << "~Socket()" << std::endl;
}

std::shared_ptr<SL::Remote_Access_Library::Network::Socket> SL::Remote_Access_Library::Network::Socket::ConnectTo(const char* host, const char* port, NetworkEvents& netevents) {
	//first thread in will initialize this and start the io_service
	static INTERNAL::io_runner _io_runner;

	auto socketimpl = INTERNAL::Create(_io_runner.io_service, netevents, _io_runner.ThreadPool);
	auto socket = std::make_shared<Socket>(socketimpl);
	socket->connect(host, port);
	return socket;
}

void SL::Remote_Access_Library::Network::Socket::send(std::shared_ptr<Packet>& pack, std::function<void()> on_sent_callback)
{
	auto self(shared_from_this());
	
	_SocketImpl->ThreadPool.Enqueue([this, pack, self, on_sent_callback]() {
		pack->compress();
		_SocketImpl->OutGoingByteCount += pack->header()->PayloadLen;
		_SocketImpl->OutGoingPacketCount += 1;
		std::cout << "Sending " << pack->header()->PayloadLen << " bytes" << std::endl;
		_SocketImpl->io_service.post([this, pack, self, on_sent_callback]()
		{
			bool write_in_progress = !_SocketImpl->OutgoingPackets.empty();
			_SocketImpl->OutgoingPackets.push_back({ pack, on_sent_callback });
			if (!write_in_progress)
			{
				do_write_header();
			}
		});
	});
}
void doesnothing() {}
void SL::Remote_Access_Library::Network::Socket::send(std::shared_ptr<Packet>& pack)
{
	send(pack, doesnothing);//empty lambda
}
void SL::Remote_Access_Library::Network::Socket::close()
{
	auto self(shared_from_this());
	_SocketImpl->NetworkEvents_.OnClose(this);
	_SocketImpl->io_service.post([self, this]() {
		_SocketImpl->socket.close();
	});
}

void SL::Remote_Access_Library::Network::Socket::connect(const char * host, const char * port)
{
	if (host == nullptr && port == nullptr) {
		_SocketImpl->NetworkEvents_.OnConnect(shared_from_this());
		do_read_header();
	}
	else {
		boost::asio::ip::tcp::resolver resolver(_SocketImpl->io_service);
		boost::asio::ip::tcp::resolver::query query(host, port);
		auto self(shared_from_this());
		auto endpoint = resolver.resolve(query);

		boost::asio::async_connect(self->_SocketImpl->socket, endpoint, [self, this](boost::system::error_code ec, boost::asio::ip::tcp::resolver::iterator)
		{
			if (!_SocketImpl->socket.is_open()) return;

			if (!ec) {
				_SocketImpl->NetworkEvents_.OnConnect(shared_from_this());
				do_read_header();
			}
			else close();
		});
	}

}

unsigned int SL::Remote_Access_Library::Network::Socket::get_OutgoingPacketCount() const
{
	return _SocketImpl->OutGoingPacketCount;
}

unsigned int SL::Remote_Access_Library::Network::Socket::get_OutgoingByteCount() const
{
	return _SocketImpl->OutGoingByteCount;
}


void SL::Remote_Access_Library::Network::Socket::do_read_header() {
	auto self(shared_from_this());
	boost::asio::async_read(self->_SocketImpl->socket, boost::asio::buffer(&self->_SocketImpl->PacketHeader, sizeof(self->_SocketImpl->PacketHeader)), [self, this](boost::system::error_code ec, std::size_t /*length*/)
	{
		if (!ec) do_read_body();
		else close();
	});
}
void SL::Remote_Access_Library::Network::Socket::do_read_body() {
	_SocketImpl->IncomingPacket = SL::Remote_Access_Library::Network::Packet::CreatePacket(_SocketImpl->PacketHeader);
	auto p(_SocketImpl->IncomingPacket->data());
	auto size(_SocketImpl->PacketHeader.PayloadLen);
	auto self(shared_from_this());
	boost::asio::async_read(_SocketImpl->socket, boost::asio::buffer(p, size), [self, this](boost::system::error_code ec, std::size_t /*length*/)
	{
		if (!ec) {
			_SocketImpl->IncomingPacket->decompress();
			_SocketImpl->NetworkEvents_.OnReceive(this, _SocketImpl->IncomingPacket);
			do_read_header();
		}
		else close();
	});
}

void SL::Remote_Access_Library::Network::Socket::do_write_header() {
	auto blk(_SocketImpl->OutgoingPackets.front().Packet.get());
	auto self(shared_from_this());
	boost::asio::async_write(_SocketImpl->socket, boost::asio::buffer(blk->header(), sizeof(SL::Remote_Access_Library::Network::PacketHeader)), [self, this](boost::system::error_code ec, std::size_t byteswritten)
	{
		if (!ec)
		{
			if (!_SocketImpl->OutgoingPackets.empty())
			{
				do_write();
			}
		}
		else close();
	});
}

void SL::Remote_Access_Library::Network::Socket::do_write() {
	auto blk(_SocketImpl->OutgoingPackets.front().Packet);
	auto self(shared_from_this());
	boost::asio::async_write(_SocketImpl->socket, boost::asio::buffer(blk->data(), blk->header()->PayloadLen), [self, this](boost::system::error_code ec, std::size_t byteswritten)
	{
		if (!ec)
		{
			_SocketImpl->OutgoingPackets.front().OnSentCallback();
			_SocketImpl->OutgoingPackets.pop_front();
			_SocketImpl->OutGoingByteCount -= byteswritten;
			_SocketImpl->OutGoingPacketCount -= 1;
		
			if (!_SocketImpl->OutgoingPackets.empty())
			{
				do_write_header();
			}
		}
		else close();
	});
}
