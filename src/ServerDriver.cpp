#include "ServerDriver.h"
#include "Shapes.h"
#include "IServerDriver.h"
#include "internal/WebSocket.h"
#include "Configs.h"
#include "turbojpeg.h"
#include "Input.h"
#include "Logging.h"
#include "ScreenCapture.h"

#include "SCCommon.h"
#include "internal/ServerHub.h"

namespace SL {
	namespace RAT {

		class ServerDriverImpl {

		public:
			std::shared_ptr<Server_Config> Config_;
			ServerHub ServerHub_;
			ServerDriverImpl(IServerDriver * r, std::shared_ptr<Server_Config> config) : Config_(config), ServerHub_(r, config) {

			}
			virtual ~ServerDriverImpl() {

			}
			void Send(IWebSocket* socket, char* data, size_t len) {
				if (socket) {
					socket->send(data, len);
				}
				else {
					ServerHub_.Broadcast(data, len);
				}
			}

			void SendScreen(IWebSocket* socket, const Screen_Capture::Image & img, const SL::Screen_Capture::Monitor& monitor, PACKET_TYPES p) {

				if (ServerHub_.get_ClientCount() <= 0) return;
				Rect r(Point(img.Bounds.left, img.Bounds.top), Height(img), Width(img));

				auto set = Config_->SendGrayScaleImages ? TJSAMP_GRAY : TJSAMP_420;
				auto maxsize = tjBufSize(Screen_Capture::Width(img), Screen_Capture::Height(img), set) + sizeof(r) + sizeof(p) + sizeof(monitor.Id);

				auto jpegCompressor = tjInitCompress();
				auto  buffer = std::make_unique<char[]>(maxsize);

				auto dst = (unsigned char*)buffer.get();
				memcpy(dst, &p, sizeof(p));
				dst += sizeof(p);
				memcpy(dst, &monitor.Id, sizeof(monitor.Id));
				dst += sizeof(monitor.Id);
				memcpy(dst, &r, sizeof(r));
				dst += sizeof(r);

				auto srcbuffer = std::make_unique<char[]>(RowStride(img)*Height(img));
				Screen_Capture::Extract(img, srcbuffer.get(), RowStride(img)*Height(img));
				auto srcbuf = (unsigned char*)srcbuffer.get();

#if __ANDROID__
				auto colorencoding = TJPF_RGBX;
#else 
				auto colorencoding = TJPF_BGRX;
#endif
				auto outjpegsize = maxsize;

				if (tjCompress2(jpegCompressor, srcbuf, r.Width, 0, r.Height, colorencoding, &dst, &outjpegsize, set, Config_->ImageCompressionSetting, TJFLAG_FASTDCT | TJFLAG_NOREALLOC) == -1) {
					SL_RAT_LOG(Logging_Levels::ERROR_log_level, tjGetErrorStr());
				}
				//	std::cout << "Sending " << r << std::endl;
				auto finalsize = sizeof(p) + sizeof(r) + sizeof(monitor.Id) + outjpegsize;//adjust the correct size
				tjDestroy(jpegCompressor);
				Send(socket, buffer.get(), finalsize);
			}
			void SendMouse(IWebSocket* socket, const Screen_Capture::Image & img) {

				if (ServerHub_.get_ClientCount() <= 0) return;
				Rect r(Point(0, 0), Height(img), Width(img));

				auto p = static_cast<unsigned int>(PACKET_TYPES::MOUSEIMAGE);
				auto finalsize = (Screen_Capture::RowStride(img) * Screen_Capture::Height(img)) + sizeof(p) + sizeof(r);
				auto buffer = std::make_unique<char[]>(finalsize);

				auto dst = buffer.get();
				memcpy(dst, &p, sizeof(p));
				dst += sizeof(p);
				memcpy(dst, &r, sizeof(r));
				dst += sizeof(r);
				Screen_Capture::Extract(img, dst, Screen_Capture::RowStride(img) * Screen_Capture::Height(img));

				Send(socket, buffer.get(), finalsize);

			}
			void SendMonitorInfo(IWebSocket * socket, const std::vector<std::shared_ptr<Screen_Capture::Monitor>>& monitors) {
				if (ServerHub_.get_ClientCount() <= 0) return;
				auto p = static_cast<unsigned int>(PACKET_TYPES::MONITORINFO);
				const auto size = (monitors.size() * sizeof(Screen_Capture::Monitor)) + sizeof(p);

				auto buffer = std::make_unique<char[]>(size);
				auto buf = buffer.get();
				memcpy(buf, &p, sizeof(p));
				buf += sizeof(p);
				for (auto& a : monitors) {
					memcpy(buf, a.get(), sizeof(Screen_Capture::Monitor));
					buf += sizeof(Screen_Capture::Monitor);
				}
				Send(socket, buffer.get(), size);

			}
			void SendMouse(IWebSocket* socket, const Point& pos)
			{
				if (ServerHub_.get_ClientCount() <= 0) return;
				auto p = static_cast<unsigned int>(PACKET_TYPES::MOUSEPOS);
				const auto size = sizeof(pos) + sizeof(p);
				char buffer[size];

				memcpy(buffer, &p, sizeof(p));
				memcpy(buffer + sizeof(p), &pos, sizeof(pos));

				Send(socket, buffer, size);
			}

			void SendClipboardText(IWebSocket* socket, const char* data, unsigned int len) {
				if (ServerHub_.get_ClientCount() <= 0) return;
				auto p = static_cast<unsigned int>(PACKET_TYPES::CLIPBOARDTEXTEVENT);
				auto size = len + sizeof(p);

				auto buffer = std::make_unique<char[]>(size);
				memcpy(buffer.get(), &p, sizeof(p));
				memcpy(buffer.get() + sizeof(p), data, len);

				Send(socket, buffer.get(), size);

			}
		};
		ServerDriver::ServerDriver(IServerDriver * r, std::shared_ptr<Server_Config> config) {
			ServerDriverImpl_ = std::make_unique<ServerDriverImpl>(r, config);

		}
		ServerDriver::~ServerDriver()
		{

		}
		void ServerDriver::Run() {
			ServerDriverImpl_->ServerHub_.Run();
		}
		void ServerDriver::SendFrameChange(IWebSocket* socket, const Screen_Capture::Image & img, const SL::Screen_Capture::Monitor& monitor)
		{
			ServerDriverImpl_->SendScreen(socket, img, monitor, PACKET_TYPES::SCREENIMAGEDIF);
		}
		void ServerDriver::SendMonitorInfo(IWebSocket * socket, const std::vector<std::shared_ptr<Screen_Capture::Monitor>>& monitors)
		{
			ServerDriverImpl_->SendMonitorInfo(socket, monitors);
		}
		void ServerDriver::SendMouse(IWebSocket* socket, const Screen_Capture::Image & img)
		{
			ServerDriverImpl_->SendMouse(socket, img);
		}
		void ServerDriver::SendMouse(IWebSocket* socket, const Point & pos)
		{
			ServerDriverImpl_->SendMouse(socket, pos);
		}

		void ServerDriver::SendClipboardText(IWebSocket* socket, const char* data, unsigned int len) {
			ServerDriverImpl_->SendClipboardText(socket, data, len);
		}


	}
}
