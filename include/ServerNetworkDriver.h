#pragma once
#include <memory>
#include <vector>
#include <uWS/uWS.h>

namespace SL {
	namespace Remote_Access_Library {
		namespace Utilities {
			class Rect;
			class Point;
		}
		namespace Network {
			class ServerNetworkDriverImpl;
			struct Server_Config;

			class ServerNetworkDriver {

				std::unique_ptr<ServerNetworkDriverImpl> _ServerNetworkDriverImpl;

			public:
				ServerNetworkDriver(IServerDriver * r, std::shared_ptr<Network::Server_Config> config);
				~ServerNetworkDriver();

				void SendScreen(uWS::WebSocket<uWS::CLIENT>* socket, const Screen_Capture::Image & img);
				void SendMouse(uWS::WebSocket<uWS::CLIENT>* socket, const Screen_Capture::Image & img);
				void SendMouse(uWS::WebSocket<uWS::CLIENT>* socket, const Utilities::Point& pos);
				void SendClipboardText(uWS::WebSocket<uWS::CLIENT>* socket, const char* data, unsigned int len);
			
			};

		}
	}
}
