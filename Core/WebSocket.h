#pragma once
#include <memory>
#include <functional>
#include "ISocket.h"

namespace SL {
	namespace Remote_Access_Library {
		namespace Network {
			class Packet;
			class IBaseNetworkDriver;

			//this class is async so all calls return immediately and are later executed
			class WebSocket : public ISocket {
			public:

				WebSocket();

				virtual ~WebSocket();
				//adds the data to the internal queue, does not block and returns immediately.
				virtual void send(std::shared_ptr<Packet>& pack) override;
				//sends a request that the socket be closed. NetworkEvents::OnClose will be called when the call is successful
				virtual void close() override;
		
				virtual SocketStats get_SocketStats() const override;

	
			};
		}
	}
}
