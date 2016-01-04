#pragma once
#include <memory>

namespace SL {
	namespace Remote_Access_Library {
		namespace Network {
			class Packet;
			class Socket;

			class BaseNetworkDriver {
			public:
				BaseNetworkDriver() {}
				virtual ~BaseNetworkDriver() {}
				virtual void OnConnect(const std::shared_ptr<Socket>& socket);
				virtual void OnReceive(const Socket* socket, std::shared_ptr<Packet>& packet);
				virtual void OnClose(const Socket* socket);
			};
		}
	}
}
