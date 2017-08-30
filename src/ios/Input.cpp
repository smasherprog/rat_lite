#include "Input_Lite.h"
#include <ApplicationServices/ApplicationServices.h>
#include <Carbon/Carbon.h>

namespace SL {
namespace Input_Lite {
    void SendInput(const KeyEvent &e)
    {
        auto ev = CGEventCreateKeyboardEvent(NULL, ConvertToNative(e.Key), e.Pressed);
        if (ev) {
            CGEventPost(kCGHIDEventTap, ev);
            CFRelease(ev);
        }
    }
    void SendInput(const MouseButtonEvent &e)
    {
        auto msevent = CGEventCreate(NULL);
        auto loc = CGEventGetLocation(msevent);
        CFRelease(msevent);

        CGEventRef ev = nullptr;
        switch (e.Button) {
        case MouseButtons::LEFT:
            ev = CGEventCreateMouseEvent(NULL, e.Pressed ? kCGEventLeftMouseDown : kCGEventLeftMouseUp, loc, kCGMouseButtonLeft);
                break;
        case MouseButtons::MIDDLE:
            ev = CGEventCreateMouseEvent(NULL, e.Pressed ? kCGEventOtherMouseDown : kCGEventOtherMouseUp, loc, kCGMouseButtonCenter);
                  break;
        case MouseButtons::RIGHT:
            ev = CGEventCreateMouseEvent(NULL, e.Pressed ? kCGEventRightMouseDown : kCGEventRightMouseUp, loc, kCGMouseButtonRight);
                break;
        default:
            break;
        }
        if (ev) {
            CGEventPost(kCGHIDEventTap, ev);
            CFRelease(ev);
        }
    }
    void SendInput(const MouseScrollEvent &e)
    {
        auto ev = CGEventCreateScrollWheelEvent(NULL, kCGScrollEventUnitLine, 1, -e.Offset);
        if (ev) {
            CGEventPost(kCGHIDEventTap, ev);
            CFRelease(ev);
        }
    }
    void SendInput(const MousePositionOffsetEvent &e)
    {
        auto msevent = CGEventCreate(NULL);
        auto loc = CGEventGetLocation(msevent);
        CFRelease(msevent);
        loc.x += e.X;
        loc.y += e.Y;
        CGWarpMouseCursorPosition(loc);
    }
    void SendInput(const MousePositionAbsoluteEvent &e)
    {
        CGPoint p;
        p.x = e.X;
        p.y = e.Y;
        CGWarpMouseCursorPosition(p);
    }
    CGKeyCode ConvertToNative(KeyCodes key)
    {
        switch (key) {

        case KeyCodes::KEY_A:
            return kVK_ANSI_A;
        case KeyCodes::KEY_B:
            return kVK_ANSI_B;
        case KeyCodes::KEY_C:
            return kVK_ANSI_C;
        case KeyCodes::KEY_D:
            return kVK_ANSI_D;
        case KeyCodes::KEY_E:
            return kVK_ANSI_E;
        case KeyCodes::KEY_F:
            return kVK_ANSI_F;
        case KeyCodes::KEY_G:
            return kVK_ANSI_G;
        case KeyCodes::KEY_H:
            return kVK_ANSI_H;
        case KeyCodes::KEY_I:
            return kVK_ANSI_I;
        case KeyCodes::KEY_J:
            return kVK_ANSI_J;
        case KeyCodes::KEY_K:
            return kVK_ANSI_K;
        case KeyCodes::KEY_L:
            return kVK_ANSI_L;
        case KeyCodes::KEY_M:
            return kVK_ANSI_M;
        case KeyCodes::KEY_N:
            return kVK_ANSI_N;
        case KeyCodes::KEY_O:
            return kVK_ANSI_O;
        case KeyCodes::KEY_P:
            return kVK_ANSI_P;
        case KeyCodes::KEY_Q:
            return kVK_ANSI_Q;
        case KeyCodes::KEY_R:
            return kVK_ANSI_R;
        case KeyCodes::KEY_S:
            return kVK_ANSI_S;
        case KeyCodes::KEY_T:
            return kVK_ANSI_T;
        case KeyCodes::KEY_U:
            return kVK_ANSI_U;
        case KeyCodes::KEY_V:
            return kVK_ANSI_V;
        case KeyCodes::KEY_W:
            return kVK_ANSI_W;
        case KeyCodes::KEY_X:
            return kVK_ANSI_X;
        case KeyCodes::KEY_Y:
            return kVK_ANSI_Y;
        case KeyCodes::KEY_Z:
            return kVK_ANSI_Z;
        case KeyCodes::KEY_1:
            return kVK_ANSI_1;
        case KeyCodes::KEY_2:
            return kVK_ANSI_2;
        case KeyCodes::KEY_3:
            return kVK_ANSI_3;
        case KeyCodes::KEY_4:
            return kVK_ANSI_4;
        case KeyCodes::KEY_5:
            return kVK_ANSI_5;
        case KeyCodes::KEY_6:
            return kVK_ANSI_6;
        case KeyCodes::KEY_7:
            return kVK_ANSI_7;
        case KeyCodes::KEY_8:
            return kVK_ANSI_8;
        case KeyCodes::KEY_9:
            return kVK_ANSI_9;
        case KeyCodes::KEY_0:
            return kVK_ANSI_0;
        case KeyCodes::KEY_Enter:
            return kVK_Return;
        case KeyCodes::KEY_Escape:
            return kVK_Escape;
        case KeyCodes::KEY_Backspace:
            return kVK_Delete;
        case KeyCodes::KEY_Tab:
            return kVK_Tab;
        case KeyCodes::KEY_Space:
            return kVK_Space;
        case KeyCodes::KEY_Minus:
            return kVK_ANSI_Minus;
        case KeyCodes::KEY_Equals:
            return kVK_ANSI_Equal;
        case KeyCodes::KEY_LeftBracket:
            return kVK_ANSI_LeftBracket;
        case KeyCodes::KEY_RightBracket:
            return kVK_ANSI_RightBracket;
        case KeyCodes::KEY_Backslash:
            return kVK_ANSI_Backslash;
        case KeyCodes::KEY_Semicolon:
            return kVK_ANSI_Semicolon;
        case KeyCodes::KEY_Quote:
            return kVK_ANSI_Quote;
        case KeyCodes::KEY_Grave:
            return kVK_ANSI_Grave;
        case KeyCodes::KEY_Comma:
            return kVK_ANSI_Comma;
        case KeyCodes::KEY_Period:
            return kVK_ANSI_Period;
        case KeyCodes::KEY_Slash:
            return kVK_ANSI_Slash;
        case KeyCodes::KEY_CapsLock:
            return kVK_CapsLock;
        case KeyCodes::KEY_F1:
            return kVK_F1;
        case KeyCodes::KEY_F2:
            return kVK_F2;
        case KeyCodes::KEY_F3:
            return kVK_F3;
        case KeyCodes::KEY_F4:
            return kVK_F4;
        case KeyCodes::KEY_F5:
            return kVK_F5;
        case KeyCodes::KEY_F6:
            return kVK_F6;
        case KeyCodes::KEY_F7:
            return kVK_F7;
        case KeyCodes::KEY_F8:
            return kVK_F8;
        case KeyCodes::KEY_F9:
            return kVK_F9;
        case KeyCodes::KEY_F10:
            return kVK_F10;
        case KeyCodes::KEY_F11:
            return kVK_F11;
        case KeyCodes::KEY_F12:
            return kVK_F12;
        case KeyCodes::KEY_F13:
            return kVK_F13;
        case KeyCodes::KEY_F14:
            return kVK_F14;
        case KeyCodes::KEY_F15:
            return kVK_F15;
        case KeyCodes::KEY_F16:
            return kVK_F16;
        case KeyCodes::KEY_F17:
            return kVK_F17;
        case KeyCodes::KEY_F18:
            return kVK_F18;
        case KeyCodes::KEY_F19:
            return kVK_F19;
        case KeyCodes::KEY_F20:
            return kVK_F20;
            /*
        case KeyCodes::KEY_F21:
            return kVK_F1;
        case KeyCodes::KEY_F22:
            return kVK_F1;
        case KeyCodes::KEY_F23:
            return kVK_F1;
        case KeyCodes::KEY_F24:
            return kVK_F1;
        



        case KeyCodes::KEY_PrintScreen:
            return VK_SNAPSHOT;
        case KeyCodes::KEY_ScrollLock:
            return VK_SCROLL;
        case KeyCodes::KEY_Pause:
            return VK_PAUSE;
        case KeyCodes::KEY_Insert:
            return VK_INSERT;
            */
        case KeyCodes::KEY_Home:
            return kVK_Home;
        case KeyCodes::KEY_PageUp:
            return kVK_PageUp;
        case KeyCodes::KEY_Delete:
            return kVK_ForwardDelete;
        case KeyCodes::KEY_End:
            return kVK_End;
        case KeyCodes::KEY_PageDown:
            return kVK_PageDown;
        case KeyCodes::KEY_Right:
            return kVK_RightArrow;
        case KeyCodes::KEY_Left:
            return kVK_LeftArrow;
        case KeyCodes::KEY_Down:
            return kVK_PageDown;
        case KeyCodes::KEY_Up:
            return kVK_UpArrow;
            /*
        case KeyCodes::KP_NumLock:
            return VK_NUMLOCK;
            */
        case KeyCodes::KP_Divide:
            return kVK_ANSI_KeypadDivide;
        case KeyCodes::KP_Multiply:
            return kVK_ANSI_KeypadMultiply;
        case KeyCodes::KP_Subtract:
            return kVK_ANSI_KeypadMinus;
        case KeyCodes::KP_Add:
            return kVK_ANSI_KeypadPlus;
        case KeyCodes::KP_Enter:
            return kVK_ANSI_KeypadEnter;
        case KeyCodes::KP_1:
            return kVK_ANSI_Keypad1;
        case KeyCodes::KP_2:
            return kVK_ANSI_Keypad2;
        case KeyCodes::KP_3:
            return kVK_ANSI_Keypad3;
        case KeyCodes::KP_4:
            return kVK_ANSI_Keypad4;
        case KeyCodes::KP_5:
            return kVK_ANSI_Keypad5;
        case KeyCodes::KP_6:
            return kVK_ANSI_Keypad6;
        case KeyCodes::KP_7:
            return kVK_ANSI_Keypad7;
        case KeyCodes::KP_8:
            return kVK_ANSI_Keypad8;
        case KeyCodes::KP_9:
            return kVK_ANSI_Keypad9;
        case KeyCodes::KP_0:
            return kVK_ANSI_Keypad0;
        case KeyCodes::KP_Point:
            return kVK_ANSI_KeypadDecimal;
        case KeyCodes::KP_Equals:
            return kVK_ANSI_KeypadEquals;
        case KeyCodes::KEY_Help:
            return kVK_Help;
        case KeyCodes::KEY_Menu:
            return kVK_Option;
        case KeyCodes::KEY_LeftControl:
            return kVK_Control;
        case KeyCodes::KEY_LeftShift:
            return kVK_Shift;
        case KeyCodes::KEY_LeftAlt:
            return kVK_Option;
        case KeyCodes::KEY_LeftMeta:
            return kVK_Command;
        case KeyCodes::KEY_RightControl:
            return kVK_RightControl;
        case KeyCodes::KEY_RightShift:
            return kVK_RightShift;
        case KeyCodes::KEY_RightAlt:
            return kVK_RightOption;
        case KeyCodes::KEY_RightMeta:
            return kVK_RightControl;
        default:
            return 255;
        }
    }
    KeyCodes ConvertToKeyCode(CGKeyCode key)
    {
        switch (key) {
        case kVK_ANSI_0:
            return KeyCodes::KEY_0;
        case kVK_ANSI_1:
            return KeyCodes::KEY_1;
        case kVK_ANSI_2:
            return KeyCodes::KEY_2;
        case kVK_ANSI_3:
            return KeyCodes::KEY_3;
        case kVK_ANSI_4:
            return KeyCodes::KEY_4;
        case kVK_ANSI_5:
            return KeyCodes::KEY_5;
        case kVK_ANSI_6:
            return KeyCodes::KEY_6;
        case kVK_ANSI_7:
            return KeyCodes::KEY_7;
        case kVK_ANSI_8:
            return KeyCodes::KEY_8;
        case kVK_ANSI_9:
            return KeyCodes::KEY_9;
        case kVK_ANSI_A:
            return KeyCodes::KEY_A;
        case kVK_ANSI_B:
            return KeyCodes::KEY_B;
        case kVK_ANSI_C:
            return KeyCodes::KEY_C;
        case kVK_ANSI_D:
            return KeyCodes::KEY_D;
        case kVK_ANSI_E:
            return KeyCodes::KEY_E;
        case kVK_ANSI_F:
            return KeyCodes::KEY_F;
        case kVK_ANSI_G:
            return KeyCodes::KEY_G;
        case kVK_ANSI_H:
            return KeyCodes::KEY_H;
        case kVK_ANSI_I:
            return KeyCodes::KEY_I;
        case kVK_ANSI_J:
            return KeyCodes::KEY_J;
        case kVK_ANSI_K:
            return KeyCodes::KEY_K;
        case kVK_ANSI_L:
            return KeyCodes::KEY_L;
        case kVK_ANSI_M:
            return KeyCodes::KEY_M;
        case kVK_ANSI_N:
            return KeyCodes::KEY_N;
        case kVK_ANSI_O:
            return KeyCodes::KEY_O;
        case kVK_ANSI_P:
            return KeyCodes::KEY_P;
        case kVK_ANSI_Q:
            return KeyCodes::KEY_Q;
        case kVK_ANSI_R:
            return KeyCodes::KEY_R;
        case kVK_ANSI_S:
            return KeyCodes::KEY_S;
        case kVK_ANSI_T:
            return KeyCodes::KEY_T;
        case kVK_ANSI_U:
            return KeyCodes::KEY_U;
        case kVK_ANSI_V:
            return KeyCodes::KEY_V;
        case kVK_ANSI_W:
            return KeyCodes::KEY_W;
        case kVK_ANSI_X:
            return KeyCodes::KEY_X;
        case kVK_ANSI_Y:
            return KeyCodes::KEY_Y;
        case kVK_ANSI_Z:
            return KeyCodes::KEY_Z;
        case kVK_Return:
            return KeyCodes::KEY_Enter;
        case kVK_Escape:
            return KeyCodes::KEY_Escape;
        case kVK_Delete:
            return KeyCodes::KEY_Backspace;
        case kVK_Tab:
            return KeyCodes::KEY_Tab;
        case kVK_Space:
            return KeyCodes::KEY_Space;
        case kVK_ANSI_Minus:
            return KeyCodes::KEY_Minus;
        case kVK_ANSI_Equal:
            return KeyCodes::KEY_Equals;
        case kVK_ANSI_LeftBracket:
            return KeyCodes::KEY_LeftBracket;
        case kVK_ANSI_RightBracket:
            return KeyCodes::KEY_RightBracket;
        case kVK_ANSI_Backslash:
            return KeyCodes::KEY_Backslash;
        case kVK_ANSI_Semicolon:
            return KeyCodes::KEY_Semicolon;
        case kVK_ANSI_Quote:
            return KeyCodes::KEY_Quote;
        case kVK_ANSI_Grave:
            return KeyCodes::KEY_Grave;
        case kVK_ANSI_Comma:
            return KeyCodes::KEY_Comma;
        case kVK_ANSI_Period:
            return KeyCodes::KEY_Period;
        case kVK_ANSI_Slash:
            return KeyCodes::KEY_Slash;
        case kVK_CapsLock:
            return KeyCodes::KEY_CapsLock;
        case kVK_F1:
            return KeyCodes::KEY_F1;
        case kVK_F2:
            return KeyCodes::KEY_F2;
        case kVK_F3:
            return KeyCodes::KEY_F3;
        case kVK_F4:
            return KeyCodes::KEY_F4;
        case kVK_F5:
            return KeyCodes::KEY_F5;
        case kVK_F6:
            return KeyCodes::KEY_F6;
        case kVK_F7:
            return KeyCodes::KEY_F7;
        case kVK_F8:
            return KeyCodes::KEY_F8;
        case kVK_F9:
            return KeyCodes::KEY_F9;
        case kVK_F10:
            return KeyCodes::KEY_F10;
        case kVK_F11:
            return KeyCodes::KEY_F11;
        case kVK_F12:
            return KeyCodes::KEY_F12;
        case kVK_F13:
            return KeyCodes::KEY_F13;
        case kVK_F14:
            return KeyCodes::KEY_F14;
        case kVK_F15:
            return KeyCodes::KEY_F15;
        case kVK_F16:
            return KeyCodes::KEY_F16;
        case kVK_F17:
            return KeyCodes::KEY_F17;
        case kVK_F18:
            return KeyCodes::KEY_F18;
        case kVK_F19:
            return KeyCodes::KEY_F19;
        case kVK_F20:
            return KeyCodes::KEY_F20;
            /* case VK_SNAPSHOT:
                 return KeyCodes::KEY_PrintScreen;
             case VK_SCROLL:
                 return KeyCodes::KEY_ScrollLock;
             case VK_PAUSE:
                 return KeyCodes::KEY_Pause;
             case VK_INSERT:
                 return KeyCodes::KEY_Insert;*/
        case kVK_Home:
            return KeyCodes::KEY_Home;
        case kVK_PageUp:
            return KeyCodes::KEY_PageUp;
        case kVK_ForwardDelete:
            return KeyCodes::KEY_Delete;
        case kVK_End:
            return KeyCodes::KEY_End;
        case kVK_PageDown:
            return KeyCodes::KEY_PageDown;
        case kVK_RightArrow:
            return KeyCodes::KEY_Right;
        case kVK_LeftArrow:
            return KeyCodes::KEY_Left;
        case kVK_DownArrow:
            return KeyCodes::KEY_Down;
        case kVK_UpArrow:
            return KeyCodes::KEY_Up;
            /*   case VK_NUMLOCK:
                   return KeyCodes::KP_NumLock;*/
        case kVK_ANSI_KeypadDivide:
            return KeyCodes::KP_Divide;
        case kVK_ANSI_KeypadMultiply:
            return KeyCodes::KP_Multiply;
        case kVK_ANSI_KeypadMinus:
            return KeyCodes::KP_Subtract;
        case kVK_ANSI_KeypadPlus:
            return KeyCodes::KP_Add;
        case kVK_ANSI_KeypadEnter:
            return KeyCodes::KP_Enter;
        case kVK_ANSI_Keypad0:
            return KeyCodes::KP_0;
        case kVK_ANSI_Keypad1:
            return KeyCodes::KP_1;
        case kVK_ANSI_Keypad2:
            return KeyCodes::KP_2;
        case kVK_ANSI_Keypad3:
            return KeyCodes::KP_3;
        case kVK_ANSI_Keypad4:
            return KeyCodes::KP_4;
        case kVK_ANSI_Keypad5:
            return KeyCodes::KP_5;
        case kVK_ANSI_Keypad6:
            return KeyCodes::KP_6;
        case kVK_ANSI_Keypad7:
            return KeyCodes::KP_7;
        case kVK_ANSI_Keypad8:
            return KeyCodes::KP_8;
        case kVK_ANSI_Keypad9:
            return KeyCodes::KP_9;
        case kVK_ANSI_KeypadDecimal:
            return KeyCodes::KP_Point;
        case kVK_ANSI_KeypadEquals:
            return KeyCodes::KP_Equals;
        case kVK_Help:
            return KeyCodes::KEY_Help;
        case kVK_Option:
            return KeyCodes::KEY_Menu;
        case kVK_Control:
            return KeyCodes::KEY_LeftControl;
        case kVK_Shift:
            return KeyCodes::KEY_LeftShift;
        case kVK_Command:
            return KeyCodes::KEY_LeftMeta;
        case kVK_RightControl:
            return KeyCodes::KEY_RightControl;
        case kVK_RightShift:
            return KeyCodes::KEY_RightShift;
        case kVK_RightOption:
            return KeyCodes::KEY_RightAlt;
        default:
            return SL::Input_Lite::KeyCodes::INVALID;
        }
    }

} // namespace Input_Lite
} // namespace SL

