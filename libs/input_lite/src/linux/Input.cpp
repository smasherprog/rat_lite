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
    void SendKey_Impl(SpecialKeyCodes key, Bool pressed)
    {
        KeySym k;
        switch(key) {
        case SpecialKeyCodes::BACKSPACE:
            k = XK_BackSpace;
            break;
        case SpecialKeyCodes::TAB:
            k = XK_Tab;
            break;
        case SpecialKeyCodes::ENTER:
            k = XK_Return;
            break;
        case SpecialKeyCodes::SHIFTLEFT:
            k = XK_Shift_L;
            break;
        case SpecialKeyCodes::CONTROLLEFT:
            k = XK_Control_L;
            break;
        case SpecialKeyCodes::ALTLEFT:
            k = XK_Alt_L;
            break;
        case SpecialKeyCodes::CAPSLOCK:
            k = XK_Caps_Lock;
            break;
        case SpecialKeyCodes::ESCAPE:
            k = XK_Escape;
            break;
        case SpecialKeyCodes::SPACE:
            k = XK_KP_Space;
            break;
        case SpecialKeyCodes::PAGEUP:
            k = XK_KP_Page_Up;
            break;
        case SpecialKeyCodes::PAGEDOWN:
            k = XK_KP_Page_Down;
            break;
        case SpecialKeyCodes::END:
            k = XK_KP_End;
            break;
        case SpecialKeyCodes::HOME:
            k = XK_KP_Home;
            break;
        case SpecialKeyCodes::ARROWLEFT:
            k = XK_Left;
            break;
        case SpecialKeyCodes::ARROWUP:
            k = XK_Up;
            break;
        case SpecialKeyCodes::ARROWRIGHT:
            k = XK_Right;
            break;
        case SpecialKeyCodes::ARROWDOWN:
            k = XK_Down;
            break;

        case SpecialKeyCodes::INSERT:
            k = XK_Insert;
            break;
        case SpecialKeyCodes::DELETE:
            k = XK_Delete;
            break;
        case SpecialKeyCodes::PRINTSCREEN:
            k = XK_Print;
            break;
        case SpecialKeyCodes::SCROLLLOCK:
            k = XK_Scroll_Lock;
            break;
        case SpecialKeyCodes::PAUSE:
            k = XK_Pause;
            break;
        case SpecialKeyCodes::OSLEFT:
            k = XK_Meta_L;
            break;
        case SpecialKeyCodes::F1:
            k = XK_F1;
            break;
        case SpecialKeyCodes::F2:
            k = XK_F2;
            break;
        case SpecialKeyCodes::F3:
            k = XK_F3;
            break;
        case SpecialKeyCodes::F4:
            k = XK_F4;
            break;
        case SpecialKeyCodes::F5:
            k = XK_F5;
            break;
        case SpecialKeyCodes::F6:
            k = XK_F6;
            break;
        case SpecialKeyCodes::F7:
            k = XK_F7;
            break;
        case SpecialKeyCodes::F8:
            k = XK_F8;
            break;
        case SpecialKeyCodes::F9:
            k = XK_F9;
            break;
        case SpecialKeyCodes::F10:
            k = XK_F10;
            break;
        case SpecialKeyCodes::F11:
            k = XK_F11;
            break;
        case SpecialKeyCodes::F12:
            k = XK_F12;
            break;
        case SpecialKeyCodes::F13:
            k = XK_F13;
            break;
        case SpecialKeyCodes::F14:
            k = XK_F14;
            break;
        case SpecialKeyCodes::F15:
            k = XK_F15;
            break;
        case SpecialKeyCodes::F16:
            k = XK_F16;
            break;
        case SpecialKeyCodes::F17:
            k = XK_F17;
            break;
        case SpecialKeyCodes::F18:
            k = XK_F18;
            break;

        case SpecialKeyCodes::NUMLOCK:
            k = XK_Num_Lock;
            break;
        case SpecialKeyCodes::NUMPAD0:
            k = XK_KP_0;
            break;
        case SpecialKeyCodes::NUMPAD1:
            k = XK_KP_1;
            break;
        case SpecialKeyCodes::NUMPAD2:
            k = XK_KP_2;
            break;
        case SpecialKeyCodes::NUMPAD3:
            k = XK_KP_3;
            break;
        case SpecialKeyCodes::NUMPAD4:
            k = XK_KP_4;
            break;
        case SpecialKeyCodes::NUMPAD5:
            k = XK_KP_5;
            break;
        case SpecialKeyCodes::NUMPAD6:
            k = XK_KP_6;
            break;
        case SpecialKeyCodes::NUMPAD7:
            k = XK_KP_7;
            break;
        case SpecialKeyCodes::NUMPAD8:
            k = XK_KP_8;
            break;
        case SpecialKeyCodes::NUMPAD9:
            k = XK_KP_9;
            break;

        case SpecialKeyCodes::NUMPADADD:
            k = XK_KP_Add;
            break;
        case SpecialKeyCodes::NUMPADDECIMAL:
            k = XK_KP_Decimal;
            break;
        case SpecialKeyCodes::NUMPADDIVIDE:
            k = XK_KP_Divide;
            break;
        case SpecialKeyCodes::NUMPADMULTIPLY:
            k = XK_KP_Multiply;
            break;
        case SpecialKeyCodes::NUMPADSUBTRACT:
            k = XK_KP_Separator;
            break;
        default:
            return;
        }
        auto display = XOpenDisplay(NULL); 
        auto keyp = XKeysymToKeycode (display, k);
        XTestFakeKeyEvent(display,keyp , pressed, CurrentTime);
        XCloseDisplay(display);
    }
    void SendKeyUp(SpecialKeyCodes key)
    {
        SendKey_Impl(key, False);
    }
    void SendKeyDown(SpecialKeyCodes key)
    {
        SendKey_Impl(key, True);
    }

    void SendMouseScroll(int offset)
    {
        auto display = XOpenDisplay(NULL);
        if(offset < 0) {
            for(auto i = 0; i < abs(offset) && i < 5; i++) { /// cap at 5
                XTestFakeButtonEvent(display, Button5, True, CurrentTime);
                XTestFakeButtonEvent(display, Button5, False, CurrentTime);
            }
        } else if(offset > 0) {
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