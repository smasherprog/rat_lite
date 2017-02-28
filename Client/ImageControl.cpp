#include "ImageControl.h"
#include "Logging.h"
#include "Shapes.h"
#include "Input.h"
#include "Image.h"
#include "SCCommon.h"

#define NOMINMAX
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
#include <shared_mutex>
#include <algorithm>
#include <assert.h>

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

		std::unique_ptr<char[]> Resize(const char* inimg, int inheight, int inwidth, int dstheight, int dstwidth)
		{//linear scaling
			auto m_width = dstwidth;
			auto m_height = dstheight;
			auto outimg = std::make_unique<char[]>(m_width*m_height*PixelStride);
			auto temp = (int*)outimg.get();
			auto pixels = (int*)inimg;
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
		std::unique_ptr<char[]> Resize(const char* inimg, int* inheight, int* inwidth, float scale)
		{
			auto height = *inheight;
			auto width = *inwidth;
			*inheight = static_cast<int>(ceil(static_cast<float>(*inheight) * scale));
			*inwidth = static_cast<int>(ceil(static_cast<float>(*inwidth) * scale));
			return Resize(inimg, height, width, *inheight, *inwidth);
		}
		void Copy(const Image& src, Rect src_rect, Image & dst, Rect dst_rect)
		{
			auto dst_start = (char*)dst.Data;
			auto src_start = (char*)src.Data;
			auto dst_rowstride = dst.Rect.Width * PixelStride;
			auto src_rowstride = src.Rect.Width * PixelStride;
			for (auto row = dst_rect.top(), src_row = 0; row < dst_rect.bottom(); row++, src_row++) {
				memcpy(dst_start + (dst_rowstride* row) + (dst_rect.left()*PixelStride), src_start + (src_rowstride*src_row) + (src_rect.left()*PixelStride), src_rowstride);
			}
		}
		struct MonitorData {
			Screen_Capture::Monitor Monitor;
			Image Original, Scaled;
			std::shared_ptr<char> OriginalBacking, ScaledBacking;
			std::shared_ptr<std::shared_mutex> Lock;
		};
		struct MouseData {
			Image MouseImage;
			std::unique_ptr<char[]> MouseImageBacking;
			Point Pos;
			std::unique_ptr<Fl_RGB_Image> FlMouseImage;
		};

		class ScreenImageImpl {
		public:

			float ScaleFactor_;
			ScreenImageCallbacks ScreenImageCallbacks_;

			mutable std::shared_mutex MonitorsLock;
			std::vector<MonitorData> Monitors;

			mutable std::shared_mutex MouseLock;
			MouseData MouseData_;


			ScreenImageImpl(ScreenImageCallbacks&& info) : ScaleFactor_(1.0f), ScreenImageCallbacks_(info) {}

			bool is_ImageScaled() const {
				return !(ScaleFactor_ >= .999f && ScaleFactor_ <= 1.001f);
			}

			int get_GreatestHeight() const {
				std::shared_lock<std::shared_mutex> l(MonitorsLock);
				int tallest = 0;
				for (auto& a : Monitors) {
					tallest = std::max(tallest, a.Monitor.Height);
				}
			}
			void setScaleFactor_(float factor) {
				ScaleFactor_ = factor;
			}
			float getScaleFactor_() const {
				return ScaleFactor_;
			}
			void set_ScreenImage(const Image& img, int monitor_id) {
				if (monitor_id != 0) return;//skip other monitors for now
				auto size = img.Rect.Width*img.Rect.Height*PixelStride;
				auto originalptr = std::shared_ptr<char>(new char[size], [](char* p) { delete[] p; });
				Image original(img.Rect, originalptr.get(), size);
				memcpy(originalptr.get(), img.Data, size);

				auto scaledptr = std::shared_ptr<char>(new char[size], [](char* p) { delete[] p; });
				Image scaled(img.Rect, scaledptr.get(), size);
				memcpy(scaledptr.get(), img.Data, size);

				//reader lock to the container
				std::shared_lock<std::shared_mutex> l(MonitorsLock);
				auto found = std::find_if(begin(Monitors), end(Monitors), [&](const MonitorData& m) { return monitor_id == m.Monitor.Id;  });
				//the monitors should always preceed the image, so this should never trigger
				assert(found != end(Monitors));
				{
					//writer lock to the element
					std::unique_lock<std::shared_mutex> ml(*found->Lock);
					found->Original = original;
					found->OriginalBacking = originalptr;
					found->Scaled = scaled;
					found->ScaledBacking = scaledptr;
				}
			}
			void Draw(int x, int y) {
				//reader lock to the container
				{
					std::shared_lock<std::shared_mutex> l(MonitorsLock);
					for (auto& a : Monitors) {
						std::shared_lock<std::shared_mutex> ml(*a.Lock);
						fl_draw_image((uchar*)a.Scaled.Data, x, y, a.Scaled.Rect.Width, a.Scaled.Rect.Height, 4);
						x += a.Scaled.Rect.Width;
					}
				}

				std::shared_lock<std::shared_mutex> l(MouseLock);
				if (MouseData_.MouseImageBacking)
					MouseData_.FlMouseImage->draw(MouseData_.Pos.X, MouseData_.Pos.Y);

			}
			void set_ImageDifference(const Image& img, int monitor_id) {
				return;

				//if (!(OriginalImage_.Data && ScaledImage_.Data)) return;// both images should exist, if not then get out!


				//if (is_ImageScaled()) {//rescale the incoming image image
				//	auto scaledrect = img.Rect;

				//	auto scaledimgbacking = Resize(img.Data, &scaledrect.Height, &scaledrect.Width, ScaleFactor_);
				//	Image scaledimg(scaledrect, scaledimgbacking.get(), scaledrect.Height * scaledrect.Width*PixelStride);

				//	auto x = static_cast<int>(floor(static_cast<float>(img.Rect.left())*ScaleFactor_));
				//	auto y = static_cast<int>(floor(static_cast<float>(img.Rect.top())*ScaleFactor_));

				//	auto dst_rect = Rect(Point(x, y), scaledrect.Height, scaledrect.Width);

				//	std::lock_guard<std::mutex> lock(ImageLock_);
				//	Copy(img, img.Rect, OriginalImage_, img.Rect);//keep original in sync
				//	Copy(scaledimg, scaledimg.Rect, ScaledImage_, dst_rect);//copy scaled down 

				//}
				//else {
				//	std::lock_guard<std::mutex> lock(ImageLock_);
				//	Copy(img, img.Rect, OriginalImage_, img.Rect);//keep original in sync
				//	Copy(img, img.Rect, ScaledImage_, img.Rect);//copy scaled down 
				//}


			}
			void setMouseImage_(const Image& img) {

				auto s = img.Rect.Width*img.Rect.Height*PixelStride;
				auto mouseimg = std::make_unique<char[]>(s);
				Image mouseimgdata(img.Rect, mouseimg.get(), s);
				memcpy(mouseimg.get(), img.Data, s);
				std::unique_lock<std::shared_mutex> l(MouseLock);
				MouseData_.MouseImageBacking = std::move(mouseimg);
				MouseData_.MouseImage = mouseimgdata;
				MouseData_.FlMouseImage = std::make_unique<Fl_RGB_Image>((uchar*)MouseData_.MouseImageBacking.get(), img.Rect.Width, img.Rect.Height, PixelStride);
			}
			void set_MousePosition(const Point* pos) {

				MouseData_.Pos = *pos;
				if (is_ImageScaled()) {//need to scale the mouse pos as well
					MouseData_.Pos.X = static_cast<int>(static_cast<float>(MouseData_.Pos.X - ScreenImageCallbacks_.get_Left())*ScaleFactor_);
					MouseData_.Pos.Y = static_cast<int>(static_cast<float>(MouseData_.Pos.Y - ScreenImageCallbacks_.get_Top())*ScaleFactor_);
				}
			}
			bool Update(int& width, int& height) {
				return false;
			//	if (!ScaledImage_.Data) return false;
			//	auto ret = false;
			//	//make sure the image is scaled properly
			//	if (is_ImageScaled()) {
			//		auto psize = ScreenImageInfo_.get_Height();
			//		if (psize != ScaledImage_.Rect.Height) {//rescale the image
			//			std::lock_guard<std::mutex> lock(ImageLock_);
			//			ScaledImage_.Rect = Rect(Point(0, 0), psize, static_cast<int>(ScaleFactor_* OriginalImage_.Rect.Height));
			//			ScaledImageBacking_ = Resize(OriginalImage_.Data, OriginalImage_.Rect.Height, OriginalImage_.Rect.Width, ScaledImage_.Rect.Height, ScaledImage_.Rect.Width);
			//			ScaledImage_.Data = ScaledImageBacking_.get();
			//			ret = true;
			//		}
			//	}
			//	else {//NO SCALING!!
			//		if (ScaledImage_.Rect.Width != OriginalImage_.Rect.Width || ScaledImage_.Rect.Height != OriginalImage_.Rect.Width) {
			//			std::lock_guard<std::mutex> lock(ImageLock_);
			//			auto size = OriginalImage_.Rect.Height*OriginalImage_.Rect.Width*PixelStride;

			//			ScaledImage_.Rect = OriginalImage_.Rect;
			//			ScaledImageBacking_ = std::make_unique<char[]>(size);
			//			ScaledImage_.Data = ScaledImageBacking_.get();
			//			memcpy((void*)ScaledImage_.Data, OriginalImage_.Data, size);
			//			ret = true;
			//		}
			//	}
			//	width = ScaledImage_.Rect.Width;
			//	height = ScaledImage_.Rect.Height;
			//	return ret;//no changes
			}
			void set_Monitors(const Screen_Capture::Monitor * monitors, int num_of_monitors) {
				std::vector<MonitorData> mons;
				mons.resize(num_of_monitors);
				for (auto i = 0; i < num_of_monitors; i++) {
					mons[i].Monitor = monitors[i];
					auto found = std::find_if(begin(Monitors), end(Monitors), [&](const MonitorData& m) { return monitors[i].Id == m.Monitor.Id;  });
					if (found != end(Monitors)) {
						mons[i].Original = found->Original;
						mons[i].OriginalBacking = found->OriginalBacking;
						mons[i].Scaled = found->Scaled;
						mons[i].ScaledBacking = found->ScaledBacking;
						mons[i].Lock = found->Lock;
					}
					else {
						mons[i].Lock = std::make_shared<std::shared_mutex>();
					}
				}
				std::unique_lock<std::shared_mutex> l(MonitorsLock);
				Monitors = mons;
			}
		};


		class ImageControlImpl : public Fl_Box {

		public:

			ScreenImageImpl ScreenImageDriver_;
			ScreenImageCallbacks ScreenImageCallbacks_;
			bool _DNDIncoming = false;

			ImageControlImpl(int X, int Y, int W, int H, const char * title, ScreenImageCallbacks&& info) :
				Fl_Box(X, Y, W, H, title),
				ScreenImageDriver_(std::forward<ScreenImageCallbacks>(info)),
				ScreenImageCallbacks_(info) {

			}
			virtual ~ImageControlImpl() {

			}
			virtual void draw() override {
				int height(0), width(0);
				if (ScreenImageDriver_.Update(width, height)) {
					this->size(width, height);
				}
				ScreenImageDriver_.Draw(x(), y());
			}
			void OnResize(int W, int H, int SS) {
				//if (_ScreenImageDriver.is_ImageScaled()) {
				//	auto pheight = h() - SS;//16 is the scrollbars size
				//	if (pheight < 0) pheight = 48;//cannot make image smaller than this..
				//	auto dims = _ScreenImageDriver.get_UnscaledImageSize();
				//	_ScreenImageDriver.setScaleFactor_(static_cast<float>(pheight) / static_cast<float>(dims.Y));
				//}

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
				if (ScreenImageDriver_.is_ImageScaled()) {
					x = static_cast<int>(static_cast<float>(x) / ScreenImageDriver_.getScaleFactor_());
					y = static_cast<int>(static_cast<float>(y) / ScreenImageDriver_.getScaleFactor_());
				}
				ScreenImageCallbacks_.OnMouse(e, button, press, x, y);
			}
			virtual int handle(int e) override {

				switch (e) {
				case FL_PUSH:
					handlemouse(e, Fl::event_button(), Press::DOWN, Fl::event_x() + ScreenImageCallbacks_.get_Left(), Fl::event_y() + ScreenImageCallbacks_.get_Top());
					return 1;
				case FL_RELEASE:
					handlemouse(e, Fl::event_button(), Press::UP, Fl::event_x() + ScreenImageCallbacks_.get_Left(), Fl::event_y() + ScreenImageCallbacks_.get_Top());
					break;
				case FL_ENTER:
					return 1;
				case FL_DRAG:
				case FL_MOUSEWHEEL:
				case FL_MOVE:
					handlemouse(e, Events::NO_EVENTDATA, Press::NO_PRESS_DATA, Fl::event_x() + ScreenImageCallbacks_.get_Left(), Fl::event_y() + ScreenImageCallbacks_.get_Top());
					break;
				case FL_KEYDOWN:
					ScreenImageCallbacks_.OnKey(e, Press::DOWN);
					return 1;
				case FL_KEYUP:
					ScreenImageCallbacks_.OnKey(e, Press::UP);
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
			void set_Monitors(const Screen_Capture::Monitor * monitors, int num_of_monitors)
			{
				ScreenImageDriver_.set_Monitors(monitors, num_of_monitors);
			}
		};
		ImageControl::ImageControl(int X, int Y, int W, int H, const char * title, ScreenImageCallbacks&& info) {
			ImageControlImpl_ = new ImageControlImpl(X, Y, W, H, title, std::forward<ScreenImageCallbacks>(info));
		}

		ImageControl::~ImageControl() {
			delete ImageControlImpl_;
		}

		void ImageControl::OnResize(int W, int H, int SS)
		{
			ImageControlImpl_->OnResize(W, H, SS);
		}

		bool ImageControl::is_ImageScaled() const
		{
			return ImageControlImpl_->ScreenImageDriver_.is_ImageScaled();
		}

		void ImageControl::set_Monitors(const Screen_Capture::Monitor * monitors, int num_of_monitors)
		{
			ImageControlImpl_->set_Monitors(monitors, num_of_monitors);
		}

		void ImageControl::set_ScreenImage(const Image& img, int monitor_id)
		{
			ImageControlImpl_->size(img.Rect.Width, img.Rect.Height);
			ImageControlImpl_->ScreenImageDriver_.set_ScreenImage(img, monitor_id);
		}

		void ImageControl::set_ImageDifference(const Image& img, int monitor_id)
		{
			ImageControlImpl_->ScreenImageDriver_.set_ImageDifference(img, monitor_id);
		}

		void ImageControl::set_MouseImage(const Image& img)
		{
			ImageControlImpl_->ScreenImageDriver_.setMouseImage_(img);
		}

		void ImageControl::set_MousePosition(const Point* pos)
		{
			ImageControlImpl_->ScreenImageDriver_.set_MousePosition(pos);
		}

	}
}

