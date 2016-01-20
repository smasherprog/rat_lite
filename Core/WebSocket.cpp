#include "stdafx.h"
#include "WebSocket.h"

SL::Remote_Access_Library::Network::WebSocket::WebSocket()
{
}

SL::Remote_Access_Library::Network::WebSocket::~WebSocket()
{
}


void SL::Remote_Access_Library::Network::WebSocket::send(std::shared_ptr<Packet>& pack)
{
}

void SL::Remote_Access_Library::Network::WebSocket::close()
{
}

SL::Remote_Access_Library::Network::SocketStats SL::Remote_Access_Library::Network::WebSocket::get_SocketStats() const {
	SocketStats ret = { 0 };
	return ret;
}