#pragma once
#include <memory>

namespace SL {
	namespace Remote_Access_Library {
		namespace Network {
			class Packet;
			struct NetworkEvents;
			struct SocketImpl;
			
			class Socket: public std::enable_shared_from_this<Socket> {
			public:
				//factory to create a socket and connect to the destination address
				static std::shared_ptr<Socket> ConnectTo(const char* host, const char* port, NetworkEvents& netevents);
				//constructed with opaque type for internal use only
				Socket(SocketImpl* impl);
				~Socket();
				void send(std::shared_ptr<Packet> pack);
				void close();

				size_t get_PendingOutgoingBytes() const;
	
			private:
				SocketImpl* _SocketImpl;

			};
		}
	}
}