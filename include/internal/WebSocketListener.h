#pragma once
#include <memory>
#include <functional>

namespace SL {
	namespace RAT {
		struct Server_Config;
		class ISocket;
		class WebSocketListenerImpl;
		class WebSocketListener {
			std::shared_ptr<WebSocketListenerImpl> _WebSocketListenerImpl;
		public:

			WebSocketListener(std::shared_ptr<Server_Config> config);
			~WebSocketListener();
		
			void onConnection(const std::function<void(const std::shared_ptr<ISocket>&)>& func);
			void onMessage(const std::function<void(const std::shared_ptr<ISocket>&, const char*, size_t)>& func);
			void onDisconnection(const std::function<void(const ISocket* socket)>& func);
		};
	}
}