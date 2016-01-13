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
std::vector<SL::Remote_Access_Library::Utilities::Rect> SL::Remote_Access_Library::Utilities::Image::GetDifs(const Image & oldimg, const Image & newimg, unsigned int maxdist)
{
	std::vector<SL::Remote_Access_Library::Utilities::Rect> rects;
	if (oldimg.Width() != newimg.Width() || oldimg.Height() != newimg.Height()) {
		rects.push_back(Rect(Point(0, 0), newimg.Height(), newimg.Width()));
		return rects;
	}
	rects.reserve(48);
	auto oldimg_ptr = (const unsigned int*)oldimg.data();
	auto newimg_ptr = (const unsigned int*)newimg.data();

	for (decltype(oldimg.Width()) row = 0; row < oldimg.Height(); row++) {
		for (decltype(oldimg.Height()) col = 0; col < oldimg.Width(); col += maxdist) {

			for (decltype(maxdist) x = 0; x < maxdist; x++) {
				auto old = oldimg_ptr[row*oldimg.Width() + col + x];
				auto ne = newimg_ptr[row*newimg.Width() + col + x];
				if (ne != old) {
					auto begrow = (row / maxdist)*maxdist;
					auto found = std::find_if(rbegin(rects), rend(rects), [=](const SL::Remote_Access_Library::Utilities::Rect& r) {
						return r.Contains(Point(col + x, row)) || 
							(SquaredDistance(Point(col + x, row), Point(r.left(), r.top())) <= maxdist*maxdist + maxdist*maxdist) || 
							(SquaredDistance(Point(col + x, row), Point(r.right(), r.top())) <= maxdist*maxdist + maxdist*maxdist) || 
							(SquaredDistance(Point(col + x, row), Point(r.left(), r.bottom())) <= maxdist*maxdist + maxdist*maxdist) || 
							(SquaredDistance(Point(col + x, row), Point(r.right(), r.bottom())) <= maxdist*maxdist + maxdist*maxdist);

					});

					if (found == rend(rects)) {//nothing found insert new rect
						rects.push_back(Rect(Point(col, begrow), maxdist, maxdist));
					}
					else {
						found->Expand_To_Include(Point(col, begrow));//expand the rect to encompas the point
					}

					break;//get out..
				}
			}
		}
	}
	for (auto& r : rects) {
		if (r.right() > static_cast<int>(newimg.Width())) {
			r.right(static_cast<int>(newimg.Width()));
		}
		if (r.bottom() > static_cast<int>(newimg.Height())) {
			r.bottom(static_cast<int>(newimg.Height()));
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
