#pragma once
#include <memory>

namespace SL {
	namespace Remote_Access_Library {
		namespace Network {
			class Packet;
			class ISocket;

			class IUserNetworkDriver {
			public:
				IUserNetworkDriver() {}
				virtual ~IUserNetworkDriver() {}
				virtual void OnConnect(const std::weak_ptr<ISocket>& socket) = 0;
				virtual void OnReceive(const ISocket* socket, Packet& packet) = 0;
				virtual void OnClose(const ISocket* socket) = 0;
			};
		}
	}
}
