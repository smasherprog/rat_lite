#include "stdafx.h"
#include <deque>
#include "Socket.h"
//namspaceing TOO much
using boost::asio::ip::tcp;

struct HEADER {
	unsigned int PayloadLen = 0;
	unsigned char Packet_Type = PACKET_TYPES::INVALID;
};

struct SL::Remote_Access_Library::Network::SocketImpl {
	boost::asio::io_service& io_service;
	boost::asio::ip::tcp::socket socket;
	std::function<void(const SL::Remote_Access_Library::Network::Socket*, const char*, size_t)> _onreceive;
	std::function<void(const SL::Remote_Access_Library::Network::Socket*)> _onclose;
	HEADER PacketHeader;
	std::deque<chat_message>
};

void do_connect(std::shared_ptr<SL::Remote_Access_Library::Network::Socket> ptr, SL::Remote_Access_Library::Network::SocketImpl* impl, tcp::resolver::iterator endpoint_iterator);
void do_read_header(std::shared_ptr<SL::Remote_Access_Library::Network::Socket> ptr, SL::Remote_Access_Library::Network::SocketImpl* impl);

SL::Remote_Access_Library::Network::Socket::Socket(boost::asio::io_service& io_service,
	boost::asio::ip::tcp::socket& socket,
	std::function<void(const Socket*, const char*, size_t)>& onreceive,
	std::function<void(const Socket*)>& onclose)
{
	_SocketImpl = std::make_unique<SocketImpl>(io_service, io_service, onreceive, onclose);
	do_read_header(shared_from_this());
}

SL::Remote_Access_Library::Network::Socket::Socket(boost::asio::io_service & io_service,
	tcp::resolver::iterator endpoint_iterator,
	std::function<void(const Socket*, const char*, size_t)>& onreceive,
	std::function<void(const Socket*)>& onclose)
{
	_SocketImpl = std::make_unique<SocketImpl>(io_service, io_service, onreceive, onclose);
	do_connect(shared_from_this(), _SocketImpl.get(), endpoint_iterator);

}
void SL::Remote_Access_Library::Network::Socket::send(const char * data, size_t num_bytes)
{
	auto ptr = _SocketImpl.get();
	ptr->io_service.post(
		[this, msg]()
	{
		bool write_in_progress = !write_msgs_.empty();
		write_msgs_.push_back(msg);
		if (!write_in_progress)
		{
			do_write();
		}
	});
}

void SL::Remote_Access_Library::Network::Socket::close()
{
	auto ptr = _SocketImpl.get();
	ptr->io_service.post([this, ptr]() {
		ptr->_onclose(this);
		ptr->socket.close();
	});
}


void do_connect(std::shared_ptr<SL::Remote_Access_Library::Network::Socket> ptr, SL::Remote_Access_Library::Network::SocketImpl* impl, tcp::resolver::iterator endpoint_iterator) {
	boost::asio::async_connect(impl->socket, endpoint_iterator,
		[ptr, impl](boost::system::error_code ec, tcp::resolver::iterator)
	{
		if (!ec)
		{
			do_read_header(ptr, impl);
		}
	});
}
void do_read_header(std::shared_ptr<SL::Remote_Access_Library::Network::Socket> ptr, SL::Remote_Access_Library::Network::SocketImpl* impl) {

	boost::asio::async_read(impl->socket,
		boost::asio::buffer(&impl->PacketHeader, sizeof(impl->PacketHeader)),
		[ptr, impl](boost::system::error_code ec, std::size_t /*length*/)
	{
		if (!ec)
		{
			do_read_body();
		}
		else
		{
			room_.leave(shared_from_this());
		}
	});
}




