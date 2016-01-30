#pragma once
#include <memory>
#include "Shapes.h"

namespace SL {
	namespace Remote_Access_Library {
		namespace Utilities {
			class Image_Wrapper;
			struct Image_Impl;
			class Image {

				std::unique_ptr<char[]> _Data;
				unsigned int _Height;
				unsigned int _Width;
				unsigned int Size;
			public:
				//Use this to generate new images!
				static std::shared_ptr<Image> CreateImage(unsigned int h, unsigned int w);
				static std::shared_ptr<Image_Wrapper> CreateWrappedImage(unsigned int h, unsigned int w);
				static std::shared_ptr<Image> CreateImage(unsigned int h, unsigned int w, const char* data, size_t len);
				static std::shared_ptr<Image_Wrapper> CreateWrappedImage(unsigned int h, unsigned int w, const char* data, size_t len);
				static std::vector<Rect> GetDifs(const Image& oldimg, const Image& newimg);
				static void Copy(Image& src, Rect src_rect, Image & dst, Rect dst_rect);

				Image(Image_Impl&);
				~Image();
			
				//pixel stride
				unsigned int Stride() const { return 4; }
				char* data() const { return _Data.get(); }
				//size of the image in bytes width* height * stride
				size_t size() const { return Size; }
				unsigned int Height() const { return _Height; }
				unsigned int Width() const { return _Width; }
	
			};
			//this object is used for shared_ptr alias constuctor
			class Image_Wrapper {
			public:
				Image_Wrapper(Image_Impl&);
				Image WrappedImage;
			};
		}
	}
};
