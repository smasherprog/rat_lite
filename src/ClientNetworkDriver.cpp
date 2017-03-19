#include "ClientNetworkDriver.h"
#include "turbojpeg.h"
#include "IClientDriver.h"
#include "WebSocket.h"
#include "Shapes.h"
#include "Image.h"
#include "Input.h"
#include "Logging.h"
#include "Configs.h"
#include "IWebSocket.h"
#include "internal/WebSocket.h"
#include "SCCommon.h"
#include "uWS.h"
#include <assert.h>


namespace SL {
	namespace RAT {

		class ClientNetworkDriverImpl {

			IClientDriver* IClientDriver_;
			std::shared_ptr<Client_Config> Config_;
			uWS::Hub h;
			Point LastMousePosition_;
			std::shared_ptr<IWebSocket> Socket_;
			std::thread Runner;
			std::mutex outputbufferLock;
			std::vector<char> outputbuffer;

			void MouseImage(const IWebSocket& socket, const char* data, size_t len) {
				assert(len >= sizeof(Rect));
				Image img(*reinterpret_cast<const Rect*>(data), data + sizeof(Rect), len - sizeof(Rect));
				assert(len >= sizeof(Rect) + (img.Rect.Width * img.Rect.Height * PixelStride));
				IClientDriver_->onReceive_MouseImage(img);
			}

			void MousePos(const IWebSocket& socket, const char* data, size_t len) {
				assert(len == sizeof(Point));
				IClientDriver_->onReceive_MousePos(reinterpret_cast<const Point*>(data));
			}
			void MonitorInfo(const IWebSocket& socket, const char* data, size_t len) {
				auto num = len / sizeof(Screen_Capture::Monitor);
				assert(num * sizeof(Screen_Capture::Monitor) == len);
				IClientDriver_->onReceive_Monitors((Screen_Capture::Monitor*)data, num);
			}
			void ScreenImage(const IWebSocket& socket, const char* data, size_t len) {
				assert(len >= sizeof(Rect));

				auto jpegDecompressor = tjInitDecompress();

				int jpegSubsamp(0), outwidth(0), outheight(0);

				auto src = (unsigned char*)data;
				auto monitor_id = 0;
				memcpy(&monitor_id, src, sizeof(monitor_id));
				src += sizeof(monitor_id);
				Rect rect;
				memcpy(&rect, src, sizeof(rect));
				src += sizeof(rect);

				if (tjDecompressHeader2(jpegDecompressor, src, static_cast<unsigned long>(len - sizeof(Rect)), &outwidth, &outheight, &jpegSubsamp) == -1) {
					SL_RAT_LOG(Logging_Levels::ERROR_log_level, tjGetErrorStr());
				}
				std::lock_guard<std::mutex> lock(outputbufferLock);
				outputbuffer.reserve(outwidth* outheight * PixelStride);

				if (tjDecompress2(jpegDecompressor, src, static_cast<unsigned long>(len - sizeof(Rect)), (unsigned char*)outputbuffer.data(), outwidth, 0, outheight, TJPF_RGBX, TJFLAG_FASTDCT | TJFLAG_NOREALLOC) == -1) {
					SL_RAT_LOG(Logging_Levels::ERROR_log_level, tjGetErrorStr());
				}
				Image img(rect, outputbuffer.data(), outwidth* outheight * PixelStride);
				assert(outwidth == img.Rect.Width && outheight == img.Rect.Height);

				IClientDriver_->onReceive_ImageDif(img, monitor_id);

			}


		public:
			ClientNetworkDriverImpl(IClientDriver* r) :
				IClientDriver_(r) {

			}
			void Connect(std::shared_ptr<Client_Config> config, const char* dst_host) {
				Config_ = config;
				auto hc = std::string("ws://") + std::string(dst_host) + std::string(":") + std::to_string(config->WebSocketTLSLPort);
				h.connect(hc, nullptr);
				h.getDefaultGroup<uWS::CLIENT>().setUserData(new std::mutex);

				h.onConnection([&](uWS::WebSocket<uWS::CLIENT> ws, uWS::HttpRequest req) {
					SL_RAT_LOG(Logging_Levels::INFO_log_level, "onConnection ");
					ws.setUserData(new SocketStats());
					IClientDriver_->onConnection(std::make_shared<WebSocket<uWS::WebSocket<uWS::CLIENT>>>(ws, (std::mutex*)h.getDefaultGroup<uWS::CLIENT>().getUserData()));
				});
				h.onDisconnection([&](uWS::WebSocket<uWS::CLIENT> ws, int code, char *message, size_t length) {
					SL_RAT_LOG(Logging_Levels::INFO_log_level, "onDisconnection ");
					WebSocket<uWS::WebSocket<uWS::CLIENT>> sock(ws, (std::mutex*)h.getDefaultGroup<uWS::CLIENT>().getUserData());
					IClientDriver_->onDisconnection(sock, code, message, length);
					delete (SocketStats*)ws.getUserData();
				});
				h.onMessage([&](uWS::WebSocket<uWS::CLIENT> ws, char *message, size_t length, uWS::OpCode code) {
					auto s = (SocketStats*)ws.getUserData();
					s->TotalBytesReceived += length;
					s->TotalPacketReceived += 1;


					auto p = *reinterpret_cast<const PACKET_TYPES*>(message);
					WebSocket<uWS::WebSocket<uWS::CLIENT>> sock(ws, (std::mutex*)h.getDefaultGroup<uWS::CLIENT>().getUserData());
					//SL_RAT_LOG(Logging_Levels::INFO_log_level, "onMessage "<<(unsigned int)p);
					switch (p) {
					case PACKET_TYPES::MONITORINFO:
						MonitorInfo(sock, message + sizeof(p), length - sizeof(p));
						break;
					case PACKET_TYPES::SCREENIMAGEDIF:
						ScreenImage(sock, message + sizeof(p), length - sizeof(p));
						break;
					case PACKET_TYPES::MOUSEIMAGE:
						MouseImage(sock, message + sizeof(p), length - sizeof(p));
						break;
					case PACKET_TYPES::MOUSEPOS:
						MousePos(sock, message + sizeof(p), length - sizeof(p));
						break;
					case PACKET_TYPES::CLIPBOARDTEXTEVENT:
						IClientDriver_->onReceive_ClipboardText(message + sizeof(p), length - sizeof(p));
						break;
					default:
						IClientDriver_->onMessage(sock, message, length);//pass up the chain
						break;
					}
					
				});
				
				Runner = std::thread([&]() {
					h.run();
					SL_RAT_LOG(Logging_Levels::INFO_log_level, "Stopping ClientNetworkDriver Thread");
					delete (std::mutex*)h.getDefaultGroup<uWS::CLIENT>().getUserData();
				});
			}

			virtual ~ClientNetworkDriverImpl() {
				if (Runner.joinable()) {
					Runner.join();
				}

			}


			void SendMouse(const MouseEvent& m) {
				if (!Socket_) {
					SL_RAT_LOG(Logging_Levels::INFO_log_level, "SendMouse called on a socket that is not open yet");
					return;
				}
				if (Socket_->is_loopback()) return;//dont send mouse info to ourselfs as this will cause a loop
				//do checks to prevent sending redundant mouse information about its position
				if (m.EventData == NO_EVENTDATA && LastMousePosition_ == m.Pos && m.PressData == NO_PRESS_DATA && m.ScrollDelta == 0) {
					return;//already did this event
				}
				LastMousePosition_ = m.Pos;
				auto ptype = PACKET_TYPES::MOUSEEVENT;
				const auto size = sizeof(ptype) + sizeof(m);
				char ptr[size];
				*reinterpret_cast<PACKET_TYPES*>(ptr) = ptype;
				memcpy(ptr + sizeof(ptype), &m, sizeof(m));
				Socket_->send(ptr, size);
			}
			void SendKey(const KeyEvent & m) {
				if (!Socket_) {
					SL_RAT_LOG(Logging_Levels::INFO_log_level, "SendKey called on a socket that is not open yet");
					return;
				}
				auto ptype = PACKET_TYPES::KEYEVENT;
				const auto size = sizeof(ptype) + sizeof(m);
				char ptr[size];
				*reinterpret_cast<PACKET_TYPES*>(ptr) = ptype;
				memcpy(ptr + sizeof(ptype), &m, sizeof(m));
				Socket_->send(ptr, size);

			}
			void SendClipboardText(const char* data, unsigned int len) {
				if (!Socket_) {
					SL_RAT_LOG(Logging_Levels::INFO_log_level, "SendClipboardText called on a socket that is not open yet");
					return;
				}
				if (Socket_->is_loopback()) return;//dont send clipboard info to ourselfs as it will cause a loop

				auto ptype = PACKET_TYPES::CLIPBOARDTEXTEVENT;
				auto size = sizeof(ptype) + len;
				auto ptr(std::make_unique<char[]>(size));
				*reinterpret_cast<PACKET_TYPES*>(ptr.get()) = ptype;
				memcpy(ptr.get() + sizeof(ptype), data, len);
				Socket_->send(ptr.get(), size);

			}
			std::shared_ptr<IWebSocket> getSocket()const {
				return Socket_;
			}
		};

	}
}

SL::RAT::ClientNetworkDriver::ClientNetworkDriver(IClientDriver * r)
	: ClientNetworkDriverImpl_(new ClientNetworkDriverImpl(r))
{

}

SL::RAT::ClientNetworkDriver::~ClientNetworkDriver()
{
	delete ClientNetworkDriverImpl_;
}

void SL::RAT::ClientNetworkDriver::Connect(std::shared_ptr<Client_Config> config, const char* dst_host)
{
	ClientNetworkDriverImpl_->Connect(config, dst_host);
}


void SL::RAT::ClientNetworkDriver::SendKey(const KeyEvent & m)
{
	ClientNetworkDriverImpl_->SendKey(m);
}

void SL::RAT::ClientNetworkDriver::SendMouse(const MouseEvent& m)
{
	ClientNetworkDriverImpl_->SendMouse(m);
}


void SL::RAT::ClientNetworkDriver::SendClipboardText(const char* data, unsigned int len) {
	return ClientNetworkDriverImpl_->SendClipboardText(data, len);
}