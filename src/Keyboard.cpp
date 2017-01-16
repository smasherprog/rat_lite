#include "stdafx.h"
#include "Keyboard.h"
#include <string>
#include "Logging.h"
#include <FL/Enumerations.H>

#define SL_MULTIPLY       0xf06A
#define SL_ADD            0xf06B
#define SL_SEPARATOR      0xf06C
#define SL_SUBTRACT       0xf06D
#define SL_DECIMAL        0xf06E
#define SL_DIVIDE         0xf06F

#if  _WIN32

unsigned int Map_ToPlatformKey(unsigned int key) {
	if (key >= 0x0030 && key <= 0x0039) return key;//VK_0 - VK_9 are the same as ASCII '0' - '9' (0x30 - 0x39)
	else if (key >= 0x0041 && key <= 0x005A) {//VK_A - VK_Z are the same as ASCII 'A' - 'Z' (0x41 - 0x5A)
		return key;// regular ascii alphabet characters
	}
	else if (key >= 0x0061 && key <= 0x007a) {
		return key - static_cast<unsigned int>('a' - 'A');//on windows, the keys are always in CAPS...
	}

	switch (key) {

	case (FL_BackSpace):
		return  VK_BACK;
	case (FL_Tab):
		return VK_TAB;
	case (FL_Enter):
		return VK_RETURN;
	case(' '):
		return VK_SPACE;

	case('='):
		return VK_OEM_PLUS;
	case(','):
		return VK_OEM_COMMA;
	case('-'):
		return VK_OEM_MINUS;
	case('.'):
		return VK_OEM_PERIOD;
	case (';'):
		return VK_OEM_1;
	case('/'):
		return VK_OEM_2;
	case('`'):
		return VK_OEM_3;
	case('['):
		return VK_OEM_4;
	case('\\'):
		return VK_OEM_5;
	case(']'):
		return VK_OEM_6;
	case('\''):
		return VK_OEM_7;

	case (FL_Help):
		return VK_HELP;
	case (FL_Shift_L):
		return VK_LSHIFT;
	case (FL_Shift_R):
		return VK_RSHIFT;
	case (FL_Control_L):
		return VK_LCONTROL;
	case (FL_Control_R):
		return VK_RCONTROL;
	case (FL_Alt_L):
		return VK_LMENU;
	case (FL_Alt_R):
		return VK_RMENU;
	case (FL_Pause):
		return VK_PAUSE;
	case (FL_Caps_Lock):
		return VK_CAPITAL;
	case (FL_Kana):
		return VK_KANA;
	case (FL_Escape):
		return VK_ESCAPE;

	case (FL_Home):
		return VK_HOME;
	case (FL_Left):
		return VK_LEFT;
	case (FL_Up):
		return VK_UP;
	case (FL_Right):
		return VK_RIGHT;
	case (FL_Down):
		return VK_DOWN;
	case (FL_Page_Up):
		return VK_PRIOR;
	case (FL_Page_Down):
		return VK_NEXT;
	case (FL_End):
		return VK_END;
	case (FL_Home_Page):
		return VK_HOME;

	case (FL_Print):
		return VK_PRINT;
	case (FL_Insert):
		return VK_INSERT;
	case (FL_Delete):
		return VK_DELETE;

	case (FL_Meta_L):
		return VK_LWIN;
	case (FL_Meta_R):
		return VK_LWIN;

	case (FL_Menu):
		return VK_LMENU;


	case (SL_MULTIPLY):
		return SL_MULTIPLY;

	case (SL_ADD):
		return VK_ADD;
	case (SL_SEPARATOR):
		return VK_SEPARATOR;
	case (SL_SUBTRACT):
		return VK_SUBTRACT;
	case (SL_DECIMAL):
		return VK_DECIMAL;
	case (SL_DIVIDE):
		return VK_DIVIDE;

	case (FL_F + 1):
		return VK_F1;
	case (FL_F + 2):
		return VK_F2;
	case (FL_F + 3):
		return VK_F3;
	case (FL_F + 4):
		return VK_F4;
	case (FL_F + 5):
		return VK_F5;
	case (FL_F + 6):
		return VK_F6;
	case (FL_F + 7):
		return VK_F7;
	case (FL_F + 8):
		return VK_F8;
	case (FL_F + 9):
		return VK_F9;
	case (FL_F + 10):
		return VK_F10;
	case (FL_F + 11):
		return VK_F11;
	case (FL_F + 12):
		return VK_F12;
	case (FL_F + 13):
		return VK_F13;
	case (FL_F + 14):
		return VK_F14;
	case (FL_F + 15):
		return VK_F15;
	case (FL_F + 16):
		return VK_F16;
	case (FL_F + 17):
		return VK_F17;
	case (FL_F + 18):
		return VK_F18;
	case (FL_F + 19):
		return VK_F19;
	case (FL_F + 20):
		return VK_F20;
	case (FL_F + 21):
		return VK_F21;
	case (FL_F + 22):
		return VK_F22;
	case (FL_F + 23):
		return VK_F23;
	case (FL_F + 24):
		return VK_F24;
	case (FL_Num_Lock):
		return VK_NUMLOCK;
	case (FL_KP +'0'):
		return VK_NUMPAD0;
	case (FL_KP + '1'):
		return VK_NUMPAD1;
	case (FL_KP + '2'):
		return VK_NUMPAD2;
	case (FL_KP + '3'):
		return VK_NUMPAD3;
	case (FL_KP + '4'):
		return VK_NUMPAD4;
	case (FL_KP + '5'):
		return VK_NUMPAD5;
	case (FL_KP + '6'):
		return VK_NUMPAD6;
	case (FL_KP + '7'):
		return VK_NUMPAD7;
	case (FL_KP + '8'):
		return VK_NUMPAD8;
	case (FL_KP + '9'):
		return VK_NUMPAD9;
	case (FL_KP + '/'):
		return VK_DIVIDE;
	case (FL_KP + '*'):
		return VK_MULTIPLY;
	case (FL_KP + '-'):
		return VK_SUBTRACT;
	case (FL_KP + '+'):
		return VK_ADD;
	case (FL_KP + '.'):
		return VK_DECIMAL;

	case (FL_KP_Enter):
		return VK_RETURN;

	case (FL_Scroll_Lock):
		return VK_SCROLL;
	case (FL_Volume_Down):
		return VK_VOLUME_DOWN;
	case (FL_Volume_Mute):
		return VK_VOLUME_MUTE;
	case (FL_Volume_Up):
		return VK_VOLUME_UP;
	default:
		return 0;
	}
}




#elif __APPLE__
unsigned int Map_ToPlatformKey(unsigned int key) {
	switch (key) {
        	default:
		return 0;
    }
}
#elif __ANDROID__


#elif __linux__
unsigned int Map_ToPlatformKey(unsigned int key) {
	switch (key) {
	case (FL_BackSpace):
		return  XK_BackSpace;
	case (FL_Tab):
		return XK_Tab;
	case (FL_Enter):
		return XK_Return;

	case (FL_Help):
		return XK_Help;
	case (FL_Shift_L):
		return XK_Shift_L;
	case (FL_Shift_R):
		return XK_Shift_R;
	case (FL_Control_L):
		return XK_Control_L;
	case (FL_Control_R):
		return XK_Control_R;
	case (FL_Alt_L):
		return XK_Alt_L;
	case (FL_Alt_R):
		return XK_Alt_R;
	case (FL_Pause):
		return XK_Pause;
	case (FL_Caps_Lock):
		return XK_Caps_Lock;

	case (FL_Escape):
		return XK_Escape;

	case (FL_Home):
		return XK_Home;
	case (FL_Left):
		return XK_Left;
	case (FL_Up):
		return XK_Up;
	case (FL_Right):
		return XK_Right;
	case (FL_Down):
		return XK_Down;
	case (FL_Page_Up):
		return XK_Page_Up;
	case (FL_Page_Down):
		return XK_Page_Down;
	case (FL_End):
		return XK_End;
	case (FL_Home_Page):
		return XK_Home;

	case (FL_Print):
		return XK_Print;
	case (FL_Insert):
		return XK_Insert;
	case (FL_Delete):
		return XK_Delete;

	case (FL_Meta_L):
		return XK_Meta_L;
	case (FL_Meta_R):
		return XK_Meta_R;

	case (FL_Menu):
		return XK_Menu;


	case (SL_MULTIPLY):
		return XK_KP_Multiply;

	case (SL_ADD):
		return XK_KP_Add;
	case (SL_SEPARATOR):
		return XK_KP_Separator;
	case (SL_SUBTRACT):
		return XK_KP_Subtract;
	case (SL_DECIMAL):
		return XK_KP_Decimal;
	case (SL_DIVIDE):
		return XK_KP_Divide;

	case (FL_F + 1):
		return XK_F1;
	case (FL_F + 2):
		return XK_F2;
	case (FL_F + 3):
		return XK_F3;
	case (FL_F + 4):
		return XK_F4;
	case (FL_F + 5):
		return XK_F5;
	case (FL_F + 6):
		return XK_F6;
	case (FL_F + 7):
		return XK_F7;
	case (FL_F + 8):
		return XK_F8;
	case (FL_F + 9):
		return XK_F9;
	case (FL_F + 10):
		return XK_F10;
	case (FL_F + 11):
		return XK_F11;
	case (FL_F + 12):
		return XK_F12;
	case (FL_F + 13):
		return XK_F13;
	case (FL_F + 14):
		return XK_F14;
	case (FL_F + 15):
		return XK_F15;
	case (FL_F + 16):
		return XK_F16;
	case (FL_F + 17):
		return XK_F17;
	case (FL_F + 18):
		return XK_F18;
	case (FL_F + 19):
		return XK_F19;
	case (FL_F + 20):
		return XK_F20;
	case (FL_F + 21):
		return XK_F21;
	case (FL_F + 22):
		return XK_F22;
	case (FL_F + 23):
		return XK_F23;
	case (FL_F + 24):
		return XK_F24;
	case (FL_Num_Lock):
		return XK_Num_Lock;
	case (FL_Scroll_Lock):
		return XK_Scroll_Lock;
	default:
		return 0;
	}
}

#endif

void SL::Remote_Access_Library::Input::SimulateKeyboardEvent(KeyEvent ev)
{
	SL_RAT_LOG(Utilities::Logging_Levels::INFO_log_level, "Received SetKeyEvent Key:" << ev.Key << " SpecialKey: " << ev.SpecialKey << " PressData: " << ev.PressData);
	if (ev.Key == 0) return;//unmapped key
#if  _WIN32
	ev.Key = Map_ToPlatformKey(ev.Key);
#else 

	if (ev.Key & 0xf000) {//special key, needs to be mapped to platform specific code
		ev.Key = Map_ToPlatformKey(ev.Key);
	}//must be ascii char, do a check to make sure it is actually an ascii char. Below are the hex values of ascii chars
	else if (ev.Key != 0x0020 && ev.Key != 0x0027 && !(ev.Key >= 0x002b && ev.Key <= 0x007f)) {
		SL_RAT_LOG(Utilities::Logging_Levels::Debug_log_level, "Recevied a key event which is outside of the asci char set");
		return;
	}
#endif

	if (ev.Key == 0) {//no mapping available
		SL_RAT_LOG(Utilities::Logging_Levels::Debug_log_level, "No Mapping Available for key");
		return;
	}

#if  _WIN32



	INPUT input;
	memset(&input, 0, sizeof(input));
	input.type = INPUT_KEYBOARD;

	input.ki.dwFlags = ev.PressData == Keyboard::Press::UP ? KEYEVENTF_KEYUP : 0;
	input.ki.wVk = ev.Key;
	SendInput(1, &input, sizeof(input));

#elif __APPLE__

#elif __ANDROID__

	
#elif __linux__


	auto display = XOpenDisplay(NULL);
	auto keycode = XKeysymToKeycode(display, ev.Key);
	SL_RAT_LOG(Utilities::Logging_Levels::Debug_log_level, "AFter XKeysymToKeycode '" << keycode << "'");
	if (keycode == 0) return;
	//XTestGrabControl(display, True);

	XTestFakeKeyEvent(display, keycode, ev.PressData == Keyboard::Press::DOWN ? True : False, CurrentTime);
	//XSync(display, True);
	//XTestGrabControl(display, False);
	XCloseDisplay(display);
	std::this_thread::sleep_for(std::chrono::milliseconds(1));

#endif

}


