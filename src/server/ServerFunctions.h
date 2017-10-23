#pragma once
#include "Client.h"
#include <memory>
#include <string>

namespace SL {
namespace WS_LITE {
    class IWSocket;
}
namespace Input_Lite {
    enum KeyCodes;
    enum class MouseButtons : unsigned char;
} // namespace Input_Lite
namespace Clipboard_Lite {
    class IClipboard_Manager;
}

namespace RAT_Lite {
    struct Point;
}
namespace RAT_Server {
    struct Point;

    int GetNewScreenCaptureRate(size_t memoryused, size_t maxmemoryused, int imgcapturerateactual, int imgcaptureraterequested);
    int GetNewImageCompression(size_t memoryused, size_t maxmemoryused, int imagecompressionactual, int imagecompressionrequested);

    void onKeyUp(bool ignoreIncomingKeyboardEvents, const std::shared_ptr<WS_LITE::IWSocket> &socket, const Input_Lite::KeyCodes &keycode);
    void onKeyDown(bool ignoreIncomingKeyboardEvents, const std::shared_ptr<WS_LITE::IWSocket> &socket, const Input_Lite::KeyCodes &keycode);

    void onMouseUp(bool ignoreIncomingMouseEvents, const std::shared_ptr<WS_LITE::IWSocket> &socket, const Input_Lite::MouseButtons &button);
    void onMouseDown(bool ignoreIncomingMouseEvents, const std::shared_ptr<WS_LITE::IWSocket> &socket, const Input_Lite::MouseButtons &button);
    void onMouseScroll(bool ignoreIncomingMouseEvents, const std::shared_ptr<SL::WS_LITE::IWSocket> &socket, int offset);
    void onMousePosition(bool ignoreIncomingMouseEvents, const std::shared_ptr<SL::WS_LITE::IWSocket> &socket, const RAT_Lite::Point &pos);
    void onClipboardChanged(bool shareclipboard, const std::string &str, std::shared_ptr<Clipboard_Lite::IClipboard_Manager> clipboard);
} // namespace RAT_Server
} // namespace SL
