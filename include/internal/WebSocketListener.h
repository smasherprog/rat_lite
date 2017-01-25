#pragma once
#include <memory>
#include "INetworkHandlers.h"

namespace SL {
	namespace Remote_Access_Library {
		struct Server_Config;
		class WebSocketListenerImpl;
		class WebSocketListener {
			std::unique_ptr<WebSocketListenerImpl> _WebSocketListenerImpl;
		public:

			WebSocketListener(INetworkHandlers* netevent, std::shared_ptr<Server_Config> config);
			~WebSocketListener();
		};
	}
}