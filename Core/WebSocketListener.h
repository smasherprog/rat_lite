#pragma once
#include <memory>

namespace SL {
	namespace Remote_Access_Library {

		namespace Network {
			namespace INTERNAL {
				class WebSocketListinerImpl;
			}
			class IBaseNetworkDriver;
			class WebSocketListener {
			public:
				//factory to create a listening socket
				static std::shared_ptr<WebSocketListener> CreateListener(unsigned short port, Network::IBaseNetworkDriver* netevents);

				WebSocketListener(INTERNAL::WebSocketListinerImpl* impl);
				~WebSocketListener();
				WebSocketListener(const WebSocketListener&) = delete;
				WebSocketListener& operator=(const WebSocketListener&) = delete;
				void Start();
				void Stop();

			private:
				INTERNAL::WebSocketListinerImpl* _ListinerImpl;
				unsigned short _ListenPort;
				Network::IBaseNetworkDriver* _IBaseNetworkDriver;
			};
		}
	}
}