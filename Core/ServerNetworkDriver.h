#pragma once
#include <memory>

namespace SL {
	namespace Remote_Access_Library {
		namespace Utilities {
			class Image;
			class Rect;
			class Point;
		}
		namespace Network {
			class ServerNetworkDriverImpl;
			class IServerDriver;
			struct Server_Config;
			class ISocket;

			class ServerNetworkDriver {
				std::unique_ptr<ServerNetworkDriverImpl> _ServerNetworkDriverImpl;

			public:
				ServerNetworkDriver(IServerDriver * r, Server_Config& config);
				~ServerNetworkDriver();

				//after creating ServerNetworkDriver, Start() must be called to start the network processessing
				void Start();
				//Before calling Stop, you must ensure that any external references to shared_ptr<ISocket> have been released
				void Stop();

				void SendScreenDif(ISocket * socket, Utilities::Rect & r, const Utilities::Image & img);
				void SendScreenFull(ISocket * socket, const Utilities::Image & img);
				void SendMouse(ISocket * socket, const Utilities::Image & img);
				void SendMouse(ISocket * socket, const Utilities::Point& pos);

				std::vector<std::shared_ptr<Network::ISocket>> GetClients();

			};

		}
	}
}
