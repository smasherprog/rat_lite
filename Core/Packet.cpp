#include <vector>
#include <mutex>
#include <algorithm>
#include "Packet.h"
#include "BufferManager.h"

SL::Remote_Access_Library::Utilities::BufferManager PacketBufferManager;

#define HEADERSIZE sizeof(PacketHeader)

std::shared_ptr<SL::Remote_Access_Library::Network::Packet> SL::Remote_Access_Library::Network::Packet::CreatePacket(PACKET_TYPES ptype, size_t packetsize) {
	return std::make_shared<Packet>(PacketBufferManager.AquireBuffer(packetsize + HEADERSIZE));
}

SL::Remote_Access_Library::Network::Packet::Packet(Packet&& pac) : _Data(std::move(pac._Data)) {}
SL::Remote_Access_Library::Network::Packet& SL::Remote_Access_Library::Network::Packet::operator=(const Packet&& pac) {
	_Data = std::move(pac._Data);
	return *this;
}

SL::Remote_Access_Library::Network::Packet::Packet(std::shared_ptr<Utilities::Blk>& blk): _Data(blk) {}

SL::Remote_Access_Library::Network::Packet::~Packet()
{
	PacketBufferManager.ReleaseBuffer(_Data);
}


char* SL::Remote_Access_Library::Network::Packet::get_Payload() {
	if (!_Data) return nullptr;
	return getData(_Data) + HEADERSIZE;
}

size_t SL::Remote_Access_Library::Network::Packet::get_Payloadsize() {
	if (!_Data) return 0;
	auto ptr = (PacketHeader*)getData(_Data);
	return ptr->PayloadLen;
}

SL::Remote_Access_Library::Network::PACKET_TYPES SL::Remote_Access_Library::Network::Packet::get_Packettype() {
	if (!_Data) return PACKET_TYPES::INVALID;
	auto ptr = (PacketHeader*)getData(_Data);
	return (PACKET_TYPES)ptr->Packet_Type;
}


