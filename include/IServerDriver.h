#pragma once
#include "INetworkHandlers.h"
#include "Input_Lite.h"

namespace SL {
namespace WS_LITE {
    class IWSocket;
}
namespace RAT {
    class Point;
    class IServerDriver : public INetworkHandlers {
      public:
        virtual ~IServerDriver() {}
        virtual void onKeyUp(const std::shared_ptr<WS_LITE::IWSocket> &socket, Input_Lite::KeyCodes key) = 0;
        virtual void onKeyDown(const std::shared_ptr<WS_LITE::IWSocket> &socket, Input_Lite::KeyCodes key) = 0;
        virtual void onMouseUp(const std::shared_ptr<WS_LITE::IWSocket> &socket, Input_Lite::MouseButtons button) = 0;
        virtual void onMouseDown(const std::shared_ptr<WS_LITE::IWSocket> &socket, Input_Lite::MouseButtons button) = 0;
        virtual void onMouseScroll(const std::shared_ptr<WS_LITE::IWSocket> &socket, int offset) = 0;
        virtual void onMousePosition(const std::shared_ptr<WS_LITE::IWSocket> &socket, const Point &pos) = 0;
        virtual void onClipboardChanged(const std::string &text) = 0;
    };
} // namespace RAT
} // namespace SL
