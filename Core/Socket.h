#pragma once
#include <memory>

namespace SL {
	namespace Remote_Access_Library {
		namespace Network {
			class Packet;
			struct NetworkEvents;
			struct SocketImpl;

			//this class is async so all calls return immediately and are later executed, which is why callbacks are passed to the factory so you can be notified 
			class Socket: public std::enable_shared_from_this<Socket> {
			public:
				//factory to create a socket and connect to the destination address
				static std::shared_ptr<Socket> ConnectTo(const char* host, const char* port, NetworkEvents& netevents);
				//constructed with opaque type for internal use only
				Socket(SocketImpl* impl);
				~Socket();
				//adds the data to the internal queue, does not block and returns immediately. This calls compress before sending 
				void send(std::shared_ptr<Packet>& pack);
				//sends a request that the socket be closed. NetworkEvents::OnClose will be called when the call is successful
				void close();

			private:
				SocketImpl* _SocketImpl;

			};
		}
	}
}