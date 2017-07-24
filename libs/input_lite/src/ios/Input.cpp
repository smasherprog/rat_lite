#include "Input_Lite.h"

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
		
        void SendMouseUp(const MouseButtons& button){
		CGPoint new_pos;
			new_pos.x = m.Pos.X;
			new_pos.y = m.Pos.Y;

			std::vector<CGEventRef> evnts;
			switch (m.EventData) {
			case Events::LEFT:
				if (m.PressData == Press::UP) evnts.push_back(CGEventCreateMouseEvent(NULL, kCGEventLeftMouseUp, new_pos, kCGMouseButtonLeft));
				else if (m.PressData == Press::DOWN) evnts.push_back(CGEventCreateMouseEvent(NULL, kCGEventLeftMouseDown, new_pos, kCGMouseButtonLeft));
				else {//double click
					evnts.push_back(CGEventCreateMouseEvent(NULL, kCGEventLeftMouseDown, new_pos, kCGMouseButtonLeft));
					evnts.push_back(CGEventCreateMouseEvent(NULL, kCGEventLeftMouseUp, new_pos, kCGMouseButtonLeft));
					evnts.push_back(CGEventCreateMouseEvent(NULL, kCGEventLeftMouseDown, new_pos, kCGMouseButtonLeft));
					evnts.push_back(CGEventCreateMouseEvent(NULL, kCGEventLeftMouseUp, new_pos, kCGMouseButtonLeft));
				}
				break;
			case Events::MIDDLE:
				if (m.PressData == Press::UP) evnts.push_back(CGEventCreateMouseEvent(NULL, kCGEventOtherMouseUp, new_pos, kCGMouseButtonCenter));
				else if (m.PressData == Press::DOWN) evnts.push_back(CGEventCreateMouseEvent(NULL, kCGEventOtherMouseDown, new_pos, kCGMouseButtonCenter));
				else {//double click
					evnts.push_back(CGEventCreateMouseEvent(NULL, kCGEventOtherMouseDown, new_pos, kCGMouseButtonCenter));
					evnts.push_back(CGEventCreateMouseEvent(NULL, kCGEventOtherMouseUp, new_pos, kCGMouseButtonCenter));
					evnts.push_back(CGEventCreateMouseEvent(NULL, kCGEventOtherMouseDown, new_pos, kCGMouseButtonCenter));
					evnts.push_back(CGEventCreateMouseEvent(NULL, kCGEventOtherMouseUp, new_pos, kCGMouseButtonCenter));
				}
				break;
			case Events::RIGHT:
				if (m.PressData == Press::UP) evnts.push_back(CGEventCreateMouseEvent(NULL, kCGEventRightMouseUp, new_pos, kCGMouseButtonRight));
				else if (m.PressData == Press::DOWN) evnts.push_back(CGEventCreateMouseEvent(NULL, kCGEventRightMouseDown, new_pos, kCGMouseButtonRight));
				else {//double click
					evnts.push_back(CGEventCreateMouseEvent(NULL, kCGEventRightMouseDown, new_pos, kCGMouseButtonRight));
					evnts.push_back(CGEventCreateMouseEvent(NULL, kCGEventRightMouseUp, new_pos, kCGMouseButtonRight));
					evnts.push_back(CGEventCreateMouseEvent(NULL, kCGEventRightMouseDown, new_pos, kCGMouseButtonRight));
					evnts.push_back(CGEventCreateMouseEvent(NULL, kCGEventRightMouseUp, new_pos, kCGMouseButtonRight));
				}
				break;
			case Events::SCROLL:
				evnts.push_back(CGEventCreateScrollWheelEvent(NULL, kCGScrollEventUnitLine, 1, -m.ScrollDelta));
				break;
			default:
				CGWarpMouseCursorPosition(new_pos);
			}
			for (auto& a : evnts) {
				CGEventPost(kCGHIDEventTap, a);
				CFRelease(a);
			}

		}
			
		
        void SendMouseDown(const MouseButtons& button){

		}
		void SendMouseScroll(int offset){
			
		}
        void SendMousePosition(const Offset& offset){
			
		}
        void SendMousePosition(const AbsolutePos& absolute){
			
		}

   
    }
}