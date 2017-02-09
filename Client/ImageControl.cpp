#include "ImageControl.h"
#include "Logging.h"
#include "Shapes.h"
#include "Input.h"
#include "Image.h"

#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/fl_draw.H>
#include <FL/Fl_RGB_Image.H>
#include <FL/Fl_PNG_Image.H>

#include <vector>
#include <sstream>
#include <string>
#include <mutex>
#include <memory>

namespace SL {
	namespace RAT {

		std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
			std::stringstream ss(s);
			std::string item;
			while (std::getline(ss, item, delim)) {
				if (item.empty()) continue;
				elems.push_back(item);
			}
			return elems;
		}
		std::vector<std::string> split(const std::string &s, char delim) {
			std::vector<std::string> elems;
			split(s, delim, elems);
			return elems;
		}

		std::shared_ptr<char> Resize(const std::shared_ptr<char>& inimg, int inheight, int inwidth, int dstheight, int dstwidth)
		{//linear scaling
			auto m_width = dstwidth;
			auto m_height = dstheight;
			auto outimg = std::shared_ptr<char>(new char[m_width*m_height*PixelStride], [](char* ptr) {delete[] ptr; });
			auto temp = (int*)outimg.get();
			auto pixels = (int*)inimg.get();
			double x_ratio = inwidth / (double)m_width;
			double y_ratio = inheight / (double)m_height;
			double px, py;
			for (int i = 0; i < m_height; i++) {
				for (int j = 0; j < m_width; j++) {
					px = floor(j*x_ratio);
					py = floor(i*y_ratio);
					temp[(i*m_width) + j] = pixels[(int)((py*inwidth) + px)];
				}
			}
			return outimg;
		}
		std::shared_ptr<char> Resize(const std::shared_ptr<char>& inimg, int* inheight, int* inwidth, float scale)
		{
			auto height = *inheight;
			auto width = *inwidth;
			*inheight = static_cast<int>(ceil(static_cast<float>(*inheight) * scale));
			*inwidth = static_cast<int>(ceil(static_cast<float>(*inwidth) * scale));
			return Resize(inimg, height, width, *inheight, *inwidth);
		}
		void Copy(Image& src, Rect src_rect, Image & dst, Rect dst_rect)
		{
			auto dst_start = (int*)dst.Data.get();
			auto src_start = (int*)src.Data.get();
			for (auto row = dst_rect.top(), src_row = 0; row < dst_rect.bottom(); row++, src_row++) {
				memcpy(dst_start + (dst.Rect.Width * row) + dst_rect.left(), src_start + (src.Rect.Width*src_row) + src_rect.left(), src_rect.Width*PixelStride);
			}
		}



		class ScreenImageImpl {
		public:
			Image _OriginalImage;//this is the original image, kept to resize the scaled if needed
			Image _ScaledImage;

			std::mutex _ImageLock;

			float _ScaleFactor;
			ScreenImageInfo _ScreenImageInfo;

			Point _MousePos;
			Image _MouseImageData;
			std::unique_ptr<Fl_RGB_Image> _MouseImage;

			ScreenImageImpl(ScreenImageInfo&& info) : _ScaleFactor(1.0f), _ScreenImageInfo(info) {}

			bool is_ImageScaled() const {
				return !(_ScaleFactor >= .999f && _ScaleFactor <= 1.001f);
			}
			Point get_UnscaledImageSize() const {
				return Point(_OriginalImage.Rect.Width, _OriginalImage.Rect.Height);
			}
			void set_ScaleFactor(float factor) {
				_ScaleFactor = factor;

			}
			float get_ScaleFactor() const {
				return _ScaleFactor;
			}
			void set_ScreenImage(const Rect* rect, std::shared_ptr<char>& data) {
				auto size = rect->Width*rect->Height*PixelStride;
				Image original;
				original.Rect = *rect;
				original.Data = std::shared_ptr<char>(new char[size], [](char* ptr) {delete[] ptr; });
				memcpy(original.Data.get(), data.get(), size);

				Image scaled;
				scaled.Rect = *rect;
				scaled.Data = data;
				{
					std::lock_guard<std::mutex> lock(_ImageLock);
					_OriginalImage = original;
					_ScaledImage = scaled;
				}
			}
			void Draw(int x, int y) {
				if (_ScaledImage.Data) {
					std::lock_guard<std::mutex> lock(_ImageLock);
					if (_ScaledImage.Data) {
						fl_draw_image((uchar*)_ScaledImage.Data.get(), x, y, _ScaledImage.Rect.Width, _ScaledImage.Rect.Height, 4);
						if (_MouseImage) _MouseImage->draw(_MousePos.X, _MousePos.Y);
					}
				}
			}
			void set_ImageDifference(const Rect* rect, std::shared_ptr<char>& data) {
				if (!(_OriginalImage.Data && _ScaledImage.Data)) return;// both images should exist, if not then get out!

				Image nonscaled;
				nonscaled.Rect = *rect;
				nonscaled.Data = data;

				if (is_ImageScaled()) {//rescale the incoming image image

					Image scaledimg;
					scaledimg.Rect = *rect;
					scaledimg.Rect.Origin = Point();
					scaledimg.Data = Resize(data, &scaledimg.Rect.Height, &scaledimg.Rect.Width, _ScaleFactor);

					auto x = static_cast<int>(floor(static_cast<float>(rect->left())*_ScaleFactor));
					auto y = static_cast<int>(floor(static_cast<float>(rect->top())*_ScaleFactor));

					auto dst_rect = Rect(Point(x, y), scaledimg.Rect.Height, scaledimg.Rect.Width);

					std::lock_guard<std::mutex> lock(_ImageLock);
					Copy(nonscaled, nonscaled.Rect, _OriginalImage, *rect);//keep original in sync
					Copy(scaledimg, scaledimg.Rect, _ScaledImage, dst_rect);//copy scaled down 

				}
				else {

					std::lock_guard<std::mutex> lock(_ImageLock);
					Copy(nonscaled, nonscaled.Rect, _OriginalImage, *rect);//keep original in sync
					Copy(nonscaled, nonscaled.Rect, _ScaledImage, *rect);//copy scaled down 


				}
			}
			void set_MouseImage(const Size* size, const char* data) {
				auto s = size->X*size->Y*PixelStride;
				_MouseImageData.Data = std::shared_ptr<char>(new char[s], [](char* p) {delete[] p; });
				_MouseImageData.Rect.Width = size->X;
				_MouseImageData.Rect.Height = size->Y;
				memcpy(_MouseImageData.Data.get(), data, s);
				_MouseImage = std::make_unique<Fl_RGB_Image>((uchar*)_MouseImageData.Data.get(), size->X, size->Y, PixelStride);
			}
			void set_MousePosition(const Point* pos) {
				_MousePos = *pos;
				if (is_ImageScaled()) {//need to scale the mouse pos as well

					_MousePos.X = static_cast<int>(static_cast<float>(_MousePos.X - _ScreenImageInfo.get_Left())*_ScaleFactor);
					_MousePos.Y = static_cast<int>(static_cast<float>(_MousePos.Y - _ScreenImageInfo.get_Top())*_ScaleFactor);
				}
			}
			bool Update(int& width, int& height) {
				if (!_ScaledImage.Data) return false;
				auto ret = false;
				//make sure the image is scaled properly
				if (is_ImageScaled()) {
					auto psize = _ScreenImageInfo.get_Height();
					if (psize != _ScaledImage.Rect.Height) {//rescale the image
						std::lock_guard<std::mutex> lock(_ImageLock);
						_ScaledImage.Rect = Rect(Point(0, 0), psize, static_cast<int>(_ScaleFactor* _OriginalImage.Rect.Height));
						_ScaledImage.Data = Resize(_OriginalImage.Data, _OriginalImage.Rect.Height, _OriginalImage.Rect.Width, _ScaledImage.Rect.Height, _ScaledImage.Rect.Width);
						ret = true;
					}
				}
				else {//NO SCALING!!
					if (_ScaledImage.Rect.Width != _OriginalImage.Rect.Width || _ScaledImage.Rect.Height != _OriginalImage.Rect.Width) {
						std::lock_guard<std::mutex> lock(_ImageLock);
						auto size = _OriginalImage.Rect.Height*_OriginalImage.Rect.Width*PixelStride;

						_ScaledImage.Rect = _OriginalImage.Rect;
						_ScaledImage.Data = std::shared_ptr<char>(new char[size], [](char* ptr) {delete[] ptr; });
						memcpy(_ScaledImage.Data.get(), _OriginalImage.Data.get(), size);
						ret = true;
					}
				}
				width = _ScaledImage.Rect.Width;
				height = _ScaledImage.Rect.Height;
				return ret;//no changes
			}
		};


		class ImageControlImpl : public Fl_Box {

		public:

			ScreenImageImpl _ScreenImageDriver;
			ScreenImageInfo _ScreenImageInfo;
			bool _DNDIncoming = false;

			ImageControlImpl(int X, int Y, int W, int H, const char * title, ScreenImageInfo&& info) :
				Fl_Box(X, Y, W, H, title),
				_ScreenImageDriver(std::forward<ScreenImageInfo>(info)),
				_ScreenImageInfo(info) {

			}
			virtual ~ImageControlImpl() {

			}
			virtual void draw() override {
				int height(0), width(0);
				if (_ScreenImageDriver.Update(width, height)) {
					this->size(width, height);
				}
				_ScreenImageDriver.Draw(x(), y());
			}
			void OnResize(int W, int H, int SS) {
				if (_ScreenImageDriver.is_ImageScaled()) {
					auto pheight = h() - SS;//16 is the scrollbars size
					if (pheight < 0) pheight = 48;//cannot make image smaller than this..
					auto dims = _ScreenImageDriver.get_UnscaledImageSize();
					_ScreenImageDriver.set_ScaleFactor(static_cast<float>(pheight) / static_cast<float>(dims.Y));
				}

			}
			int pastedstuff() {
				auto t = std::string(Fl::event_text(), Fl::event_length());
				auto splits = split(t, '\n');
				for (auto& s : splits) {
					SL_RAT_LOG(Logging_Levels::INFO_log_level, "split " << s);
				}
				return 1;
			}
			void handlemouse(int e, int button, Press press, int x, int y) {
				if (_ScreenImageDriver.is_ImageScaled()) {
					x = static_cast<int>(static_cast<float>(x) / _ScreenImageDriver.get_ScaleFactor());
					y = static_cast<int>(static_cast<float>(y) / _ScreenImageDriver.get_ScaleFactor());
				}
				_ScreenImageInfo.OnMouse(e, button, press, x, y);
			}
			virtual int handle(int e) override {

				switch (e) {
				case FL_PUSH:
					handlemouse(e, Fl::event_button(), Press::DOWN, Fl::event_x() + _ScreenImageInfo.get_Left(), Fl::event_y() + _ScreenImageInfo.get_Top());
					return 1;
				case FL_RELEASE:
					handlemouse(e, Fl::event_button(), Press::UP, Fl::event_x() + _ScreenImageInfo.get_Left(), Fl::event_y() + _ScreenImageInfo.get_Top());
					break;
				case FL_ENTER:
					return 1;
				case FL_DRAG:
				case FL_MOUSEWHEEL:
				case FL_MOVE:
					handlemouse(e, Events::NO_EVENTDATA, Press::NO_PRESS_DATA, Fl::event_x() + _ScreenImageInfo.get_Left(), Fl::event_y() + _ScreenImageInfo.get_Top());
					break;
				case FL_KEYDOWN:
					_ScreenImageInfo.OnKey(e, Press::DOWN);
					return 1;
				case FL_KEYUP:
					_ScreenImageInfo.OnKey(e, Press::UP);
					return 1;
				case FL_FOCUS:
					return 1;
				case FL_PASTE:
					if (_DNDIncoming) {
						_DNDIncoming = false;
						return pastedstuff();
					}
				case FL_DND_ENTER:
				case FL_DND_LEAVE:
				case FL_DND_DRAG:
					return 1;
				case FL_DND_RELEASE:
					_DNDIncoming = true;
					return 1;

				default:
					break;
				};
				return Fl_Box::handle(e);
			}

		};

	}
}
SL::RAT::ImageControl::ImageControl(int X, int Y, int W, int H, const char * title, ScreenImageInfo&& info) {
	_ImageControlImpl = new ImageControlImpl(X, Y, W, H, title, std::forward<ScreenImageInfo>(info));
}

SL::RAT::ImageControl::~ImageControl() {
	delete _ImageControlImpl;
}

void SL::RAT::ImageControl::OnResize(int W, int H, int SS)
{
	_ImageControlImpl->OnResize(W, H, SS);
}

bool SL::RAT::ImageControl::is_ImageScaled() const
{
	return _ImageControlImpl->_ScreenImageDriver.is_ImageScaled();
}

void SL::RAT::ImageControl::set_ScreenImage(const Rect* rect, std::shared_ptr<char>& data)
{
	_ImageControlImpl->size(rect->Width, rect->Height);
	_ImageControlImpl->_ScreenImageDriver.set_ScreenImage(rect, data);
}

void SL::RAT::ImageControl::set_ImageDifference(const Rect* rect, std::shared_ptr<char>& data)
{
	_ImageControlImpl->_ScreenImageDriver.set_ImageDifference(rect, data);
}

void SL::RAT::ImageControl::set_MouseImage(const Size* size, const char* data)
{
	_ImageControlImpl->_ScreenImageDriver.set_MouseImage(size, data);
}

void SL::RAT::ImageControl::set_MousePosition(const Point* pos)
{
	_ImageControlImpl->_ScreenImageDriver.set_MousePosition(pos);
}
