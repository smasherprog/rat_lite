#pragma once
#include <memory>
#include <vector>
#include "WS_Lite.h"

namespace SL {
	namespace Screen_Capture {
		struct Image;
		struct Monitor;
	}
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

            void SendMonitorsChanged(const std::shared_ptr<WS_LITE::IWSocket>& socket, const std::vector<Screen_Capture::Monitor>& monitors);
            void SendFrameChanged(const std::shared_ptr<WS_LITE::IWSocket>& socket, const Screen_Capture::Image& image, const Screen_Capture::Monitor& monitor);
            void SendNewFrame(const std::shared_ptr<WS_LITE::IWSocket>& socket, const Screen_Capture::Image& image, const Screen_Capture::Monitor& monitor);
            void SendMouseImageChanged(const std::shared_ptr<WS_LITE::IWSocket>& socket, const Screen_Capture::Image& image);

            void SendMousePositionChanged(const std::shared_ptr<WS_LITE::IWSocket>& socket, const Point& mevent);
            void SendClipboardChanged(const std::shared_ptr<WS_LITE::IWSocket>& socket, const std::string& text);

		};


	}
}
