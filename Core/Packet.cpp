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
	_Data= Remote_Access_Library::INTERNAL::_PacketBuffer.AquireBuffer(ZSTD_compressBound(std::max(_PacketHeader.PayloadLen, _PacketHeader.UnCompressedlen)));
}
SL::Remote_Access_Library::Network::Packet::~Packet()
{
	Remote_Access_Library::INTERNAL::_PacketBuffer.ReleaseBuffer(_Data);
}

void SL::Remote_Access_Library::Network::Packet::compress()
{
	if (_PacketHeader.UnCompressedlen > 0) return;//allready compressed
	auto maxsize = ZSTD_compressBound(_PacketHeader.PayloadLen);
	auto buf = Remote_Access_Library::INTERNAL::_PacketBuffer.AquireBuffer(maxsize);
	
	_PacketHeader.UnCompressedlen = _PacketHeader.PayloadLen;
	_PacketHeader.PayloadLen = static_cast<unsigned int>(ZSTD_compress(buf.data, maxsize, data(), _PacketHeader.PayloadLen));
	memcpy(data(), buf.data, _PacketHeader.PayloadLen);
	Remote_Access_Library::INTERNAL::_PacketBuffer.ReleaseBuffer(buf);
}

void SL::Remote_Access_Library::Network::Packet::decompress()
{
	if (_PacketHeader.UnCompressedlen <= 0) return;//allready decompressed

	auto buf = Remote_Access_Library::INTERNAL::_PacketBuffer.AquireBuffer(_PacketHeader.UnCompressedlen);

	auto dstsize = ZSTD_decompress(buf.data, _PacketHeader.UnCompressedlen, data(), _PacketHeader.PayloadLen);
	memcpy(data(), buf.data, dstsize);
	_PacketHeader.PayloadLen = static_cast<unsigned int>(dstsize);
	_PacketHeader.UnCompressedlen = 0;
	Remote_Access_Library::INTERNAL::_PacketBuffer.ReleaseBuffer(buf);
}
