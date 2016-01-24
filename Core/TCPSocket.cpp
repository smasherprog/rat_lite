#include "stdafx.h"
#include "TCPSocket.h"
#include "IBaseNetworkDriver.h"
#include "Packet.h"
#include "zstd.h"


namespace SL {
	namespace Remote_Access_Library {
		namespace Network {

			class IO_Runner {
			public:
				boost::asio::io_service io_service;
				IO_Runner() :work(io_service) {
					io_servicethread = std::thread([this]() {
						std::cout << "Starting io_service for Connecto Factory" << std::endl;
						io_service.run();
						std::cout << "stopping io_service for Connecto Factory" << std::endl;
					});
				}
				~IO_Runner()
				{
					io_service.stop();
					io_servicethread.join();
				}
			private:
				std::thread io_servicethread;
				boost::asio::io_service::work work;
			};
		}

		//this namespace should only be accessed by the Core library and NEVER by a consumer of this library
		namespace INTERNAL {
			class ListinerImpl : public Network::IO_Runner {
			public:
				ListinerImpl(const boost::asio::ip::tcp::endpoint& endpoint, Network::IBaseNetworkDriver* netevents) :
					NetworkEvents_(netevents),
					acceptor_(io_service, endpoint),
					socket_(io_service) {}
				~ListinerImpl() {
					socket_.close();
					acceptor_.close();
				}

				boost::asio::ip::tcp::acceptor acceptor_;
				boost::asio::ip::tcp::socket socket_;
				Network::IBaseNetworkDriver* NetworkEvents_;
			};

			class TCPSocketImpl {
			public:
				std::vector<char> _IncomingBuffer;
				std::deque<std::shared_ptr<Network::Packet>> _OutgoingPackets;
				std::mutex _OutgoingPacketsLock;
				boost::asio::io_service& io_service;
				boost::asio::ip::tcp::socket socket;
				Network::IBaseNetworkDriver* NetworkEvents;
				Network::PacketHeader PacketHeader;
				Network::SocketStats SocketStats;
				bool Writing;
				bool Closed;

				TCPSocketImpl(boost::asio::io_service& io, Network::IBaseNetworkDriver* netevents, boost::asio::ip::tcp::socket&& s) : io_service(io), socket(std::move(s)), NetworkEvents(netevents) {
					SocketStats = { 0 };
					Closed = Writing = false;
				}
				TCPSocketImpl(boost::asio::io_service& io, Network::IBaseNetworkDriver* netevents) : io_service(io), socket(io), NetworkEvents(netevents) {
					SocketStats = { 0 };
					Closed = Writing = false;
				}



				std::shared_ptr<Network::Packet> decompressPacket(Network::PacketHeader& header, std::vector<char>& buffer)
				{
					auto p = Network::Packet::CreatePacket(header);
					auto dst = ZSTD_decompress(p->data(), header.UnCompressedlen, buffer.data(), header.PayloadLen);
					if (ZSTD_isError(dst) > 0) {
						std::string error = ZSTD_getErrorName(dst);
						int k = 0;
					}
					p->header()->PayloadLen = static_cast<unsigned int>(dst);
				//	std::cout << "decompress from: " << p->header()->UnCompressedlen << " To " << p->header()->PayloadLen << std::endl;
					p->header()->UnCompressedlen = 0;

					return p;
				}

				std::shared_ptr<Network::Packet> compressPacket(Network::Packet& pack, int compressionlevel)
				{
					assert(compressionlevel >= 1 && compressionlevel <= 19);

					Network::PacketHeader header;
					header.UnCompressedlen = pack.header()->PayloadLen;
					header.PayloadLen = ZSTD_compressBound(pack.header()->PayloadLen);
					header.Packet_Type = pack.header()->Packet_Type;
					auto p = Network::Packet::CreatePacket(header);
					p->header()->PayloadLen = static_cast<unsigned int>(ZSTD_compress(p->data(), header.PayloadLen, pack.data(), pack.header()->PayloadLen, compressionlevel));

				//	std::cout << "compress from: " << p->header()->UnCompressedlen << " To " << p->header()->PayloadLen << std::endl;
					//assert(p->header()->PayloadLen < 1024 * 1024 * 48);//sanitfy check for packet sizes
					return p;
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


				void TCPSocketImpl::PushToOutgoing(std::shared_ptr<Network::Packet>& pack)
				{
					SocketStats.TotalPacketSent += 1;
					SocketStats.TotalBytesSent += pack->header()->PayloadLen;
					auto compack = compressPacket(*pack, 1);

					SocketStats.NetworkBytesSent += compack->header()->PayloadLen;
					std::lock_guard<std::mutex> lock(_OutgoingPacketsLock);
					_OutgoingPackets.push_back(compack);
				}
				std::shared_ptr<Network::Packet> TCPSocketImpl::PopFromOutgoing()
				{
					std::lock_guard<std::mutex> lock(_OutgoingPacketsLock);
					auto ret = _OutgoingPackets.back();
					_OutgoingPackets.pop_back();
					return ret;
				}
				std::shared_ptr<Network::Packet> TCPSocketImpl::get_IncomingPacket()
				{

					SocketStats.TotalPacketReceived += 1;
					SocketStats.NetworkBytesReceived += PacketHeader.PayloadLen;
					SocketStats.TotalBytesReceived += PacketHeader.UnCompressedlen;
					return decompressPacket(PacketHeader, _IncomingBuffer);
				}

			};

		}
	}
}












std::shared_ptr<SL::Remote_Access_Library::INTERNAL::TCPSocketImpl> Create(boost::asio::io_service& io, SL::Remote_Access_Library::Network::IBaseNetworkDriver* netevents, boost::asio::ip::tcp::socket&& s) {
	return std::make_shared<SL::Remote_Access_Library::INTERNAL::TCPSocketImpl>(io, netevents, std::move(s));
}
std::shared_ptr<SL::Remote_Access_Library::INTERNAL::TCPSocketImpl> Create(boost::asio::io_service& io, SL::Remote_Access_Library::Network::IBaseNetworkDriver* netevents) {
	return std::make_shared<SL::Remote_Access_Library::INTERNAL::TCPSocketImpl>(io, netevents);
}

SL::Remote_Access_Library::Network::TCPSocket::TCPSocket(std::shared_ptr<INTERNAL::TCPSocketImpl>& impl) : _SocketImpl(impl) {
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
		if (!_SocketImpl->Writing)
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
	if (!_SocketImpl->Closed) {//prevent double calls
		_SocketImpl->Closed = true;
		_SocketImpl->NetworkEvents->OnClose(this);
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
	boost::asio::async_read(_SocketImpl->socket, boost::asio::buffer(&_SocketImpl->PacketHeader, sizeof(_SocketImpl->PacketHeader)), [self, this](boost::system::error_code ec, std::size_t len/*length*/)
	{
		if (!ec) {
			assert(len == sizeof(_SocketImpl->PacketHeader));
			do_read_body();
		}
		else close();
	});
}
void SL::Remote_Access_Library::Network::TCPSocket::do_read_body() {
	auto self(shared_from_this());
	auto p(_SocketImpl->get_Buffer());
	auto size(_SocketImpl->get_BufferSize());
	boost::asio::async_read(_SocketImpl->socket, boost::asio::buffer(p, size), [self, this](boost::system::error_code ec, std::size_t len/*length*/)
	{
		if (!ec) {
			assert(len == _SocketImpl->get_BufferSize());

			_SocketImpl->NetworkEvents->OnReceive(this, _SocketImpl->get_IncomingPacket());
			do_read_header();
		}
		else close();
	});
}

void SL::Remote_Access_Library::Network::TCPSocket::do_write_header() {
	auto packet(_SocketImpl->PopFromOutgoing());
	auto self(shared_from_this());
	_SocketImpl->Writing = true;
	boost::asio::async_write(_SocketImpl->socket, boost::asio::buffer(packet->header(), sizeof(SL::Remote_Access_Library::Network::PacketHeader)), [self, packet, this](boost::system::error_code ec, std::size_t byteswritten)
	{
		if (!ec)
		{
			assert(byteswritten == sizeof(_SocketImpl->PacketHeader));
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
			assert(byteswritten == packet->header()->PayloadLen);
			_SocketImpl->_OutgoingPackets.size();
			if (!_SocketImpl->_OutgoingPackets.empty())
			{
				do_write_header();
			}
			else {
				_SocketImpl->Writing = false;
			}
		}
		else close();
	});
}




/*
/
/
/
/	LISTENER BELOW
/
/
/
/
/
*/



void do_accept(std::shared_ptr<SL::Remote_Access_Library::Network::Listener> ptr, SL::Remote_Access_Library::INTERNAL::ListinerImpl* impl) {
	impl->acceptor_.async_accept(impl->socket_, [ptr, impl](boost::system::error_code ec)
	{
		if (!ec)
		{
			auto sock = std::make_shared<SL::Remote_Access_Library::Network::TCPSocket>(std::make_shared<SL::Remote_Access_Library::INTERNAL::TCPSocketImpl>(impl->io_service, impl->NetworkEvents_, std::move(impl->socket_)));
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


std::shared_ptr<SL::Remote_Access_Library::Network::Listener> SL::Remote_Access_Library::Network::Listener::CreateListener(unsigned short port, Network::IBaseNetworkDriver* netevents) {
	return std::make_shared<Network::Listener>(new INTERNAL::ListinerImpl(boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port), netevents));
}
