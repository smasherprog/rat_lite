#pragma once
#include <memory>
#include <functional>

namespace SL {
	namespace Remote_Access_Library {
		namespace Network {
			class Packet;

			//this class is async so all calls return immediately and are later executed
			class ISocket : public std::enable_shared_from_this<ISocket> {
			public:
				

				virtual ~ISocket() { }
				//adds the data to the internal queue, does not block and returns immediately. This calls compress before sending, and calls on_sent_callback once the packet has completed sending the packet
				virtual void send(std::shared_ptr<Packet>& pack, std::function<void()> on_sent_callback) = 0;
				//adds the data to the internal queue, does not block and returns immediately. This calls compress before sending 
				virtual void send(std::shared_ptr<Packet>& pack) = 0;
				//sends a request that the socket be closed. NetworkEvents::OnClose will be called when the call is successful
				virtual void close() = 0;

				//pending packets which are queued up and waiting to be sent
				virtual unsigned int get_OutgoingPacketCount() const = 0;
				//pending data which is queued up and waiting to be sent
				virtual unsigned int get_OutgoingByteCount() const = 0;

			};
		}
	}
}