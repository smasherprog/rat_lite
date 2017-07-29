#include "Input_Lite.h"
#include <ApplicationServices/ApplicationServices.h>

namespace SL
{
    namespace Input_Lite
    {

        void SendKeyUp(char key) {
          
        }
        void SendKeyUp(wchar_t key) {
          
        }
        void SendKeyDown(wchar_t key) {
          
        }
        void SendKeyDown(char key) {
         
        }
        void SendKeyUp(SpecialKeyCodes key) {
            
        }
        void SendKeyDown(SpecialKeyCodes key) {
            
        }
        
        void SendMouseScroll(int offset)
        {
            auto ev = CGEventCreateScrollWheelEvent(NULL, kCGScrollEventUnitLine, 1, -offset);
            if(ev){
                CGEventPost(kCGHIDEventTap, ev);
                CFRelease(ev);
            }
        }
        void SendMouse_Impl(const MouseButtons button, bool pressed)
        {
            auto msevent = CGEventCreate(NULL);
            auto loc = CGEventGetLocation(msevent);
            CFRelease(msevent);
            
            CGEventRef ev = nullptr;
            switch(button) {
                case MouseButtons::LEFT:
                    ev = CGEventCreateMouseEvent(NULL, pressed ? kCGEventLeftMouseDown: kCGEventLeftMouseUp, loc, kCGMouseButtonLeft);
                case MouseButtons::MIDDLE:
                    ev = CGEventCreateMouseEvent(NULL, pressed ? kCGEventOtherMouseDown: kCGEventOtherMouseUp, loc, kCGMouseButtonCenter);
                case MouseButtons::RIGHT:
                    ev = CGEventCreateMouseEvent(NULL, pressed ? kCGEventRightMouseDown: kCGEventRightMouseUp, loc, kCGMouseButtonRight);
                default:
                    break;
            }
            if(ev){
                CGEventPost(kCGHIDEventTap, ev);
                CFRelease(ev);
            }
        }
        void SendMouseUp(const MouseButtons button){
            SendMouse_Impl(button, false);
		}
        void SendMouseDown(const MouseButtons button){
            SendMouse_Impl(button, true);
		}
        
        void SendMousePosition(const Offset& offset){
            auto msevent = CGEventCreate(NULL);
            auto loc = CGEventGetLocation(msevent);
            CFRelease(msevent);
            loc.x+=offset.X;
            loc.y += offset.Y;
			CGWarpMouseCursorPosition(loc);
		}
        void SendMousePosition(const AbsolutePos& absolute){
            CGPoint p;
            p.x = absolute.X;
            p.y = absolute.Y;
			CGWarpMouseCursorPosition(p);
        
		}
    }
}
