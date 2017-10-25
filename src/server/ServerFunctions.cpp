#include "Logging.h"
#include "ServerFunctions.h"

#include "Clipboard_Lite.h"
#include "Input_Lite.h"
#include "Logging.h"
#include "RAT.h"
#include "ScreenCapture.h"
#include "WS_Lite.h"

namespace SL {
namespace RAT_Server {

    int GetNewScreenCaptureRate(const size_t memoryused, size_t maxmemoryused, int imgcapturerateactual, int imgcaptureraterequested)
    {
        if (memoryused >= maxmemoryused) {
            // decrease the capture rate to accomidate slow connections
            return imgcapturerateactual + 100; // slower the capture rate ....
        }
        else if (memoryused < maxmemoryused && imgcapturerateactual != imgcaptureraterequested) {
            // increase the capture rate here
            imgcapturerateactual -= 100; // increase the capture rate ....
            if (imgcapturerateactual <= 0) {
                return imgcaptureraterequested;
            }
            return imgcapturerateactual;
        }
        return imgcapturerateactual;
    }
    int GetNewImageCompression(size_t memoryused, size_t maxmemoryused, int imagecompressionactual, int imagecompressionrequested)
    {
        if (memoryused >= maxmemoryused) {
            if (imagecompressionactual - 10 < 30) { // anything below 30 will be pretty bad....
                return imagecompressionactual;
            }
            return imagecompressionactual - 10;
        }
        else if (memoryused < maxmemoryused && imagecompressionactual != imagecompressionrequested) {
            imagecompressionactual += 10;
            if (imagecompressionactual >= imagecompressionrequested) {
                return imagecompressionrequested;
            }
            return imagecompressionactual;
        }
        return imagecompressionactual;
    }
    void onClientSettingsChanged(WS_LITE::IWSocket *socket, std::vector<std::shared_ptr<Client>> &clients,
                                 const std::vector<Screen_Capture::Monitor> &monitors, const RAT_Lite::ClientSettings &clientsettings)
    {
        auto found = std::find_if(std::begin(clients), std::end(clients), [socket](const auto &client) { return client->Socket.get() == socket; });
        if (found != std::end(clients)) {
            (*found)->EncodeImagesAsGrayScale = clientsettings.EncodeImagesAsGrayScale;
            (*found)->ImageCompressionSetting = clientsettings.ImageCompressionSetting;
            // cap compressionsettings
            if ((*found)->ImageCompressionSetting < 30) {
                (*found)->ImageCompressionSetting = 30;
            }
            else if ((*found)->ImageCompressionSetting > 100) {
                (*found)->ImageCompressionSetting = 100;
            }
            (*found)->ShareClip = clientsettings.ShareClip;
            for (auto &m : clientsettings.MonitorsToWatch) {
                auto newmonitor = std::find_if(std::begin(monitors), std::end(monitors), [&m](const auto &mon) { return mon.Id == m.Id; });
                if (newmonitor == std::end(monitors)) {
                    (*found)->MonitorsNeeded.push_back(m);
                }
            }
            (*found)->MonitorsToWatch = clientsettings.MonitorsToWatch;
        }
    }
    void onGetMonitors(std::vector<std::shared_ptr<Client>> &clients, const std::vector<Screen_Capture::Monitor> &monitors)
    {
        for (auto &a : clients) {
            for (auto &monitor : monitors) {
                auto clientneeds =
                    std::find_if(begin(a->MonitorsToWatch), end(a->MonitorsToWatch), [&monitor](const auto &m) { return m.Id == monitor.Id; });
                a->MonitorsNeeded = monitors;
            }
        }
    }

    void onKeyUp(bool ignoreIncomingKeyboardEvents, const std::shared_ptr<WS_LITE::IWSocket> &socket, const Input_Lite::KeyCodes &keycode)
    {
        if (!ignoreIncomingKeyboardEvents) {
            Input_Lite::KeyEvent kevent;
            kevent.Key = keycode;
            kevent.Pressed = false;
            Input_Lite::SendInput(kevent);
        }
        UNUSED(socket);
    }

    void onKeyDown(bool ignoreIncomingKeyboardEvents, const std::shared_ptr<WS_LITE::IWSocket> &socket, const Input_Lite::KeyCodes &keycode)
    {
        if (!ignoreIncomingKeyboardEvents) {
            Input_Lite::KeyEvent kevent;
            kevent.Key = keycode;
            kevent.Pressed = true;
            Input_Lite::SendInput(kevent);
        }
        UNUSED(socket);
    }

    void onMouseScroll(bool ignoreIncomingMouseEvents, const std::shared_ptr<SL::WS_LITE::IWSocket> &socket, int offset)
    {
        if (!ignoreIncomingMouseEvents) {
            Input_Lite::MouseScrollEvent mbevent;
            mbevent.Offset = offset;
            Input_Lite::SendInput(mbevent);
        }
        UNUSED(socket);
    }

    void onMouseUp(bool ignoreIncomingMouseEvents, const std::shared_ptr<WS_LITE::IWSocket> &socket, const Input_Lite::MouseButtons &button)
    {
        if (!ignoreIncomingMouseEvents) {
            Input_Lite::MouseButtonEvent mbevent;
            mbevent.Pressed = false;
            mbevent.Button = button;
            Input_Lite::SendInput(mbevent);
        }
        UNUSED(socket);
    }
    void onMouseDown(bool ignoreIncomingMouseEvents, const std::shared_ptr<WS_LITE::IWSocket> &socket, const Input_Lite::MouseButtons &button)
    {
        if (!ignoreIncomingMouseEvents) {
            Input_Lite::MouseButtonEvent mbevent;
            mbevent.Pressed = true;
            mbevent.Button = button;
            Input_Lite::SendInput(mbevent);
        }
        UNUSED(socket);
    }
    void onMousePosition(bool ignoreIncomingMouseEvents, const std::shared_ptr<SL::WS_LITE::IWSocket> &socket, const RAT_Lite::Point &pos)
    {
        if (!ignoreIncomingMouseEvents) {
            Input_Lite::MousePositionAbsoluteEvent mbevent;
            mbevent.X = pos.X;
            mbevent.Y = pos.Y;
            Input_Lite::SendInput(mbevent);
        }
        UNUSED(socket);
    }
    void onClipboardChanged(bool shareclip, const std::string &str, std::shared_ptr<Clipboard_Lite::IClipboard_Manager> clipboard)
    {
        if (shareclip) {
            clipboard->copy(str);
        }
    }
} // namespace RAT_Server
} // namespace SL
