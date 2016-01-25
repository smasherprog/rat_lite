#include "stdafx.h"
#include "TCPSocket.h"
#include "IBaseNetworkDriver.h"
#include "Packet.h"
#include "zstd.h"
#include "ThreadPool.h"

namespace SL {
	namespace Remote_Access_Library {
		namespace Network {

			class IO_Runner {
			public:
				boost::asio::io_service io_service;
				Utilities::ThreadPool _ThreadPool;

				IO_Runner() :work(io_service), _ThreadPool(1) {
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

				boost::asio::io_service& io_service;
				Utilities::ThreadPool& _ThreadPool;
				boost::asio::ip::tcp::socket socket;
				Network::IBaseNetworkDriver* NetworkEvents;
				Network::PacketHeader PacketHeader;
				Network::SocketStats SocketStats;
				bool Writing;
				bool Closed;

				TCPSocketImpl(Utilities::ThreadPool& tpool, boost::asio::io_service& io, Network::IBaseNetworkDriver* netevents, boost::asio::ip::tcp::socket&& s) :_ThreadPool(tpool), io_service(io), socket(std::move(s)), NetworkEvents(netevents) {
					SocketStats = { 0 };
					Closed = Writing = false;
				}
				TCPSocketImpl(Utilities::ThreadPool& tpool, boost::asio::io_service& io, Network::IBaseNetworkDriver* netevents) :_ThreadPool(tpool), io_service(io), socket(io), NetworkEvents(netevents) {
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
	p->header()->PayloadLen = static_cast<unsigned int>(ZSTD_compress(p->data(), header.PayloadLen, pack.data(), pack.header()->PayloadLen, compressionlevel));

	//assert(p->header()->PayloadLen < 1024 * 1024 * 48);//sanitfy check for packet sizes
	return p;
}






std::shared_ptr<SL::Remote_Access_Library::INTERNAL::TCPSocketImpl> Create(SL::Remote_Access_Library::Utilities::ThreadPool& tpool, boost::asio::io_service& io, SL::Remote_Access_Library::Network::IBaseNetworkDriver* netevents, boost::asio::ip::tcp::socket&& s) {
	return std::make_shared<SL::Remote_Access_Library::INTERNAL::TCPSocketImpl>(tpool, io, netevents, std::move(s));
}
std::shared_ptr<SL::Remote_Access_Library::INTERNAL::TCPSocketImpl> Create(SL::Remote_Access_Library::Utilities::ThreadPool& tpool, boost::asio::io_service& io, SL::Remote_Access_Library::Network::IBaseNetworkDriver* netevents) {
	return std::make_shared<SL::Remote_Access_Library::INTERNAL::TCPSocketImpl>(tpool, io, netevents);
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

	auto socketimpl = Create(_io_runner._ThreadPool, _io_runner.io_service, netevents);
	auto socket = std::make_shared<TCPSocket>(socketimpl);
	socket->connect(host, port);
	return socket;
}




void SL::Remote_Access_Library::Network::TCPSocket::send(std::shared_ptr<Packet>& pack)
{
	auto self(shared_from_this());

	_SocketImpl->_ThreadPool.Enqueue([this, self, pack]() {

		_SocketImpl->SocketStats.TotalPacketSent += 1;
		_SocketImpl->SocketStats.TotalBytesSent += pack->header()->PayloadLen;
		auto compack = compressPacket(*pack, 1);
		_SocketImpl->SocketStats.NetworkBytesSent += compack->header()->PayloadLen;

		_SocketImpl->io_service.post([this, self, compack]()
		{
			_SocketImpl->_OutgoingPackets.push_back(compack);
			if (!_SocketImpl->Writing)
			{
				Write();
			}
		});
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
		Read();
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

				if (!ec && !_SocketImpl->Closed){
					_SocketImpl->NetworkEvents->OnConnect(self);
					Read();
				}
				else close();
			});
		}
		catch (...) {
			_SocketImpl->NetworkEvents->OnClose(this);
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
					auto decpack = decompressPacket(_SocketImpl->PacketHeader, _SocketImpl->_IncomingBuffer);

					_SocketImpl->_ThreadPool.Enqueue([self, this, decpack]() {
						auto tmpreceveier(decpack);
						if (!_SocketImpl->Closed) {
							_SocketImpl->NetworkEvents->OnReceive(this, tmpreceveier);
						}
					});
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
			auto sock = std::make_shared<SL::Remote_Access_Library::Network::TCPSocket>(std::make_shared<SL::Remote_Access_Library::INTERNAL::TCPSocketImpl>(impl->_ThreadPool, impl->io_service, impl->NetworkEvents_, std::move(impl->socket_)));
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
