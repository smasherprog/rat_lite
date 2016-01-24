#pragma once
#include "IBaseNetworkDriver.h"
#include <memory>


namespace SL {
	namespace Remote_Access_Library {

		namespace Network {
			class IClientDriver;
			class ClientNetworkDriverImpl;

			class ClientNetworkDriver {
				std::unique_ptr<ClientNetworkDriverImpl> _ClientNetworkDriverImpl;

			public:
				ClientNetworkDriver(IClientDriver* r, const char* dst_host, const char* dst_port);
				virtual ~ClientNetworkDriver();

				//call this to ensure all network processing have stopped for this instance. This is good pratice before destroying the object to prevent callbacks from firing 
				void Close();
			};
		}
	}
}
