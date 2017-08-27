#pragma once
#include "WS_Lite.h"
#include <memory>
#include <vector>

namespace SL {
namespace Screen_Capture {
    struct Image;
    struct Monitor;
    struct Point;
} // namespace Screen_Capture
namespace WS_LITE {
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
        ServerDriver(IServerDriver *r, std::shared_ptr<Server_Config> config);
        ~ServerDriver();

        WS_LITE::WSMessage PrepareMonitorsChanged(const std::vector<Screen_Capture::Monitor> &monitors);
        WS_LITE::WSMessage PrepareFrameChanged(const Screen_Capture::Image &image, const Screen_Capture::Monitor &monitor);
        WS_LITE::WSMessage PrepareNewFrame(const Screen_Capture::Image &image, const Screen_Capture::Monitor &monitor);
        WS_LITE::WSMessage PrepareMouseImageChanged(const Screen_Capture::Image &image);
        WS_LITE::WSMessage PrepareMousePositionChanged(const SL::Screen_Capture::Point &mevent);
        WS_LITE::WSMessage PrepareClipboardChanged(const std::string &text);
    };

} // namespace RAT
} // namespace SL
