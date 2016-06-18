#include "stdafx.h"
#include "ClientNetworkDriver.h"
#include "WebSocket.h"
#include "Shapes.h"
#include "IClientDriver.h"
#include "Image.h"
#include "Packet.h"
#include "turbojpeg.h"
#include "Mouse.h"
#include "Logging.h"
#include "Keyboard.h"
#include "Client_Config.h"
#include "ISocket.h"

#include <assert.h>

namespace SL {
	namespace Remote_Access_Library {
		namespace Network {

			class ClientNetworkDriverImpl : public IBaseNetworkDriver {
                
				IClientDriver* _IClientDriver;
                std::shared_ptr<Network::Client_Config> _Config;
				std::shared_ptr<Network::ISocket> _Socket;
                std::string _dst_host;
				Utilities::Point _LastMousePosition;
                bool _ConectedToSelf;
				
                
				void MouseImage(std::shared_ptr<Packet>& p) {
					auto imgsize = (Utilities::Point*)p->Payload;
					auto img(Utilities::Image::CreateImage(imgsize->Y, imgsize->X, p->Payload + sizeof(Utilities::Rect), 4));
					_IClientDriver->OnReceive_MouseImage(img);
				}

				void MousePos(std::shared_ptr<Packet>& p) {
					assert(p->Payload_Length == sizeof(Utilities::Point));
					_IClientDriver->OnReceive_MousePos((Utilities::Point*)p->Payload);
				}
				void ScreenImageDif(std::shared_ptr<Packet>& p) {
					auto imgrect = (Utilities::Rect*)p->Payload;
					auto compfree = [](void* handle) {tjDestroy(handle); };
					auto _jpegDecompressor(std::unique_ptr<void, decltype(compfree)>(tjInitDecompress(), compfree));

					int jpegSubsamp(0), outwidth(0), outheight(0);
					auto src = (unsigned char*)(p->Payload + sizeof(Utilities::Rect));

					if (tjDecompressHeader2(_jpegDecompressor.get(), src, static_cast<unsigned long>(p->Payload_Length - sizeof(Utilities::Rect)), &outwidth, &outheight, &jpegSubsamp) == -1) {
						SL_RAT_LOG(Utilities::Logging_Levels::ERROR_log_level, tjGetErrorStr());
					}
					auto img = Utilities::Image::CreateImage(outheight, outwidth);

					if (tjDecompress2(_jpegDecompressor.get(), src, static_cast<unsigned long>(p->Payload_Length - sizeof(Utilities::Rect)), (unsigned char*)img->data(), outwidth, 0, outheight, TJPF_RGBX, TJFLAG_FASTDCT | TJFLAG_NOREALLOC) == -1) {
						SL_RAT_LOG(Utilities::Logging_Levels::ERROR_log_level, tjGetErrorStr());
					}
					_IClientDriver->OnReceive_ImageDif(imgrect->Origin, img);

				}
				void ScreenImage(std::shared_ptr<Packet>& p) {

					auto compfree = [](void* handle) {tjDestroy(handle); };
					auto _jpegDecompressor(std::unique_ptr<void, decltype(compfree)>(tjInitDecompress(), compfree));
					int jpegSubsamp(0), outwidth(0), outheight(0);
					auto src = (unsigned char*)(p->Payload + sizeof(Utilities::Rect));

					if (tjDecompressHeader2(_jpegDecompressor.get(), src, static_cast<unsigned long>(p->Payload_Length - sizeof(Utilities::Rect)), &outwidth, &outheight, &jpegSubsamp) == -1) {
						SL_RAT_LOG(Utilities::Logging_Levels::ERROR_log_level, "tjDecompressHeader2 " << tjGetErrorStr());
					}
					auto img = Utilities::Image::CreateImage(outheight, outwidth);

					if (tjDecompress2(_jpegDecompressor.get(), src, static_cast<unsigned long>(p->Payload_Length - sizeof(Utilities::Rect)), (unsigned char*)img->data(), outwidth, 0, outheight, TJPF_RGBX, TJFLAG_FASTDCT | TJFLAG_NOREALLOC) == -1) {
						SL_RAT_LOG(Utilities::Logging_Levels::ERROR_log_level, tjGetErrorStr());
					}
					_IClientDriver->OnReceive_Image(img);

				}
				void ClipboardTextEvent(std::shared_ptr<Packet>& p) {
				
					_IClientDriver->OnReceive_ClipboardText(p->Payload, p->Payload_Length);
				}

			public:
				ClientNetworkDriverImpl(IClientDriver* r, std::shared_ptr<Network::Client_Config> config, const char * dst_host) : 
                _IClientDriver(r), _Config(config), _dst_host(dst_host), _ConectedToSelf(false){
					memset(&_LastMousePosition, 0, sizeof(_LastMousePosition));
				}
				
				void Start() {
					Stop();
					WebSocket::Connect(_Config.get(), this, _dst_host.c_str());
					_ConectedToSelf = (std::string("127.0.0.1") == _dst_host) || (std::string("localhost") == _dst_host) || (std::string("::1") == _dst_host);

				}
				
				void Stop() {
					if (_Socket) _Socket->close("Stopping Listener");
					_Socket.reset();//decrement count
				}
				virtual ~ClientNetworkDriverImpl() {
					Stop();
				}
				virtual bool ValidateUntrustedCert(const std::shared_ptr<ISocket>& socket) override { 
					UNUSED(socket);
					return true;
				}
				virtual void OnConnect(const std::shared_ptr<ISocket>& socket) override { 
					_Socket = socket;
					_IClientDriver->OnConnect(socket); 
				}
				virtual void OnClose(const ISocket* socket) override {
					_IClientDriver->OnClose(socket); 
				}

				virtual void OnReceive(const std::shared_ptr<ISocket>& socket, std::shared_ptr<Packet>& p) override {

					switch (p->Packet_Type) {
					case static_cast<unsigned int>(PACKET_TYPES::SCREENIMAGE) :
						ScreenImage(p);
						break;
					case static_cast<unsigned int>(PACKET_TYPES::SCREENIMAGEDIF) :
						ScreenImageDif(p);
						break;
					case static_cast<unsigned int>(PACKET_TYPES::MOUSEIMAGE) :
						MouseImage(p);
						break;
					case static_cast<unsigned int>(PACKET_TYPES::MOUSEPOS) :
						MousePos(p);
						break;
					case static_cast<unsigned int>(PACKET_TYPES::CLIPBOARDTEXTEVENT) :
						ClipboardTextEvent(p);
						break;
					default:
						_IClientDriver->OnReceive(socket, p);//pass up the chain
						break;
					}

				}
				void SendMouse(const Input::MouseEvent& m) {
					if (!_Socket) {
						SL_RAT_LOG(Utilities::Logging_Levels::INFO_log_level, "SendMouse called on a socket that is not open yet");
						return;
					}
					//do checks to prevent sending redundant mouse information about its position
					if (m.EventData == Input::Mouse::NO_EVENTDATA && _LastMousePosition == m.Pos && m.PressData == Input::Mouse::NO_PRESS_DATA && m.ScrollDelta == 0) {
						return;//already did this event
					}
					_LastMousePosition = m.Pos;
					Packet p(static_cast<unsigned int>(PACKET_TYPES::MOUSEEVENT), sizeof(m));
					memcpy(p.Payload, &m, sizeof(m));
					_Socket->send(p);
				}
				void SendKey(const Input::KeyEvent & m) {
					if (!_Socket) {
						SL_RAT_LOG(Utilities::Logging_Levels::INFO_log_level, "SendKey called on a socket that is not open yet");
						return;
					}
					Packet p(static_cast<unsigned int>(PACKET_TYPES::KEYEVENT), sizeof(m));
					memcpy(p.Payload, &m, sizeof(m));
					_Socket->send(p);
				}
				void SendClipboardText(const char* data, unsigned int len) {
					if (!_Socket) {
						SL_RAT_LOG(Utilities::Logging_Levels::INFO_log_level, "SendKey called on a socket that is not open yet");
						return;
					}
					Packet p(static_cast<unsigned int>(PACKET_TYPES::CLIPBOARDTEXTEVENT), len, (char*)data, false);
					_Socket->send(p);
				}
				bool ConnectedToSelf() const {
					return _ConectedToSelf;	
				}
                std::shared_ptr<ISocket> get_Socket()const{
                    return _Socket;
                }
			};
		}
	}
}


SL::Remote_Access_Library::Network::ClientNetworkDriver::ClientNetworkDriver(IClientDriver * r, std::shared_ptr<Network::Client_Config> config, const char * dst_host) : _ClientNetworkDriverImpl(new ClientNetworkDriverImpl(r, config, dst_host))
{

}

SL::Remote_Access_Library::Network::ClientNetworkDriver::~ClientNetworkDriver()
{
	Stop();
	delete _ClientNetworkDriverImpl;
}

void SL::Remote_Access_Library::Network::ClientNetworkDriver::Start()
{
	_ClientNetworkDriverImpl->Start();
}

void SL::Remote_Access_Library::Network::ClientNetworkDriver::Stop()
{
	_ClientNetworkDriverImpl->Stop();
}

void SL::Remote_Access_Library::Network::ClientNetworkDriver::SendKey(const Input::KeyEvent & m)
{
	_ClientNetworkDriverImpl->SendKey(m);
}

void SL::Remote_Access_Library::Network::ClientNetworkDriver::SendMouse(const Input::MouseEvent& m)
{
	_ClientNetworkDriverImpl->SendMouse(m);
}

bool SL::Remote_Access_Library::Network::ClientNetworkDriver::ConnectedToSelf() const {
	return _ClientNetworkDriverImpl->ConnectedToSelf();
}
std::shared_ptr<SL::Remote_Access_Library::Network::ISocket> SL::Remote_Access_Library::Network::ClientNetworkDriver::get_Socket()const{
    return _ClientNetworkDriverImpl->get_Socket();
}

void SL::Remote_Access_Library::Network::ClientNetworkDriver::SendClipboardText(const char* data, unsigned int len) {
	return _ClientNetworkDriverImpl->SendClipboardText(data, len);
}