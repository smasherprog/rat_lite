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
				};

			}

		}
	}
}

SL::Remote_Access_Library::Network::HttpSocket::HttpSocket(IBaseNetworkDriver * netevents, void* socket) : TCPSocket(netevents, socket)
{

	std::cout << "HttpSocket()" << std::endl;
	_HttpSocketImpl = new INTERNAL::HttpSocketImpl();
}

SL::Remote_Access_Library::Network::HttpSocket::~HttpSocket()
{
	delete _HttpSocketImpl;
	std::cout << "~HttpSocket()" << std::endl;
}



void SL::Remote_Access_Library::Network::HttpSocket::readheader()
{
	auto self(shared_from_this());
	boost::asio::async_read_until(get_socket(), _HttpSocketImpl->_IncomingBuffer, "\r\n\r\n", [this, self](const boost::system::error_code ec, std::size_t s)
	{
		if (!ec) {
			size_t num_additional_bytes = _HttpSocketImpl->_IncomingBuffer.size() - s;
			std::string line, ignore;
			std::istream stream{ &_HttpSocketImpl->_IncomingBuffer };
			std::getline(stream, line, '\r');
			std::getline(stream, ignore, '\n');
			_HttpSocketImpl->Header.on_read_request_line(line);
			while (std::getline(stream, line, '\r')) {
				_HttpSocketImpl->Header.on_read_header(line);
				std::getline(stream, ignore, '\n');
			}

			if (_HttpSocketImpl->Header.content_length() == 0)
			{
			
			}
			else {
				std::cout << "done" << std::endl;
			}
		}
		else {
			std::cout << "Server: Error in connection. " << "Error: " << ec << ", error message: " << ec.message() << std::endl;
			close();
		}
	});
}

void SL::Remote_Access_Library::Network::HttpSocket::readbody()
{
	//nothing for now
}

void SL::Remote_Access_Library::Network::HttpSocket::writeheader(std::shared_ptr<Packet> pack)
{
	//skip the header writing and go straight to writing the payload
	writebody(pack);
}

std::shared_ptr<SL::Remote_Access_Library::Network::Packet> SL::Remote_Access_Library::Network::HttpSocket::decompress(PacketHeader& header, char * buffer)
{
	//no decompression yet
	auto p = SL::Remote_Access_Library::Network::Packet::CreatePacket(header);
	memcpy(p->data(), buffer, header.PayloadLen);
	p->header()->UnCompressedlen = 0;
	return p;
}

std::shared_ptr<SL::Remote_Access_Library::Network::Packet> SL::Remote_Access_Library::Network::HttpSocket::compress(std::shared_ptr<Packet>& packet)
{
	return packet;//no compression yet
}



