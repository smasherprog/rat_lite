#pragma once
#include <boost/asio.hpp>
#include <boost/asio/deadline_timer.hpp>
#include "IBaseNetworkDriver.h"
#include "Packet.h"
#include "SocketStats.h"

namespace SL {
	namespace Remote_Access_Library {
		namespace Network {
			struct PacketHeader {
				unsigned int Packet_Type;
				unsigned int Payload_Length;
				unsigned int UncompressedLength;
			};
			struct OutgoingPacket {
				std::shared_ptr<Network::Packet> Pack;
				unsigned int UncompressedLength;
			};
			class SocketImpl {

				std::deque<OutgoingPacket> _OutgoingPackets;
				SocketStats _SocketStats;

				IBaseNetworkDriver* _IBaseNetworkDriver;
			
			public:

				SocketImpl(boost::asio::io_service& io_service, IBaseNetworkDriver* netevents);
				~SocketImpl();
				void StartReadTimer(int seconds);
				void StartWriteTimer(int seconds);
				void CancelTimers();
				IBaseNetworkDriver* get_Driver() const;
				SocketStats get_Socketstats() const;

				char* get_ReadBuffer();
				unsigned int get_ReadBufferSize();
				bool writing() const;
				void writing(bool w);

				bool OutGoingBuffer_empty() const;
				void AddOutgoingPacket(std::shared_ptr<Packet> pac, unsigned int uncompressedsize);

				std::shared_ptr<SL::Remote_Access_Library::Network::Packet> GetNextWritePacket();
				Packet GetNextReadPacket();

				void UpdateReadStats();
				void UpdateWriteStats(Packet& packet, size_t beforesize);

				std::vector<char> _IncomingBuffer;
				boost::asio::deadline_timer read_deadline_;
				boost::asio::deadline_timer write_deadline_;
				PacketHeader WritePacketHeader;
				PacketHeader ReadPacketHeader;
				std::unordered_map<std::string, std::string> Header;
				bool Server = false;
				std::string Host;
				std::string Port;
				bool Closed;
				bool Writing;
				int readtimeout;
				int writetimeout;
			};
		}

	}
}