#pragma once
#include <memory>

namespace SL {
	namespace RAT {
		class ISocket;
		class INetworkHandlers {
		public:
			virtual ~INetworkHandlers() {}

			virtual void onConnection(const std::shared_ptr<ISocket>& socket) = 0;
			virtual void onMessage(const std::shared_ptr<ISocket>& socket, const char* data, size_t len) = 0;
			virtual void onDisconnection(const ISocket* socket) = 0;
		};
	}
}
