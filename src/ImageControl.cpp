#include "ImageControl.h"
#include "ScreenCapture.h"
#include "Logging.h"
#include "Shapes.h"
#include "Input.h"

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
	namespace Remote_Access_Library {

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

		class ScreenImageImpl {
		public:
			std::shared_ptr<Screen_Capture::Image> _OriginalImage;//this is the original image, kept to resize the scaled if needed
			Point _OriginalImageSize;//helper to avoid locking where its not needed

			std::shared_ptr<Screen_Capture::Image> _ScaledImage;
			Point _ScaledImageSize;//helper to avoid locking where its not needed

			std::mutex _ImageLock;

			float _ScaleFactor;
			ScreenImageInfo _ScreenImageInfo;

			Point _MousePos;
			std::shared_ptr<Screen_Capture::Image> _MouseImageData;
			std::unique_ptr<Fl_RGB_Image> _MouseImage;

			ScreenImageImpl(ScreenImageInfo&& info) : _ScaleFactor(1.0f), _ScreenImageInfo(info) {}

			bool is_ImageScaled() const {
				return !(_ScaleFactor >= .999f && _ScaleFactor <= 1.001f);
			}
			Point get_UnscaledImageSize() const {
				if (_OriginalImage) return _OriginalImageSize;
				return Point(0, 0);
			}
			void set_ScaleFactor(float factor) {
				_ScaleFactor = factor;

			}
			float get_ScaleFactor() const {
				return _ScaleFactor;
			}
			void set_ScreenImage(std::shared_ptr<Screen_Capture::Image>& img) {
				/*auto tmp = Image::CreateImage(img->Height(), img->Width(), img->data(), 4);
				{
					std::lock_guard<std::mutex> lock(_ImageLock);
					_OriginalImageSize = Point(img->Width(), img->Height());
					_OriginalImage = img;
					_ScaledImageSize = Point(img->Width(), img->Height());
					_ScaledImage = tmp;
				}*/
			}
			void Draw(int x, int y) {
		/*		if (_ScaledImage) {
					std::lock_guard<std::mutex> lock(_ImageLock);
					if (_ScaledImage) {
						fl_draw_image((uchar*)_ScaledImage->data(), x, y, _ScaledImage->Width(), _ScaledImage->Height(), 4);
						if (_MouseImage) _MouseImage->draw(_MousePos.X, _MousePos.Y);
					}
				}*/
			}
			void set_ImageDifference(Point& pos, const std::shared_ptr<Screen_Capture::Image>& img) {

				//if (_OriginalImage) {

				//	auto difdst_rect = Rect(pos, (int)img->Height(), (int)img->Width());
				//	auto difsrc_rect = Rect(Point(0, 0), (int)img->Height(), (int)img->Width());

				//	if (_ScaledImage) {
				//		if (is_ImageScaled() && _OriginalImageSize.Y != _ScaledImageSize.Y) {//rescale the incoming image image

				//			auto resampledimg = Image::Resize(img, _ScaleFactor);
				//			pos.X = static_cast<int>(floor(static_cast<float>(pos.X)*_ScaleFactor));
				//			pos.Y = static_cast<int>(floor(static_cast<float>(pos.Y)*_ScaleFactor));
				//			auto dst_rect = Rect(pos, (int)resampledimg->Height(), (int)resampledimg->Width());
				//			auto src_rect = Rect(Point(0, 0), (int)resampledimg->Height(), (int)resampledimg->Width());

				//			{
				//				std::lock_guard<std::mutex> lock(_ImageLock);
				//				Image::Copy(*img, difsrc_rect, *_OriginalImage, difdst_rect);//keep original in sync
				//				Image::Copy(*resampledimg, src_rect, *_ScaledImage, dst_rect);//copy scaled down 
				//			}
				//		}
				//		else {
				//			auto dst_rect = Rect(pos, (int)img->Height(), (int)img->Width());
				//			auto src_rect = Rect(Point(0, 0), (int)img->Height(), (int)img->Width());
				//			{
				//				std::lock_guard<std::mutex> lock(_ImageLock);
				//				Image::Copy(*img, difsrc_rect, *_OriginalImage, difdst_rect);//keep original in sync
				//				Image::Copy(*img, src_rect, *_ScaledImage, dst_rect);//no sling going on here 
				//			}
				//		}
				//	}
				//	else {
				//		std::lock_guard<std::mutex> lock(_ImageLock);
				//		Image::Copy(*img, difsrc_rect, *_OriginalImage, difdst_rect);//keep original in sync
				//	}
				//}
			}
			void set_MouseImage(std::shared_ptr<Screen_Capture::Image>& img) {
				_MouseImageData = img;
//				_MouseImage = std::make_unique<Fl_RGB_Image>((uchar*)_MouseImageData->data(), _MouseImageData->Width(), _MouseImageData->Height(), 4);
			}
			void set_MousePosition(Point* pos) {
				_MousePos = *pos;
				if (is_ImageScaled()) {//need to scale the mouse pos as well

					_MousePos.X = static_cast<int>(static_cast<float>(_MousePos.X - _ScreenImageInfo.get_Left())*_ScaleFactor);
					_MousePos.Y = static_cast<int>(static_cast<float>(_MousePos.Y - _ScreenImageInfo.get_Top())*_ScaleFactor);
				}
			}
			bool Update(int& width, int& height) {
				//if (!_ScaledImage) return false;

				////make sure the image is scaled properly
				//if (is_ImageScaled()) {
				//	auto psize = _ScreenImageInfo.get_Height();
				//	if (psize != _ScaledImageSize.Y) {//rescale the image
				//		{
				//			std::lock_guard<std::mutex> lock(_ImageLock);
				//			_ScaledImage = Image::Resize(_OriginalImage, psize, static_cast<int>(_ScaleFactor*_OriginalImage->Width()));
				//			_ScaledImageSize = Point(_ScaledImage->Width(), _ScaledImage->Height());
				//		}
				//		width = _ScaledImage->Width();
				//		height = _ScaledImage->Height();
				//		return true;
				//	}
				//}
				//else {//NO SCALING!!
				//	if (_ScaledImageSize.X != _OriginalImageSize.X || _ScaledImageSize.Y != _OriginalImageSize.Y) {
				//		{
				//			std::lock_guard<std::mutex> lock(_ImageLock);
				//			_ScaledImage = Image::CreateImage(_OriginalImage->Height(), _OriginalImage->Width(), _OriginalImage->data(), 4);
				//			_ScaledImageSize = Point(_ScaledImage->Width(), _ScaledImage->Height());
				//		}
				//		width = _ScaledImage->Width();
				//		height = _ScaledImage->Height();
				//		return true;
				//	}
				//}
				//width = _ScaledImage->Width();
				//height = _ScaledImage->Height();
				return false;//no changes
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


SL::Remote_Access_Library::ImageControl::ImageControl(int X, int Y, int W, int H, const char * title, ScreenImageInfo&& info) {
	_ImageControlImpl = new ImageControlImpl(X, Y, W, H, title, std::forward<ScreenImageInfo>(info));
}

SL::Remote_Access_Library::ImageControl::~ImageControl() {
	delete _ImageControlImpl;
}

void SL::Remote_Access_Library::ImageControl::OnResize(int W, int H, int SS)
{
	//_ImageControlImpl->OnResize(W, H, SS);
}

bool SL::Remote_Access_Library::ImageControl::is_ImageScaled() const
{
	//return _ImageControlImpl->_ScreenImageDriver.is_ImageScaled();
	return true;
}

void SL::Remote_Access_Library::ImageControl::set_ScreenImage(std::shared_ptr<Screen_Capture::Image>& img)
{
	//_ImageControlImpl->size(Width(*img), Height(*img));
	//return _ImageControlImpl->_ScreenImageDriver.set_ScreenImage(img);
}

void SL::Remote_Access_Library::ImageControl::set_ImageDifference(Point & pos, const std::shared_ptr<Screen_Capture::Image>& img)
{
	//return _ImageControlImpl->_ScreenImageDriver.set_ImageDifference(pos, img);
}

void SL::Remote_Access_Library::ImageControl::set_MouseImage(std::shared_ptr<Screen_Capture::Image>& img)
{
	//return _ImageControlImpl->_ScreenImageDriver.set_MouseImage(img);
}

void SL::Remote_Access_Library::ImageControl::set_MousePosition(Point * pos)
{
	//return _ImageControlImpl->_ScreenImageDriver.set_MousePosition(pos);
}
