#pragma once
#include <memory>
#include "Shapes.h"
#include <vector>

namespace SL {
	namespace Remote_Access_Library {
		namespace Utilities {
			class Image_Wrapper;
			struct Image_Impl;
			class Image {
				enum Image_Resamples {
					NEAREST_NEIGHBOR,
				};
				std::unique_ptr<char[]> _Data;
				unsigned int _Height;
				unsigned int _Width;
				unsigned int Size;
			public:
				//Use this to generate new images!
				static std::shared_ptr<Image> CreateImage(unsigned int h, unsigned int w);
				static std::shared_ptr<Image> CreateImage(unsigned int h, unsigned int w, const char* data, size_t pixel_stride);
				static std::shared_ptr<Image> Resize(const std::shared_ptr<Image>& inimg, float scale, Image_Resamples resample = Image_Resamples::NEAREST_NEIGHBOR);
				static std::shared_ptr<Image> Resize(const std::shared_ptr<Image>& inimg, int height, int width, Image_Resamples resample = Image_Resamples::NEAREST_NEIGHBOR);
				static std::vector<Rect> GetDifs(const Image& oldimg, const Image& newimg);
		
				static void Copy(Image& src, Rect src_rect, Image & dst, Rect dst_rect);

				Image(Image_Impl&);
				~Image();

				//pixel stride

				static unsigned int DefaultStride() { return 4; }
				unsigned int Stride() const { return DefaultStride(); }
				char* data() const { return _Data.get(); }
				//size of the image in bytes width* height * stride
				size_t size() const { return Size; }
				unsigned int Height() const { return _Height; }
				unsigned int Width() const { return _Width; }

			};
		}
	}
}
