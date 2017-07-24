#include "Input_Lite.h"
#include <string>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef DELETE 

namespace SL {
    namespace Input_Lite {
        void SendKey_Impl(char key, DWORD press) {
            if (key >= ' ' && key <= '~') {
                auto vkkey = VkKeyScan(key);
                auto mappedKey = LOBYTE(vkkey);
                auto highkey = HIBYTE(vkkey);
                if (highkey == 1) {
                    INPUT inp[3] = { 0 };
                    inp[0].type = INPUT_KEYBOARD;
                    inp[0].ki.wVk = VK_LSHIFT;
                    inp[1].type = INPUT_KEYBOARD;
                    inp[1].ki.wVk = mappedKey;
                    inp[1].ki.dwFlags = press;
                    inp[2].type = INPUT_KEYBOARD;
                    inp[2].ki.wVk = VK_LSHIFT;
                    inp[2].ki.dwFlags = KEYEVENTF_KEYUP;
                    SendInput(sizeof(inp) / sizeof(INPUT), inp, sizeof(INPUT));
                }
                else {
                    INPUT inp = { 0 };
                    inp.type = INPUT_KEYBOARD;
                    inp.ki.wVk = mappedKey;
                    inp.ki.dwFlags = press;
                    SendInput(1, &inp, sizeof(INPUT));
                }
            }
            else {

            }
        }
        void SendKey_Impl(wchar_t key, DWORD press) {
            if (key < 128) {
                SendKey_Impl(static_cast<char>(key), press);
            }
            else {
                INPUT inp = { 0 };
                inp.type = INPUT_KEYBOARD;
                inp.ki.wScan = key;
                inp.ki.dwFlags = KEYEVENTF_UNICODE | press;
                SendInput(1, &inp, sizeof(INPUT));
            }
        }
        void SendKeyUp(char key) {
            SendKey_Impl(key, KEYEVENTF_KEYUP);
        }
        void SendKeyUp(wchar_t key) {
            SendKey_Impl(key, KEYEVENTF_KEYUP);
        }
        void SendKeyDown(wchar_t key) {
            SendKey_Impl(key, 0);
        }
        void SendKeyDown(char key) {
            SendKey_Impl(key, 0);
        }
        void SendKey_Impl(SpecialKeyCodes key, DWORD press) {

            INPUT k = { 0 };
            k.type = INPUT_KEYBOARD;
            k.ki.dwFlags = press;
            switch (key)
            {
            case ALTLEFT:
                //case ALTRIGHT:
                k.ki.wVk = VK_LMENU;
                break;
            case CAPSLOCK:
                k.ki.wVk = VK_CAPITAL;
                break;
            case  CONTROLLEFT:
                // case  CONTROLRIGHT:
                k.ki.wVk = VK_LCONTROL;
                break;
            case  OSLEFT:
                k.ki.wVk = VK_LWIN;
                break;
            case  OSRIGHT:
                k.ki.wVk = VK_RWIN;
                break;
            case  SHIFTLEFT:
                //case  SHIFTRIGHT:
                k.ki.wVk = VK_LSHIFT;
                break;
            case  ENTER:
                //  case NUMPADENTER:
                k.ki.wVk = VK_RETURN;
                break;
            case   SPACE:
                k.ki.wVk = VK_SPACE;
                break;
            case TAB:
                k.ki.wVk = VK_TAB;
                break;
            case DELETE:
                k.ki.wVk = VK_DELETE;
                break;
            case END:
                k.ki.wVk = VK_END;
                break;
            case  HOME:
                k.ki.wVk = VK_HOME;
                break;
            case INSERT:
                k.ki.wVk = VK_INSERT;
                break;
            case PAGEDOWN:
                k.ki.wVk = VK_NEXT;
                break;
            case PAGEUP:
                k.ki.wVk = VK_PRIOR;
                break;
            case  ARROWDOWN:
                k.ki.wVk = VK_DOWN;
                break;
            case  ARROWLEFT:
                k.ki.wVk = VK_LEFT;
                break;
            case  ARROWRIGHT:
                k.ki.wVk = VK_RIGHT;
                break;
            case  ARROWUP:
                k.ki.wVk = VK_UP;
                break;
            case  ESCAPE:
                k.ki.wVk = VK_ESCAPE;
                break;
            case  PRINTSCREEN:
                k.ki.wVk = VK_SNAPSHOT;
                break;
            case  SCROLLLOCK:
                k.ki.wVk = VK_SCROLL;
                break;
            case  PAUSE:
                k.ki.wVk = VK_PAUSE;
                break;
            case F1:
            case F2:
            case F3:
            case F4:
            case F5:
            case F6:
            case F7:
            case F8:
            case F9:
            case F10:
            case F11:
            case F12:
            case F13:
            case F14:
            case F15:
            case F16:
            case F17:
            case F18:
            case F19:
            case F20:
            case F21:
            case F22:
            case F23:
            case F24:
                k.ki.wVk = static_cast<WORD>(VK_F1 + (key - F1));
                break;
            case NUMLOCK:
                k.ki.wVk = VK_NUMLOCK;
                break;
            case NUMPAD0:
            case NUMPAD1:
            case NUMPAD2:
            case NUMPAD3:
            case NUMPAD4:
            case NUMPAD5:
            case NUMPAD6:
            case NUMPAD7:
            case NUMPAD8:
            case NUMPAD9:
                k.ki.wVk = static_cast<WORD>(VK_NUMPAD0 + (key - NUMPAD0));
                break;
            case NUMPADADD:
                k.ki.wVk = VK_ADD;
                break;
            case NUMPADDECIMAL:
                k.ki.wVk = VK_DECIMAL;
                break;
            case NUMPADDIVIDE:
                k.ki.wVk = VK_DIVIDE;
                break;
            case NUMPADMULTIPLY:
                k.ki.wVk = VK_MULTIPLY;
                break;
            case NUMPADSUBTRACT:
                k.ki.wVk = VK_SUBTRACT;
                break;
            default:
                return;
            }

            SendInput(1, &k, sizeof(INPUT));

        }

        void SendKeyUp(SpecialKeyCodes key) {
            SendKey_Impl(key, KEYEVENTF_KEYUP);
        }
        void SendKeyDown(SpecialKeyCodes key) {
            SendKey_Impl(key, 0);
        }

        void SendMouseUp(const MouseButtons& button) {

            INPUT inp = { 0 };
            inp.type = INPUT_MOUSE;
            switch (button) {
            case (MouseButtons::LEFT):
                inp.mi.dwFlags = MOUSEEVENTF_LEFTUP;
                break;
            case (MouseButtons::RIGHT):
                inp.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
                break;
            case (MouseButtons::MIDDLE):
                inp.mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
                break;
            default:
                return;
            }
            SendInput(1, &inp, sizeof(INPUT));
        }

        void SendMouseDown(const MouseButtons& button) {

            INPUT inp = { 0 };
            inp.type = INPUT_MOUSE;
            switch (button) {
            case (MouseButtons::LEFT):
                inp.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
                break;
            case (MouseButtons::RIGHT):
                inp.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
                break;
            case (MouseButtons::MIDDLE):
                inp.mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
                break;
            default:
                return;
            }
            SendInput(1, &inp, sizeof(INPUT));
        }

        void SendMousePosition_Impl(int x, int y, int modifier) {
            INPUT inp = { 0 };
            inp.type = INPUT_MOUSE;
            inp.mi.dwFlags = MOUSEEVENTF_MOVE | modifier;
            inp.mi.dx = x;
            inp.mi.dy = y;
            SendInput(1, &inp, sizeof(INPUT));
        }
        void SendMousePosition(const Offset& offset) {
            SendMousePosition_Impl(offset.X, offset.Y, 0);
        }
        void SendMousePosition(const AbsolutePos& absolute) {
            SendMousePosition_Impl(
                (absolute.X * 65536) / GetSystemMetrics(SM_CXSCREEN),
                (absolute.Y * 65536) / GetSystemMetrics(SM_CYSCREEN),
                MOUSEEVENTF_ABSOLUTE);
        }
        void SendMouseScroll(int offset) {
            INPUT inp = { 0 };
            inp.type = INPUT_MOUSE;
            inp.mi.dwFlags = MOUSEEVENTF_WHEEL;
            inp.mi.mouseData = offset * 120;
            SendInput(1, &inp, sizeof(INPUT));
        }
    };
}