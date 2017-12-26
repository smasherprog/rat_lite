#pragma once
#include "Clipboard_Lite.h"
#include "RAT.h"
#include <memory>
#include <string>

namespace SL {
namespace RAT_Server {
    struct Point;

    struct Client {
        RAT_Lite::ClipboardSharing ShareClip = RAT_Lite::ClipboardSharing::NOT_SHARED;
        int ImageCompressionSetting = 70;

        bool IgnoreIncomingKeyboardEvents = false;
        bool IgnoreIncomingMouseEvents = false;
        RAT_Lite::ImageEncoding EncodeImagesAsGrayScale = RAT_Lite::ImageEncoding::COLOR;
        std::vector<Screen_Capture::Monitor> MonitorsToWatch;
        std::vector<Screen_Capture::Monitor> MonitorsNeeded;

        std::shared_ptr<WS_LITE::IWebSocket> Socket;
    };

    int GetNewScreenCaptureRate(size_t memoryused, size_t maxmemoryused, int imgcapturerateactual, int imgcaptureraterequested);
    int GetNewImageCompression(size_t memoryused, size_t maxmemoryused, int imagecompressionactual, int imagecompressionrequested);

    void onClientSettingsChanged(WS_LITE::IWebSocket *socket, std::vector<std::shared_ptr<Client>> &clients,
                                 const std::vector<Screen_Capture::Monitor> &monitors, const RAT_Lite::ClientSettings &clientsettings);
    void onGetMonitors(std::vector<std::shared_ptr<Client>> &clients, const std::vector<Screen_Capture::Monitor> &monitors);

    void onKeyUp(bool ignoreIncomingKeyboardEvents, const std::shared_ptr<WS_LITE::IWebSocket> &socket, const Input_Lite::KeyCodes &keycode);

    void onKeyUp(bool ignoreIncomingKeyboardEvents, const std::shared_ptr<WS_LITE::IWebSocket> &socket, const Input_Lite::KeyCodes &keycode);
    void onKeyDown(bool ignoreIncomingKeyboardEvents, const std::shared_ptr<WS_LITE::IWebSocket> &socket, const Input_Lite::KeyCodes &keycode);

    void onMouseUp(bool ignoreIncomingMouseEvents, const std::shared_ptr<WS_LITE::IWebSocket> &socket, const Input_Lite::MouseButtons &button);
    void onMouseDown(bool ignoreIncomingMouseEvents, const std::shared_ptr<WS_LITE::IWebSocket> &socket, const Input_Lite::MouseButtons &button);
    void onMouseScroll(bool ignoreIncomingMouseEvents, const std::shared_ptr<SL::WS_LITE::IWebSocket> &socket, int offset);
    void onMousePosition(bool ignoreIncomingMouseEvents, const std::shared_ptr<SL::WS_LITE::IWebSocket> &socket, const RAT_Lite::Point &pos);
    void onClipboardChanged(bool shareclipboard, const std::string &str, std::shared_ptr<Clipboard_Lite::IClipboard_Manager> clipboard);
} // namespace RAT_Server
} // namespace SL
