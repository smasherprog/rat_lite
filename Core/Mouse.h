#pragma once
#include "stdafx.h"
#include <vector>

namespace SL {
	namespace Mouse_Capture {
		struct Mouse_Info {
			int left = 0;//left coordinate of the mouse
			int top = 0;//top coordinate of the mouse
			int height = 0;//height of the mouse icon
			int width = 0;//width of the mouse icon
			bool Hidden = true;
		};
		class Mouse {
			Mouse_Info MouseInfo;
			std::vector<char> Data;
		public:
			Mouse() {}
			Mouse(Mouse_Info& m, std::vector<char>& d) : MouseInfo(std::move(m)), Data(std::move(d)) {}
			//data is always 32 bit RGBA formated
			const char* get_data() const { return Data.data(); }
			const Mouse_Info get_MouseInfo() const { return MouseInfo; }
		};

		Mouse GetMouse();

	}
}