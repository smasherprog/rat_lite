#pragma once
#include <memory>
#include "SocketStats.h"

namespace SL {
	namespace Remote_Access_Library {
		namespace Network {
			class Packet;
			//this class is async so all calls return immediately and are later executed
			class ISocket : public std::enable_shared_from_this<ISocket> {
			public:

				virtual ~ISocket() { }
				//adds the data to the internal queue, does not block and returns immediately.
				virtual void send(std::shared_ptr<Packet>& pack) = 0;
				//sends a request that the socket be closed. NetworkEvents::OnClose will be called when the call is successful
				virtual void close() = 0;

				//Get the statstics for this socket
				virtual SocketStats get_SocketStats() const = 0;
		
			};
		}
	}
}