#include "stdafx.h"
#include "Packet.h"
#include "zstd.h"


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

bool SL::Remote_Access_Library::Network::Packet::compressed() const {
	return _PacketHeader.UnCompressedlen > 0;
}
void SL::Remote_Access_Library::Network::Packet::compress(int compressionlevel)
{
	assert(compressionlevel >= 1 && compressionlevel <= 19);
	if (compressed()) return;//allready compressed
	auto maxsize = ZSTD_compressBound(_PacketHeader.PayloadLen);
	auto buf = std::make_unique<char[]>(maxsize);

	_PacketHeader.UnCompressedlen = _PacketHeader.PayloadLen;
	_PacketHeader.PayloadLen = static_cast<unsigned int>(ZSTD_compress(buf.get(), maxsize, data(), _PacketHeader.PayloadLen, compressionlevel));
	std::cout << "compress from: " << _PacketHeader.UnCompressedlen << " To " << _PacketHeader.PayloadLen<<std::endl;
	_Data = std::move(buf);
}

void SL::Remote_Access_Library::Network::Packet::decompress()
{
	if (!compressed()) return;//allready decompressed
	auto buf = std::make_unique<char[]>(_PacketHeader.UnCompressedlen);

	auto dstsize = ZSTD_decompress(buf.get(), _PacketHeader.UnCompressedlen, data(), _PacketHeader.PayloadLen);
	_PacketHeader.PayloadLen = static_cast<unsigned int>(dstsize);
	std::cout << "decompress from: " << _PacketHeader.UnCompressedlen << " To " << _PacketHeader.PayloadLen << std::endl;
	_PacketHeader.UnCompressedlen = 0;
	_Data = std::move(buf);
}
