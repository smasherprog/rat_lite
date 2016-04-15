#include "stdafx.h"
#include "SocketImpl.h"

SL::Remote_Access_Library::Network::SocketImpl::SocketImpl(asio::io_service& io_service, Network::IBaseNetworkDriver * netevents) :
	_IBaseNetworkDriver(netevents), read_deadline_(io_service), write_deadline_(io_service) {

	read_deadline_.expires_at(boost::posix_time::pos_infin);
	write_deadline_.expires_at(boost::posix_time::pos_infin);
	memset(&_SocketStats, 0, sizeof(_SocketStats));
	Writing = Closed = false;
	readtimeout = 5;
	writetimeout = 5;
}

SL::Remote_Access_Library::Network::SocketImpl::~SocketImpl()
{
	CancelTimers();
}

void SL::Remote_Access_Library::Network::SocketImpl::StartReadTimer(int seconds)
{
	if (seconds <= 0) read_deadline_.expires_at(boost::posix_time::pos_infin);
	else read_deadline_.expires_from_now(boost::posix_time::seconds(seconds));
}

void SL::Remote_Access_Library::Network::SocketImpl::StartWriteTimer(int seconds)
{
	if (seconds <= 0) write_deadline_.expires_at(boost::posix_time::pos_infin);
	else write_deadline_.expires_from_now(boost::posix_time::seconds(seconds));
}

void SL::Remote_Access_Library::Network::SocketImpl::CancelTimers()
{
	read_deadline_.cancel();
	write_deadline_.cancel();
}


SL::Remote_Access_Library::Network::IBaseNetworkDriver* SL::Remote_Access_Library::Network::SocketImpl::get_Driver() const
{
	return _IBaseNetworkDriver;
}

SL::Remote_Access_Library::Network::SocketStats SL::Remote_Access_Library::Network::SocketImpl::get_Socketstats() const
{
	return _SocketStats;
}


char* SL::Remote_Access_Library::Network::SocketImpl::get_ReadBuffer()
{
	_IncomingBuffer.reserve(ReadPacketHeader.Payload_Length);
	return _IncomingBuffer.data();
}

unsigned int SL::Remote_Access_Library::Network::SocketImpl::get_ReadBufferSize()
{
	return ReadPacketHeader.Payload_Length;
}

bool SL::Remote_Access_Library::Network::SocketImpl::writing() const
{
	return Writing;
}

void SL::Remote_Access_Library::Network::SocketImpl::writing(bool w)
{
	Writing = w;
}

bool SL::Remote_Access_Library::Network::SocketImpl::OutGoingBuffer_empty() const
{
	return _OutgoingPackets.empty();
}

void SL::Remote_Access_Library::Network::SocketImpl::AddOutgoingPacket(std::shared_ptr<Packet> pac, unsigned int uncompressedsize)
{
	_OutgoingPackets.push_back({ pac, uncompressedsize });
}

std::shared_ptr<SL::Remote_Access_Library::Network::Packet> SL::Remote_Access_Library::Network::SocketImpl::GetNextWritePacket()
{
	auto pac = _OutgoingPackets.front();
	_OutgoingPackets.pop_front();//remove the packet
	WritePacketHeader.Packet_Type = pac.Pack->Packet_Type;
	WritePacketHeader.Payload_Length = pac.Pack->Payload_Length;
	WritePacketHeader.UncompressedLength = pac.UncompressedLength;
	return pac.Pack;
}

SL::Remote_Access_Library::Network::Packet SL::Remote_Access_Library::Network::SocketImpl::GetNextReadPacket()
{
	//dont free the memory on this... Owned internally
	Packet p(ReadPacketHeader.Packet_Type, static_cast<size_t>(ReadPacketHeader.Payload_Length), std::move(Header), _IncomingBuffer.data(), false);
	Header.clear();//reset the header after move
	return p;
}

void SL::Remote_Access_Library::Network::SocketImpl::UpdateReadStats()
{
	_SocketStats.TotalPacketReceived += 1;
	_SocketStats.NetworkBytesReceived += ReadPacketHeader.Payload_Length;
	_SocketStats.TotalBytesReceived += ReadPacketHeader.UncompressedLength;
}

void SL::Remote_Access_Library::Network::SocketImpl::UpdateWriteStats(Packet & packet, size_t beforesize)
{
	_SocketStats.TotalPacketSent += 1;
	_SocketStats.TotalBytesSent += beforesize;
	_SocketStats.NetworkBytesSent += packet.Payload_Length;
}
