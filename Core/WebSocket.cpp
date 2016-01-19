#include "stdafx.h"
#include "WebSocket.h"

SL::Remote_Access_Library::Network::WebSocket::WebSocket()
{
}

SL::Remote_Access_Library::Network::WebSocket::~WebSocket()
{
}

void SL::Remote_Access_Library::Network::WebSocket::send(std::shared_ptr<Packet>& pack, std::function<void()> on_sent_callback)
{
}

void SL::Remote_Access_Library::Network::WebSocket::send(std::shared_ptr<Packet>& pack)
{
}

void SL::Remote_Access_Library::Network::WebSocket::close()
{
}

unsigned int SL::Remote_Access_Library::Network::WebSocket::get_OutgoingPacketCount() const
{
	return 0;
}

unsigned int SL::Remote_Access_Library::Network::WebSocket::get_OutgoingByteCount() const
{
	return 0;
}
