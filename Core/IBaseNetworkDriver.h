#pragma once
#include <memory>

namespace SL {
	namespace Remote_Access_Library {
		namespace Network {
			class Packet;
			class ISocket;

			class IBaseNetworkDriver {
			public:
				IBaseNetworkDriver() {}
				virtual ~IBaseNetworkDriver() {}
				virtual void OnConnect(const std::shared_ptr<ISocket>& socket);
				virtual void OnReceive(const ISocket* socket, std::shared_ptr<Packet>& packet);
				virtual void OnClose(const ISocket* socket);
			};
		}
	}
}
