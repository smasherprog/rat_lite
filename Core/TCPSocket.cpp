#include "stdafx.h"
#include "TCPSocket.h"
#include "IBaseNetworkDriver.h"
#include "Packet.h"
#include "zstd.h"
#include <boost/asio.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/bind.hpp>

namespace SL {
	namespace Remote_Access_Library {
		namespace Network {

			//this namespace should only be accessed by the Core library and NEVER by a consumer of this library
			namespace INTERNAL {
				struct PacketHeaderWrapper {
					unsigned int Packet_Type;
					unsigned int Payload_Length;
					unsigned int UncompressedLength;
				};
				struct OutgoingPacket {
					std::shared_ptr<Network::Packet> Pack;
					unsigned int UncompressedLength;
				};
				class TCPSocketImpl {
					std::vector<char> _IncomingBuffer;
					std::deque<OutgoingPacket> _OutgoingPackets;
					Network::SocketStats SocketStats;
					std::unordered_map<std::string, std::string> Header;

					boost::asio::ip::tcp::socket socket;

					Network::IBaseNetworkDriver* _IBaseNetworkDriver;

					bool Closed;
					bool Writing;
				public:

					PacketHeaderWrapper WritePacketHeader;
					PacketHeaderWrapper ReadPacketHeader;
					boost::asio::deadline_timer read_deadline_;
					boost::asio::deadline_timer write_deadline_;

					TCPSocketImpl(Network::IBaseNetworkDriver* netevents, boost::asio::ip::tcp::socket& s) : socket(std::move(s)), _IBaseNetworkDriver(netevents), read_deadline_(socket.get_io_service()), write_deadline_(socket.get_io_service()) {
						Init();
					}
					TCPSocketImpl(Network::IBaseNetworkDriver* netevents, boost::asio::io_service& io) : socket(io), _IBaseNetworkDriver(netevents), read_deadline_(socket.get_io_service()), write_deadline_(socket.get_io_service()) {
						Init();
					}

					void Init() {
						read_deadline_.expires_at(boost::posix_time::pos_infin);
						write_deadline_.expires_at(boost::posix_time::pos_infin);
						SocketStats = { 0 };
						Writing = Closed = false;
					}
					Network::SocketStats& get_SocketStats() { return SocketStats; }
					Network::SocketStats get_SocketStats() const { return SocketStats; }

					void StartReadTimer(int seconds) {
						if (seconds <= 0) read_deadline_.expires_at(boost::posix_time::pos_infin);
						else read_deadline_.expires_from_now(boost::posix_time::seconds(seconds));
					}
					void StartWriteTimer(int seconds) {
						if (seconds <= 0) write_deadline_.expires_at(boost::posix_time::pos_infin);
						else write_deadline_.expires_from_now(boost::posix_time::seconds(seconds));
					}
					boost::asio::ip::tcp::socket& get_socket() {
						return socket;
					}

					Network::IBaseNetworkDriver* get_IBaseNetworkDriver() {
						return _IBaseNetworkDriver;
					}
					char* get_ReadBuffer()
					{
						_IncomingBuffer.reserve(ReadPacketHeader.Payload_Length);
						return _IncomingBuffer.data();
					}
					size_t get_ReadBufferSize()
					{
						return ReadPacketHeader.Payload_Length;
					}
					bool writing() const {
						return Writing;
					}
					void writing(bool w) {
						Writing = w;
					}
					bool closed() const {
						return Closed || !socket.is_open();
					}
					void close() {
						try
						{
							std::cout << "Closing Socket" << std::endl;
							socket.shutdown(boost::asio::socket_base::shutdown_send);
							socket.close();
						}
						catch (...) {}//I dont care about exceptions when the socket is being closed!
						Closed = true;
					}
					bool OutGoingBuffer_empty() const {
						return _OutgoingPackets.empty();
					}
					void AddOutgoingPacket(std::shared_ptr<Packet> pac, unsigned int uncompressedsize) {
						_OutgoingPackets.push_back({ pac, uncompressedsize });
					}
					std::shared_ptr<Packet> GetNextWritePacket() {
						auto pac = _OutgoingPackets.front();
						_OutgoingPackets.pop_front();//remove the packet
						WritePacketHeader.Packet_Type = pac.Pack->Packet_Type;
						WritePacketHeader.Payload_Length = pac.Pack->Payload_Length;
						WritePacketHeader.UncompressedLength = pac.UncompressedLength;
						return pac.Pack;
					}
					
					Packet GetNextReadPacket() {
						//dont free the memory on this... Owned internally
						Packet p(ReadPacketHeader.Packet_Type, static_cast<size_t>(ReadPacketHeader.Payload_Length), std::move(Header), _IncomingBuffer.data(), false);
						Header.clear();//reset the header after move
						return p;
					}
					void UpdateReadStats() {
						SocketStats.TotalPacketReceived += 1;
						SocketStats.NetworkBytesReceived += ReadPacketHeader.Payload_Length;
						SocketStats.TotalBytesReceived += ReadPacketHeader.UncompressedLength;
					}
					void UpdateWriteStats(Packet& packet, size_t beforesize) {
						SocketStats.TotalPacketSent += 1;
						SocketStats.TotalBytesSent += beforesize;
						SocketStats.NetworkBytesSent += packet.Payload_Length;
					}
				};

			}
		}
	}
}


SL::Remote_Access_Library::Network::TCPSocket::TCPSocket(SL::Remote_Access_Library::Network::IBaseNetworkDriver * netevents, void * socket)
{
	std::cout << "TCPSocket()" << std::endl;
	_SocketImpl = new INTERNAL::TCPSocketImpl(netevents, *((boost::asio::ip::tcp::socket*)socket));
}
SL::Remote_Access_Library::Network::TCPSocket::TCPSocket(SL::Remote_Access_Library::Network::IBaseNetworkDriver * netevents, boost::asio::io_service & io_service)
{
	std::cout << "TCPSocket()" << std::endl;
	_SocketImpl = new INTERNAL::TCPSocketImpl(netevents, io_service);
}

SL::Remote_Access_Library::Network::TCPSocket::~TCPSocket() {
	delete _SocketImpl;
	std::cout << "~TCPSocket()" << std::endl;
}

SL::Remote_Access_Library::Network::Packet SL::Remote_Access_Library::Network::TCPSocket::compress(Packet& packet)
{
	auto maxsize = ZSTD_compressBound(packet.Payload_Length);
	Packet p(packet.Packet_Type, static_cast<unsigned int>(maxsize), std::move(packet.Header));
	p.Payload_Length = static_cast<unsigned int>(ZSTD_compress(p.Payload, maxsize, packet.Payload, static_cast<unsigned int>(packet.Payload_Length), 3));
	_SocketImpl->UpdateWriteStats(p, packet.Payload_Length);
	return p;
}
SL::Remote_Access_Library::Network::Packet SL::Remote_Access_Library::Network::TCPSocket::decompress(Packet& pack)
{
	Packet p(pack.Packet_Type, _SocketImpl->ReadPacketHeader.UncompressedLength, std::move(pack.Header));
	p.Payload_Length = static_cast<unsigned int>(ZSTD_decompress(p.Payload, _SocketImpl->ReadPacketHeader.UncompressedLength, pack.Payload, pack.Payload_Length));
	if (ZSTD_isError(p.Payload_Length) > 0) {
		std::cout << ZSTD_getErrorName(p.Payload_Length) << std::endl;
		return Packet(static_cast<unsigned int>(PACKET_TYPES::INVALID));//empty packet
	}
	_SocketImpl->UpdateReadStats();
	return p;
}
void SL::Remote_Access_Library::Network::TCPSocket::send(Packet& pack)
{
	auto self(shared_from_this());
	auto compack(std::make_shared<Packet>(std::move(compress(pack))));
	auto beforesize = pack.Payload_Length;
	_SocketImpl->get_socket().get_io_service().post([this, self, compack, beforesize]()
	{
		_SocketImpl->AddOutgoingPacket(compack, beforesize);
		if (!_SocketImpl->writing())
		{
			writeexpire_from_now(30);
			writeheader(_SocketImpl->GetNextWritePacket());
		}
	});
}
SL::Remote_Access_Library::Network::SocketStats SL::Remote_Access_Library::Network::TCPSocket::get_SocketStats() const {
	return _SocketImpl->get_SocketStats();
}
void SL::Remote_Access_Library::Network::TCPSocket::close()
{
	if (!_SocketImpl->closed()) {//prevent double calls
		_SocketImpl->get_IBaseNetworkDriver()->OnClose(shared_from_this());
		_SocketImpl->close();
	}
}

bool SL::Remote_Access_Library::Network::TCPSocket::closed() const
{
	return _SocketImpl->closed();
}

void SL::Remote_Access_Library::Network::TCPSocket::handshake()
{
	auto self(shared_from_this());
	_SocketImpl->get_IBaseNetworkDriver()->OnConnect(self);
	readheader();
}

void SL::Remote_Access_Library::Network::TCPSocket::readheader()
{
	readexpire_from_now(30);
	auto self(shared_from_this());
	boost::asio::async_read(_SocketImpl->get_socket(), boost::asio::buffer(&_SocketImpl->ReadPacketHeader, sizeof(_SocketImpl->ReadPacketHeader)), [self, this](boost::system::error_code ec, std::size_t len/*length*/)
	{
		if (!ec && !_SocketImpl->closed()) {
			assert(len == sizeof(_SocketImpl->ReadPacketHeader));
			readbody();
		}
		else close();
	});
}

void SL::Remote_Access_Library::Network::TCPSocket::readbody()
{
	readexpire_from_now(30);
	auto self(shared_from_this());
	auto p(_SocketImpl->get_ReadBuffer());
	auto size(_SocketImpl->get_ReadBufferSize());
	boost::asio::async_read(_SocketImpl->get_socket(), boost::asio::buffer(p, size), [self, this](boost::system::error_code ec, std::size_t len/*length*/)
	{
		if (!ec && !_SocketImpl->closed()) {
			assert(len == _SocketImpl->get_ReadBufferSize());
			_SocketImpl->get_IBaseNetworkDriver()->OnReceive(self, std::make_shared<Packet>(std::move(decompress(_SocketImpl->GetNextReadPacket()))));
			readheader();
		}
		else close();
	});
}

void SL::Remote_Access_Library::Network::TCPSocket::writeheader(std::shared_ptr<Packet> pack)
{
	auto self(shared_from_this());
	boost::asio::async_write(_SocketImpl->get_socket(), boost::asio::buffer(&_SocketImpl->WritePacketHeader, sizeof(_SocketImpl->WritePacketHeader)), [self, this, pack](boost::system::error_code ec, std::size_t byteswritten)
	{
		if (!ec && !_SocketImpl->closed())
		{
			assert(byteswritten == sizeof(_SocketImpl->WritePacketHeader));
			writebody(pack);
		}
		else close();
	});
}

void SL::Remote_Access_Library::Network::TCPSocket::writebody(std::shared_ptr<Packet> pack)
{
	auto self(shared_from_this());
	boost::asio::async_write(_SocketImpl->get_socket(), boost::asio::buffer(pack->Payload, pack->Payload_Length), [self, this, pack](boost::system::error_code ec, std::size_t byteswritten)
	{
		if (!ec && !_SocketImpl->closed())
		{
			assert(byteswritten == pack->Payload_Length);
			if (!_SocketImpl->OutGoingBuffer_empty()) {
				writeexpire_from_now(30);
				writeheader(_SocketImpl->GetNextWritePacket());
			}
			else {
				writeexpire_from_now(0);
				_SocketImpl->writing(false);
			}
		}
		else close();
	});
}




boost::asio::ip::tcp::socket& SL::Remote_Access_Library::Network::TCPSocket::get_socket() const
{
	return _SocketImpl->get_socket();
}

void SL::Remote_Access_Library::Network::TCPSocket::readexpire_from_now(int seconds)
{
	return;//DONT USE THIS YET
	auto self(shared_from_this());
	_SocketImpl->StartReadTimer(seconds);
	if (seconds >= 0) {
		_SocketImpl->read_deadline_.async_wait([this, self](boost::system::error_code ec) {  if (ec != boost::asio::error::operation_aborted) close(); });
	}
}

void SL::Remote_Access_Library::Network::TCPSocket::writeexpire_from_now(int seconds)
{
	return;//DONT USE THIS YET
	auto self(shared_from_this());
	_SocketImpl->StartWriteTimer(seconds);
	if (seconds >= 0) {
		_SocketImpl->write_deadline_.async_wait([this, self](boost::system::error_code ec) {  if (ec != boost::asio::error::operation_aborted) close(); });
	}
}

void SL::Remote_Access_Library::Network::TCPSocket::connect(const char * host, const char * port)
{
	auto self(shared_from_this());
	if (host == nullptr && port == nullptr) {
		handshake();
	}
	else {
		try
		{
			boost::asio::ip::tcp::resolver resolver(_SocketImpl->get_socket().get_io_service());
			boost::asio::ip::tcp::resolver::query query(host, port);
			auto endpoint = resolver.resolve(query);
			boost::asio::async_connect(_SocketImpl->get_socket(), endpoint, [self, this](boost::system::error_code ec, boost::asio::ip::tcp::resolver::iterator)
			{
				if (!_SocketImpl->closed()) {
					_SocketImpl->get_IBaseNetworkDriver()->OnConnect(self);
					readheader();
				}
				else close();
			});
		}
		catch (...) {
			close();
		}
	}
}
