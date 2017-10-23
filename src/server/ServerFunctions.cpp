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

    void onKeyUp(bool ignoreIncomingKeyboardEvents, const std::shared_ptr<WS_LITE::IWSocket> &socket, const Input_Lite::KeyCodes &keycode)
    {
        if (!ignoreIncomingKeyboardEvents) {
            Input_Lite::KeyEvent kevent;
            kevent.Key = keycode;
            kevent.Pressed = false;
            Input_Lite::SendInput(kevent);
        }
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
    void onClipboardChanged(const std::string &str, std::shared_ptr<Clipboard_Lite::IClipboard_Manager> clipboard)
    {
        SL_RAT_LOG(RAT_Lite::Logging_Levels::INFO_log_level, "onClipboardChanged " << str.size());
        clipboard->copy(str);
    }
} // namespace RAT_Server
} // namespace SL
