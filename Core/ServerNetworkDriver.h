#pragma once
#include <memory>

namespace SL {
	namespace Remote_Access_Library {
		namespace Utilities {
			class Image;
			class Rect;
		}
		namespace Network {
			class ServerNetworkDriverImpl;
			class IServerDriver;
			struct Server_Config;
			class ISocket;

			class ServerNetworkDriver  {
				std::unique_ptr<ServerNetworkDriverImpl> _ServerNetworkDriverImpl;

			public:
				ServerNetworkDriver(IServerDriver * r, Server_Config& config);
				~ServerNetworkDriver();

				//after creating ServerNetworkDriver, StartNetworkProcessing() must be called to start the network processessing
				void StartNetworkProcessing();
				//Before calling StopNetworkProcessing, you must ensure that any external references to shared_ptr<ISocket> have been released
				void StopNetworkProcessing();

				void Send(Utilities::Rect& r, const Utilities::Image & img);
				void Send(ISocket* socket, Utilities::Rect& r, const Utilities::Image & img);

				std::vector<std::shared_ptr<Network::ISocket>> GetClients();

			};
		
		}
	}
}
