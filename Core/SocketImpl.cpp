#include "stdafx.h"
#include "SocketImpl.h"
#include "zstd.h"

std::shared_ptr<SL::Remote_Access_Library::Network::Packet> decompressPacket(SL::Remote_Access_Library::Network::PacketHeader& header, std::vector<char>& buffer)
{
	auto p = SL::Remote_Access_Library::Network::Packet::CreatePacket(header);
	auto dst = ZSTD_decompress(p->data(), header.UnCompressedlen, buffer.data(), header.PayloadLen);

	p->header()->PayloadLen = static_cast<unsigned int>(dst);
	std::cout << "decompress from: " << p->header()->UnCompressedlen << " To " << p->header()->PayloadLen << std::endl;
	p->header()->UnCompressedlen = 0;
	return p;
}

std::shared_ptr<SL::Remote_Access_Library::Network::Packet> compressPacket(SL::Remote_Access_Library::Network::Packet& pack, int compressionlevel)
{
	assert(compressionlevel >= 1 && compressionlevel <= 19);

	SL::Remote_Access_Library::Network::PacketHeader header;
	header.UnCompressedlen = pack.header()->PayloadLen;
	header.PayloadLen = ZSTD_compressBound(pack.header()->PayloadLen);
	header.Packet_Type = pack.header()->Packet_Type;
	auto p = SL::Remote_Access_Library::Network::Packet::CreatePacket(header);
	p->header()->PayloadLen = static_cast<unsigned int>(ZSTD_compress(p->data(), header.PayloadLen, pack.data(), pack.header()->PayloadLen, compressionlevel));

	std::cout << "compress from: " << p->header()->UnCompressedlen << " To " << p->header()->PayloadLen << std::endl;
	return p;
}

char * SL::Remote_Access_Library::INTERNAL::SocketImpl::get_Buffer()
{
	_IncomingBuffer.reserve(PacketHeader.PayloadLen);
	return _IncomingBuffer.data();
}

size_t SL::Remote_Access_Library::INTERNAL::SocketImpl::get_BufferSize()
{
	return PacketHeader.PayloadLen;
}

bool SL::Remote_Access_Library::INTERNAL::SocketImpl::PacketsWaitingToBeSent() const
{
	return !_OutgoingPackets.empty();
}

void SL::Remote_Access_Library::INTERNAL::SocketImpl::PushToOutgoing(std::shared_ptr<Network::Packet>& pack)
{
	SocketStats.TotalPacketSent += 1;
	SocketStats.TotalBytesSent += pack->header()->PayloadLen;
	auto compack = compressPacket(*pack, 1);

	SocketStats.NetworkBytesSent += compack->header()->PayloadLen;
	std::lock_guard<std::mutex> lock(_OutgoingPacketsLock);
	_OutgoingPackets.push_back(compack);
}
std::shared_ptr<SL::Remote_Access_Library::Network::Packet> SL::Remote_Access_Library::INTERNAL::SocketImpl::PopFromOutgoing()
{
	std::lock_guard<std::mutex> lock(_OutgoingPacketsLock);
	auto ret = _OutgoingPackets.back();
	_OutgoingPackets.pop_back();
	return ret;
}
std::shared_ptr<SL::Remote_Access_Library::Network::Packet> SL::Remote_Access_Library::INTERNAL::SocketImpl::get_IncomingPacket()
{

	SocketStats.TotalPacketReceived += 1;
	SocketStats.NetworkBytesReceived += PacketHeader.PayloadLen;
	SocketStats.TotalBytesReceived += PacketHeader.UnCompressedlen;
	return decompressPacket(PacketHeader, _IncomingBuffer);
}
