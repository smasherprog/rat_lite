#pragma once
#include <memory>
#include <vector>
#include <algorithm>
#include "BufferManager.h"



namespace SL {
	namespace Remote_Access_Library {
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
		
			struct Image_Impl;
			class Image {
			
				Utilities::Blk _Data;
				unsigned int _Height;
				unsigned int _Width;

			public:
				//Use this to generate new images!
				static std::shared_ptr<Image> CreateImage(unsigned int h, unsigned int w);
				Image(Image_Impl&);
				~Image();

				//data is always rgba 32 bit stride
				char* data() const;
				size_t size() const;
				unsigned int Height() const { return _Height; }
				unsigned int Width() const { return _Width; }

			};
			inline void Reorder(std::vector<Screen_Info>& screens) {
				//organize the monitors so that the ordering is left to right for displaying purposes
				std::sort(begin(screens), end(screens), [](const Screen_Info& i, const Screen_Info& j) { return i.Offsetx < j.Offsetx; });
				auto index = 0;
				for (auto& x : screens) x.Index = index++;
			}
			//getmonitors will give you information about the attached monitors, from left to right
			std::vector<Screen_Info> GetMoitors();
			//if capturemouse  == true, the mouse will be included in the output image.
			std::shared_ptr<Image> CaptureDesktop(bool capturemouse);
		}
	}
};
