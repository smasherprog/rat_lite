#include "Input_Lite.h"
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/extensions/XInput.h>
#include <X11/extensions/XTest.h>
#include <X11/keysym.h>

#include <iostream>
namespace SL
{
namespace Input_Lite
{

    void SendKey_Impl(char key, int pressed, Display* display)
    {
        if((key >= '0' && key <= '9') || (key >= 'a' && key <= 'z')) {
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
        return;
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

    void SendMouseScroll(int offset)
    {
        auto display = XOpenDisplay(NULL);
        if(offset < 0) {
            for(auto i = 0; i < abs(offset) && i < 5; i++) { /// cap at 5
                XTestFakeButtonEvent(display, Button5, True, CurrentTime);
                XTestFakeButtonEvent(display, Button5, False, CurrentTime);
            }
        } else if(offset> 0) {
            for(auto i = 0; i < offset && i < 5; i++) { /// cap at 5
                XTestFakeButtonEvent(display, Button4, True, CurrentTime);
                XTestFakeButtonEvent(display, Button4, False, CurrentTime);
            }
        }
        XCloseDisplay(display);
    } 
    void SendMousePosition(const Offset& offset)
    {
        auto display = XOpenDisplay(NULL);
        XTestFakeRelativeMotionEvent(display, -1, offset.X, offset.Y);
        XCloseDisplay(display);
    }
    void SendMousePosition(const AbsolutePos& a)
    {
        auto display = XOpenDisplay(NULL);
        XTestFakeMotionEvent(display, -1, a.X, a.Y, CurrentTime);
        XCloseDisplay(display);
    }
    void SendMouse_Impl(const MouseButtons button, Bool pressed)
    {
        auto display = XOpenDisplay(NULL);
        switch(button) {
        case MouseButtons::LEFT:
            XTestFakeButtonEvent(display, Button1, pressed, CurrentTime);
            break;
        case MouseButtons::MIDDLE:
            XTestFakeButtonEvent(display, Button2, pressed, CurrentTime);
             break;
        case MouseButtons::RIGHT:
            XTestFakeButtonEvent(display, Button3, pressed, CurrentTime);
             break;
        default:
            break;
        }
        XCloseDisplay(display);
    }
    void SendMouseUp(const MouseButtons button)
    {
        SendMouse_Impl(button, False);
    }
    void SendMouseDown(const MouseButtons button)
    {
        SendMouse_Impl(button, True);
    }
}
}