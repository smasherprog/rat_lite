#include "ClientNetworkDriver.h"
#include "RAT.h"
#include "turbojpeg.h"
#include "IClientDriver.h"
#include <assert.h>

namespace SL {
	namespace RAT {
	
		class ClientNetworkDriverImpl : public INetworkHandlers {

			IClientDriver* _IClientDriver;
			std::shared_ptr<Client_Config> _Config;
			std::shared_ptr<ISocket> _Socket;
			std::string _dst_host;
			Point _LastMousePosition;

			void MouseImage(const std::shared_ptr<ISocket>& socket, const char* data, size_t len) {
				auto imgrect = reinterpret_cast<const Size*>(data);
				assert(len == sizeof(Size) + (imgrect->X * imgrect->Y*PixelStride));
				_IClientDriver->onReceive_MouseImage(imgrect, data + sizeof(Size));
			}

			void MousePos(const std::shared_ptr<ISocket>& socket, const char* data, size_t len) {
				assert(len == sizeof(Point));
				_IClientDriver->onReceive_MousePos((Point*)data);
			}
			void ScreenImage(const std::shared_ptr<ISocket>& socket, const char* data, size_t len, bool dif) {
				auto imgrect = reinterpret_cast<const Rect*>(data);
				auto compfree = [](void* handle) {tjDestroy(handle); };
				auto _jpegDecompressor(std::unique_ptr<void, decltype(compfree)>(tjInitDecompress(), compfree));

				int jpegSubsamp(0), outwidth(0), outheight(0);
				auto src = (unsigned char*)(data + sizeof(Rect));

				if (tjDecompressHeader2(_jpegDecompressor.get(), src, static_cast<unsigned long>(len - sizeof(Rect)), &outwidth, &outheight, &jpegSubsamp) == -1) {
					SL_RAT_LOG(Logging_Levels::ERROR_log_level, tjGetErrorStr());
				}
				assert(outwidth == imgrect->Width && outheight == imgrect->Height);
				auto imgbacking = std::shared_ptr<char>(new char[outwidth* outheight * PixelStride], [](char* ptr) {delete[] ptr; });

				if (tjDecompress2(_jpegDecompressor.get(), src, static_cast<unsigned long>(len - sizeof(Rect)), (unsigned char*)imgbacking.get(), outwidth, 0, outheight, TJPF_RGBX, TJFLAG_FASTDCT | TJFLAG_NOREALLOC) == -1) {
					SL_RAT_LOG(Logging_Levels::ERROR_log_level, tjGetErrorStr());
				}
				if (dif) {
					_IClientDriver->onReceive_ImageDif(imgrect, imgbacking);
				}
				else {
					_IClientDriver->onReceive_Image(imgrect, imgbacking);
				}
			}
			void ClipboardTextEvent(const std::shared_ptr<ISocket>& socket, const char* data, size_t len) {

				_IClientDriver->onReceive_ClipboardText(data, len);
			}

		public:
			ClientNetworkDriverImpl(IClientDriver* r) :
				_IClientDriver(r) {
		
			}

			void Connect(std::shared_ptr<Client_Config> config, const char* dst_host) {
				_Config = config;
				_dst_host = dst_host;
			//	SL::RAT::Connect(_Config.get(), this, dst_host);
			}

			virtual ~ClientNetworkDriverImpl() {
				if (_Socket) {
					_Socket->close("Stopping Listener");
					_Socket.reset();//decrement count
				}

			}

			virtual void onConnection(const std::shared_ptr<ISocket>& socket) override {
				_Socket = socket;
				_IClientDriver->onConnection(socket);
			}

			virtual void onDisconnection(const ISocket* socket) override {
				_IClientDriver->onDisconnection(socket);
			}

			virtual void onMessage(const std::shared_ptr<ISocket>& socket, const char* data, size_t len)  override {
				auto p = *reinterpret_cast<const PACKET_TYPES*>(data);
				switch (p) {
				case PACKET_TYPES::SCREENIMAGE :
					ScreenImage(socket, data + sizeof(p), len-sizeof(p), false);
					break;
				case PACKET_TYPES::SCREENIMAGEDIF:
					ScreenImage(socket, data + sizeof(p), len - sizeof(p), true);
					break;
				case PACKET_TYPES::MOUSEIMAGE :
					MouseImage(socket, data + sizeof(p), len - sizeof(p));
					break;
				case PACKET_TYPES::MOUSEPOS:
					MousePos(socket, data + sizeof(p), len - sizeof(p));
					break;
				case PACKET_TYPES::CLIPBOARDTEXTEVENT :
					ClipboardTextEvent(socket, data + sizeof(p), len - sizeof(p));
					break;
				default:
					_IClientDriver->onMessage(socket, data, len);//pass up the chain
					break;
				}

			}

	
			void SendMouse(const MouseEvent& m) {
				if (!_Socket) {
					SL_RAT_LOG(Logging_Levels::INFO_log_level, "SendMouse called on a socket that is not open yet");
					return;
				}
				if (_Socket->is_loopback()) return;//dont send mouse info to ourselfs as this will cause a loop
				//do checks to prevent sending redundant mouse information about its position
				if (m.EventData == NO_EVENTDATA && _LastMousePosition == m.Pos && m.PressData == NO_PRESS_DATA && m.ScrollDelta == 0) {
					return;//already did this event
				}
				_LastMousePosition = m.Pos;
				auto ptype = PACKET_TYPES::MOUSEEVENT;
				auto size = sizeof(ptype) + sizeof(m);
				auto ptr(std::shared_ptr<char>(new char[size], [](char* ptr) { delete[] ptr; }));
				*reinterpret_cast<PACKET_TYPES*>(ptr.get()) = ptype;
				memcpy(ptr.get() + sizeof(ptype), &m, sizeof(m));
				_Socket->send(ptr, size);
			}
			void SendKey(const KeyEvent & m) {
				if (!_Socket) {
					SL_RAT_LOG(Logging_Levels::INFO_log_level, "SendKey called on a socket that is not open yet");
					return;
				}
				auto ptype = PACKET_TYPES::KEYEVENT;
				auto size = sizeof(ptype) + sizeof(m);
				auto ptr(std::shared_ptr<char>(new char[size], [](char* ptr) { delete[] ptr; }));
				*reinterpret_cast<PACKET_TYPES*>(ptr.get()) = ptype;
				memcpy(ptr.get() + sizeof(ptype), &m, sizeof(m));
				_Socket->send(ptr, size);

			}
			void SendClipboardText(const char* data, unsigned int len) {
				if (!_Socket) {
					SL_RAT_LOG(Logging_Levels::INFO_log_level, "SendClipboardText called on a socket that is not open yet");
					return;
				}
				if (_Socket->is_loopback()) return;//dont send clipboard info to ourselfs as it will cause a loop
			
				auto ptype = PACKET_TYPES::CLIPBOARDTEXTEVENT;
				auto size = sizeof(ptype) + len;
				auto ptr(std::shared_ptr<char>(new char[size], [](char* ptr) { delete[] ptr; }));
				*reinterpret_cast<PACKET_TYPES*>(ptr.get()) = ptype;
				memcpy(ptr.get() + sizeof(ptype), data, len);
				_Socket->send(ptr, size);

			}
			std::shared_ptr<ISocket> get_Socket()const {
				return _Socket;
			}
		};

	}
}

SL::RAT::ClientNetworkDriver::ClientNetworkDriver(IClientDriver * r) 
	: _ClientNetworkDriverImpl(new ClientNetworkDriverImpl(r))
{

}

SL::RAT::ClientNetworkDriver::~ClientNetworkDriver()
{
	delete _ClientNetworkDriverImpl;
}

void SL::RAT::ClientNetworkDriver::Connect(std::shared_ptr<Client_Config> config, const char* dst_host)
{
	_ClientNetworkDriverImpl->Connect(config, dst_host);
}


void SL::RAT::ClientNetworkDriver::SendKey(const KeyEvent & m)
{
	_ClientNetworkDriverImpl->SendKey(m);
}

void SL::RAT::ClientNetworkDriver::SendMouse(const MouseEvent& m)
{
	_ClientNetworkDriverImpl->SendMouse(m);
}


void SL::RAT::ClientNetworkDriver::SendClipboardText(const char* data, unsigned int len) {
	return _ClientNetworkDriverImpl->SendClipboardText(data, len);
}