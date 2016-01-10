#include "stdafx.h"
#include "Image.h"

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
std::shared_ptr<SL::Remote_Access_Library::Utilities::Image_Wrapper> SL::Remote_Access_Library::Utilities::Image::CreateWrappedImage(unsigned int h, unsigned int w)
{
	Image_Impl i;
	i.height = h;
	i.width = w;
	return std::shared_ptr<Image_Wrapper>(new Image_Wrapper(i));
}
std::shared_ptr<SL::Remote_Access_Library::Utilities::Image> SL::Remote_Access_Library::Utilities::Image::CreateImage(unsigned int h, unsigned int w, const char * data, size_t len)
{
	assert(h*w * 4 == len);
	auto img = CreateImage(h, w);
	memcpy(img->data(), data, len);
	img->Size = len;
	return img;
}

std::shared_ptr<SL::Remote_Access_Library::Utilities::Image_Wrapper> SL::Remote_Access_Library::Utilities::Image::CreateWrappedImage(unsigned int h, unsigned int w, const char * data, size_t len)
{
	assert(h*w * 4 == len);
	auto img = CreateWrappedImage(h, w);
	memcpy(img->WrappedImage.data(), data, len);
	img->WrappedImage.Size = len;
	return img;
}
std::vector<SL::Remote_Access_Library::Utilities::Rect> SL::Remote_Access_Library::Utilities::Image::GetDifs(const Image & oldimg, const Image & newimg, const unsigned int maxdist)
{
	std::vector<SL::Remote_Access_Library::Utilities::Rect> rects;
	if (oldimg.Width() != newimg.Width() || oldimg.Height() != newimg.Height()) {
		rects.push_back(Rect(Point(0, 0), newimg.Height(), newimg.Width()));
		return rects;
	}
	rects.reserve(48);
	auto oldimg_ptr = (const unsigned int*)oldimg.data();
	auto newimg_ptr = (const unsigned int*)newimg.data();

	for (auto row = 0; row < oldimg.Width(); row++) {
		for (auto col = 0; col < oldimg.Height(); col += maxdist) {

			for (auto x = 0; x < maxdist; x++) {
				auto old = oldimg_ptr[col*oldimg.Width() + row + x];
				auto ne = newimg_ptr[col*newimg.Width() + row + x];
				if (ne != old) {
					auto begrow = (row / maxdist)*maxdist;
					auto found = std::find_if(rbegin(rects), rend(rects), [=](const SL::Remote_Access_Library::Utilities::Rect& r) {
						return SquaredDistance(Point(row, col), r) <= (maxdist*maxdist + 128 * 128);
					});
					if (found == rend(rects)) {//nothing found insert new rect
						rects.push_back(Rect(Point(begrow, col), maxdist, maxdist));
					}
					else {
						found->Expand_To_Include(Point(begrow, col));//expand the rect to encompas the point
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
		std::cout << r << std::endl;
	}
	std::cout << "Found " << rects.size() << " Rects for Dif" << std::endl;
	return rects;
}



SL::Remote_Access_Library::Utilities::Image::Image(Image_Impl& impl) : _Height(impl.height), _Width(impl.width), Size(impl.height*impl.width * 4) {
	_Data = std::make_unique<char[]>(Size);
}
SL::Remote_Access_Library::Utilities::Image::~Image() {

}

SL::Remote_Access_Library::Utilities::Image_Wrapper::Image_Wrapper(Image_Impl & impl) : WrappedImage(impl)
{

}
