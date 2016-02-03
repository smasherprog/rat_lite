#include "stdafx.h"
#include "WebSocket.h"
#include <boost\asio.hpp>
#include "Packet.h"

namespace SL {
	namespace Remote_Access_Library {
		namespace Network {
			namespace INTERNAL {

				class WebSocketSocketImpl {
				public:
					WebSocketSocketImpl(boost::asio::ip::tcp::socket& s) : socket(s) {}
					boost::asio::streambuf _IncomingBuffer;
	
					boost::asio::ip::tcp::socket& socket;
				};

			}
		}
	}
}


SL::Remote_Access_Library::Network::WebSocket::WebSocket(IBaseNetworkDriver * netevents, void* socket) : TCPSocket(netevents, socket)
{
	_WebSocketSocketImpl = new INTERNAL::WebSocketSocketImpl(*((boost::asio::ip::tcp::socket*)socket));
}

SL::Remote_Access_Library::Network::WebSocket::~WebSocket()
{
	delete _WebSocketSocketImpl;
}

void SL::Remote_Access_Library::Network::WebSocket::readheader()
{
	//nothing for now
}

void SL::Remote_Access_Library::Network::WebSocket::readbody()
{
	//nothing for now
}

void SL::Remote_Access_Library::Network::WebSocket::writeheader()
{	
	//skip the header writing and go straight to writing the payload
	writebody();
}

std::shared_ptr<SL::Remote_Access_Library::Network::Packet> SL::Remote_Access_Library::Network::WebSocket::decompress(PacketHeader & header, char * buffer)
{
	//no decompression yet
	auto p = SL::Remote_Access_Library::Network::Packet::CreatePacket(header);
	memcpy(p->data(), buffer, header.PayloadLen);
	p->header()->UnCompressedlen = 0;
	return p;
}

std::shared_ptr<SL::Remote_Access_Library::Network::Packet> SL::Remote_Access_Library::Network::WebSocket::compress(std::shared_ptr<Packet>& packet)
{
	return packet;//no compression yet
}
