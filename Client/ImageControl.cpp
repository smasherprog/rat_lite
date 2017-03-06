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


		class ImageControlImpl : public Fl_Box {
			float ScaleFactor_;
			mutable std::shared_mutex MonitorsLock;
			std::vector<MonitorData> Monitors;

			mutable std::shared_mutex MouseLock;
			MouseData MouseData_;

		public:
			std::function<void(int, int, Press, int, int)> OnMouse;
			std::function<void(int, Press)> OnKey;
			std::function<void(std::vector<std::string>&)> OnDragNDrop;


			bool Scaling_;
			bool DNDIncoming_;

			ImageControlImpl(int X, int Y, int W, int H, const char * title) :
				Fl_Box(X, Y, W, H, title), ScaleFactor_(1.0f), DNDIncoming_(false), Scaling_(false){

			}
			virtual ~ImageControlImpl() {

			}
			virtual void draw() override {
				int height(0), width(0);

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


				//if (ScreenImageDriver_.Update(width, height)) {
				//	this->size(width, height);
				//}
				//reader lock to the container
				{
					auto xp = x();
					std::shared_lock<std::shared_mutex> l(MonitorsLock);
					for (auto& a : Monitors) {
						std::shared_lock<std::shared_mutex> ml(*a.Lock);
						fl_draw_image((uchar*)a.Scaled.Data, xp, y(), a.Scaled.Rect.Width, a.Scaled.Rect.Height, 4);
						xp += a.Scaled.Rect.Width;
					}
				}

				std::shared_lock<std::shared_mutex> l(MouseLock);
				if (MouseData_.MouseImageBacking)
					MouseData_.FlMouseImage->draw(MouseData_.Pos.X, MouseData_.Pos.Y);
			}

			int get_GreatestHeight() const {
				std::shared_lock<std::shared_mutex> l(MonitorsLock);
				int tallest = 0;
				for (auto& a : Monitors) {
					tallest = std::max(tallest, a.Monitor.Height);
				}
			}
		
			void set_ImageDifference(const Image& img, int monitor_id) {
				//reader lock to the container
				std::shared_lock<std::shared_mutex> l(MonitorsLock);
				auto found = std::find_if(begin(Monitors), end(Monitors), [&](const MonitorData& m) { return monitor_id == m.Monitor.Id;  });
				assert(found != end(Monitors));
				{
					//writer lock to the element
					Rect srcrect(Point(0, 0), img.Rect.Height, img.Rect.Width);
					std::unique_lock<std::shared_mutex> ml(*found->Lock);
					Copy(img, srcrect, found->Original, img.Rect);//keep original in sync
					Copy(img, srcrect, found->Scaled, img.Rect);//copy scaled down 
				}
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
				if (Scaling_) {//need to scale the mouse pos as well
					MouseData_.Pos.X = static_cast<int>(static_cast<float>(MouseData_.Pos.X)*ScaleFactor_);
					MouseData_.Pos.Y = static_cast<int>(static_cast<float>(MouseData_.Pos.Y)*ScaleFactor_);
				}
			}
			void OnResize(int W, int H, int SS) {
				if (Scaling_) {
					auto pheight = h() - SS;//16 is the scrollbars size
					if (pheight < 0) pheight = 48;//cannot make image smaller than this..
					std::shared_lock<std::shared_mutex> l(MonitorsLock);

					_ScreenImageDriver.setScaleFactor_(static_cast<float>(pheight) / static_cast<float>(dims.Y));
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
				if (Scaling_) {
					x = static_cast<int>(static_cast<float>(x) / ScaleFactor_);
					y = static_cast<int>(static_cast<float>(y) / ScaleFactor_);
				}

				OnMouse(e, button, press, x, y);
			}
			virtual int handle(int e) override {

				switch (e) {
				case FL_PUSH:
					handlemouse(e, Fl::event_button(), Press::DOWN, Fl::event_x(), Fl::event_y());
					return 1;
				case FL_RELEASE:
					handlemouse(e, Fl::event_button(), Press::UP, Fl::event_x(), Fl::event_y());
					break;
				case FL_ENTER:
					return 1;
				case FL_DRAG:
				case FL_MOUSEWHEEL:
				case FL_MOVE:
					handlemouse(e, Events::NO_EVENTDATA, Press::NO_PRESS_DATA, Fl::event_x(), Fl::event_y());
					break;
				case FL_KEYDOWN:
					OnKey(e, Press::DOWN);
					return 1;
				case FL_KEYUP:
					OnKey(e, Press::UP);
					return 1;
				case FL_FOCUS:
					return 1;
				case FL_PASTE:
					if (DNDIncoming_) {
						DNDIncoming_ = false;
						return pastedstuff();
					}
				case FL_DND_ENTER:
				case FL_DND_LEAVE:
				case FL_DND_DRAG:
					return 1;
				case FL_DND_RELEASE:
					DNDIncoming_ = true;
					return 1;

				default:
					break;
				};
				return Fl_Box::handle(e);
			}
			void set_Monitors(const Screen_Capture::Monitor * monitors, int num_of_monitors)
			{
				SL::RAT::Point s;
		
				std::vector<MonitorData> mons;
				mons.resize(num_of_monitors);
				for (auto i = 0; i < num_of_monitors; i++) {
					auto curx = 0, cury = 0;
					if (Scaling_) {
						cury = static_cast<int>(static_cast<float>(monitors[i].Height) / ScaleFactor_);
						curx = static_cast<int>(static_cast<float>(monitors[i].Width) / ScaleFactor_);
						s.Y = std::max(s.Y, cury);
						s.X += curx;
					}
					else {
						cury = monitors[i].Height;
						curx = monitors[i].Width;
						s.Y = std::max(s.Y, cury);
						s.X += curx;
					}

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

						//allocate the images needed
						Rect r(Point(0, 0), cury, curx);
						auto size = r.Height* r.Width*PixelStride;

						auto originalptr = std::shared_ptr<char>(new char[size], [](char* p) { delete[] p; });
						Image original(r, originalptr.get(), size);

						auto scaledptr = std::shared_ptr<char>(new char[size], [](char* p) { delete[] p; });
						Image scaled(r, scaledptr.get(), size);

						mons[i].Original = original;
						mons[i].OriginalBacking = originalptr;
						mons[i].Scaled = scaled;
						mons[i].ScaledBacking = scaledptr;

						mons[i].Lock = std::make_shared<std::shared_mutex>();
					}
				}
				this->size(s.X, s.Y);
				std::unique_lock<std::shared_mutex> l(MonitorsLock);
				Monitors = mons;

			}
		};
		ImageControl::ImageControl(int X, int Y, int W, int H, const char * title) {
			ImageControlImpl_ = new ImageControlImpl(X, Y, W, H, title);
		}

		ImageControl::~ImageControl() {
			delete ImageControlImpl_;
		}

		void ImageControl::OnResize(int W, int H, int SS)
		{
			ImageControlImpl_->OnResize(W, H, SS);
		}

		void ImageControl::Scaled(bool scaled)
		{
			ImageControlImpl_->Scaling_ = scaled;
		}

		bool ImageControl::Scaled() const
		{
			return ImageControlImpl_->Scaling_;
		}

		void ImageControl::setMonitors(const Screen_Capture::Monitor * monitors, int num_of_monitors)
		{
			ImageControlImpl_->set_Monitors(monitors, num_of_monitors);
		}


		void ImageControl::setImageDifference(const Image& img, int monitor_id)
		{
			ImageControlImpl_->set_ImageDifference(img, monitor_id);
		}

		void ImageControl::setMouseImage(const Image& img)
		{
			ImageControlImpl_->setMouseImage_(img);
		}

		void ImageControl::setMousePosition(const Point* pos)
		{
			ImageControlImpl_->set_MousePosition(pos);
		}

		void ImageControl::onKey(const std::function<void(int, Press)>& func)
		{
			ImageControlImpl_->OnKey = func;
		}

		void ImageControl::onMouse(const std::function<void(int, int, Press, int, int)>& func)
		{
			ImageControlImpl_->OnMouse = func;
		}

		void ImageControl::onDragNDrop(const std::function<void(std::vector<std::string>&)>& func)
		{
			ImageControlImpl_->OnDragNDrop = func;
		}

	}
}

