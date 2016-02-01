#include "stdafx.h"
#include "TCPSocket.h"
#include "IBaseNetworkDriver.h"
#include "Packet.h"
#include "zstd.h"
#include <boost/asio.hpp>

namespace SL {
	namespace Remote_Access_Library {
		namespace Network {

			//this namespace should only be accessed by the Core library and NEVER by a consumer of this library
			namespace INTERNAL {

				class TCPSocketImpl {
				public:
					std::vector<char> _IncomingBuffer;
					std::deque<std::shared_ptr<Network::Packet>> _OutgoingPackets;

					boost::asio::ip::tcp::socket socket;
					Network::IBaseNetworkDriver* NetworkEvents;
					Network::PacketHeader PacketHeader;
					Network::SocketStats SocketStats;
					bool Writing;
					bool Closed;

					TCPSocketImpl(Network::IBaseNetworkDriver* netevents, boost::asio::ip::tcp::socket& s) :  socket(std::move(s)), NetworkEvents(netevents) {
						
						SocketStats = { 0 };
						Closed = Writing = false;
					}
					TCPSocketImpl(Network::IBaseNetworkDriver* netevents, boost::asio::io_service& io) :  socket(io), NetworkEvents(netevents) {
						SocketStats = { 0 };
						Closed = Writing = false;
					}

					char* TCPSocketImpl::get_Buffer()
					{
						_IncomingBuffer.reserve(PacketHeader.PayloadLen);
						return _IncomingBuffer.data();
					}

					size_t TCPSocketImpl::get_BufferSize()
					{
						return PacketHeader.PayloadLen;
					}



				};

			}
		}
	}
}

std::shared_ptr<SL::Remote_Access_Library::Network::Packet> decompressPacket(SL::Remote_Access_Library::Network::PacketHeader& header, std::vector<char>& buffer)
{
	auto p = SL::Remote_Access_Library::Network::Packet::CreatePacket(header);
	auto dst = ZSTD_decompress(p->data(), header.UnCompressedlen, buffer.data(), header.PayloadLen);
	if (ZSTD_isError(dst) > 0) {
		std::string error = ZSTD_getErrorName(dst);
		int k = 0;
	}
	p->header()->PayloadLen = static_cast<unsigned int>(dst);
	p->header()->UnCompressedlen = 0;

	return p;
}

std::shared_ptr<SL::Remote_Access_Library::Network::Packet> compressPacket(SL::Remote_Access_Library::Network::Packet& pack, int compressionlevel)
{
	assert(compressionlevel >= 1 && compressionlevel <= 19);

	SL::Remote_Access_Library::Network::PacketHeader header;
	header.UnCompressedlen = pack.header()->PayloadLen;
	header.PayloadLen = ZSTD_compressBound(pack.header()->PayloadLen);
	header.Packet_Type = pack.header()->Packet_Type;
	auto p = SL::Remote_Access_Library::Network::Packet::CreatePacket(header);


	auto start = std::chrono::steady_clock::now();
	p->header()->PayloadLen = static_cast<unsigned int>(ZSTD_compress(p->data(), header.PayloadLen, pack.data(), pack.header()->PayloadLen, 3));
	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start);
	std::cout << "It took " << elapsed.count() << " milliseconds compare run compress " << p->header()->PayloadLen << std::endl;



	//assert(p->header()->PayloadLen < 1024 * 1024 * 48);//sanitfy check for packet sizes

	return p;
}



std::shared_ptr<SL::Remote_Access_Library::Network::TCPSocket> SL::Remote_Access_Library::Network::TCPSocket::Create(SL::Remote_Access_Library::Network::IBaseNetworkDriver* netevents, const char * dst_host, const char * dst_port, void* io_service) {
	
	auto ret= std::make_shared<TCPSocket>(new INTERNAL::TCPSocketImpl(netevents, *((boost::asio::io_service*)io_service)));
	ret->connect(dst_host, dst_port);
	return ret;
}
std::shared_ptr<SL::Remote_Access_Library::Network::TCPSocket> SL::Remote_Access_Library::Network::TCPSocket::Create(SL::Remote_Access_Library::Network::IBaseNetworkDriver* netevents, void* socket) {
	auto ret = std::make_shared<TCPSocket>(new INTERNAL::TCPSocketImpl(netevents,  *((boost::asio::ip::tcp::socket*)socket)));
	ret->connect(nullptr, nullptr);
	return ret;
}
SL::Remote_Access_Library::Network::TCPSocket::TCPSocket(INTERNAL::TCPSocketImpl * impl) : _SocketImpl(impl)
{	
	std::cout << "TCPSocket()" << std::endl;
}
SL::Remote_Access_Library::Network::TCPSocket::~TCPSocket() {
	delete _SocketImpl;
	std::cout << "~TCPSocket()" << std::endl;
}


void SL::Remote_Access_Library::Network::TCPSocket::send(std::shared_ptr<Packet>& pack)
{
	auto self(shared_from_this());

	_SocketImpl->SocketStats.TotalPacketSent += 1;
	_SocketImpl->SocketStats.TotalBytesSent += pack->header()->PayloadLen;
	auto compack = compressPacket(*pack, 1);
	_SocketImpl->SocketStats.NetworkBytesSent += compack->header()->PayloadLen;
	_SocketImpl->socket.get_io_service().post([this, self, compack]()
	{
		_SocketImpl->_OutgoingPackets.push_back(compack);
		if (!_SocketImpl->Writing)
		{
			Write();
		}
	});
}
SL::Remote_Access_Library::Network::SocketStats SL::Remote_Access_Library::Network::TCPSocket::get_SocketStats() const {
	return _SocketImpl->SocketStats;
}
void SL::Remote_Access_Library::Network::TCPSocket::close()
{
	if (!_SocketImpl->Closed) {//prevent double calls
		_SocketImpl->Closed = true;
		auto self(shared_from_this());
		_SocketImpl->NetworkEvents->OnClose(self);
		try
		{
			_SocketImpl->socket.shutdown(boost::asio::socket_base::shutdown_send);
			_SocketImpl->socket.close();
		}
		catch (...) {}//I dont care about exceptions when the socket is being closed!
	}
}

void SL::Remote_Access_Library::Network::TCPSocket::connect(const char * host, const char * port)
{
	auto self(shared_from_this());
	if (host == nullptr && port == nullptr) {
		_SocketImpl->NetworkEvents->OnConnect(self);
		Read();
	}
	else {
		boost::asio::ip::tcp::resolver resolver(_SocketImpl->socket.get_io_service());
		boost::asio::ip::tcp::resolver::query query(host, port);

		try
		{
			auto endpoint = resolver.resolve(query);
			boost::asio::async_connect(_SocketImpl->socket, endpoint, [self, this](boost::system::error_code ec, boost::asio::ip::tcp::resolver::iterator)
			{
				if (!_SocketImpl->socket.is_open()) return;

				if (!ec && !_SocketImpl->Closed) {

					_SocketImpl->NetworkEvents->OnConnect(self);
					Read();
				}
				else close();
			});
		}
		catch (...) {
			_SocketImpl->NetworkEvents->OnClose(self);
		}
	}
}


void SL::Remote_Access_Library::Network::TCPSocket::Read() {
	auto self(shared_from_this());
	boost::asio::async_read(_SocketImpl->socket, boost::asio::buffer(&_SocketImpl->PacketHeader, sizeof(_SocketImpl->PacketHeader)), [self, this](boost::system::error_code ec, std::size_t len/*length*/)
	{
		if (!ec && !_SocketImpl->Closed) {
			assert(len == sizeof(_SocketImpl->PacketHeader));
			auto p(_SocketImpl->get_Buffer());
			auto size(_SocketImpl->get_BufferSize());

			boost::asio::async_read(_SocketImpl->socket, boost::asio::buffer(p, size), [self, this](boost::system::error_code ec, std::size_t len/*length*/)
			{
				if (!ec && !_SocketImpl->Closed) {

					assert(len == _SocketImpl->get_BufferSize());

					_SocketImpl->SocketStats.TotalPacketReceived += 1;
					_SocketImpl->SocketStats.NetworkBytesReceived += _SocketImpl->PacketHeader.PayloadLen;
					_SocketImpl->SocketStats.TotalBytesReceived += _SocketImpl->PacketHeader.UnCompressedlen;
					if (!_SocketImpl->Closed) {
						_SocketImpl->NetworkEvents->OnReceive(self, decompressPacket(_SocketImpl->PacketHeader, _SocketImpl->_IncomingBuffer));
					}
					Read();
				}
				else close();
			});

		}
		else close();
	});
}

void SL::Remote_Access_Library::Network::TCPSocket::Write() {
	auto packet(_SocketImpl->_OutgoingPackets.front());
	_SocketImpl->_OutgoingPackets.pop_front();//remove the packet
	auto self(shared_from_this());
	_SocketImpl->Writing = true;
	boost::asio::async_write(_SocketImpl->socket, boost::asio::buffer(packet->header(), sizeof(SL::Remote_Access_Library::Network::PacketHeader)), [self, packet, this](boost::system::error_code ec, std::size_t byteswritten)
	{
		if (!ec && !_SocketImpl->Closed)
		{
			assert(byteswritten == sizeof(_SocketImpl->PacketHeader));
			boost::asio::async_write(_SocketImpl->socket, boost::asio::buffer(packet->data(), packet->header()->PayloadLen), [self, packet, this](boost::system::error_code ec, std::size_t byteswritten)
			{
				if (!ec && !_SocketImpl->Closed)
				{
					assert(byteswritten == packet->header()->PayloadLen);
					_SocketImpl->_OutgoingPackets.size();
					if (!_SocketImpl->_OutgoingPackets.empty())
					{
						Write();
					}
					else {
						_SocketImpl->Writing = false;
					}
				}
				else close();
			});
		}
		else close();
	});
}



