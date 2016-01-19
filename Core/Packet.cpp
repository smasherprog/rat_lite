#include "stdafx.h"
#include "Packet.h"



struct SL::Remote_Access_Library::Network::Packet_Impl {
	PacketHeader h;
};
std::shared_ptr<SL::Remote_Access_Library::Network::Packet> SL::Remote_Access_Library::Network::Packet::CreatePacket(PacketHeader header) {
	Packet_Impl p;
	p.h = header;
	return std::make_shared<SL::Remote_Access_Library::Network::Packet>(p);
}
SL::Remote_Access_Library::Network::Packet::Packet(Packet_Impl& priv) {
	_PacketHeader = priv.h;
	_Data= std::make_unique<char[]>(_PacketHeader.PayloadLen);
}
SL::Remote_Access_Library::Network::Packet::~Packet()
{
}

char * SL::Remote_Access_Library::Network::Packet::data() const
{
	return _Data.get();
}

SL::Remote_Access_Library::Network::PacketHeader * SL::Remote_Access_Library::Network::Packet::header()
{
	return &_PacketHeader;
}
