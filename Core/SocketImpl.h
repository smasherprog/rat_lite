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
				std::vector<char> _IncomingBuffer;
				std::deque<OutgoingPacket> _OutgoingPackets;
				SocketStats _SocketStats;
				std::unordered_map<std::string, std::string> Header;


				boost::asio::deadline_timer read_deadline_;
				boost::asio::deadline_timer write_deadline_;
				IBaseNetworkDriver* _IBaseNetworkDriver;
				boost::asio::streambuf _IncomingStreamBuffer;
				bool Closed;
				bool Writing;
			public:

				SocketImpl(boost::asio::io_service& io_service, IBaseNetworkDriver* netevents);

				void StartReadTimer(int seconds);
				void StartWriteTimer(int seconds);

			
				void parse_header(std::string defaultheaderversion, size_t bytesread);
				std::unordered_map<std::string, std::string>& get_Header();
				IBaseNetworkDriver* get_Driver() const;
				SocketStats get_Socketstats() const;
				boost::asio::streambuf& get_IncomingStreamBuffer();

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

				
	
				PacketHeaderWrapper WritePacketHeader;
				PacketHeaderWrapper ReadPacketHeader;
			};
		}

	}
}