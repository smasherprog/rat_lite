#include "stdafx.h"
#include "ImageControl.h"
#include "Image.h"
#include "Logging.h"
#include "Shapes.h"

#include <FL/Fl.H>
#include <FL/fl_draw.H>
#include <FL/Fl_RGB_Image.H>
#include <FL/Fl_PNG_Image.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Box.H>

#include <mutex>
#include <assert.h>
#include <utility>

namespace SL {
	namespace Remote_Access_Library {
		namespace UI {
			class ImageControlImpl : public Fl_Box {
				std::shared_ptr<Utilities::Image> _OriginalImage;//this is the original image, kept to resize the scaled if needed
				std::shared_ptr<Utilities::Image> _ScaledImage;
				std::mutex _ImageLock;

				Utilities::Point _MousePos;
				std::shared_ptr<Utilities::Image> _MouseImageData;
				std::unique_ptr<Fl_RGB_Image> _MouseImage;

				bool _ScaleImage = false;
				ImageControlInfo _ImageControlInfo;

				int ParentSize() const {
					auto pheight = this->parent()->h() - _ImageControlInfo._Scroller->scrollbar_size();
					if (pheight < 0) pheight = 48;//cannot make image smaller than this..
					return pheight;
				}


			public:

				ImageControlImpl(int X, int Y, int W, int H, const char * title, ImageControlInfo&& info) : Fl_Box(X, Y, W, H, title), _ImageControlInfo(std::move(info)){}
				virtual ~ImageControlImpl() {

				}
				virtual void draw() override {

					if (_ScaledImage) {
						//make sure the image is scaled properly
						if (_ScaleImage) {
							auto psize = ParentSize();
							if (psize != static_cast<int>(_ScaledImage->Height())) {//rescale the image

								auto tmpscaled = Utilities::Image::Resize(_OriginalImage, psize, static_cast<int>(GetScaleFactor()*_OriginalImage->Width()));
								{
									std::lock_guard<std::mutex> lock(_ImageLock);
									_ScaledImage = tmpscaled;
								}
								this->size(_ScaledImage->Width(), _ScaledImage->Height());
							}
						}
						else {//NO SCALING!!
							if (_ScaledImage->Width() != _OriginalImage->Width() || _ScaledImage->Height() != _OriginalImage->Height()) {
								auto tmpscaled = Utilities::Image::CreateImage(_OriginalImage->Height(), _OriginalImage->Width(), _OriginalImage->data(), 4);
								{
									std::lock_guard<std::mutex> lock(_ImageLock);
									_ScaledImage = tmpscaled;
								}
								this->size(_ScaledImage->Width(), _ScaledImage->Height());
							}
						}

						fl_draw_image((uchar*)_ScaledImage->data(), x(), y(), _ScaledImage->Width(), _ScaledImage->Height(), 4);
					}
					if (_MouseImage) {
						_MouseImage->draw(_MousePos.X, _MousePos.Y);
					}
				}

				void SetImage(const std::shared_ptr<Utilities::Image>& m) {
					_OriginalImage = m;
					auto tmpscaled = Utilities::Image::CreateImage(_OriginalImage->Height(), _OriginalImage->Width(), _OriginalImage->data(), 4);
					{
						std::lock_guard<std::mutex> lock(_ImageLock);
						_ScaledImage = tmpscaled;
					}
					this->size(m->Width(), m->Height());
				}
				void SetImageDif(Utilities::Point pos, const std::shared_ptr<Utilities::Image>& img) {
					if (_OriginalImage) {
						auto dst_rect = Utilities::Rect(pos, (int)img->Height(), (int)img->Width());
						auto src_rect = Utilities::Rect(Utilities::Point(0, 0), (int)img->Height(), (int)img->Width());
						Utilities::Image::Copy(*img, src_rect, *_OriginalImage, dst_rect);//keep original in sync
						if (_ScaledImage) {
							if (_ScaleImage && _OriginalImage->Height() != _ScaledImage->Height()) {//rescale the incoming image image
								auto scalefactor = GetScaleFactor();
								auto resampledimg = Utilities::Image::Resize(img, scalefactor);
								pos.X = static_cast<int>(floor(static_cast<float>(pos.X)*scalefactor));
								pos.Y = static_cast<int>(floor(static_cast<float>(pos.Y)*scalefactor));
								auto dst_rect = Utilities::Rect(pos, (int)resampledimg->Height(), (int)resampledimg->Width());
								auto src_rect = Utilities::Rect(Utilities::Point(0, 0), (int)resampledimg->Height(), (int)resampledimg->Width());
								{
									std::lock_guard<std::mutex> lock(_ImageLock);
									Utilities::Image::Copy(*resampledimg, src_rect, *_ScaledImage, dst_rect);//copy scaled down 
								}

							}
							else {
								auto dst_rect = Utilities::Rect(pos, (int)img->Height(), (int)img->Width());
								auto src_rect = Utilities::Rect(Utilities::Point(0, 0), (int)img->Height(), (int)img->Width());
								{
									std::lock_guard<std::mutex> lock(_ImageLock);
									Utilities::Image::Copy(*img, src_rect, *_ScaledImage, dst_rect);//no sling going on here 
								}
							}
						}
					}
				}
				float GetScaleFactor() const {
					if (_OriginalImage && _ScaleImage) {
						auto pheight = this->parent()->h() - _ImageControlInfo._Scroller->scrollbar_size();//16 is the scrollbars size
						if (pheight < 0) pheight = 48;//cannot make image smaller than this..
						return static_cast<float>(pheight) / static_cast<float>(_OriginalImage->Height());
					}
					return 1.0f;
				}
				void SetMouseImage(const std::shared_ptr<Utilities::Image>& m) {
					_MouseImageData = m;
					_MouseImage = std::make_unique<Fl_RGB_Image>((uchar*)_MouseImageData->data(), _MouseImageData->Width(), _MouseImageData->Height(), 4);
				}
				void SetMousePosition(const Utilities::Point& m) {
					_MousePos = m;
					if (_ScaledImage) {//need to scale the mouse pos as well
						auto scalefactor = GetScaleFactor();
						
						_MousePos.X = static_cast<int>(static_cast<float>( _MousePos.X - _ImageControlInfo._Scroller->xposition())*scalefactor);
						_MousePos.Y = static_cast<int>(static_cast<float>(_MousePos.Y - _ImageControlInfo._Scroller->yposition())*scalefactor);
					}
				}
				void ScaleImage(bool b) {
					_ScaleImage = b;
				}
				virtual int handle(int e) override {
					switch (e) {
					
					case FL_PUSH:
						_ImageControlInfo._MouseCallback(e, Fl::event_button(), Input::Mouse::Press::DOWN, Fl::event_x() + _ImageControlInfo._Scroller->xposition(), Fl::event_y() + _ImageControlInfo._Scroller->yposition());
						return 1;
					case FL_RELEASE:
						_ImageControlInfo._MouseCallback(e, Fl::event_button(), Input::Mouse::Press::UP, Fl::event_x() + _ImageControlInfo._Scroller->xposition(), Fl::event_y() + _ImageControlInfo._Scroller->yposition());
						break;	
					case FL_ENTER:
						return 1;
					case FL_DRAG:
					case FL_MOUSEWHEEL:
					case FL_MOVE:
						_ImageControlInfo._MouseCallback(e, Input::Mouse::Events::NO_EVENTDATA, Input::Mouse::Press::NO_PRESS_DATA, Fl::event_x() + _ImageControlInfo._Scroller->xposition(), Fl::event_y() + _ImageControlInfo._Scroller->yposition());
						break;
					case FL_KEYDOWN:
						_ImageControlInfo._KeyCallback(e, Input::Keyboard::Press::DOWN);
						return 1;
					case FL_KEYUP:
						_ImageControlInfo._KeyCallback(e, Input::Keyboard::Press::UP);
						return 1;
					case FL_FOCUS:
				
						return 1;
					default:
						break;
					};
					return Fl_Box::handle(e);
				}

			};
		}
	}
}


SL::Remote_Access_Library::UI::ImageControl::ImageControl(int X, int Y, int W, int H, const char * title, ImageControlInfo&& info) {
	_ImageControlImpl = new ImageControlImpl(X, Y, W, H, title, std::forward<ImageControlInfo>(info));
}

SL::Remote_Access_Library::UI::ImageControl::~ImageControl() {

}
float SL::Remote_Access_Library::UI::ImageControl::GetScaleFactor() const {

	return _ImageControlImpl->GetScaleFactor();
}

void SL::Remote_Access_Library::UI::ImageControl::ScaleImage(bool b)
{
	_ImageControlImpl->ScaleImage(b);
}
void SL::Remote_Access_Library::UI::ImageControl::SetMousePosition(const Utilities::Point& m)
{
	_ImageControlImpl->SetMousePosition(m);
}
void SL::Remote_Access_Library::UI::ImageControl::SetMouseImage(const std::shared_ptr<Utilities::Image>& m) {

	_ImageControlImpl->SetMouseImage(m);
}

void SL::Remote_Access_Library::UI::ImageControl::SetImage(const std::shared_ptr<Utilities::Image>& m) {
	_ImageControlImpl->SetImage(m);
}

void SL::Remote_Access_Library::UI::ImageControl::SetImageDif(Utilities::Point pos, const std::shared_ptr<Utilities::Image>& img) {
	_ImageControlImpl->SetImageDif(pos, img);
}