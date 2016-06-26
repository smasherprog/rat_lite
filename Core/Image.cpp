#include "stdafx.h"
#include "Image.h"
#include <assert.h>
#include <iterator>
#include <array>
#include <string.h>
#include "Logging.h"

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
	assert(pixel_stride == 3 || pixel_stride == 4);

	auto img = CreateImage(h, w);
	img->Size = h*w*img->Stride();
	if (pixel_stride == 4) {
		memcpy(img->data(), data, img->Size);
	}
	else {
		auto dst = (unsigned int*)img->data();
		auto src = data;
		for (decltype(h*w) i = 0; i < h*w; i++) {
			auto srcdata = *((unsigned int*)src);
			*dst++ = (srcdata & 0xffffff00) | 0x000000ff;
			src += 3;
		}
	}
	return img;
}
//const unsigned char* GetPixelClamped(const unsigned char* image, int x, int y)
//{
//	CLAMP(x, 0, image.m_width - 1);
//	CLAMP(y, 0, image.m_height - 1);
//	return &image.m_pixels[(y * image.m_pitch) + x * 3];
//}
//
//float Lerp(float A, float B, float t)
//{
//	return A * (1.0f - t) + B * t;
//}
//
//
//std::array<uint8, 4> SampleLinear(const SImageData& image, float u, float v)
//{
//	// calculate coordinates -> also need to offset by half a pixel to keep image from shifting down and left half a pixel
//	float x = (u * image.m_width) - 0.5f;
//	int xint = int(x);
//	float xfract = x - floor(x);
//
//	float y = (v * image.m_height) - 0.5f;
//	int yint = int(y);
//	float yfract = y - floor(y);
//
//	// get pixels
//	auto p00 = GetPixelClamped(image, xint + 0, yint + 0);
//	auto p10 = GetPixelClamped(image, xint + 1, yint + 0);
//	auto p01 = GetPixelClamped(image, xint + 0, yint + 1);
//	auto p11 = GetPixelClamped(image, xint + 1, yint + 1);
//
//	// interpolate bi-linearly!
//	std::array<uint8, 3> ret;
//	for (int i = 0; i < 3; ++i)
//	{
//		float col0 = Lerp(p00[i], p10[i], xfract);
//		float col1 = Lerp(p01[i], p11[i], xfract);
//		float value = Lerp(col0, col1, yfract);
//		CLAMP(value, 0.0f, 255.0f);
//		ret[i] = uint8(value);
//	}
//	return ret;
//}
//

std::shared_ptr<SL::Remote_Access_Library::Utilities::Image> SL::Remote_Access_Library::Utilities::Image::Resize(const std::shared_ptr<Image>& inimg, float scale, Image_Resamples resample)
{
	return Resize(inimg, static_cast<int>(ceil(static_cast<float>(inimg->Height()) * scale)), static_cast<int>(ceil(static_cast<float>(inimg->Width()) * scale)), resample);
}

std::shared_ptr<SL::Remote_Access_Library::Utilities::Image> SL::Remote_Access_Library::Utilities::Image::Resize(const std::shared_ptr<Image>& inimg, int height, int width, Image_Resamples resample)
{
	UNUSED(resample);
	/*
	TODO ADD SUPPORT FOR LINEAR AND BICUBIC RESAMPLE.. They are in commented out code above and below
	*/

	auto m_width = width;
	auto m_height = height;
	auto outimg = CreateImage(m_height, m_width);
	auto temp = (int*)outimg->data();
	auto pixels = (int*)inimg->data();
	double x_ratio = inimg->Width() / (double)m_width;
	double y_ratio = inimg->Height() / (double)m_height;
	double px, py;
	for (int i = 0; i < m_height; i++) {
		for (int j = 0; j < m_width; j++) {
			px = floor(j*x_ratio);
			py = floor(i*y_ratio);
			temp[(i*m_width) + j] = pixels[(int)((py*inimg->Width()) + px)];
		}
	}
	return outimg;
}

//
//std::vector<unsigned char> bicubicresize(const std::vector<unsigned char>& in,
//	std::size_t src_width, std::size_t src_height, std::size_t dest_width, std::size_t dest_height)
//{
//	std::vector<unsigned char> out(dest_width * dest_height * 3);
//
//	const float tx = float(src_width) / dest_width;
//	const float ty = float(src_height) / dest_height;
//	const int channels = 3;
//	const std::size_t row_stride = dest_width * channels;
//
//	unsigned char C[5] = { 0 };
//
//	for (int i = 0; i < dest_height; ++i)
//	{
//		for (int j = 0; j < dest_width; ++j)
//		{
//			const int x = int(tx * j);
//			const int y = int(ty * i);
//			const float dx = tx * j - x;
//			const float dy = ty * i - y;
//
//			for (int k = 0; k < 3; ++k)
//			{
//				for (int jj = 0; jj < 4; ++jj)
//				{
//					const int z = y - 1 + jj;
//					unsigned char a0 = getpixel(in, src_width, src_height, z, x, k);
//					unsigned char d0 = getpixel(in, src_width, src_height, z, x - 1, k) - a0;
//					unsigned char d2 = getpixel(in, src_width, src_height, z, x + 1, k) - a0;
//					unsigned char d3 = getpixel(in, src_width, src_height, z, x + 2, k) - a0;
//					unsigned char a1 = -1.0 / 3 * d0 + d2 - 1.0 / 6 * d3;
//					unsigned char a2 = 1.0 / 2 * d0 + 1.0 / 2 * d2;
//					unsigned char a3 = -1.0 / 6 * d0 - 1.0 / 2 * d2 + 1.0 / 6 * d3;
//					C[jj] = a0 + a1 * dx + a2 * dx * dx + a3 * dx * dx * dx;
//
//					d0 = C[0] - C[1];
//					d2 = C[2] - C[1];
//					d3 = C[3] - C[1];
//					a0 = C[1];
//					a1 = -1.0 / 3 * d0 + d2 - 1.0 / 6 * d3;
//					a2 = 1.0 / 2 * d0 + 1.0 / 2 * d2;
//					a3 = -1.0 / 6 * d0 - 1.0 / 2 * d2 + 1.0 / 6 * d3;
//					out[i * row_stride + j * channels + k] = a0 + a1 * dy + a2 * dy * dy + a3 * dy * dy * dy;
//				}
//			}
//		}
//	}
//
//	return out;
//}
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
struct Image_Range {
	unsigned int row, col, col_end;//row_end is one passed the end
};
#define maxdist 256
void Get_Range(Image_Range& prev_range, std::vector<int>& grid, unsigned int grid_width, unsigned int real_col_end) {

	prev_range.row = prev_range.row / maxdist;
	prev_range.col = prev_range.col / maxdist;
	for (; prev_range.col < grid_width; prev_range.col++) {
		if (grid[prev_range.row*grid_width + prev_range.col] != 1) {
			prev_range.row *= maxdist;
			prev_range.col *= maxdist;
			auto poscolend = prev_range.col + maxdist;
			prev_range.col_end = std::min(poscolend, real_col_end);
			return;
		}
	}
	prev_range.row *= maxdist;
	prev_range.col *= maxdist;
	prev_range.col_end = prev_range.col;
}

std::vector<SL::Remote_Access_Library::Utilities::Rect> SL::Remote_Access_Library::Utilities::Image::GetDifs(const Image & oldimg, const Image & newimg)
{

	auto start = std::chrono::steady_clock::now();


	std::vector<SL::Remote_Access_Library::Utilities::Rect> rects;
	if (oldimg.Width() != newimg.Width() || oldimg.Height() != newimg.Height() || newimg.Width() == 0 || newimg.Height() == 0) {
		rects.push_back(Rect(Point(0, 0), newimg.Height(), newimg.Width()));
		return rects;
	}
	rects.reserve((newimg.Height() / maxdist) + 1 * (newimg.Width() / maxdist) + 1);
	auto oldimg_ptr = (const int*)oldimg.data();
	auto newimg_ptr = (const int*)newimg.data();

	auto imgwidth = oldimg.Width();
	auto imgheight = oldimg.Height();

	for (decltype(imgheight) row = 0; row < imgheight; row += maxdist) {
		for (decltype(imgwidth) col = 0; col < imgwidth; col += maxdist) {

			for (decltype(row) y = row; y < maxdist + row && y < imgheight; y++) {
				for (decltype(col) x = col; x < maxdist + col&& x <imgwidth; x++) {
					auto old = oldimg_ptr[y*imgwidth + x];
					auto ne = newimg_ptr[y*imgwidth + x];
					if (ne != old) {
						rects.push_back(Rect(Point(col, row), maxdist, maxdist));
						y += maxdist;
						x += maxdist;
					}
				}
			}
		}
	}

	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start);
	//SL_RAT_LOG(Logging_Levels::INFO_log_level, "It took " << elapsed.count() << " milliseconds to compare run GetDifs ");

	if (rects.size() <= 2) {
		SanitizeRects(rects, newimg);
		return rects;//make sure there is at least 2
	}
	std::vector<SL::Remote_Access_Library::Utilities::Rect> outrects;
	outrects.reserve(rects.size());
	outrects.push_back(rects[0]);
	//horizontal scan
	for (size_t i = 1; i < rects.size(); i++) {
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
	for (auto row = dst_rect.top(), src_row = 0; row < dst_rect.bottom(); row++, src_row++) {
		memcpy(dst_start + (dst.Width() * row) + dst_rect.left(), src_start + (src.Width()*src_row) + src_rect.left(), src_rect.Width*src.Stride());
	}
}




SL::Remote_Access_Library::Utilities::Image::Image(Image_Impl& impl) : _Height(impl.height), _Width(impl.width), Size(impl.height*impl.width * 4) {
	_Data = std::make_unique<char[]>(Size);
}
SL::Remote_Access_Library::Utilities::Image::~Image() {

}
