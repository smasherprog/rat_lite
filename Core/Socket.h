#pragma once
#include <memory>
#include <functional>


namespace SL {
	namespace Remote_Access_Library {
		namespace Network {
			class Packet;
			struct NetworkEvents;
			struct SocketImpl;
			class Socket: public std::enable_shared_from_this<Socket> {
			public:

				static std::shared_ptr<Socket> ConnectTo(const char* host, const char* port, NetworkEvents& netevents);

				Socket(std::unique_ptr<SocketImpl>& impl);
				
				void send(std::shared_ptr<Packet> pack);
				void close();
				
	
			private:
				std::unique_ptr<SocketImpl> _SocketImpl;

			};
		}
	}
}