#include "ServerNetworkDriver.h"
#include "Shapes.h"
#include "IServerDriver.h"
#include "Configs.h"
#include "turbojpeg.h"
#include "Input.h"
#include "Logging.h"
#include "ScreenCapture.h"
#include "WebSocketListener.h"
#include "ISocket.h"

#include <atomic>
#include <mutex>
#include <vector>
#include <assert.h>
#include <array>

namespace SL {
	namespace Remote_Access_Library {

		class ServerNetworkDriverImpl {

			IServerDriver* _IServerDriver;

			std::shared_ptr<Server_Config> _Config;
			std::vector<std::shared_ptr<ISocket>> Clients;
			std::mutex ClientLock;
			WebSocketListener _WebSocketListener;

		public:
			ServerNetworkDriverImpl(IServerDriver * r, std::shared_ptr<Server_Config> config) :
				_IServerDriver(r), _Config(config), _WebSocketListener(config) {
				_WebSocketListener.onConnection([&](const std::shared_ptr<ISocket>& sock) {

					if (_Config->MaxNumConnections == -1 || static_cast<int>(Clients.size()) < _Config->MaxNumConnections) {
						std::lock_guard<std::mutex> lock(ClientLock);
						Clients.push_back(sock);
					}
					else
					{
						sock->close(std::string("Closing due to max number of connections!"));
					}
					_IServerDriver->onConnection(sock);

				});
				_WebSocketListener.onDisconnection([&](const ISocket* ws) {
					{
						std::lock_guard<std::mutex> lock(ClientLock);
						Clients.erase(std::remove_if(begin(Clients), end(Clients), [ws](const std::shared_ptr<ISocket>& p) { return p.get() == ws; }), Clients.end());
					}
					_IServerDriver->onDisconnection(ws);

				});
				_WebSocketListener.onMessage([&](const std::shared_ptr<ISocket>& sock, const char *message, size_t length) {
					auto pactype = PACKET_TYPES::INVALID;
					assert(length >= sizeof(pactype));

					pactype = *reinterpret_cast<const PACKET_TYPES*>(message);
					length -= sizeof(pactype);
					message += sizeof(pactype);

					switch (pactype) {
					case PACKET_TYPES::MOUSEEVENT:
						assert(length == sizeof(MouseEvent));
						_IServerDriver->OnReceive_Mouse(reinterpret_cast<const MouseEvent*>(message));
						break;
					case PACKET_TYPES::KEYEVENT:
						assert(length == sizeof(KeyEvent));
						_IServerDriver->OnReceive_Key(reinterpret_cast<const KeyEvent*>(message));
						break;
					case PACKET_TYPES::CLIPBOARDTEXTEVENT:
						_IServerDriver->OnReceive_ClipboardText(message, length);
						break;
					default:
						_IServerDriver->onMessage(sock, message - sizeof(pactype), length + sizeof(pactype));
						break;
					}
				});


			}
			virtual ~ServerNetworkDriverImpl() {

			}
			void Send(ISocket* socket, std::shared_ptr<char> data, size_t len) {
				if (socket) {
					socket->send(data, len);
				}
				else {

					std::lock_guard<std::mutex> lock(ClientLock);
					for (auto& c : Clients) {
						c->send(data, len);
					}
				}

			}

			void SendScreen(ISocket* socket, const Screen_Capture::Image & img, PACKET_TYPES p) {
				Rect r(Point(0, 0), Height(img), Width(img));


				auto compfree = [](void* handle) {tjDestroy(handle); };
				auto _jpegCompressor(std::unique_ptr<void, decltype(compfree)>(tjInitCompress(), compfree));

				auto set = _Config->SendGrayScaleImages ? TJSAMP_GRAY : TJSAMP_420;

				auto maxsize = std::max(tjBufSize(Screen_Capture::Width(img), Screen_Capture::Height(img), set), static_cast<unsigned long>((Screen_Capture::RowStride(img) + Screen_Capture::RowPadding(img)) * Screen_Capture::Height(img))) + sizeof(r) + sizeof(p);
				auto _jpegSize = maxsize;
				auto buffer = std::shared_ptr<char>(new char[maxsize], [](char* ptr) { delete[] ptr; });

				auto dst = (unsigned char*)buffer.get();
				memcpy(dst, &p, sizeof(p));
				dst += sizeof(p);
				memcpy(dst, &r, sizeof(r));
				dst += sizeof(r);

				auto srcbuffer = std::make_unique<char[]>(RowStride(img)*Height(img));
				Screen_Capture::Extract(img, srcbuffer.get(), RowStride(img)*Height(img));
				auto srcbuf = (const unsigned char*)srcbuffer.get();

#if __ANDROID__
				auto colorencoding = TJPF_RGBX;
#else 
				auto colorencoding = TJPF_BGRX;
#endif

				if (tjCompress2(_jpegCompressor.get(), srcbuf, r.Width, 0, r.Height, colorencoding, &dst, &_jpegSize, set, _Config->ImageCompressionSetting, TJFLAG_FASTDCT | TJFLAG_NOREALLOC) == -1) {
					SL_RAT_LOG(Logging_Levels::ERROR_log_level, tjGetErrorStr());
				}
				//	std::cout << "Sending " << r << std::endl;
				auto finalsize = sizeof(p) + sizeof(r) + _jpegSize;//adjust the correct size
				Send(socket, buffer, finalsize);
			}
			void SendMouse(ISocket* socket, const Screen_Capture::Image & img) {

				Point r(Width(img), Height(img));
				auto p = static_cast<unsigned int>(PACKET_TYPES::MOUSEIMAGE);
				auto finalsize = (Screen_Capture::RowStride(img) * Screen_Capture::Height(img)) + sizeof(p) + sizeof(r);
				auto buffer = std::shared_ptr<char>(new char[finalsize], [](char* ptr) { delete[] ptr; });

				auto dst = buffer.get();
				memcpy(dst, &p, sizeof(p));
				dst += sizeof(p);
				memcpy(dst, &r, sizeof(r));
				dst += sizeof(r);
				Screen_Capture::Extract(img, dst, Screen_Capture::RowStride(img) * Screen_Capture::Height(img));

				Send(socket, buffer, finalsize);

			}
			void SendMouse(ISocket* socket, const Point& pos)
			{
				auto p = static_cast<unsigned int>(PACKET_TYPES::MOUSEPOS);
				const auto size = sizeof(pos) + sizeof(p);
				auto buffer = std::shared_ptr<char>(new char[size], [](char* ptr) { delete[] ptr; });

				memcpy(buffer.get(), &p, sizeof(p));
				memcpy(buffer.get() + sizeof(p), &pos, sizeof(pos));

				Send(socket, buffer, size);
			}

			void SendClipboardText(ISocket* socket, const char* data, unsigned int len) {
				auto p = static_cast<unsigned int>(PACKET_TYPES::CLIPBOARDTEXTEVENT);
				auto size = len + sizeof(p);
				auto buffer = std::shared_ptr<char>(new char[size], [](char* ptr) { delete[] ptr; });
				memcpy(buffer.get(), &p, sizeof(p));
				memcpy(buffer.get() + sizeof(p), data, len);

				Send(socket, buffer, size);

			}
		};
		ServerNetworkDriver::ServerNetworkDriver()
		{

		}
		ServerNetworkDriver::~ServerNetworkDriver()
		{

		}
		void ServerNetworkDriver::Start(IServerDriver * r, std::shared_ptr<Server_Config> config) {
			_ServerNetworkDriverImpl = std::make_unique<ServerNetworkDriverImpl>(r, config);
		}
		void ServerNetworkDriver::Stop() {
			_ServerNetworkDriverImpl.reset();
		}

		void ServerNetworkDriver::SendFrameChange(ISocket* socket, const Screen_Capture::Image & img)
		{
			_ServerNetworkDriverImpl->SendScreen(socket, img, PACKET_TYPES::SCREENIMAGEDIF);
		}
		void ServerNetworkDriver::SendFrame(ISocket* socket, const Screen_Capture::Image & img)
		{
			_ServerNetworkDriverImpl->SendScreen(socket, img, PACKET_TYPES::SCREENIMAGE);
		}
		void ServerNetworkDriver::SendMouse(ISocket* socket, const Screen_Capture::Image & img)
		{
			_ServerNetworkDriverImpl->SendMouse(socket, img);
		}
		void ServerNetworkDriver::SendMouse(ISocket* socket, const Point & pos)
		{
			_ServerNetworkDriverImpl->SendMouse(socket, pos);
		}

		void ServerNetworkDriver::SendClipboardText(ISocket* socket, const char* data, unsigned int len) {
			SL_RAT_LOG(Logging_Levels::INFO_log_level, "OnSend_ClipboardText " << len);
			_ServerNetworkDriverImpl->SendClipboardText(socket, data, len);
		}


	}
}
