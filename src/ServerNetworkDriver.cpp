#include "ServerNetworkDriver.h"
#include "Shapes.h"
#include "IServerDriver.h"
#include "Server_Config.h"
#include "turbojpeg.h"
#include "Input.h"

#include "Logging.h"
#include "Packet.h"
#include "ScreenCapture.h"

#include <atomic>
#include <mutex>
#include <assert.h>
#include <uWS/uWS.h>

namespace SL {
	namespace Remote_Access_Library {

		class ServerNetworkDriverImpl {

			uWS::Hub _Listener;

			IServerDriver* _IServerDriver;

			std::shared_ptr<Server_Config> _Config;
			std::atomic_int ClientCount;

		public:
			ServerNetworkDriverImpl(std::shared_ptr<Server_Config> config, IServerDriver* svrd) :
				_IServerDriver(svrd), _Config(config) {

				_Listener.onConnection([&](uWS::WebSocket<uWS::CLIENT> ws, uWS::UpgradeInfo ui) {
					if (ClientCount >= static_cast<size_t>(_Config->MaxNumConnections)) {
						char *closeMessage = "Closing due to max number of connections!";
						size_t closeMessageLength = strlen(closeMessage);
						ws.close(1000, closeMessage, closeMessageLength);
					}
					else {
						ClientCount += 1;
						_IServerDriver->onConnection(ws, ui);
					}
				});
				_Listener.onDisconnection([&](uWS::WebSocket<uWS::CLIENT> ws, int code, char *message, size_t length) {
					ClientCount -= 1;
					_IServerDriver->onDisconnection(ws, code, message, length);

				});
				_Listener.onMessage([&](uWS::WebSocket<uWS::CLIENT> ws, char *message, size_t length, uWS::OpCode opCode) {
					auto pactype = PACKET_TYPES::INVALID;
					assert(length >= sizeof(pactype));
					pactype = *reinterpret_cast<PACKET_TYPES*>(message);
					length -= sizeof(pactype);
					message += sizeof(pactype);

					switch (pactype) {
					case PACKET_TYPES::MOUSEEVENT:
						assert(length == sizeof(MouseEvent));
						_IServerDriver->OnReceive_Mouse(reinterpret_cast<MouseEvent*>(message));
						break;
					case PACKET_TYPES::KEYEVENT:
						assert(length == sizeof(KeyEvent));
						_IServerDriver->OnReceive_Key(reinterpret_cast<KeyEvent*>(message));
						break;
					case PACKET_TYPES::CLIPBOARDTEXTEVENT:
						_IServerDriver->OnReceive_ClipboardText(message, length);
						break;
					default:
						_IServerDriver->onMessage(ws, message - sizeof(pactype), length + sizeof(pactype), opCode);
						break;
					}

					_IServerDriver->onMessage(ws, message, length, opCode);
				});

				//auto c = uS::TLS::createContext(_Config->Certficate_Public_FilePath, _Config->Certficate_Private_FilePath, _Config->PasswordToPrivateKey);

				_Listener.listen(_Config->WebSocketTLSLPort);
				_Listener.run();
				int k = 0;
			}
			virtual ~ServerNetworkDriverImpl() {

			}
			void Start() {
			
			}
			void Stop() {
				_Listener.getDefaultGroup<uWS::SERVER>().close();
			}
			

			void SendScreen(uWS::WebSocket<uWS::CLIENT> * socket, const Screen_Capture::Image & img) {
				Rect r(Point(0, 0), Height(img), Width(img));
				auto p = static_cast<unsigned int>(PACKET_TYPES::SCREENIMAGE);

				auto compfree = [](void* handle) {tjDestroy(handle); };
				auto _jpegCompressor(std::unique_ptr<void, decltype(compfree)>(tjInitCompress(), compfree));

				auto set = _Config->SendGrayScaleImages ? TJSAMP_GRAY : TJSAMP_420;

				auto maxsize = std::max(tjBufSize(Screen_Capture::Width(img), Screen_Capture::Height(img), set), static_cast<unsigned long>((Screen_Capture::RowStride(img) + Screen_Capture::RowPadding(img)) * Screen_Capture::Height(img))) + sizeof(r) + sizeof(p);
				auto _jpegSize = maxsize;
				auto buffer1 = std::make_unique<char[]>(maxsize);

				auto dst = (unsigned char*)buffer1.get();
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

				if (socket == nullptr)	_Listener.getDefaultGroup<uWS::SERVER>().broadcast(buffer1.get(), finalsize, uWS::OpCode::BINARY);
				else socket->send(buffer1.get(), finalsize, uWS::OpCode::BINARY);
			}
			void SendMouse(uWS::WebSocket<uWS::CLIENT> * socket, const Screen_Capture::Image & img) {

				Point r(Width(img), Height(img));
				auto p = static_cast<unsigned int>(PACKET_TYPES::MOUSEIMAGE);
				auto finalsize = (Screen_Capture::RowStride(img) * Screen_Capture::Height(img)) + sizeof(p) + sizeof(r);
				auto buffer = std::make_unique<char[]>(finalsize);

				auto dst = buffer.get();
				memcpy(dst, &p, sizeof(p));
				dst += sizeof(p);
				memcpy(dst, &r, sizeof(r));
				dst += sizeof(r);
				Screen_Capture::Extract(img, dst, Screen_Capture::RowStride(img) * Screen_Capture::Height(img));

				if (socket == nullptr)	_Listener.getDefaultGroup<uWS::SERVER>().broadcast(buffer.get(), finalsize, uWS::OpCode::BINARY);
				else socket->send(buffer.get(), finalsize, uWS::OpCode::BINARY);

			}
			void SendMouse(uWS::WebSocket<uWS::CLIENT> * socket, const Point& pos)
			{
				auto p = static_cast<unsigned int>(PACKET_TYPES::MOUSEPOS);
				const auto size = sizeof(pos) + sizeof(p);

				char buffer[size];
				memcpy(buffer, &p, sizeof(p));
				memcpy(buffer + sizeof(p), &pos, sizeof(pos));

				if (socket == nullptr)	_Listener.getDefaultGroup<uWS::SERVER>().broadcast(buffer, size, uWS::OpCode::BINARY);
				else socket->send(buffer, size, uWS::OpCode::BINARY);
			}

			void SendClipboardText(uWS::WebSocket<uWS::CLIENT> * socket, const char* data, unsigned int len) {
				auto p = static_cast<unsigned int>(PACKET_TYPES::CLIPBOARDTEXTEVENT);
				auto size = len + sizeof(p);
				auto buffer = std::make_unique<char[]>(size);

				if (socket == nullptr)	_Listener.getDefaultGroup<uWS::SERVER>().broadcast(buffer.get(), size, uWS::OpCode::BINARY);
				else socket->send(buffer.get(), size, uWS::OpCode::BINARY);

			}
		};
		ServerNetworkDriver::ServerNetworkDriver(IServerDriver * r, std::shared_ptr<Server_Config> config) :_ServerNetworkDriverImpl(std::make_unique<ServerNetworkDriverImpl>(config, r))
		{
		}
		ServerNetworkDriver::~ServerNetworkDriver()
		{

		}
		void ServerNetworkDriver::Start() {
			_ServerNetworkDriverImpl->Start();
		}
		void ServerNetworkDriver::Stop() {
			_ServerNetworkDriverImpl->Stop();
		}

		void ServerNetworkDriver::SendScreen(uWS::WebSocket<uWS::CLIENT> * socket, const Screen_Capture::Image & img)
		{
			_ServerNetworkDriverImpl->SendScreen(socket, img);
		}
		void ServerNetworkDriver::SendMouse(uWS::WebSocket<uWS::CLIENT> * socket, const Screen_Capture::Image & img)
		{
			_ServerNetworkDriverImpl->SendMouse(socket, img);
		}
		void ServerNetworkDriver::SendMouse(uWS::WebSocket<uWS::CLIENT> * socket, const Point & pos)
		{
			_ServerNetworkDriverImpl->SendMouse(socket, pos);
		}

		void ServerNetworkDriver::SendClipboardText(uWS::WebSocket<uWS::CLIENT> * socket, const char* data, unsigned int len) {
			SL_RAT_LOG(Logging_Levels::INFO_log_level, "OnSend_ClipboardText " << len);
			_ServerNetworkDriverImpl->SendClipboardText(socket, data, len);
		}


	}
}
