#include "stdafx.h"
#include "TCPSocket.h"
#include "IBaseNetworkDriver.h"
#include "SocketImpl.h"
#include "Packet.h"
#include "IO_Runner.h"
#include "zstd.h"

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


std::shared_ptr<SL::Remote_Access_Library::Network::Packet> compressPacket(SL::Remote_Access_Library::Network::Packet& pack, int compressionlevel)
{
	assert(compressionlevel >= 1 && compressionlevel <= 19);

	SL::Remote_Access_Library::Network::PacketHeader header;
	header.UnCompressedlen = pack.header()->PayloadLen;
	header.PayloadLen = ZSTD_compressBound(pack.header()->PayloadLen);
	header.Packet_Type = pack.header()->Packet_Type;
	auto p = SL::Remote_Access_Library::Network::Packet::CreatePacket(header);
	p->header()->PayloadLen = static_cast<unsigned int>(ZSTD_compress(p->data(), header.PayloadLen, pack.data() , pack.header()->PayloadLen, compressionlevel));
	std::cout << "compress from: " << p->header()->UnCompressedlen << " To " << p->header()->PayloadLen << std::endl;
	return p;
}

std::shared_ptr<SL::Remote_Access_Library::Network::Packet> decompressPacket(SL::Remote_Access_Library::Network::Packet& pack)
{

	SL::Remote_Access_Library::Network::PacketHeader header;
	header.Packet_Type = pack.header()->Packet_Type;
	header.PayloadLen= header.UnCompressedlen = pack.header()->UnCompressedlen;
	auto p = SL::Remote_Access_Library::Network::Packet::CreatePacket(header);
	auto dst = ZSTD_decompress(p->data(), pack.header()->UnCompressedlen, pack.data(), pack.header()->PayloadLen);

	p->header()->PayloadLen = static_cast<unsigned int>(dst);
	std::cout << "decompress from: " << p->header()->UnCompressedlen << " To " << p->header()->PayloadLen << std::endl;
	p->header()->UnCompressedlen = 0;
	return p;
}


void SL::Remote_Access_Library::Network::TCPSocket::send(std::shared_ptr<Packet>& pack, std::function<void()> on_sent_callback)
{
	auto self(shared_from_this());
	auto compack = compressPacket(*pack, 1);

	_SocketImpl->OutGoingByteCount += compack->header()->PayloadLen;
	_SocketImpl->OutGoingPacketCount += 1;
	std::cout << "Sending " << compack->header()->PayloadLen << " bytes" << std::endl;
	_SocketImpl->io_service.post([this, compack, self, on_sent_callback]()
	{
		bool write_in_progress = !_SocketImpl->OutgoingPackets.empty();
		_SocketImpl->OutgoingPackets.push_back({ compack, on_sent_callback });
		if (!write_in_progress)
		{
			do_write_header();
		}
	});
}
void doesnothing() {}
void SL::Remote_Access_Library::Network::TCPSocket::send(std::shared_ptr<Packet>& pack)
{
	send(pack, doesnothing);//empty lambda
}
void SL::Remote_Access_Library::Network::TCPSocket::close()
{
	_SocketImpl->NetworkEvents_->OnClose(this);
	try
	{
	_SocketImpl->socket.shutdown(boost::asio::socket_base::shutdown_send);
	_SocketImpl->socket.close();
	}
	catch (...) {}//I dont care about exceptions when the socket is being closed!
}

void SL::Remote_Access_Library::Network::TCPSocket::connect(const char * host, const char * port)
{
	if (host == nullptr && port == nullptr) {
		_SocketImpl->NetworkEvents_->OnConnect(shared_from_this());
		do_read_header();
	}
	else {
		boost::asio::ip::tcp::resolver resolver(_SocketImpl->io_service);
		boost::asio::ip::tcp::resolver::query query(host, port);
		auto self(std::dynamic_pointer_cast<TCPSocket>( shared_from_this()));

		try
		{
			auto endpoint = resolver.resolve(query);
			boost::asio::async_connect(self->_SocketImpl->socket, endpoint, [self, this](boost::system::error_code ec, boost::asio::ip::tcp::resolver::iterator)
			{
				if (!_SocketImpl->socket.is_open()) return;

				if (!ec) {
					_SocketImpl->NetworkEvents_->OnConnect(shared_from_this());
					do_read_header();
				}
				else close();
			});
		}
		catch (...) {
			_SocketImpl->NetworkEvents_->OnClose(this);
		}
	}
}
unsigned int SL::Remote_Access_Library::Network::TCPSocket::get_OutgoingPacketCount() const
{
	return _SocketImpl->OutGoingPacketCount;
}

unsigned int SL::Remote_Access_Library::Network::TCPSocket::get_OutgoingByteCount() const
{
	return _SocketImpl->OutGoingByteCount;
}


void SL::Remote_Access_Library::Network::TCPSocket::do_read_header() {
	auto self(std::dynamic_pointer_cast<TCPSocket>(shared_from_this()));
	boost::asio::async_read(self->_SocketImpl->socket, boost::asio::buffer(&self->_SocketImpl->PacketHeader, sizeof(self->_SocketImpl->PacketHeader)), [self, this](boost::system::error_code ec, std::size_t /*length*/)
	{
		if (!ec) do_read_body();
		else close();
	});
}
void SL::Remote_Access_Library::Network::TCPSocket::do_read_body() {
	_SocketImpl->IncomingPacket = SL::Remote_Access_Library::Network::Packet::CreatePacket(_SocketImpl->PacketHeader);
	auto p(_SocketImpl->IncomingPacket->data());
	auto size(_SocketImpl->PacketHeader.PayloadLen);
	auto self(shared_from_this());
	boost::asio::async_read(_SocketImpl->socket, boost::asio::buffer(p, size), [self, this](boost::system::error_code ec, std::size_t /*length*/)
	{
		if (!ec) {
			_SocketImpl->IncomingPacket = decompressPacket(*_SocketImpl->IncomingPacket);
			_SocketImpl->NetworkEvents_->OnReceive(this, _SocketImpl->IncomingPacket);
			do_read_header();
		}
		else close();
	});
}

void SL::Remote_Access_Library::Network::TCPSocket::do_write_header() {
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

void SL::Remote_Access_Library::Network::TCPSocket::do_write() {
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
