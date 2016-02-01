#include "stdafx.h"
#include "HttpSocket.h"
#include "IBaseNetworkDriver.h"
#include "Packet.h"
#include "zstd.h"
#include <boost/asio.hpp>
#include <boost/iostreams/stream.hpp>

class http_headers
{
	std::string method;
	std::string url;
	std::string version;

	std::map<std::string, std::string> headers;

public:

	std::string get_response()
	{
		std::stringstream ssOut;

		if (url == "/")
		{
			std::string sHTML =
				"<html><body><h1>Hello World</h1><p>This is a web server in c++</p></body></html>";
			ssOut << "HTTP/1.1 200 OK\r\n";
			ssOut << "content-type: text/html\r\n";
			ssOut << "content-length: " << sHTML.length() << "\r\n\r\n";
			ssOut << sHTML;
		}
		else
		{
			std::string sHTML =
				"<html><body><h1>404 Not Found</h1><p>There's nothing here.</p></body></html>";
			ssOut << "HTTP/1.1 404 Not Found\r\n";
			ssOut << "content-type: text/html\r\n";
			ssOut << "content-length: " << sHTML.length() << "\r\n\r\n";
			ssOut << sHTML;
		}
		return ssOut.str();
	}

	int content_length()
	{
		auto request = headers.find("content-length");
		if (request != headers.end())
		{
			std::stringstream ssLength(request->second);
			int content_length;
			ssLength >> content_length;
			return content_length;
		}
		return 0;
	}

	void on_read_header(std::string line)
	{
		std::stringstream ssHeader(line);
		std::string headerName;
		std::getline(ssHeader, headerName, ':');

		std::string value;
		std::getline(ssHeader, value);
		headers[headerName] = value;
	}

	void on_read_request_line(std::string line)
	{
		std::stringstream ssRequestLine(line);
		ssRequestLine >> method;
		ssRequestLine >> url;
		ssRequestLine >> version;

		std::cout << "request for resource: " << url << std::endl;
	}
};

namespace SL {
	namespace Remote_Access_Library {
		namespace Network {

			//this namespace should only be accessed by the Core library and NEVER by a consumer of this library
			namespace INTERNAL {

				class HttpSocketImpl {
				public:
					boost::asio::streambuf _IncomingBuffer;
					http_headers Header;

					std::deque<std::shared_ptr<Network::Packet>> _OutgoingPackets;

					boost::asio::ip::tcp::socket socket;
					Network::IBaseNetworkDriver* NetworkEvents;
					Network::SocketStats SocketStats;
					bool Writing;
					bool Closed;

					HttpSocketImpl(Network::IBaseNetworkDriver* netevents, boost::asio::ip::tcp::socket& s) : socket(std::move(s)), NetworkEvents(netevents) {

						SocketStats = { 0 };
						Closed = Writing = false;
					}

				};

			}
		}
	}
}


std::shared_ptr<SL::Remote_Access_Library::Network::HttpSocket> SL::Remote_Access_Library::Network::HttpSocket::Create(SL::Remote_Access_Library::Network::IBaseNetworkDriver * netevents, void * socket)
{
	auto ret = std::make_shared<SL::Remote_Access_Library::Network::HttpSocket>(new INTERNAL::HttpSocketImpl(netevents, *((boost::asio::ip::tcp::socket*)socket)));
	ret->Start();
	return ret;
}

SL::Remote_Access_Library::Network::HttpSocket::HttpSocket(INTERNAL::HttpSocketImpl * impl) : _SocketImpl(impl)
{
	std::cout << "HttpSocket()" << std::endl;
}

SL::Remote_Access_Library::Network::HttpSocket::~HttpSocket()
{
	delete _SocketImpl;
	std::cout << "~HttpSocket()" << std::endl;
}

void SL::Remote_Access_Library::Network::HttpSocket::send(std::shared_ptr<Packet>& pack)
{
	auto self(shared_from_this());

	_SocketImpl->SocketStats.TotalPacketSent += 1;
	_SocketImpl->SocketStats.TotalBytesSent += pack->header()->PayloadLen;
	//auto compack = compressPacket(*pack, 1);
	_SocketImpl->SocketStats.NetworkBytesSent += pack->header()->PayloadLen;
	_SocketImpl->socket.get_io_service().post([this, self, pack]()
	{
		_SocketImpl->_OutgoingPackets.push_back(pack);
		if (!_SocketImpl->Writing)
		{
			write();
		}
	});
}

void SL::Remote_Access_Library::Network::HttpSocket::close()
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

SL::Remote_Access_Library::Network::SocketStats SL::Remote_Access_Library::Network::HttpSocket::get_SocketStats() const
{
	return _SocketImpl->SocketStats;
}

void SL::Remote_Access_Library::Network::HttpSocket::read_body()
{

		write_response();


}
void SL::Remote_Access_Library::Network::HttpSocket::write_response()
{
	std::shared_ptr<std::string> str = std::make_shared<std::string>(_SocketImpl->Header.get_response());
	auto self(shared_from_this());
	boost::asio::async_write(_SocketImpl->socket, boost::asio::buffer(str->c_str(), str->length()), [self, str](const boost::system::error_code e, std::size_t s)
	{
		std::cout << "done" << std::endl;

	});
}

void SL::Remote_Access_Library::Network::HttpSocket::read_header()
{
	auto self(shared_from_this());
	boost::asio::async_read_until(_SocketImpl->socket, _SocketImpl->_IncomingBuffer, "\r\n\r\n", [this, self](const boost::system::error_code ec, std::size_t s)
	{
		if (!ec) {
			size_t num_additional_bytes = _SocketImpl->_IncomingBuffer.size() - s;
			std::string line, ignore;
			std::istream stream{ &_SocketImpl->_IncomingBuffer };
			std::getline(stream, line, '\r');
			std::getline(stream, ignore, '\n');
			_SocketImpl->Header.on_read_request_line(line);
			while (std::getline(stream, line, '\r')) {
				_SocketImpl->Header.on_read_header(line);
				std::getline(stream, ignore, '\n');
			}
		
			if (_SocketImpl->Header.content_length() == 0)
			{
				read_body();
			}
			else {
				std::cout << "done" << std::endl;
			}
		}
		else close();
	});
}


void SL::Remote_Access_Library::Network::HttpSocket::write()
{
	auto packet(_SocketImpl->_OutgoingPackets.front());
	_SocketImpl->_OutgoingPackets.pop_front();//remove the packet
	auto self(shared_from_this());
	_SocketImpl->Writing = true;
	boost::asio::async_write(_SocketImpl->socket, boost::asio::buffer(packet->data(), packet->header()->PayloadLen), [self, packet, this](boost::system::error_code ec, std::size_t byteswritten)
	{
		if (!ec && !_SocketImpl->Closed)
		{
			assert(byteswritten == packet->header()->PayloadLen);
			_SocketImpl->_OutgoingPackets.size();
			if (!_SocketImpl->_OutgoingPackets.empty())
			{
				write();
			}
			else {
				_SocketImpl->Writing = false;
			}
		}
		else close();
	});
}

void SL::Remote_Access_Library::Network::HttpSocket::Start()
{
	auto self(shared_from_this());
	_SocketImpl->NetworkEvents->OnConnect(self);
	read_header();
}

