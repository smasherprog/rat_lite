#include "stdafx.h"
#include "Socket.h"
#include "zstd.h"
#include "Internal_Impls.h"
#include <boost/asio.hpp>
#include "Packet.h"
#include <boost/asio.hpp>

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
	std::deque<std::shared_ptr<Network::Packet>> OutgoingPackets;
	std::shared_ptr<Network::Packet> IncomingPacket;
	std::vector<char> WorkingBuffer;

	unsigned int OutGoingPacketCount = 0;
	unsigned int OutGoingByteCount = 0;
};
std::shared_ptr<SL::Remote_Access_Library::INTERNAL::SocketImpl> SL::Remote_Access_Library::INTERNAL::Create(boost::asio::io_service& io, Network::NetworkEvents& netevents, Utilities::ThreadPool& t, boost::asio::ip::tcp::socket&& s) {
	return std::make_shared<SocketImpl>(io, netevents, t, std::move(s));
}
std::shared_ptr<SL::Remote_Access_Library::INTERNAL::SocketImpl> SL::Remote_Access_Library::INTERNAL::Create(boost::asio::io_service& io, Network::NetworkEvents& netevents, Utilities::ThreadPool& t) {
	return std::make_shared<SocketImpl>(io, netevents, t);
}

void decompress_payload(std::shared_ptr<SL::Remote_Access_Library::Network::Packet>& pack, std::vector<char>& workingbuffer)
{
	if (pack->header()->UnCompressedlen <= 0) return;//allready compressed
	workingbuffer.resize(pack->header()->UnCompressedlen);
	auto dstsize = ZSTD_decompress(workingbuffer.data(), pack->header()->UnCompressedlen, pack->data(), pack->header()->PayloadLen);
	memcpy(pack->data(), workingbuffer.data(), dstsize);
	pack->header()->PayloadLen = static_cast<unsigned int>(dstsize);
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

void SL::Remote_Access_Library::Network::Socket::send(std::shared_ptr<Packet>& pack)
{
	auto self(shared_from_this());

	_SocketImpl->ThreadPool.Enqueue([this, pack, self]() {

		if (pack->header()->UnCompressedlen > 0) return;//allready compressed
		pack->header()->UnCompressedlen = pack->header()->PayloadLen;
		pack->header()->PayloadLen = static_cast<unsigned int>(ZSTD_compress(pack->data(), ZSTD_compressBound(pack->header()->UnCompressedlen), pack->data(), pack->header()->PayloadLen));
		_SocketImpl->OutGoingByteCount += pack->header()->PayloadLen;
		_SocketImpl->OutGoingPacketCount += 1;
		std::cout << "Sending " << pack->header()->PayloadLen << " bytes" << std::endl;
		_SocketImpl->io_service.post([this, pack, self]()
		{
			bool write_in_progress = !_SocketImpl->OutgoingPackets.empty();
			_SocketImpl->OutgoingPackets.push_back(pack);
			if (!write_in_progress)
			{
				do_write_header();
			}
		});
	});

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
			decompress_payload(_SocketImpl->IncomingPacket, _SocketImpl->WorkingBuffer);
			_SocketImpl->NetworkEvents_.OnReceive(this, _SocketImpl->IncomingPacket);
			do_read_header();
		}
		else close();
	});
}

void SL::Remote_Access_Library::Network::Socket::do_write_header() {
	auto blk(_SocketImpl->OutgoingPackets.front().get());
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
	auto blk(_SocketImpl->OutgoingPackets.front());
	auto self(shared_from_this());
	boost::asio::async_write(_SocketImpl->socket, boost::asio::buffer(blk->data(), blk->header()->PayloadLen), [self, this](boost::system::error_code ec, std::size_t byteswritten)
	{
		if (!ec)
		{
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
