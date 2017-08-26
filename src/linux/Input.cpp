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
    void SendInput(const KeyEvent& e)
    {
        auto display = XOpenDisplay(NULL);
        auto mappedkey = ConvertToNative(e.Key);
        if(mappedkey == 255)
            return; // key doesnt exist 
        XTestFakeKeyEvent(display, mappedkey, e.Pressed ? True : False, CurrentTime);
        XCloseDisplay(display);
    }
    void SendInput(const MouseButtonEvent& e)
    {
        auto display = XOpenDisplay(NULL);
        switch(e.Button) {
        case MouseButtons::LEFT:
            XTestFakeButtonEvent(display, Button1, e.Pressed ? True : False, CurrentTime);
            break;
        case MouseButtons::MIDDLE:
            XTestFakeButtonEvent(display, Button2, e.Pressed ? True : False, CurrentTime);
            break;
        case MouseButtons::RIGHT:
            XTestFakeButtonEvent(display, Button3, e.Pressed ? True : False, CurrentTime);
            break;
        default:
            break;
        }
        XCloseDisplay(display);
    }
    void SendInput(const MouseScrollEvent& e)
    {
        auto display = XOpenDisplay(NULL);
        if(e.Offset < 0) {
            for(auto i = 0; i < abs(e.Offset) && i < 5; i++) { /// cap at 5
                XTestFakeButtonEvent(display, Button5, True, CurrentTime);
                XTestFakeButtonEvent(display, Button5, False, CurrentTime);
            }
        } else if(e.Offset > 0) {
            for(auto i = 0; i < e.Offset && i < 5; i++) { /// cap at 5
                XTestFakeButtonEvent(display, Button4, True, CurrentTime);
                XTestFakeButtonEvent(display, Button4, False, CurrentTime);
            }
        }
        XCloseDisplay(display);
    }
    void SendInput(const MousePositionOffsetEvent& e)
    {
        auto display = XOpenDisplay(NULL);
        XTestFakeRelativeMotionEvent(display, -1, e.X, e.Y);
        XCloseDisplay(display);
    }
    void SendInput(const MousePositionAbsoluteEvent& e)
    {
        auto display = XOpenDisplay(NULL);
        XTestFakeMotionEvent(display, -1, e.X, e.Y, CurrentTime);
        XCloseDisplay(display);
    }

    KeyCode ConvertToNative(Input_Lite::KeyCodes key)
    {
        switch(key) {
        case KeyCodes::KEY_A:
            return 38;
        case KeyCodes::KEY_B:
            return 56;
        case KeyCodes::KEY_C:
            return 54;
        case KeyCodes::KEY_D:
            return 40;
        case KeyCodes::KEY_E:
            return 26;
        case KeyCodes::KEY_F:
            return 41;
        case KeyCodes::KEY_G:
            return 42;
        case KeyCodes::KEY_H:
            return 43;
        case KeyCodes::KEY_I:
            return 31;
        case KeyCodes::KEY_J:
            return 44;
        case KeyCodes::KEY_K:
            return 45;
        case KeyCodes::KEY_L:
            return 46;
        case KeyCodes::KEY_M:
            return 58;
        case KeyCodes::KEY_N:
            return 57;
        case KeyCodes::KEY_O:
            return 32;
        case KeyCodes::KEY_P:
            return 33;
        case KeyCodes::KEY_Q:
            return 24;
        case KeyCodes::KEY_R:
            return 27;
        case KeyCodes::KEY_S:
            return 39;
        case KeyCodes::KEY_T:
            return 28;
        case KeyCodes::KEY_U:
            return 30;
        case KeyCodes::KEY_V:
            return 55;
        case KeyCodes::KEY_W:
            return 25;
        case KeyCodes::KEY_X:
            return 53;
        case KeyCodes::KEY_Y:
            return 29;
        case KeyCodes::KEY_Z:
            return 52;
        case KeyCodes::KEY_1:
            return 10;
        case KeyCodes::KEY_2:
            return 11;
        case KeyCodes::KEY_3:
            return 12;
        case KeyCodes::KEY_4:
            return 13;
        case KeyCodes::KEY_5:
            return 14;
        case KeyCodes::KEY_6:
            return 15;
        case KeyCodes::KEY_7:
            return 16;
        case KeyCodes::KEY_8:
            return 17;
        case KeyCodes::KEY_9:
            return 18;
        case KeyCodes::KEY_0:
            return 19;
        case KeyCodes::KEY_Enter:
            return 36;
        case KeyCodes::KEY_Escape:
            return 9;
        case KeyCodes::KEY_Backspace:
            return 22;
        case KeyCodes::KEY_Tab:
            return 23;
        case KeyCodes::KEY_Space:
            return 65;
        case KeyCodes::KEY_Minus:
            return 20;
        case KeyCodes::KEY_Equals:
            return 21;
        case KeyCodes::KEY_LeftBracket:
            return 34;
        case KeyCodes::KEY_RightBracket:
            return 35;
        case KeyCodes::KEY_Backslash:
            return 51;
        case KeyCodes::KEY_Semicolon:
            return 47;
        case KeyCodes::KEY_Quote:
            return 48;
        case KeyCodes::KEY_Grave:
            return 49;
        case KeyCodes::KEY_Comma:
            return 59;
        case KeyCodes::KEY_Period:
            return 60;
        case KeyCodes::KEY_Slash:
            return 61;
        case KeyCodes::KEY_CapsLock:
            return 66;
        case KeyCodes::KEY_F1:
        case KeyCodes::KEY_F2:
        case KeyCodes::KEY_F3:
        case KeyCodes::KEY_F4:
        case KeyCodes::KEY_F5:
        case KeyCodes::KEY_F6:
        case KeyCodes::KEY_F7:
        case KeyCodes::KEY_F8:
        case KeyCodes::KEY_F9:
        case KeyCodes::KEY_F10:
            return 67 + (key - KeyCodes::KEY_F1);
        case KeyCodes::KEY_F11:
            return 95;
        case KeyCodes::KEY_F12:
            return 96;
        case KeyCodes::KEY_F13:
        case KeyCodes::KEY_F14:
        case KeyCodes::KEY_F15:
        case KeyCodes::KEY_F16:
        case KeyCodes::KEY_F17:
        case KeyCodes::KEY_F18:
        case KeyCodes::KEY_F19:
        case KeyCodes::KEY_F20:
        case KeyCodes::KEY_F21:
        case KeyCodes::KEY_F22:
        case KeyCodes::KEY_F23:
        case KeyCodes::KEY_F24:
            return 191 + (key - KeyCodes::KEY_F13);
        case KeyCodes::KEY_PrintScreen:
            return 107;
        case KeyCodes::KEY_ScrollLock:
            return 78;
        case KeyCodes::KEY_Pause:
            return 127;
        case KeyCodes::KEY_Insert:
            return 118;
        case KeyCodes::KEY_Home:
            return 110;
        case KeyCodes::KEY_PageUp:
            return 112;
        case KeyCodes::KEY_Delete:
            return 119;
        case KeyCodes::KEY_End:
            return 115;
        case KeyCodes::KEY_PageDown:
            return 117;
        case KeyCodes::KEY_Right:
            return 114;
        case KeyCodes::KEY_Left:
            return 113;
        case KeyCodes::KEY_Down:
            return 116;
        case KeyCodes::KEY_Up:
            return 111;
        case KeyCodes::KP_NumLock:
            return 77;
        case KeyCodes::KP_Divide:
            return 106;
        case KeyCodes::KP_Multiply:
            return 63;
        case KeyCodes::KP_Subtract:
            return 82;
        case KeyCodes::KP_Add:
            return 86;
        case KeyCodes::KP_Enter:
            return 104;
        case KeyCodes::KP_1:
            return 87;
        case KeyCodes::KP_2:
            return 88;
        case KeyCodes::KP_3:
            return 89;
        case KeyCodes::KP_4:
            return 83;
        case KeyCodes::KP_5:
            return 84;
        case KeyCodes::KP_6:
            return 85;
        case KeyCodes::KP_7:
            return 79;
        case KeyCodes::KP_8:
            return 80;
        case KeyCodes::KP_9:
            return 81;
        case KeyCodes::KP_0:
            return 90;
        case KeyCodes::KP_Point:
            return 91;
        case KeyCodes::KEY_Help:
            return 146;
        case KeyCodes::KEY_Menu:
            return 135;
        case KeyCodes::KEY_LeftControl:
            return 37;
        case KeyCodes::KEY_LeftShift:
            return 50;
        case KeyCodes::KEY_LeftAlt:
            return 64;
        case KeyCodes::KEY_LeftMeta:
            return 133;
        case KeyCodes::KEY_RightControl:
            return 105;
        case KeyCodes::KEY_RightShift:
            return 62;
        case KeyCodes::KEY_RightAlt:
            return 108;
        case KeyCodes::KEY_RightMeta:
            return 134;
        default:
            return 255;
        }
    }

    Input_Lite::KeyCodes ConvertToKeyCode(KeyCode key)
    {
        switch(key) {
        case 9:
            return KeyCodes::KEY_Escape;
        case 10:
            return KeyCodes::KEY_1;
        case 11:
            return KeyCodes::KEY_2;
        case 12:
            return KeyCodes::KEY_3;
        case 13:
            return KeyCodes::KEY_4;
        case 14:
            return KeyCodes::KEY_5;
        case 15:
            return KeyCodes::KEY_6;
        case 16:
            return KeyCodes::KEY_7;
        case 17:
            return KeyCodes::KEY_8;
        case 18:
            return KeyCodes::KEY_9;
        case 19:
            return KeyCodes::KEY_0;
        case 20:
            return KeyCodes::KEY_Minus;
        case 21:
            return KeyCodes::KEY_Equals;
        case 22:
            return KeyCodes::KEY_Backspace;
        case 23:
            return KeyCodes::KEY_Tab;
        case 24:
            return KeyCodes::KEY_Q;
        case 25:
            return KeyCodes::KEY_W;
        case 26:
            return KeyCodes::KEY_E;
        case 27:
            return KeyCodes::KEY_R;
        case 28:
            return KeyCodes::KEY_T;
        case 29:
            return KeyCodes::KEY_Y;
        case 30:
            return KeyCodes::KEY_U;
        case 31:
            return KeyCodes::KEY_I;
        case 32:
            return KeyCodes::KEY_O;
        case 33:
            return KeyCodes::KEY_P;
        case 34:
            return KeyCodes::KEY_LeftBracket;
        case 35:
            return KeyCodes::KEY_RightBracket;
        case 36:
            return KeyCodes::KEY_Enter;
        case 37:
            return KeyCodes::KEY_LeftControl;
        case 38:
            return KeyCodes::KEY_A;
        case 39:
            return KeyCodes::KEY_S;
        case 40:
            return KeyCodes::KEY_D;
        case 41:
            return KeyCodes::KEY_F;
        case 42:
            return KeyCodes::KEY_G;
        case 43:
            return KeyCodes::KEY_H;
        case 44:
            return KeyCodes::KEY_J;
        case 45:
            return KeyCodes::KEY_K;
        case 46:
            return KeyCodes::KEY_L;
        case 47:
            return KeyCodes::KEY_Semicolon;
        case 48:
            return KeyCodes::KEY_Quote;
        case 49:
            return KeyCodes::KEY_Grave;
        case 50:
            return KeyCodes::KEY_LeftShift;
        case 51:
            return KeyCodes::KEY_Backslash;
        case 52:
            return KeyCodes::KEY_Z;
        case 53:
            return KeyCodes::KEY_X;
        case 54:
            return KeyCodes::KEY_C;
        case 55:
            return KeyCodes::KEY_V;
        case 56:
            return KeyCodes::KEY_B;
        case 57:
            return KeyCodes::KEY_N;
        case 58:
            return KeyCodes::KEY_M;
        case 59:
            return KeyCodes::KEY_Comma;
        case 60:
            return KeyCodes::KEY_Period;
        case 61:
            return KeyCodes::KEY_Slash;
        case 62:
            return KeyCodes::KEY_RightShift;
        case 63:
            return KeyCodes::KP_Multiply;
        case 64:
            return KeyCodes::KEY_LeftAlt;
        case 65:
            return KeyCodes::KEY_Space;
        case 66:
            return KeyCodes::KEY_CapsLock;
        case 67: // f1
        case 68:
        case 69:
        case 70:
        case 71:
        case 72:
        case 73:
        case 74:
        case 75:
        case 76: // f10
            return static_cast<KeyCodes>(KeyCodes::KEY_F1 + (key - 67));
        case 77:
            return KeyCodes::KP_NumLock;
        case 78:
            return KeyCodes::KEY_ScrollLock;
        case 79:
            return KeyCodes::KP_7;
        case 80:
            return KeyCodes::KP_8;
        case 81:
            return KeyCodes::KP_9;
        case 82:
            return KeyCodes::KP_Subtract;
        case 83:
            return KeyCodes::KP_4;
        case 84:
            return KeyCodes::KP_5;
        case 85:
            return KeyCodes::KP_6;
        case 86:
            return KeyCodes::KP_Add;
        case 87:
            return KeyCodes::KP_1;
        case 88:
            return KeyCodes::KP_2; 
        case 89:
            return KeyCodes::KP_3;
        case 90:
            return KeyCodes::KP_0;
        case 91:
            return KeyCodes::KP_Point;
        case 94:
            return KeyCodes::KEY_NonUSBackslash;
        case 95:
            return KeyCodes::KEY_F11;
        case 96:
            return KeyCodes::KEY_F12;
        case 104:
            return KeyCodes::KP_Enter;
        case 105:
            return KeyCodes::KEY_RightControl;
        case 106:
            return KeyCodes::KP_Divide;
        case 107:
            return KeyCodes::KEY_PrintScreen;
        case 108:
            return KeyCodes::KEY_RightAlt;
        case 110:
            return KeyCodes::KEY_Home;
        case 111:
            return KeyCodes::KEY_Up;
        case 112:
            return KeyCodes::KEY_PageUp;
        case 113:
            return KeyCodes::KEY_Left;
        case 114:
            return KeyCodes::KEY_Right;
        case 115:
            return KeyCodes::KEY_End;
        case 116:
            return KeyCodes::KEY_Down;
        case 117:
            return KeyCodes::KEY_PageDown;
        case 118:
            return KeyCodes::KEY_Insert;
        case 119:
            return KeyCodes::KEY_Delete;
        case 125:
            return KeyCodes::KP_Equals;
        case 127:
            return KeyCodes::KEY_Pause; 
        case 133:
            return KeyCodes::KEY_LeftMeta;
        case 134:
            return KeyCodes::KEY_RightMeta;
        case 135:
            return KeyCodes::KEY_Menu;
            case 146:
            return KeyCodes::KEY_Help;
        case 191://f13
        case 192:
        case 193:
        case 194:
        case 195:
        case 196:
        case 197:
        case 198:
        case 199:
        case 200:
        case 201:
        case 202:
            return static_cast<KeyCodes>(KeyCodes::KEY_F13 + (key - 191));
        default:
            return SL::Input_Lite::KeyCodes::INVALID;
        }
    }
}
}