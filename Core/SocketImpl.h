#pragma once
#include <boost/asio.hpp>
#include "BaseNetworkDriver.h"
#include "Packet.h"

namespace SL {
	namespace Remote_Access_Library {
		//this namespace should only be accessed by the Core library and NEVER by a consumer of this library
		namespace INTERNAL {
			struct PacketSendCommand {
				std::shared_ptr<SL::Remote_Access_Library::Network::Packet> Packet;
				std::function<void()> OnSentCallback;
			};
			struct SocketImpl {
				SocketImpl(boost::asio::io_service& io, Network::BaseNetworkDriver* netevents, boost::asio::ip::tcp::socket&& s) : io_service(io), socket(std::move(s)), NetworkEvents_(netevents) {}
				SocketImpl(boost::asio::io_service& io, Network::BaseNetworkDriver* netevents) : io_service(io), socket(io), NetworkEvents_(netevents) {}

				boost::asio::io_service& io_service;
				boost::asio::ip::tcp::socket socket;

				Network::BaseNetworkDriver* NetworkEvents_;
				Network::PacketHeader PacketHeader;
				std::deque<PacketSendCommand> OutgoingPackets;
				std::shared_ptr<Network::Packet> IncomingPacket;

				unsigned int OutGoingPacketCount = 0;
				unsigned int OutGoingByteCount = 0;
			};
		}

	}
}
