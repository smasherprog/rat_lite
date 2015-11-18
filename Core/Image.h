#pragma once
#include "BufferManager.h"
#include <memory>
#include <vector>

namespace SL {
	namespace Remote_Access_Library {
		namespace Utilities {
			struct Image_Impl;
			class Image {

				Blk _Data;
				unsigned int _Height;
				unsigned int _Width;
				bool Compressed;
				unsigned int Size;
			public:
				//Use this to generate new images!
				static std::shared_ptr<Image> CreateImage(unsigned int h, unsigned int w);
				static std::shared_ptr<Image> CreateImage(unsigned int h, unsigned int w, char* data, size_t len, bool compressed);
				Image(Image_Impl&);
				~Image();
				//quality from 0 to 100, 0 being the WORST to 100 being the best
				void compress(int quality = 70);
				void decompress();

				//data is always rgba 32 bit stride
				char* data() const { return _Data.data; }
				size_t size() const { return Size; }
				unsigned int Height() const { return _Height; }
				unsigned int Width() const { return _Width; }
				bool compressed() const { return Compressed; }
			
			};
	
		}
	}
};
