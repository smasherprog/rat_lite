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
void SanitizeRects(std::vector<SL::Remote_Access_Library::Utilities::Rect>& rects, const SL::Remote_Access_Library::Utilities::Image & img) {
	for (auto& r : rects) {
		if (r.right() > static_cast<int>(img.Width())) {
			r.right(static_cast<int>(img.Width()));
		}
		if (r.bottom() > static_cast<int>(img.Height())) {
			r.bottom(static_cast<int>(img.Height()));
		}
		//std::cout << r << std::endl;
	}
}
std::vector<SL::Remote_Access_Library::Utilities::Rect> SL::Remote_Access_Library::Utilities::Image::GetDifs(const Image & oldimg, const Image & newimg)
{

	auto start = std::chrono::steady_clock::now();


#define maxdist 128
	std::vector<SL::Remote_Access_Library::Utilities::Rect> rects;
	if (oldimg.Width() != newimg.Width() || oldimg.Height() != newimg.Height()) {
		rects.push_back(Rect(Point(0, 0), newimg.Height(), newimg.Width()));
		return rects;
	}
	rects.reserve(48);
	auto oldimg_ptr = (const int*)oldimg.data();
	auto newimg_ptr = (const int*)newimg.data();
	auto counter = 0;


	for (int row = 0; row < oldimg.Height(); row += maxdist) {
		for (int col = 0; col < oldimg.Width(); col += maxdist) {

			for (int y = row; y < maxdist + row && y < oldimg.Height(); y++) {
				for (int x = col; x < maxdist + col&& x < oldimg.Width(); x++) {
					auto old = oldimg_ptr[y*oldimg.Width() + x];
					auto ne = newimg_ptr[y*newimg.Width() + x];
					if (ne != old) {
						rects.push_back(Rect(Point(col, row), maxdist, maxdist));
						y += maxdist;
						break;//this will get out of the loop
					}
				}
			}
		}
	}

	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start);
	//std::cout << "It took " << elapsed.count() << " milliseconds to compare run GetDifs " << counter << std::endl;

	if (rects.size() <= 2) {
		SanitizeRects(rects, newimg);
		return rects;//make sure there is at least 2
	}
	std::vector<SL::Remote_Access_Library::Utilities::Rect> outrects;
	outrects.reserve(rects.size() / 4);
	outrects.push_back(rects[0]);
	//horizontal scan
	for (size_t i = 1; i < rects.size() - 1;i++) {
		if (outrects.back().right() == rects[i].left() && outrects.back().bottom() == rects[i].bottom()) {
			outrects.back().right(rects[i].right());
		}
		else {
			outrects.push_back(rects[i]);
		}
	}	
	if (outrects.size() <= 2) 	{
		SanitizeRects(outrects, newimg);
		return outrects;//make sure there is at least 2
	}
	rects.resize(0);
	//vertical scan
	for (auto& or: outrects){

		auto found = std::find_if(rbegin(rects), rend(rects), [=](const SL::Remote_Access_Library::Utilities::Rect& rec) {
			return rec.bottom() == or.top() && rec.left() == or.left() && rec.right() == or.right();
		});
		if (found == rend(rects)) {
			rects.push_back(or);
		}
		else {
			found->bottom(or.bottom());
		}
	}
	SanitizeRects(rects, newimg);
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
