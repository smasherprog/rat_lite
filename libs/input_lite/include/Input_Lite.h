#pragma once
#include <string>
#undef DELETE

namespace SL {
    namespace Input_Lite {

        //use already existing codes for special keys
        //https://developer.mozilla.org/en-US/docs/Web/API/KeyboardEvent/keyCode#Constants_for_keyCode_value

        enum SpecialKeyCodes : long long {
            TAB = 9,
            BACKSPACE = 8,
            ENTER = 13,
    
            SHIFTLEFT = 16,
            SHIFTRIGHT = 16,
            CONTROLLEFT = 17,
            CONTROLRIGHT = 17,        

            ALTLEFT = 18,    
            ALTRIGHT = 18,   
            CAPSLOCK = 20,
  
            ESCAPE = 27,
            SPACE = 32,
            PAGEUP = 33,
            PAGEDOWN = 34,
            END = 35,
            HOME = 36,
        
            ARROWLEFT = 37,
            ARROWUP = 38,
            ARROWRIGHT = 39,  
            ARROWDOWN = 40,
            INSERT = 45,
            DELETE = 46,
#ifdef WIN32
            PRINTSCREEN = 44,
#elif __APPLE__
            PRINTSCREEN = 124,
#elif __linux__
            PRINTSCREEN = 42,
#endif 
#if  __APPLE__
            SCROLLLOCK = 125,
#else 
            SCROLLLOCK = 145,
#endif 
#if  !__APPLE__
            PAUSE = 19,
#else 
            PAUSE = 126,
#endif 

            OSLEFT = 91,
            OSRIGHT = 92,
            F1 = 112,
            F2 = 113,
            F3 = 114,
            F4 = 115,
            F5 = 116,
            F6 = 117,
            F7 = 118,
            F8 = 119,
            F9 = 120,
            F10 = 121,
            F11 = 122,
            F12 = 123,
            F13 = 124,
            F14 = 125,
            F15 = 126,
            F16 = 127,
            F17 = 128,
            F18 = 129,

#if  !__linux__
            F19 = 130,
            F20 = 131,
#endif 
#if  WIN32
            F21 = 132,
            F22 = 133,
            F23 = 134,
            F24 = 135,
#endif 
#if     __APPLE__
            NUMLOCK = 12,
#else 
            NUMLOCK = 144,
#endif 
            NUMPAD0 = 96,
            NUMPAD1 = 97,
            NUMPAD2 = 98,
            NUMPAD3 = 99,
            NUMPAD4 = 100,
            NUMPAD5 = 101,
            NUMPAD6 = 102,
            NUMPAD7 = 103,
            NUMPAD8 = 104,
            NUMPAD9 = 105,

            NUMPADADD = 107,
            NUMPADDECIMAL = 110,
            NUMPADDIVIDE = 111,
            NUMPADENTER = 13,

            NUMPADMULTIPLY = 106,
            NUMPADSUBTRACT = 109


        };
        void SendKeyUp(char key);
        void SendKeyUp(wchar_t key);
        void SendKeyUp(SpecialKeyCodes key);
        template <class T> void SendKeyUp(T) = delete;

        void SendKeyDown(char key);
        void SendKeyDown(wchar_t key);
        void SendKeyDown(SpecialKeyCodes key);
        template <class T> void SendKeyDown(T) = delete;

        inline void SendKey(char key) {
            SendKeyDown(key);
            SendKeyUp(key);
        }
        inline void SendKey(wchar_t key) {
            SendKeyDown(key);
            SendKeyUp(key);
        }
        inline void SendKey(SpecialKeyCodes key) {
            SendKeyDown(key);
            SendKeyUp(key);
        }
        template <class T> void SendKey(T) = delete;

        inline void SendKeys(std::string keys) {
            for (auto k : keys) {
                SendKey(k);
            }
        }
        inline void SendKeys(std::wstring keys) {
            for (auto k : keys) {
                SendKey(k);
            }
        }
        template<typename T, size_t ELEMENTSIZE>inline void SendKeys(T(&keys)[ELEMENTSIZE]) {
            for (auto k : keys) {
                SendKey(k);
            }
        }
        template <class T> void SendKeys(T) = delete;


        enum MouseButtons :int {
            LEFT,
            MIDDLE,
            RIGHT
        };
        void SendMouseUp(const MouseButtons button);
        void SendMouseDown(const MouseButtons button);
        inline void SendMouseClick(const MouseButtons button) {
            SendMouseDown(button);
            SendMouseUp(button);
        }
        inline void SendMouseDoubleClick(const MouseButtons button) {
            SendMouseClick(button);
            SendMouseClick(button);
        }
        void SendMouseScroll(int offset);
        struct Offset { int X = 0; int Y = 0; };
        void SendMousePosition(const Offset& offset);
        struct AbsolutePos { int X = 0; int Y = 0; };
        void SendMousePosition(const AbsolutePos& absolute);



    }
}
