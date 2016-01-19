#include "stdafx.h"
#include "IBaseNetworkDriver.h"
#include "Packet.h"

void SL::Remote_Access_Library::Network::IBaseNetworkDriver::OnConnect(const std::shared_ptr<ISocket>& socket)
{
	std::cout << "DEFAULT OnConnected Called, OVERRIDE THIS METHOD!" << std::endl;
}

void SL::Remote_Access_Library::Network::IBaseNetworkDriver::OnReceive(const ISocket * socket, std::shared_ptr<Packet>& packet)
{
	std::cout << "DEFAULT OnReceive Called! OVERRIDE THIS METHOD PacketType: " << (int)packet->header()->Packet_Type << " Size: " << packet->header()->PayloadLen << std::endl;
}

void SL::Remote_Access_Library::Network::IBaseNetworkDriver::OnClose(const ISocket * socket)
{
	std::cout << "DEFAULT OnClose Called! OVERRIDE THIS METHOD" << std::endl;
}
