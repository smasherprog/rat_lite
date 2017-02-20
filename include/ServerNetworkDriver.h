#pragma once
#include <memory>
#include <vector>

namespace SL {
	namespace Screen_Capture {
		struct Image;
	};
	namespace RAT {
		class Point;

		class ServerNetworkDriverImpl;
		struct Server_Config;
		class IServerDriver;
		class IWebSocket;
		class ServerNetworkDriver {
			std::unique_ptr<ServerNetworkDriverImpl> _ServerNetworkDriverImpl;

		public:
			ServerNetworkDriver();
			~ServerNetworkDriver();

			void Start(IServerDriver * r, std::shared_ptr<Server_Config> config);
			void Stop();
			//frames are images
			void SendFrameChange(IWebSocket* socket, const Screen_Capture::Image & img);
			void SendFrame(IWebSocket* socket, const Screen_Capture::Image & img);

			void SendMouse(IWebSocket* socket, const Screen_Capture::Image & img);
			void SendMouse(IWebSocket* socket, const Point& pos);
			void SendClipboardText(IWebSocket* socket, const char* data, unsigned int len);

		};


	}
}
