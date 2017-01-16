#include "Mouse.h"
#include <assert.h>
#include "Logging.h"

#if __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

void SL::Remote_Access_Library::Input::SimulateMouseEvent(const Input::MouseEvent & m)
{

	SL_RAT_LOG(Utilities::Logging_Levels::INFO_log_level,"SetMouseEvent EventData:"<<m.EventData<<" ScrollDelta: "<<m.ScrollDelta<<" PressData: "<<m.PressData);
	
#if _WIN32

	INPUT input;
	memset(&input, 0, sizeof(input));
	input.type = INPUT_MOUSE;
	input.mi.mouseData = m.ScrollDelta * 120;
	input.mi.dx = static_cast<LONG>(static_cast<float>(m.Pos.X)*(65536.0f / static_cast<float>(GetSystemMetrics(SM_CXSCREEN))));//x being coord in pixels
	input.mi.dy = static_cast<LONG>(static_cast<float>(m.Pos.Y)*(65536.0f / static_cast<float>(GetSystemMetrics(SM_CYSCREEN))));//y being coord in pixels
	input.mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;

	switch (m.EventData) {
	case Input::Mouse::Events::LEFT:
		if (m.PressData == Input::Mouse::Press::UP) input.mi.dwFlags |= MOUSEEVENTF_LEFTUP;
		else if (m.PressData == Input::Mouse::Press::DOWN) input.mi.dwFlags |= MOUSEEVENTF_LEFTDOWN;
		else input.mi.dwFlags |= MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP;
		break;
	case Input::Mouse::Events::MIDDLE:
		if (m.PressData == Input::Mouse::Press::UP) input.mi.dwFlags |= MOUSEEVENTF_MIDDLEUP;
		else if (m.PressData == Input::Mouse::Press::DOWN) input.mi.dwFlags |= MOUSEEVENTF_MIDDLEDOWN;
		else input.mi.dwFlags |= MOUSEEVENTF_MIDDLEDOWN | MOUSEEVENTF_MIDDLEUP;
		break;
	case Input::Mouse::Events::RIGHT:
		if (m.PressData == Input::Mouse::Press::UP) input.mi.dwFlags |= MOUSEEVENTF_RIGHTUP;
		else if (m.PressData == Input::Mouse::Press::DOWN) input.mi.dwFlags |= MOUSEEVENTF_RIGHTDOWN;
		else input.mi.dwFlags |= MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_RIGHTUP;
		break;
	case Input::Mouse::Events::SCROLL:
		input.mi.dwFlags |= MOUSEEVENTF_WHEEL;
		break;
	default:
		break;
	}

	SendInput(1, &input, sizeof(input));
	if (m.PressData == Input::Mouse::Press::DBLCLICK) SendInput(1, &input, sizeof(input));
#elif __APPLE__

	CGPoint new_pos;
	new_pos.x = m.Pos.X;
	new_pos.y = m.Pos.Y;
	
    std::vector<CGEventRef> evnts;
    switch (m.EventData) {
	case Input::Mouse::Events::LEFT:
		if (m.PressData == Input::Mouse::Press::UP) evnts.push_back(CGEventCreateMouseEvent(NULL, kCGEventLeftMouseUp, new_pos, kCGMouseButtonLeft));
		else if (m.PressData == Input::Mouse::Press::DOWN) evnts.push_back(CGEventCreateMouseEvent(NULL, kCGEventLeftMouseDown, new_pos, kCGMouseButtonLeft));
		else {//double click
            evnts.push_back(CGEventCreateMouseEvent(NULL, kCGEventLeftMouseDown, new_pos, kCGMouseButtonLeft));
            evnts.push_back(CGEventCreateMouseEvent(NULL, kCGEventLeftMouseUp, new_pos, kCGMouseButtonLeft));
            evnts.push_back(CGEventCreateMouseEvent(NULL, kCGEventLeftMouseDown, new_pos, kCGMouseButtonLeft));
            evnts.push_back(CGEventCreateMouseEvent(NULL, kCGEventLeftMouseUp, new_pos, kCGMouseButtonLeft));
        }
		break;
	case Input::Mouse::Events::MIDDLE:
		if (m.PressData == Input::Mouse::Press::UP) evnts.push_back(CGEventCreateMouseEvent(NULL, kCGEventOtherMouseUp, new_pos, kCGMouseButtonCenter));
		else if (m.PressData == Input::Mouse::Press::DOWN) evnts.push_back(CGEventCreateMouseEvent(NULL, kCGEventOtherMouseDown, new_pos, kCGMouseButtonCenter));
		else {//double click
            evnts.push_back(CGEventCreateMouseEvent(NULL, kCGEventOtherMouseDown, new_pos, kCGMouseButtonCenter));
            evnts.push_back(CGEventCreateMouseEvent(NULL, kCGEventOtherMouseUp, new_pos, kCGMouseButtonCenter));
            evnts.push_back(CGEventCreateMouseEvent(NULL, kCGEventOtherMouseDown, new_pos, kCGMouseButtonCenter));
            evnts.push_back(CGEventCreateMouseEvent(NULL, kCGEventOtherMouseUp, new_pos, kCGMouseButtonCenter));
        }
		break;
	case Input::Mouse::Events::RIGHT:
		if (m.PressData == Input::Mouse::Press::UP) evnts.push_back(CGEventCreateMouseEvent(NULL, kCGEventRightMouseUp, new_pos, kCGMouseButtonRight));
		else if (m.PressData == Input::Mouse::Press::DOWN) evnts.push_back(CGEventCreateMouseEvent(NULL, kCGEventRightMouseDown, new_pos, kCGMouseButtonRight));
		else {//double click
            evnts.push_back(CGEventCreateMouseEvent(NULL, kCGEventRightMouseDown, new_pos, kCGMouseButtonRight));
            evnts.push_back(CGEventCreateMouseEvent(NULL, kCGEventRightMouseUp, new_pos, kCGMouseButtonRight));
            evnts.push_back(CGEventCreateMouseEvent(NULL, kCGEventRightMouseDown, new_pos, kCGMouseButtonRight));
            evnts.push_back(CGEventCreateMouseEvent(NULL, kCGEventRightMouseUp, new_pos, kCGMouseButtonRight));
        }
		break;
	case Input::Mouse::Events::SCROLL:
        evnts.push_back(CGEventCreateScrollWheelEvent(NULL, kCGScrollEventUnitLine, 1, -m.ScrollDelta));
		break;
	default:
		CGWarpMouseCursorPosition(new_pos);
	}
    for(auto& a: evnts){
        CGEventPost(kCGHIDEventTap, a);
        CFRelease(a);
    }
    
    
    
#elif __ANDROID__

#elif __linux__
    
	auto display = XOpenDisplay(NULL);
    XTestFakeMotionEvent(display, 0, m.Pos.X, m.Pos.Y,0);
    
	
    
    switch (m.EventData) {
	case Input::Mouse::Events::LEFT:
		if (m.PressData == Input::Mouse::Press::UP) XTestFakeButtonEvent(display, Button1,False, CurrentTime);
		else if (m.PressData == Input::Mouse::Press::DOWN) XTestFakeButtonEvent(display, Button1,True,CurrentTime);
		else {//double click
            XTestFakeButtonEvent(display, Button1,True, CurrentTime);
            XTestFakeButtonEvent(display, Button1,False, CurrentTime);
            XTestFakeButtonEvent(display, Button1,True, CurrentTime );
            XTestFakeButtonEvent(display, Button1,False, CurrentTime );
        }
		break;
	case Input::Mouse::Events::MIDDLE:
		if (m.PressData == Input::Mouse::Press::UP) XTestFakeButtonEvent(display, Button2,False, CurrentTime);
		else if (m.PressData == Input::Mouse::Press::DOWN) XTestFakeButtonEvent(display, Button2,True, CurrentTime);
		else {//double click
            XTestFakeButtonEvent(display, Button2,True, CurrentTime);
            XTestFakeButtonEvent(display, Button2,False, CurrentTime);
            XTestFakeButtonEvent(display, Button2,True, CurrentTime);
            XTestFakeButtonEvent(display, Button2,False, CurrentTime);
        }
		break;
	case Input::Mouse::Events::RIGHT:
		if (m.PressData == Input::Mouse::Press::UP) XTestFakeButtonEvent(display, Button3,False, CurrentTime );
		else if (m.PressData == Input::Mouse::Press::DOWN) XTestFakeButtonEvent(display, Button3,True, CurrentTime);
		else {//double click
            XTestFakeButtonEvent(display, Button3,True, CurrentTime);
            XTestFakeButtonEvent(display, Button3,False, CurrentTime);
            XTestFakeButtonEvent(display, Button3,True,CurrentTime );
            XTestFakeButtonEvent(display, Button3,False, CurrentTime);
        }
		break;
	case Input::Mouse::Events::SCROLL:
        if(m.ScrollDelta <0){
            for(auto i=0;i< abs(m.ScrollDelta) && i< 5; i++){///cap at 5
                XTestFakeButtonEvent(display, Button4,True, CurrentTime );  
                XTestFakeButtonEvent(display, Button4,False, CurrentTime); 
            }   
        } else if(m.ScrollDelta>0) {
             for(auto i=0;i< m.ScrollDelta && i< 5; i++){///cap at 5
                XTestFakeButtonEvent(display, Button5,True, CurrentTime );  
                XTestFakeButtonEvent(display, Button5,False, CurrentTime); 
            }     
        }
        
		break;
	default:
		break;
	}
    
	XCloseDisplay(display);

#endif



}

