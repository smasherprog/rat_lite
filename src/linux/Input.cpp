#include "Input_Lite.h"
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/extensions/XInput.h>
#include <X11/extensions/XTest.h>
#include <X11/keysym.h>

namespace SL
{
namespace Input_Lite
{

    void SendKey_Impl(char key, int pressed, Display* display)
    {
        if(key >= ' ' && key <= '~') {
            char bufkey[2];
            bufkey[0] = key;
            bufkey[1] = 0;
            auto ky = XStringToKeysym(bufkey);
            auto k = XKeysymToKeycode(display, ky);
            XTestFakeKeyEvent(display, k, pressed, CurrentTime);

        } else {
        }
    }
    void SendKey_Impl(wchar_t key, int pressed, Display* display)
    {

        if(key < 128) {
            SendKey_Impl(static_cast<char>(key), pressed, display);
        } else {
            auto k = XKeysymToKeycode(display, key);
            XTestFakeKeyEvent(display, k, pressed, CurrentTime);
        }
    }
    void SendKeyUp(char key)
    {
        auto display = XOpenDisplay(NULL);
        SendKey_Impl(key, False, display);
        XCloseDisplay(display);
    }
    void SendKeyUp(wchar_t key)
    {
        auto display = XOpenDisplay(NULL);
        SendKey_Impl(key, False, display);
        XCloseDisplay(display);
    }
    void SendKeyDown(wchar_t key)
    {
        auto display = XOpenDisplay(NULL);
        SendKey_Impl(key, True, display);
        XCloseDisplay(display);
    }
    void SendKeyDown(char key)
    {
        auto display = XOpenDisplay(NULL);
        SendKey_Impl(key, True, display);
        XCloseDisplay(display);
    }
    void SendKeyUp(SpecialKeyCodes key)
    {
    }
    void SendKeyDown(SpecialKeyCodes key)
    {
    }
    void SendMouseUp(const MouseButtons& button)
    {
    }
    void SendMouseDown(const MouseButtons& button)
    {
    }

    void SendMouseScroll(int offset)
    {
    }

    void SendMousePosition(const Offset& offset)
    {
    }
    void SendMousePosition(const AbsolutePos& a)
    {
    }
}
}