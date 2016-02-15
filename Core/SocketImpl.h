#pragma once
#include <boost/asio.hpp>
#include <boost/asio/deadline_timer.hpp>
#include "IBaseNetworkDriver.h"
#include "Packet.h"
#include "SocketStats.h"

namespace SL {
	namespace Remote_Access_Library {
		namespace Network {
			struct PacketHeaderWrapper {
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
				


				boost::asio::deadline_timer read_deadline_;
				boost::asio::deadline_timer write_deadline_;
				IBaseNetworkDriver* _IBaseNetworkDriver;
				bool Closed;
				bool Writing;
			public:

				SocketImpl(boost::asio::io_service& io_service, IBaseNetworkDriver* netevents);

				void StartReadTimer(int seconds);
				void StartWriteTimer(int seconds);

			

				IBaseNetworkDriver* get_Driver() const;
				SocketStats get_Socketstats() const;

				boost::asio::deadline_timer& get_read_deadline_timer();
				boost::asio::deadline_timer& get_write_deadline_timer();
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

				void close(boost::asio::basic_socket<boost::asio::ip::tcp, boost::asio::stream_socket_service <boost::asio::ip::tcp>>& sock);
				bool closed(boost::asio::basic_socket<boost::asio::ip::tcp, boost::asio::stream_socket_service <boost::asio::ip::tcp>>& sock);

				std::vector<char> _IncomingBuffer;
	
				PacketHeaderWrapper WritePacketHeader;
				PacketHeaderWrapper ReadPacketHeader;
				std::unordered_map<std::string, std::string> Header;

			};
		}

	}
}