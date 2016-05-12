#include "stdafx.h"
#include "Keyboard.h"
#include <string>
#include "Logging.h"

void SL::Remote_Access_Library::Input::SimulateKeyboardEvent(KeyEvent ev)
{
	SL_RAT_LOG(std::string("SetKeyEvent Key:") +std::to_string( ev.Key) + std::string(" SpecialKey: ") + std::to_string(ev.SpecialKey) + std::string(" PressData: ") + std::to_string(ev.PressData), Utilities::Logging_Levels::INFO_log_level);


#if defined _WIN32

	INPUT input;
	memset(&input, 0, sizeof(input));
	input.type = INPUT_KEYBOARD;
	input.ki.dwFlags = ev.PressData == Keyboard::Press::UP ? KEYEVENTF_KEYUP : 0;
	input.ki.wVk = MapVirtualKey(ev.Key, MAPVK_VSC_TO_VK);
	//SendInput(1, &input, sizeof(input));

#elif defined __APPLE__
	
#elif __linux__
	auto display = XOpenDisplay(NULL);
	auto root = DefaultRootWindow(display);


	XCloseDisplay(display);
	
#endif




}
