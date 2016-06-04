#pragma once
#include <memory>

namespace SL {
	namespace Remote_Access_Library {

		namespace Network {
			class HttpsServerNetworkDriverImpl;
			class IHttpsDriver;
			struct Server_Config;

			class HttpsServerNetworkDriver {
				HttpsServerNetworkDriverImpl* _HttpsServerNetworkDriverImpl;

			public:
				HttpsServerNetworkDriver(IHttpsDriver * r, std::shared_ptr<Network::Server_Config> config);
				~HttpsServerNetworkDriver();

				//after creating HttpsServerNetworkDriver, Start() must be called to start the network processessing
				void Start();
				//Before calling Stop, you must ensure that any external references to shared_ptr<ISocket> have been released
				void Stop();


			};

		}
	}
}
