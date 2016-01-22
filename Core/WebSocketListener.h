#pragma once
#include <memory>

namespace SL {
	namespace Remote_Access_Library {
		namespace INTERNAL {
			class WebSocketListinerImpl;
		}
		namespace Network {

			class IBaseNetworkDriver;
			class WebSocketListener : public std::enable_shared_from_this<WebSocketListener> {
			public:
				//factory to create a listening socket
				static std::shared_ptr<WebSocketListener> CreateListener(unsigned short port, Network::IBaseNetworkDriver* netevents);

				WebSocketListener(INTERNAL::WebSocketListinerImpl* impl);
				~WebSocketListener();

				void Start();

			private:
				INTERNAL::WebSocketListinerImpl* _ListinerImpl;
			};
		}
	}
}