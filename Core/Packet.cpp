#include "stdafx.h"
#include "Packet.h"
#include "zstd.h"
#include "Internal_Impls.h"

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
	_Data= Remote_Access_Library::INTERNAL::_Buffer.AquireBuffer(ZSTD_compressBound(_PacketHeader.PayloadLen));
	_PacketHeader.UnCompressedlen = 0;
}
SL::Remote_Access_Library::Network::Packet::~Packet()
{
	Remote_Access_Library::INTERNAL::_Buffer.ReleaseBuffer(_Data);
}
