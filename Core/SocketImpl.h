#pragma once
#include <boost/asio.hpp>
#include "IBaseNetworkDriver.h"
#include "Packet.h"
#include "SocketStats.h"


namespace SL {
	namespace Remote_Access_Library {
		//this namespace should only be accessed by the Core library and NEVER by a consumer of this library
		namespace INTERNAL {

			class SocketImpl {
				
				std::vector<char> _IncomingBuffer;
				std::deque<std::shared_ptr<Network::Packet>> _OutgoingPackets;
				std::mutex _OutgoingPacketsLock;
			public:
				SocketImpl(boost::asio::io_service& io, Network::IBaseNetworkDriver* netevents, boost::asio::ip::tcp::socket&& s) : io_service(io), socket(std::move(s)), NetworkEvents(netevents) {
					SocketStats = { 0 };
				}
				SocketImpl(boost::asio::io_service& io, Network::IBaseNetworkDriver* netevents) : io_service(io), socket(io), NetworkEvents(netevents) {
					SocketStats = { 0 };
				}

				boost::asio::io_service& io_service;
				boost::asio::ip::tcp::socket socket;
				Network::IBaseNetworkDriver* NetworkEvents;
				Network::PacketHeader PacketHeader;
				Network::SocketStats SocketStats;

				char* get_Buffer();
				size_t get_BufferSize();
				bool PacketsWaitingToBeSent() const;
				void PushToOutgoing(std::shared_ptr<Network::Packet>& pack);
				std::shared_ptr<Network::Packet> PopFromOutgoing();
				std::shared_ptr<Network::Packet> get_IncomingPacket();
			};
		}

	}
}
