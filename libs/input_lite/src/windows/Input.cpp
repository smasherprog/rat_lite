#include "InputManager.h"
#include "Input_Lite.h"

#include <string>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace SL {
namespace Input_Lite {

    void SendInput(const KeyEvent &e)
    {
        INPUT k = {0};
        k.type = INPUT_KEYBOARD;
        k.ki.dwFlags = e.Pressed ? 0 : KEYEVENTF_KEYUP;
        k.ki.wVk = static_cast<WORD>(ConvertToNative(e.Key));
        if (k.ki.wVk == 255) {
            return; // no good!
        }
        SendInput(1, &k, sizeof(INPUT));
    }
    void SendInput(const MouseButtonEvent &e)
    {
        INPUT inp = {0};
        inp.type = INPUT_MOUSE;
        switch (e.Button) {
        case (MouseButtons::LEFT):
            inp.mi.dwFlags = e.Pressed ? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_LEFTUP;
            break;
        case (MouseButtons::RIGHT):
            inp.mi.dwFlags = e.Pressed ? MOUSEEVENTF_RIGHTDOWN : MOUSEEVENTF_RIGHTUP;
            break;
        case (MouseButtons::MIDDLE):
            inp.mi.dwFlags = e.Pressed ? MOUSEEVENTF_MIDDLEDOWN : MOUSEEVENTF_MIDDLEUP;
            break;
        default:
            return;
        }
        SendInput(1, &inp, sizeof(INPUT));
    }

    void SendInput(const MouseScrollEvent &e)
    {
        INPUT inp = {0};
        inp.type = INPUT_MOUSE;
        inp.mi.dwFlags = MOUSEEVENTF_WHEEL;
        inp.mi.mouseData = e.Offset * 120;
        SendInput(1, &inp, sizeof(INPUT));
    }

    void SendMousePosition_Impl(int x, int y, int modifier)
    {
        INPUT inp = {0};
        inp.type = INPUT_MOUSE;
        inp.mi.dwFlags = MOUSEEVENTF_MOVE | modifier;
        inp.mi.dx = x;
        inp.mi.dy = y;
        SendInput(1, &inp, sizeof(INPUT));
    }
    void SendInput(const MousePositionOffsetEvent &e) { SendMousePosition_Impl(e.X, e.Y, 0); }
    void SendInput(const MousePositionAbsoluteEvent &e)
    {
        SendMousePosition_Impl((e.X * 65536) / GetSystemMetrics(SM_CXSCREEN), (e.Y * 65536) / GetSystemMetrics(SM_CYSCREEN), MOUSEEVENTF_ABSOLUTE);
    }
    DWORD ConvertToNative(KeyCodes key)
    {
        switch (key) {
        case KeyCodes::KEY_A:
        case KeyCodes::KEY_B:
        case KeyCodes::KEY_C:
        case KeyCodes::KEY_D:
        case KeyCodes::KEY_E:
        case KeyCodes::KEY_F:
        case KeyCodes::KEY_G:
        case KeyCodes::KEY_H:
        case KeyCodes::KEY_I:
        case KeyCodes::KEY_J:
        case KeyCodes::KEY_K:
        case KeyCodes::KEY_L:
        case KeyCodes::KEY_M:
        case KeyCodes::KEY_N:
        case KeyCodes::KEY_O:
        case KeyCodes::KEY_P:
        case KeyCodes::KEY_Q:
        case KeyCodes::KEY_R:
        case KeyCodes::KEY_S:
        case KeyCodes::KEY_T:
        case KeyCodes::KEY_U:
        case KeyCodes::KEY_V:
        case KeyCodes::KEY_W:
        case KeyCodes::KEY_X:
        case KeyCodes::KEY_Y:
        case KeyCodes::KEY_Z:
            return static_cast<int>('A') + (key - KeyCodes::KEY_A);
        case KeyCodes::KEY_1:
        case KeyCodes::KEY_2:
        case KeyCodes::KEY_3:
        case KeyCodes::KEY_4:
        case KeyCodes::KEY_5:
        case KeyCodes::KEY_6:
        case KeyCodes::KEY_7:
        case KeyCodes::KEY_8:
        case KeyCodes::KEY_9:
            return static_cast<int>('1') + (key - KeyCodes::KEY_1);
        case KeyCodes::KEY_0:
            return static_cast<int>('0');
        case KeyCodes::KEY_Enter:
            return VK_RETURN;
        case KeyCodes::KEY_Escape:
            return VK_ESCAPE;
        case KeyCodes::KEY_Backspace:
            return VK_BACK;
        case KeyCodes::KEY_Tab:
            return VK_TAB;
        case KeyCodes::KEY_Space:
            return VK_SPACE;
        case KeyCodes::KEY_Minus:
            return VK_OEM_MINUS;
        case KeyCodes::KEY_Equals:
            return VK_OEM_PLUS; // this is correct and not a mistype
        case KeyCodes::KEY_LeftBracket:
            return VK_OEM_4;
        case KeyCodes::KEY_RightBracket:
            return VK_OEM_6;
        case KeyCodes::KEY_Backslash:
            return VK_OEM_5;
        case KeyCodes::KEY_Semicolon:
            return VK_OEM_1;
        case KeyCodes::KEY_Quote:
            return VK_OEM_7;
        case KeyCodes::KEY_Grave:
            return VK_OEM_3;
        case KeyCodes::KEY_Comma:
            return VK_OEM_COMMA;
        case KeyCodes::KEY_Period:
            return VK_OEM_PERIOD;
        case KeyCodes::KEY_Slash:
            return VK_OEM_2;
        case KeyCodes::KEY_CapsLock:
            return VK_CAPITAL;
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
        case KeyCodes::KEY_F11:
        case KeyCodes::KEY_F12:
            return VK_F1 + (key - KeyCodes::KEY_F1);
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
            return VK_F13 + (key - KeyCodes::KEY_F13);
        case KeyCodes::KEY_PrintScreen:
            return VK_SNAPSHOT;
        case KeyCodes::KEY_ScrollLock:
            return VK_SCROLL;
        case KeyCodes::KEY_Pause:
            return VK_PAUSE;
        case KeyCodes::KEY_Insert:
            return VK_INSERT;
        case KeyCodes::KEY_Home:
            return VK_HOME;
        case KeyCodes::KEY_PageUp:
            return VK_PRIOR;
        case KeyCodes::KEY_Delete:
            return VK_DELETE;
        case KeyCodes::KEY_End:
            return VK_END;
        case KeyCodes::KEY_PageDown:
            return VK_NEXT;
        case KeyCodes::KEY_Right:
            return VK_RIGHT;
        case KeyCodes::KEY_Left:
            return VK_LEFT;
        case KeyCodes::KEY_Down:
            return VK_DOWN;
        case KeyCodes::KEY_Up:
            return VK_UP;
        case KeyCodes::KP_NumLock:
            return VK_NUMLOCK;
        case KeyCodes::KP_Divide:
            return VK_DIVIDE;
        case KeyCodes::KP_Multiply:
            return VK_MULTIPLY;
        case KeyCodes::KP_Subtract:
            return VK_SUBTRACT;
        case KeyCodes::KP_Add:
            return VK_ADD;
        case KeyCodes::KP_Enter:
            return VK_RETURN;
        case KeyCodes::KP_1:
        case KeyCodes::KP_2:
        case KeyCodes::KP_3:
        case KeyCodes::KP_4:
        case KeyCodes::KP_5:
        case KeyCodes::KP_6:
        case KeyCodes::KP_7:
        case KeyCodes::KP_8:
        case KeyCodes::KP_9:
            return VK_NUMPAD1 + (key - KeyCodes::KP_1);
        case KeyCodes::KP_0:
            return VK_NUMPAD0;
        case KeyCodes::KP_Point:
            return VK_DECIMAL;
        case KeyCodes::KEY_Help:
            return VK_HELP;
        case KeyCodes::KEY_Menu:
            return VK_MENU;
        case KeyCodes::KEY_LeftControl:
            return VK_CONTROL;
        case KeyCodes::KEY_LeftShift:
            return VK_SHIFT;
        case KeyCodes::KEY_LeftAlt:
            return VK_MENU;
        case KeyCodes::KEY_LeftMeta:
            return VK_LWIN;
        case KeyCodes::KEY_RightControl:
            return VK_CONTROL;
        case KeyCodes::KEY_RightShift:
            return VK_SHIFT;
        case KeyCodes::KEY_RightAlt:
            return VK_MENU;
        case KeyCodes::KEY_RightMeta:
            return VK_RWIN;
        default:
            return 255;
        }
    }
    KeyCodes ConvertToKeyCode(DWORD key)
    {

        switch (key) {
        case '0':
            return KeyCodes::KEY_0;
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            return static_cast<KeyCodes>(KeyCodes::KEY_1 + (key - static_cast<int>('1')));
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
        case 'G':
        case 'H':
        case 'I':
        case 'J':
        case 'K':
        case 'L':
        case 'M':
        case 'N':
        case 'O':
        case 'P':
        case 'Q':
        case 'R':
        case 'S':
        case 'T':
        case 'U':
        case 'V':
        case 'W':
        case 'X':
        case 'Y':
        case 'Z':
            return static_cast<KeyCodes>(KeyCodes::KEY_A + (key - static_cast<int>('A')));
        case VK_RETURN:
            return KeyCodes::KEY_Enter;
        case VK_ESCAPE:
            return KeyCodes::KEY_Escape;
        case VK_BACK:
            return KeyCodes::KEY_Backspace;
        case VK_TAB:
            return KeyCodes::KEY_Tab;
        case VK_SPACE:
            return KeyCodes::KEY_Space;
        case VK_OEM_MINUS:
            return KeyCodes::KEY_Minus;
        case VK_OEM_PLUS:
            return KeyCodes::KEY_Equals; // this is correct and not a mistype
        case VK_OEM_4:
            return KeyCodes::KEY_LeftBracket;
        case VK_OEM_6:
            return KeyCodes::KEY_RightBracket;
        case VK_OEM_5:
            return KeyCodes::KEY_Backslash;
        case VK_OEM_1:
            return KeyCodes::KEY_Semicolon;
        case VK_OEM_7:
            return KeyCodes::KEY_Quote;
        case VK_OEM_3:
            return KeyCodes::KEY_Grave;
        case VK_OEM_COMMA:
            return KeyCodes::KEY_Comma;
        case VK_OEM_PERIOD:
            return KeyCodes::KEY_Period;
        case VK_OEM_2:
            return KeyCodes::KEY_Slash;
        case VK_CAPITAL:
            return KeyCodes::KEY_CapsLock;
        case VK_F1:
        case VK_F2:
        case VK_F3:
        case VK_F4:
        case VK_F5:
        case VK_F6:
        case VK_F7:
        case VK_F8:
        case VK_F9:
        case VK_F10:
        case VK_F11:
        case VK_F12:
            return static_cast<KeyCodes>(KeyCodes::KEY_F1 + (key - VK_F1));
        case VK_F13:
        case VK_F14:
        case VK_F15:
        case VK_F16:
        case VK_F17:
        case VK_F18:
        case VK_F19:
        case VK_F20:
        case VK_F21:
        case VK_F22:
        case VK_F23:
        case VK_F24:
            return static_cast<KeyCodes>(KeyCodes::KEY_F13 + (key - VK_F13));
        case VK_SNAPSHOT:
            return KeyCodes::KEY_PrintScreen;
        case VK_SCROLL:
            return KeyCodes::KEY_ScrollLock;
        case VK_PAUSE:
            return KeyCodes::KEY_Pause;
        case VK_INSERT:
            return KeyCodes::KEY_Insert;
        case VK_HOME:
            return KeyCodes::KEY_Home;
        case VK_PRIOR:
            return KeyCodes::KEY_PageUp;
        case VK_DELETE:
            return KeyCodes::KEY_Delete;
        case VK_END:
            return KeyCodes::KEY_End;
        case VK_NEXT:
            return KeyCodes::KEY_PageDown;
        case VK_RIGHT:
            return KeyCodes::KEY_Right;
        case VK_LEFT:
            return KeyCodes::KEY_Left;
        case VK_DOWN:
            return KeyCodes::KEY_Down;
        case VK_UP:
            return KeyCodes::KEY_Up;
        case VK_NUMLOCK:
            return KeyCodes::KP_NumLock;
        case VK_DIVIDE:
            return KeyCodes::KP_Divide;
        case VK_MULTIPLY:
            return KeyCodes::KP_Multiply;
        case VK_SUBTRACT:
            return KeyCodes::KP_Subtract;
        case VK_ADD:
            return KeyCodes::KP_Add;
        case VK_NUMPAD1:
        case VK_NUMPAD2:
        case VK_NUMPAD3:
        case VK_NUMPAD4:
        case VK_NUMPAD5:
        case VK_NUMPAD6:
        case VK_NUMPAD7:
        case VK_NUMPAD8:
        case VK_NUMPAD9:
            return static_cast<KeyCodes>(KeyCodes::KP_1 + (key - VK_NUMPAD1));
        case VK_NUMPAD0:
            return KeyCodes::KP_0;
        case VK_DECIMAL:
            return KeyCodes::KP_Point;
        case VK_HELP:
            return KeyCodes::KEY_Help;
        case VK_MENU:
        case VK_LMENU:
        case VK_RMENU:
            return KeyCodes::KEY_Menu;
        case VK_CONTROL:
        case VK_LCONTROL:
        case VK_RCONTROL:
            return KeyCodes::KEY_LeftControl;
        case VK_SHIFT:
        case VK_LSHIFT:
        case VK_RSHIFT:
            return KeyCodes::KEY_LeftShift;
        case VK_LWIN:
            return KeyCodes::KEY_LeftMeta;
        case VK_RWIN:
            return KeyCodes::KEY_RightMeta;
        default:
            return SL::Input_Lite::KeyCodes::INVALID;
        }
    }

} // namespace Input_Lite
} // namespace SL