#include "stdafx.h"
#include "BaseNetworkDriver.h"
#include "Packet.h"

void SL::Remote_Access_Library::Network::BaseNetworkDriver::OnConnect(const std::shared_ptr<Socket>& socket)
{
	std::cout << "DEFAULT OnConnected Called, OVERRIDE THIS METHOD!" << std::endl;
}

void SL::Remote_Access_Library::Network::BaseNetworkDriver::OnReceive(const Socket * socket, std::shared_ptr<Packet>& packet)
{
	std::cout << "DEFAULT OnReceive Called! OVERRIDE THIS METHOD PacketType: " << (int)packet->header()->Packet_Type << " Size: " << packet->header()->PayloadLen << std::endl;
}

void SL::Remote_Access_Library::Network::BaseNetworkDriver::OnClose(const Socket * socket)
{
	std::cout << "DEFAULT OnClose Called! OVERRIDE THIS METHOD" << std::endl;
}
