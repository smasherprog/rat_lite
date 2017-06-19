#pragma once
#include <memory>
#include <vector>

namespace SL {
	namespace Screen_Capture {
		struct Image;
		struct Monitor;
	};
    namespace WS_LITE{
        class IWSocket;
    }
	namespace RAT {
		class Point;

		class ServerDriverImpl;
		struct Server_Config;
		class IServerDriver;
		class ServerDriver {
			std::unique_ptr<ServerDriverImpl> ServerDriverImpl_;

		public:
			ServerDriver(IServerDriver * r, std::shared_ptr<Server_Config> config);
			~ServerDriver();

			//frames are images
			void SendFrameChange(const std::shared_ptr<SL::WS_LITE::IWSocket>& socket, const Screen_Capture::Image & img, const SL::Screen_Capture::Monitor& monitor);
			void SendMonitorInfo(const std::shared_ptr<SL::WS_LITE::IWSocket>& socket, const std::vector<std::shared_ptr<Screen_Capture::Monitor>> & monitors);

			void SendMouse(const std::shared_ptr<SL::WS_LITE::IWSocket>& socket, const Screen_Capture::Image & img);
			void SendMouse(const std::shared_ptr<SL::WS_LITE::IWSocket>& socket, const Point& pos);
			void SendClipboardText(const std::shared_ptr<SL::WS_LITE::IWSocket>& socket, const char* data, unsigned int len);

		};


	}
}
