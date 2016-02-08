#pragma once
#include <memory>

namespace SL {
	namespace Remote_Access_Library {
		namespace Network {
			class ISocket;
			class Packet;
			class IBaseNetworkDriver {
			public:
				IBaseNetworkDriver() {}
				virtual ~IBaseNetworkDriver() {}
				virtual void OnConnect(const std::shared_ptr<ISocket>& socket) = 0;
				virtual void OnReceive(const std::shared_ptr<ISocket>& socket, std::shared_ptr<Packet>& pack) = 0;
				virtual void OnClose(const std::shared_ptr<ISocket>& socket) = 0;
			};
		}
	}
}
