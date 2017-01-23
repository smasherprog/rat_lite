#pragma once
#include <memory>
#include <vector>
#include <uWS/uWS.h>

namespace SL {
	namespace Screen_Capture {
		struct Image;
	};
	namespace Remote_Access_Library {
		class Point;

		class ServerNetworkDriverImpl;
		struct Server_Config;
		class IServerDriver;

		class ServerNetworkDriver {
			std::unique_ptr<ServerNetworkDriverImpl> _ServerNetworkDriverImpl;
		public:
			ServerNetworkDriver(IServerDriver * r, std::shared_ptr<Server_Config> config);
			~ServerNetworkDriver();

			void Start();
			void Stop();
			void SendScreen(uWS::WebSocket<uWS::CLIENT>* socket, const Screen_Capture::Image & img);
			void SendMouse(uWS::WebSocket<uWS::CLIENT>* socket, const Screen_Capture::Image & img);
			void SendMouse(uWS::WebSocket<uWS::CLIENT>* socket, const Point& pos);
			void SendClipboardText(uWS::WebSocket<uWS::CLIENT>* socket, const char* data, unsigned int len);

		};


	}
}
