#pragma once
#include <memory>

namespace SL {
	namespace Remote_Access_Library {
		namespace Network {
			class IUserNetworkDriver;	
			struct Server_Config;
		}
		class ServerImpl;
	
		class Server {
		public:
			// to extend your own functionality, pass a pointer to a class derived from Network::IUserNetworkDriver. This class will receive the events and you can extend your own functionality
			Server(Network::Server_Config& config, Network::IUserNetworkDriver* parent = nullptr);
			~Server();

			int Run();

		private:
			std::shared_ptr<ServerImpl> _ServerImpl;
			
		};
	}
}

