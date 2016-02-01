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
				virtual void OnConnect(const std::shared_ptr<Network::ISocket>& socket) = 0;
				virtual void OnReceive(const std::shared_ptr<Network::ISocket>& socket, Packet& packet) = 0;
				virtual void OnClose(const std::shared_ptr<Network::ISocket>& socket) = 0;
			};
		}
	}
}
