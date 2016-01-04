#pragma once
#include "BufferManager.h"
#include <memory>
#include "Shapes.h"

namespace SL {
	namespace Remote_Access_Library {
		namespace INTERNAL {
			extern Utilities::BufferManager _ImageBuffer;
		}
		namespace Utilities {
			struct Image_Impl;
			class Image {

				Blk _Data;
				unsigned int _Height;
				unsigned int _Width;
				unsigned int Size;
			public:
				//Use this to generate new images!
				static std::shared_ptr<Image> CreateImage(unsigned int h, unsigned int w);
				static std::shared_ptr<Image> CreateImage(unsigned int h, unsigned int w, const char* data, size_t len);
				static std::vector<Rect> GetDifs(const Image& oldimg, const Image& newimg, const unsigned int maxdist = 16);

				Image(Image_Impl&);
				~Image();
			

				//data is always rgba 32 bit stride
				char* data() const { return _Data.data; }
				size_t size() const { return Size; }
				unsigned int Height() const { return _Height; }
				unsigned int Width() const { return _Width; }

			
			};
	
		}
	}
};
