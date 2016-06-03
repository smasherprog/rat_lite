#pragma once
#include <memory>
#include "ISocket.h"
#include "IBaseNetworkDriver.h"


namespace SL {
	namespace Remote_Access_Library {
		namespace Network {

			class Packet;
			class HttpsServerImpl;
			struct Server_Config;
			class HttpsListener {
				std::shared_ptr<HttpsServerImpl> _HttpsServerImpl;
			public:

				HttpsListener(IBaseNetworkDriver* netevent,  std::shared_ptr<Network::Server_Config> config);
				~HttpsListener();

			};

		}
	}
}