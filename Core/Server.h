#pragma once
#include <memory>

namespace SL {
	namespace Remote_Access_Library {
		namespace Network {
			class IBaseNetworkDriver;
			struct Server_Config;
		}
		class ServerImpl;
		namespace Capturing{
			class Screen;
		}

		enum Server_Status {
			SERVER_RUNNING,
			SERVER_STOPPING,
			SERVER_STOPPED
		};
		class Server {
		public:
			// to extend your own functionality, pass a pointer to a class derived from Network::IUserNetworkDriver. This class will receive the events and you can extend your own functionality
			Server(std::shared_ptr<Network::Server_Config> config, Network::IBaseNetworkDriver* parent = nullptr);
			~Server();

			int Run();
			void Stop(bool block);
			Server_Status get_Status()const;
			
			Capturing::Screen get_ScreenHandler();

#if __ANDROID__
			void OnImage(char* buf, int width, int height);
#endif

		private:
			std::shared_ptr<ServerImpl> _ServerImpl;

		};
	}
}

