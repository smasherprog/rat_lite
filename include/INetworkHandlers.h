#pragma once
#include <memory>

namespace SL {
	namespace RAT {
		class IWebSocket;
		class INetworkHandlers {
		public:
			virtual ~INetworkHandlers() {}

			virtual void onConnection(const std::shared_ptr<IWebSocket>& socket) = 0;
			virtual void onMessage(const IWebSocket& socket, const char* data, size_t length) = 0;
			virtual void onDisconnection(const IWebSocket& socket, int code, char* message, size_t length) = 0;
		};
	}
}
