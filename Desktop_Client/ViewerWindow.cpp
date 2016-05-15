#include "../Core/stdafx.h"
#include "ViewerWindow.h"
#include "../Core/Image.h"
#include "../Core/ClientNetworkDriver.h"
#include "../Core/Packet.h"
#include "../Core/ISocket.h"
#include "../Core/IClientDriver.h"
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_RGB_Image.H>
#include <FL/Fl_PNG_Image.H>
#include "../Core/Mouse.h"
#include "../Core/Keyboard.h"
#include <chrono>
#include <mutex>
#include "../Core/Logging.h"
#include <assert.h>

namespace SL {
	namespace Remote_Access_Library {
		namespace UI {
			class MyCanvas : public Fl_Widget {
				std::shared_ptr<Utilities::Image> _OriginalImage;//this is the original image, kept to resize the scaled if needed
				std::shared_ptr<Utilities::Image> _ScaledImage;
				std::mutex _ImageLock;

				Utilities::Point _MousePos;
				std::shared_ptr<Utilities::Image> _MouseImageData;
				std::unique_ptr<Fl_RGB_Image> _MouseImage;

				bool _ScaleImage = false;
#define SCROLLBARSIZE 16

				int ParentSize() const {
					auto pheight = this->parent()->h() - SCROLLBARSIZE;//16 is the scrollbars size
					if (pheight < 0) pheight = 48;//cannot make image smaller than this..
					return pheight;
				}


			public:

				MyCanvas(int X, int Y, int W, int H, const char*L = 0) : Fl_Widget(X, Y, W, H, L) {}
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
						auto pheight = this->parent()->h() - SCROLLBARSIZE;//16 is the scrollbars size
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
						_MousePos.X = static_cast<int>(static_cast<float>(_MousePos.X)*scalefactor);
						_MousePos.Y = static_cast<int>(static_cast<float>(_MousePos.Y)*scalefactor);
					}
				}
				void ScaleImage(bool b) {
					_ScaleImage = b;

				}
			};
			class ViewerWindowImpl : public Fl_Double_Window, public Network::IClientDriver
			{
			public:
				MyCanvas* _MyCanvas;
				//Fl_RGB_Image* _Fl_RGB_Image = nullptr;
				Fl_Scroll* _Fl_Scroll = nullptr;
				std::mutex _ImageLock;
				Network::ClientNetworkDriver _ClientNetworkDriver;


				std::chrono::time_point<std::chrono::steady_clock> _NetworkStatsTimer;
				std::chrono::time_point<std::chrono::steady_clock> _FrameTimer;
				float MaxFPS = 30.0f;
				int FPS = 0;
				int FrameCounter = 0;
				Network::SocketStats LastStats;
				bool _BeingClosed = false;
				bool _CursorHidden = false;
				char _Title[255];
				bool _HasFocus = false;

				static void window_cb(Fl_Widget *widget, void *)
				{
					auto wnd = (ViewerWindowImpl*)widget;
					wnd->Close();
				}
				ViewerWindowImpl(const char* dst_host, const char*  dst_port) :Fl_Double_Window(900, 700, "Remote Host"), _ClientNetworkDriver(this, dst_host, dst_port)
				{

					_FrameTimer = _NetworkStatsTimer = std::chrono::steady_clock::now();
					callback(window_cb);
					_Fl_Scroll = new Fl_Scroll(0, 0, 900, 700);

					_MyCanvas = new MyCanvas(0, 0, 900, 700, nullptr);
					_Fl_Scroll->end();
					end();
					resizable(_MyCanvas);
					show();

				}

				virtual int handle(int e) override {
					switch (e) {

					case FL_PUSH:
						handle_mouse(e, Fl::event_button(), Input::Mouse::Press::DOWN, Fl::event_x(), Fl::event_y());
						break;
					case FL_RELEASE:
						handle_mouse(e, Fl::event_button(), Input::Mouse::Press::UP, Fl::event_x(), Fl::event_y());
						break;
					case FL_DRAG:
					case FL_MOUSEWHEEL:
					case FL_MOVE:
						handle_mouse(e, Fl::event_button(), Input::Mouse::Press::NO_PRESS_DATA, Fl::event_x(), Fl::event_y());
						break;
					case FL_KEYDOWN:
						return handle_key(e, Input::Keyboard::Press::DOWN);
					case FL_KEYUP:
						return handle_key(e, Input::Keyboard::Press::UP);
					case FL_FOCUS:
						_HasFocus = true;
						return 1;
					case FL_UNFOCUS:
						_HasFocus = false;
						return 1;
					default:
						break;
					};
					return Fl_Window::handle(e);
				}
		

				int handle_key(int e, Input::Keyboard::Press press) {
					auto key = Fl::event_key();
					auto text = Fl::event_text();
					auto len = Fl::event_length();
					auto t = *text;
					if (t >= '0' && t < 'Z') key= static_cast<unsigned int>(t);
					if (t >= 'a' && t <= 'z') key= static_cast<unsigned int>(t - ('a' - 'A'));

					SL_RAT_LOG(Utilities::Logging_Levels::INFO_log_level, "key: '" << key << "' text: '" << text << "' len: '" << len << "'");
					

					return 1;
				}
				void handle_mouse(int e, int button, Input::Mouse::Press press, int x, int y) {

					auto scale = _MyCanvas->GetScaleFactor();

					Input::MouseEvent ev;
					ev.Pos = Utilities::Point(static_cast<int>(static_cast<float>(x) / scale), static_cast<int>(static_cast<float>(y) / scale));
					if (e == FL_MOUSEWHEEL) {
						ev.ScrollDelta = Fl::event_dy();
						ev.EventData = Input::Mouse::Events::SCROLL;
					}
					else {
						ev.ScrollDelta = 0;
						switch (button) {
						case FL_LEFT_MOUSE:
							ev.EventData = Input::Mouse::Events::LEFT;
							break;
						case FL_MIDDLE_MOUSE:
							ev.EventData = Input::Mouse::Events::MIDDLE;
							break;
						case FL_RIGHT_MOUSE:
							ev.EventData = Input::Mouse::Events::RIGHT;
							break;
						default:
							ev.EventData = Input::Mouse::Events::NO_EVENTDATA;
							break;
						};
					}
					ev.PressData = press;
					if (!_ClientNetworkDriver.ConnectedToSelf()) {
						_ClientNetworkDriver.SendMouse(ev);//sending input to yourself will lead to an infinite loop...
					}
				}
				virtual ~ViewerWindowImpl() {
					_ClientNetworkDriver.Stop();
				}
				virtual void OnConnect(const std::shared_ptr<Network::ISocket>& socket) override
				{
					UNUSED(socket);
				}

				virtual void OnReceive(const std::shared_ptr<Network::ISocket>& socket, std::shared_ptr<Network::Packet>& packet) override
				{
					UNUSED(socket);
					UNUSED(packet);
				}

				virtual void OnClose(const std::shared_ptr<Network::ISocket>& socket) override
				{
					UNUSED(socket);
					Close();
				}
				void Close() {
					if (!_BeingClosed) {
						Fl::delete_widget(this);
						//this->hide();
					}
					_BeingClosed = true;
				}
				static void awakenredraw(void* data) {
					auto imp = ((ViewerWindowImpl*)data);
					if (imp->_BeingClosed) return;
					if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - imp->_FrameTimer).count() > imp->MaxFPS / 1000.0f) {
						imp->_FrameTimer = std::chrono::steady_clock::now();
						imp->FrameCounter += 1;
						if (!imp->_BeingClosed) imp->redraw();
					}
				}
				void ScaleView(bool b)
				{
					_MyCanvas->ScaleImage(b);
				}
				virtual void OnReceive_Image(const std::shared_ptr<Network::ISocket>& socket, std::shared_ptr<Utilities::Image>& img) override
				{
					UNUSED(socket);
					_MyCanvas->SetImage(img);
					Fl::awake(awakenredraw, this);

				}
				static void awakensettitle(void* data) {
					auto imp = ((ViewerWindowImpl*)data);
					if (imp->_BeingClosed) return;
					imp->label(imp->_Title);
				}
				virtual void OnReceive_ImageDif(const std::shared_ptr<Network::ISocket>& socket, Utilities::Point pos, std::shared_ptr<Utilities::Image>& img) override {
					_MyCanvas->SetImageDif(pos, img);
					Fl::awake(awakenredraw, this);
					if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - _NetworkStatsTimer).count() > 1000) {
						_NetworkStatsTimer = std::chrono::steady_clock::now();
						auto stats = socket->get_SocketStats();
						std::string st = "Client ";
						st += std::to_string((stats.NetworkBytesReceived - LastStats.NetworkBytesReceived) / 1000) + " Kbs Received ";
						st += std::to_string((stats.NetworkBytesSent - LastStats.NetworkBytesSent) / 1000) + " Kbs Sent";
						FPS = FrameCounter;
						FrameCounter = 0;
						st += " Fps: " + std::to_string(FPS);
						LastStats = stats;
						if (st.size() > sizeof(_Title) - 1) st = st.substr(0, sizeof(_Title) - 1);
						memcpy(_Title, st.c_str(), st.size() + 1);
						Fl::awake(awakensettitle, this);
					}

				}
				static void awakenhidecursor(void* data) {
					auto imp = ((ViewerWindowImpl*)data);
					imp->cursor(Fl_Cursor::FL_CURSOR_NONE);
				}
				virtual void OnReceive_MouseImage(const std::shared_ptr<Network::ISocket>& socket, std::shared_ptr<Utilities::Image>& img)override {
					if (_HasFocus && !_CursorHidden) {
						Fl::awake(awakenhidecursor, this);
						_CursorHidden = true;
					}
					_MyCanvas->SetMouseImage(img);
					Fl::awake(awakenredraw, this);
				}

				virtual void OnReceive_MousePos(const std::shared_ptr<Network::ISocket>& socket, Utilities::Point* pos)override {
					_MyCanvas->SetMousePosition(*pos);
				}
			};

		}
	}
}


SL::Remote_Access_Library::UI::ViewerWindow::ViewerWindow(const char * dst_host, const char * dst_port) {
	_ViewerWindowImpl = new ViewerWindowImpl(dst_host, dst_port);
	_ViewerWindowImpl->_ClientNetworkDriver.Start();
}

SL::Remote_Access_Library::UI::ViewerWindow::~ViewerWindow() {
}

void SL::Remote_Access_Library::UI::ViewerWindow::ScaleView(bool b)
{
	_ViewerWindowImpl->ScaleView(b);
}
