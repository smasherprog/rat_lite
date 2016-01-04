#include "stdafx.h"
#include "Image.h"


SL::Remote_Access_Library::Utilities::BufferManager SL::Remote_Access_Library::INTERNAL::_ImageBuffer;

struct SL::Remote_Access_Library::Utilities::Image_Impl {
	unsigned int height;
	unsigned int width;
};

std::shared_ptr<SL::Remote_Access_Library::Utilities::Image> SL::Remote_Access_Library::Utilities::Image::CreateImage(unsigned int h, unsigned int w) {
	Image_Impl i;
	i.height = h;
	i.width = w;
	return std::make_shared<SL::Remote_Access_Library::Utilities::Image>(i);
}
std::shared_ptr<SL::Remote_Access_Library::Utilities::Image> SL::Remote_Access_Library::Utilities::Image::CreateImage(unsigned int h, unsigned int w, const char * data, size_t len)
{
	assert(h*w * 4 == len);
	auto img = CreateImage(h, w);
	memcpy(img->data(), data, len);
	img->Size = len;
	return img;
}

std::vector<SL::Remote_Access_Library::Utilities::Rect> SL::Remote_Access_Library::Utilities::Image::GetDifs(const Image & oldimg, const Image & newimg, const unsigned int maxdist)
{
	std::vector<SL::Remote_Access_Library::Utilities::Rect> rects;
	if (oldimg.Width() != newimg.Width() || oldimg.Height() != newimg.Height()) {
		rects.push_back(Rect(Point(0, 0), newimg.Height(), newimg.Width()));
		return rects;
	}

	auto oldimg_ptr = (const unsigned int*)oldimg.data();
	auto newimg_ptr = (const unsigned int*)newimg.data();
	for (auto row = 0; row < oldimg.Width(); row++) {
		for (auto col = 0; col < oldimg.Height(); col += maxdist) {

			for (auto x = 0; x < maxdist; x++) {
				auto old = oldimg_ptr[col*oldimg.Width() + row + x];
				auto ne = newimg_ptr[col*newimg.Width() + row + x];
				if (ne  != old) {
					auto found = std::find_if(rbegin(rects), rend(rects), [=](const SL::Remote_Access_Library::Utilities::Rect& r) {
						return SquaredDistance(Point(row, col), r) <= (maxdist*maxdist + 128 * 128);
					});
					if (found == rend(rects)) {//nothing found insert new rect
						rects.push_back(Rect(Point(row, col), maxdist, maxdist));
					}
					else {
						found->Expand_To_Include(Point(row, col));//expand the rect to encompas the point
					}
					break;//get out..
				}
			}
		}
	}
	for (auto& r : rects) {
		if (r.Origin.X + r.Width>newimg.Width()) {
			r.Origin.X = newimg.Width() - maxdist;
			r.Width = maxdist;
		}
		if (r.Origin.Y + r.Height > newimg.Height()) {
			r.Origin.Y = newimg.Height() - maxdist;
			r.Height = maxdist;
		}
	}
	return rects;
}

SL::Remote_Access_Library::Utilities::Image::Image(Image_Impl& impl) : _Height(impl.height), _Width(impl.width), Size(impl.height*impl.width * 4) {
	_Data = Remote_Access_Library::INTERNAL::_ImageBuffer.AquireBuffer(impl.height*impl.width * 4);//stride is always 4 bytes
}
SL::Remote_Access_Library::Utilities::Image::~Image() {

	Remote_Access_Library::INTERNAL::_ImageBuffer.ReleaseBuffer(_Data);
}
