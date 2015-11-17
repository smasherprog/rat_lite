#pragma once
#include <memory>
#include <vector>
#include <algorithm>
#include "BufferManager.h"

namespace SL {
	namespace Remote_Access_Library {
		namespace Utilities {
			class Image;
		}
		namespace Screen_Capture {
			

			struct Screen_Info {
				int Width = 0;//width in pixels of the screen
				int Height = 0;//Height in pixels of the screen
				int Depth = 0;//Depth in pixels of the screen, i.e. 32 bit
				char Device[32];//name of the screen
				int Offsetx = 0;//distance in pixels from the MOST left screen. This can be negative because the primary monitor starts at 0, but this screen could be layed out to the left of the primary, in which case the offset is negative
				int Offsety = 0;//distance in pixels from the MOST bottom of the screen
				int Index = 0;//Index of the screen from LEFT to right of the physical monitors
			};

			void Reorder(std::vector<Screen_Info>& screens);

			//getmonitors will give you information about the attached monitors, from left to right
			std::vector<Screen_Info> GetMoitors();
			//if capturemouse  == true, the mouse will be included in the output image.
			std::shared_ptr<Utilities::Image> CaptureDesktop(bool capturemouse);
		}
	}
};
