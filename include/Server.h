#pragma once
#include <memory>
#include "IBaseNetworkDriver.h"

namespace SL {
	namespace Remote_Access_Library {
		namespace Network {
			struct Server_Config;
			class ISocket;
			class Packet;
		}
		

		enum Server_Status {
			SERVER_RUNNING,
			SERVER_STOPPING,
			SERVER_STOPPED
		};
		namespace Server {

			class ServerImpl;
			class RA_Server {
				std::shared_ptr<ServerImpl> _ServerImpl;
			public:
				// to extend your own functionality, pass a pointer to a class derived from Network::IBaseNetworkDriver. This class will receive the events and you can extend your own functionality
				RA_Server(std::shared_ptr<Network::Server_Config> config, Network::IBaseNetworkDriver* parent = nullptr);
				~RA_Server();

				int Run();
				void Stop(bool block);
				Server_Status get_Status()const;
				static std::string Validate_Settings(std::shared_ptr<Network::Server_Config> config);

#if __ANDROID__
				void OnImage(char* buf, int width, int height);
#endif

		
			};
		}

	}
}

