#pragma once
#include <memory>
#include "IBaseNetworkDriver.h"


namespace SL {
	namespace Remote_Access_Library {
		namespace Network {	
			struct Server_Config;
			struct Client_Config;
			class ISocket;
			class Packet;

			namespace WebSocket {
				class ListinerImpl;
				
				class Listener {
					std::shared_ptr<ListinerImpl> _ListinerImpl;
				public:

					Listener(IBaseNetworkDriver* netevent, std::shared_ptr<Network::Server_Config> config);
					~Listener();
				};

				void Connect(Client_Config* config, IBaseNetworkDriver* driver, const char* host);

			}

		}
	}
}