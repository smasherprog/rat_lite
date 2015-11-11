#include "stdafx.h"
#include "CommonNetwork.h"
#include "Packet.h"

void SL::Remote_Access_Library::Network::DefaultOnConnect(const std::shared_ptr<SL::Remote_Access_Library::Network::Socket> ptr) {
	std::cout << "DEFAULT OnConnected Called!" << std::endl;
};
void SL::Remote_Access_Library::Network::DefaultOnReceive(const std::shared_ptr<SL::Remote_Access_Library::Network::Socket> ptr, std::shared_ptr<SL::Remote_Access_Library::Network::Packet>& pac) {
	std::cout << "DEFAULT OnReceive Called! PacketType: " << (int)pac->header()->Packet_Type << " Size: " << pac->header()->PayloadLen << std::endl;
};
void SL::Remote_Access_Library::Network::DefaultOnClose(const std::shared_ptr<SL::Remote_Access_Library::Network::Socket> ptr) {
	std::cout << "DEFAULT OnClose Called!" << std::endl;
};