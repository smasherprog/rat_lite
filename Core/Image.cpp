#include "stdafx.h"
#include "Image.h"
#include <assert.h>
#include <iterator>

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

std::shared_ptr<SL::Remote_Access_Library::Utilities::Image> SL::Remote_Access_Library::Utilities::Image::CreateImage(unsigned int h, unsigned int w, const char * data, size_t pixel_stride)
{
	assert(pixel_stride==3 || pixel_stride==4);
	
	auto img = CreateImage(h, w);
	img->Size = h*w*img->Stride();
	if(pixel_stride==4){
		memcpy(img->data(), data, img->Size);
	}else {
		auto dst = (unsigned int*)img->data();
		auto src = data;
		for(decltype(h*w) i=0;i<h*w; i++){
			auto srcdata = *((unsigned int*)src);
			*dst++ = (srcdata & 0xffffff00) | 0x000000ff;
			src += 3;
		}
	}
	
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

	//auto start = std::chrono::steady_clock::now();

#define maxdist 128
	std::vector<SL::Remote_Access_Library::Utilities::Rect> rects;
	if (oldimg.Width() != newimg.Width() || oldimg.Height() != newimg.Height() || newimg.Width()==0 || newimg.Height()==0) {
		rects.push_back(Rect(Point(0, 0), newimg.Height(), newimg.Width()));
		return rects;
	}
	rects.reserve((newimg.Height()/ maxdist) +1 * (newimg.Width() / maxdist) + 1);
	auto oldimg_ptr = (const int*)oldimg.data();
	auto newimg_ptr = (const int*)newimg.data();

	for (decltype(oldimg.Height()) row = 0; row < oldimg.Height(); row += maxdist) {
		for (decltype(oldimg.Width()) col = 0; col < oldimg.Width(); col += maxdist) {

			for (decltype(row) y = row; y < maxdist + row && y < oldimg.Height(); y++) {
				for (decltype(col) x = col; x < maxdist + col&& x < oldimg.Width(); x++) {
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

	//auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start);
	//std::cout << "It took " << elapsed.count() << " milliseconds to compare run GetDifs " << std::endl;

	if (rects.size() <= 2) {
		SanitizeRects(rects, newimg);
		return rects;//make sure there is at least 2
	}
	std::vector<SL::Remote_Access_Library::Utilities::Rect> outrects;
	outrects.reserve(rects.size());
	outrects.push_back(rects[0]);
	//horizontal scan
	for (size_t i = 1; i < rects.size() - 1; i++) {
		if (outrects.back().right() == rects[i].left() && outrects.back().bottom() == rects[i].bottom()) {
			outrects.back().right(rects[i].right());
		}
		else {
			outrects.push_back(rects[i]);
		}
	}
	if (outrects.size() <= 2) {
		SanitizeRects(outrects, newimg);
		return outrects;//make sure there is at least 2
	}
	rects.resize(0);
	//vertical scan
	for (auto& otrect : outrects) {
		
		auto found = std::find_if(rects.rbegin(), rects.rend(), [=](const SL::Remote_Access_Library::Utilities::Rect& rec) {
			return rec.bottom() == otrect.top() && rec.left() == otrect.left() && rec.right() == otrect.right();
		});
		if (found == rects.rend()) {
			rects.push_back(otrect);
		}
		else {
			found->bottom(otrect.bottom());
		}
	}
	SanitizeRects(rects, newimg);
	return rects;
}

void SL::Remote_Access_Library::Utilities::Image::Copy(Image& src, Rect src_rect, Image & dst, Rect dst_rect)
{
	auto dst_start = (int*)dst.data();
	auto src_start = (int*)src.data();
	for (auto row = dst_rect.top(), src_row=0; row < dst_rect.bottom(); row++, src_row++) {
		memcpy(dst_start + (dst.Width() * row) + dst_rect.left(), src_start + (src.Width()*src_row) + src_rect.left(), src_rect.Width*src.Stride());
	}
}



SL::Remote_Access_Library::Utilities::Image::Image(Image_Impl& impl) : _Height(impl.height), _Width(impl.width), Size(impl.height*impl.width * 4) {
	_Data = std::make_unique<char[]>(Size);
}
SL::Remote_Access_Library::Utilities::Image::~Image() {

}
