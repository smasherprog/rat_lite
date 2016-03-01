#include "stdafx.h"
#include "Screen.h"
#include "Image.h"
#include "ThreadPool.h"
#include <FL/Fl.H>
#include <assert.h>
#define DESKTOPCAPTURERATE 100
#define DESKTOPWAITCAPTURERATE 20

namespace SL {
	namespace Remote_Access_Library {

#if _WIN32

		//RAII Objects to ensure proper destruction
#define RAIIHDC(handle) std::unique_ptr<std::remove_pointer<HDC>::type, decltype(&::DeleteDC)>(handle, &::DeleteDC)
#define RAIIHBITMAP(handle) std::unique_ptr<std::remove_pointer<HBITMAP>::type, decltype(&::DeleteObject)>(handle, &::DeleteObject)
#define RAIIHANDLE(handle) std::unique_ptr<std::remove_pointer<HANDLE>::type, decltype(&::CloseHandle)>(handle, &::CloseHandle)

		std::shared_ptr<Utilities::Image_Wrapper> CaptureDesktopImage()
		{
			int left(0), top(0), width(0), height(0);
			for (auto i = 0; i < Fl::screen_count(); i++) {
				auto l(0), t(0), w(0), h(0);
				Fl::screen_xywh(l, t, w, h, i);
				width += w;
				height = std::max(height, h);
			}

			static auto desktopdc(RAIIHDC(CreateDCA("DISPLAY", NULL, NULL, NULL)));
			static auto capturedc(RAIIHDC(CreateCompatibleDC(desktopdc.get())));
			static auto capturebmp(RAIIHBITMAP(CreateCompatibleBitmap(desktopdc.get(), width, height)));

			if (!desktopdc || !capturedc || !capturebmp) return Utilities::Image::CreateWrappedImage(0, 0);

			// Selecting an object into the specified DC 
			auto originalBmp = SelectObject(capturedc.get(), capturebmp.get());

			auto retimg(Utilities::Image::CreateWrappedImage(height, width));

			if (BitBlt(capturedc.get(), 0, 0, width, height, desktopdc.get(), left, top, SRCCOPY | CAPTUREBLT) == FALSE) {
				//if the screen cannot be captured, set everything to 1 and return 
				memset(retimg->WrappedImage.data(), 1, retimg->WrappedImage.size());
				SelectObject(capturedc.get(), originalBmp);
				return retimg;
			}
			BITMAPINFO bmpInfo;
			memset(&bmpInfo, 0, sizeof(BITMAPINFO));

			bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bmpInfo.bmiHeader.biWidth = width;
			bmpInfo.bmiHeader.biHeight = -height;
			bmpInfo.bmiHeader.biPlanes = 1;
			bmpInfo.bmiHeader.biBitCount = 32;//always 32 bits damnit!!!
			bmpInfo.bmiHeader.biCompression = BI_RGB;
			bmpInfo.bmiHeader.biSizeImage = ((width * bmpInfo.bmiHeader.biBitCount + 31) / 32) * 4 * height;

			GetDIBits(desktopdc.get(), capturebmp.get(), 0, (UINT)height, retimg->WrappedImage.data(), (BITMAPINFO *)&bmpInfo, DIB_RGB_COLORS);

			SelectObject(capturedc.get(), originalBmp);
		/*	struct utrgba {
				unsigned char r, g, b, a;
			};
			auto startdata = (utrgba*)retimg->WrappedImage.data();
			for (auto r = 0; r < height; r++) {
				for (auto c = 0; c < width; c++) {
					auto& tmp = startdata[c + r*width];
					auto tmpr = tmp.r;
					tmp.r = tmp.b;
					tmp.b = tmpr;
				}
			}*/
			//Sanity check to ensure the data is correct
			//SaveBMP(bmpInfo.bmiHeader, SL::Screen_Capture::getData(blk), "c:\\users\\scott\\desktop\\one.bmp");
			return retimg;
		}

#elif __APPLE__
#include "TargetConditionals.h"
#if TARGET_IPHONE_SIMULATOR
		// iOS Simulator
#elif TARGET_OS_IPHONE
		// iOS device
#elif TARGET_OS_MAC
		// Other kinds of Mac OS
#else
#   error "Unknown Apple platform"
#endif

#error Applie specific implementation of CaptureDesktopImage has not been written yet. You can help out by writing it!
#elif __linux__



	#include <X11/X.h>
	#include <X11/Xlib.h>
	#include <X11/Xlibint.h>
	#include <X11/Xutil.h>
	#include <sys/shm.h>
	#include <X11/extensions/XShm.h>

		std::shared_ptr<Utilities::Image_Wrapper> CaptureDesktopImage()
		{
			auto display = XOpenDisplay(NULL);
			auto root = DefaultRootWindow(display);
			auto screen = XDefaultScreen(display);
			auto visual = DefaultVisual(display, screen);
			auto depth = DefaultDepth(display, screen);
			  
			XWindowAttributes gwa;
			XGetWindowAttributes(display, root , &gwa);
			auto width = gwa.width;
			auto height = gwa.height;
			   
			XShmSegmentInfo shminfo;
			auto image = XShmCreateImage(display,visual, depth,ZPixmap, NULL, &shminfo, width, height);
			shminfo.shmid = shmget(IPC_PRIVATE, image->bytes_per_line * image->height, IPC_CREAT|0777);
			
			shminfo.readOnly = False;
			shminfo.shmaddr = image->data = (char*)shmat(shminfo.shmid, 0, 0);
		
			XShmAttach(display,&shminfo);

			XShmGetImage(display,root,image,0,0,AllPlanes);

			XShmDetach(display,&shminfo);
   
			auto px= Utilities::Image::CreateWrappedImage(height, width);
			assert(image->bits_per_pixel==32);//this should always be true... Ill write a case where it isnt, but for now it should be
			struct utrgba {
				unsigned char r, g, b, a;
			};
			auto startdata = (utrgba*)px->WrappedImage.data();
			auto src = (utrgba*)shminfo.shmaddr;
			for (auto r = 0; r < height; r++) {
				for (auto c = 0; c < width; c++) {
					auto& tmp = startdata[c + r*width] = src[c + r*width];
					auto tmpr = tmp.r;
					tmp.r = tmp.b;
					tmp.b = tmpr;
				}
			}
			XDestroyImage(image);
			shmdt (shminfo.shmaddr);
			shmctl (shminfo.shmid, IPC_RMID, 0);
			XCloseDisplay(display);
			
/*            auto root_window = gdk_get_default_root_window();
            if(root_window==nullptr) return Utilities::Image::CreateWrappedImage(0, 0);
            int height, width, left, top;
            gdk_window_get_geometry(root_window, &left, &top, &width, &height);
                
            auto screenshot = gdk_pixbuf_get_from_window(root_window, left, top, width, height);
			auto px= Utilities::Image::CreateWrappedImage(height, width, (const char*)gdk_pixbuf_read_pixels(screenshot), gdk_pixbuf_get_n_channels(screenshot));
            g_object_unref(screenshot);*/
            return px;
            
        }
#elif __ANDROID__
#error Andriod specific implementation  of CaptureDesktopImage has not been written yet. You can help out by writing it!
#endif


		class DesktopCache {
			Utilities::ThreadPool _ThreadPool;
			std::shared_ptr<Utilities::Image_Wrapper> _Image;
			std::chrono::time_point<std::chrono::steady_clock> _Timer;
			std::mutex _TimerLock;
		public:
			DesktopCache() : _ThreadPool(1), _Timer(std::chrono::steady_clock::now()) {
				//get an image on startup to seed
				_ThreadPool.Enqueue([this]() {
					_Image = CaptureDesktopImage();
				});
			}

			std::shared_ptr<Utilities::Image> GetImage() {
				{
					std::lock_guard<std::mutex> lock(_TimerLock);
					if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - _Timer).count() > DESKTOPCAPTURERATE && _ThreadPool.TaskCount() == 0) {
						_Timer = std::chrono::steady_clock::now();
						_ThreadPool.Enqueue([this]() {
							_Image.reset();
							auto start = std::chrono::steady_clock::now();
							_Image = CaptureDesktopImage();
							std::cout << "It took " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count() << " milliseconds to CaptureDesktopImage() " << std::endl;
						});
					}
				}
				while (true) {
					auto tmpimage = _Image;
					if (tmpimage) {
						return std::shared_ptr<Utilities::Image>(tmpimage, &tmpimage->WrappedImage);
					}
					else { //wait for an image to become available. This should only happen on the first call to this function
						std::this_thread::sleep_for(std::chrono::milliseconds(DESKTOPWAITCAPTURERATE));
					}
				}
			}
		};
	}
}


const std::shared_ptr<SL::Remote_Access_Library::Utilities::Image> SL::Remote_Access_Library::Capturing::CaptureDesktop() {
	static DesktopCache _DesktopCache;
	return _DesktopCache.GetImage();
}