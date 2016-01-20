#include "stdafx.h"
#include "TCPSocket.h"
#include "IBaseNetworkDriver.h"
#include "SocketImpl.h"
#include "Packet.h"
#include "IO_Runner.h"


std::shared_ptr<SL::Remote_Access_Library::INTERNAL::SocketImpl> Create(boost::asio::io_service& io, SL::Remote_Access_Library::Network::IBaseNetworkDriver* netevents, boost::asio::ip::tcp::socket&& s) {
	return std::make_shared<SL::Remote_Access_Library::INTERNAL::SocketImpl>(io, netevents, std::move(s));
}
std::shared_ptr<SL::Remote_Access_Library::INTERNAL::SocketImpl> Create(boost::asio::io_service& io, SL::Remote_Access_Library::Network::IBaseNetworkDriver* netevents) {
	return std::make_shared<SL::Remote_Access_Library::INTERNAL::SocketImpl>(io, netevents);
}

SL::Remote_Access_Library::Network::TCPSocket::TCPSocket(std::shared_ptr<INTERNAL::SocketImpl>& impl) : _SocketImpl(impl) {
	std::cout << "TCPSocket()" << std::endl;

}

SL::Remote_Access_Library::Network::TCPSocket::~TCPSocket() {
	std::cout << "~TCPSocket()" << std::endl;
}

std::shared_ptr<SL::Remote_Access_Library::Network::ISocket> SL::Remote_Access_Library::Network::TCPSocket::ConnectTo(const char* host, const char* port, SL::Remote_Access_Library::Network::IBaseNetworkDriver* netevents) {
	//first thread in will initialize this and start the io_service
	static IO_Runner _io_runner;

	auto socketimpl = Create(_io_runner.io_service, netevents);
	auto socket = std::make_shared<TCPSocket>(socketimpl);
	socket->connect(host, port);
	return socket;
}




void SL::Remote_Access_Library::Network::TCPSocket::send(std::shared_ptr<Packet>& pack)
{
	auto self(shared_from_this());
	_SocketImpl->PushToOutgoing(pack);
	_SocketImpl->io_service.post([this, self]()
	{
		if (_SocketImpl->PacketsWaitingToBeSent())
		{
			do_write_header();
		}
	});
}
SL::Remote_Access_Library::Network::SocketStats SL::Remote_Access_Library::Network::TCPSocket::get_SocketStats() const {
	return _SocketImpl->SocketStats;
}
void SL::Remote_Access_Library::Network::TCPSocket::close()
{
	_SocketImpl->NetworkEvents->OnClose(this);
	try
	{
		_SocketImpl->socket.shutdown(boost::asio::socket_base::shutdown_send);
		_SocketImpl->socket.close();
	}
	catch (...) {}//I dont care about exceptions when the socket is being closed!
}

void SL::Remote_Access_Library::Network::TCPSocket::connect(const char * host, const char * port)
{
	auto self(shared_from_this());
	if (host == nullptr && port == nullptr) {
		_SocketImpl->NetworkEvents->OnConnect(self);
		do_read_header();
	}
	else {
		boost::asio::ip::tcp::resolver resolver(_SocketImpl->io_service);
		boost::asio::ip::tcp::resolver::query query(host, port);

		try
		{
			auto endpoint = resolver.resolve(query);
			boost::asio::async_connect(_SocketImpl->socket, endpoint, [self, this](boost::system::error_code ec, boost::asio::ip::tcp::resolver::iterator)
			{
				if (!_SocketImpl->socket.is_open()) return;

				if (!ec) {
					_SocketImpl->NetworkEvents->OnConnect(self);
					do_read_header();
				}
				else close();
			});
		}
		catch (...) {
			_SocketImpl->NetworkEvents->OnClose(this);
		}
	}
}


void SL::Remote_Access_Library::Network::TCPSocket::do_read_header() {
	auto self(shared_from_this());
	boost::asio::async_read(_SocketImpl->socket, boost::asio::buffer(&_SocketImpl->PacketHeader, sizeof(_SocketImpl->PacketHeader)), [self, this](boost::system::error_code ec, std::size_t /*length*/)
	{
		if (!ec) do_read_body();
		else close();
	});
}
void SL::Remote_Access_Library::Network::TCPSocket::do_read_body() {
	auto self(shared_from_this());
	auto p(_SocketImpl->get_Buffer());
	auto size(_SocketImpl->get_BufferSize());
	boost::asio::async_read(_SocketImpl->socket, boost::asio::buffer(p, size), [self, this](boost::system::error_code ec, std::size_t /*length*/)
	{
		if (!ec) {
			_SocketImpl->NetworkEvents->OnReceive(this, _SocketImpl->get_IncomingPacket());
			do_read_header();
		}
		else close();
	});
}

void SL::Remote_Access_Library::Network::TCPSocket::do_write_header() {
	auto packet(_SocketImpl->PopFromOutgoing());
	auto self(shared_from_this());
	boost::asio::async_write(_SocketImpl->socket, boost::asio::buffer(packet->header(), sizeof(SL::Remote_Access_Library::Network::PacketHeader)), [self, packet, this](boost::system::error_code ec, std::size_t byteswritten)
	{
		if (!ec)
		{
			do_write(packet);
		}
		else close();
	});
}

void SL::Remote_Access_Library::Network::TCPSocket::do_write(std::shared_ptr<Network::Packet> packet) {
	auto self(shared_from_this());
	boost::asio::async_write(_SocketImpl->socket, boost::asio::buffer(packet->data(), packet->header()->PayloadLen), [self, packet, this](boost::system::error_code ec, std::size_t byteswritten)
	{
		if (!ec)
		{
			if (_SocketImpl->PacketsWaitingToBeSent())
			{
				do_write_header();
			}
		}
		else close();
	});
}
