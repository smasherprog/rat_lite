#include "stdafx.h"
#include "ServerNetworkDriver.h"
#include "Image.h"
#include "Shapes.h"
#include "Packet.h"
#include "IServerDriver.h"
#include "Server_Config.h"
#include "Socket.h"
#include "turbojpeg.h"
#include "Mouse.h"
#include "Keyboard.h"
#include "Logging.h"
#include "ISocket.h"

#include <mutex>
#include <assert.h>

namespace SL {
	namespace Remote_Access_Library {
		namespace Network {

			class ServerNetworkDriverImpl : public IBaseNetworkDriver {

				std::shared_ptr<Listener> _Listener;

				IServerDriver* _IServerDriver;

				std::vector<std::shared_ptr<ISocket>> _Clients;
				std::vector<std::shared_ptr<ISocket>> _PendingClients;
				std::mutex _ClientsLock;
				std::shared_ptr<Network::Server_Config> _Config;
				std::vector<char> _CompressBuffer;
                Utilities::Point _LastMousePos;
                
				void KeyboardEvent(const std::shared_ptr<ISocket>& socket, std::shared_ptr<Packet>& p) {
					UNUSED(socket);
					assert(p->Payload_Length == sizeof(Input::KeyEvent));
					_IServerDriver->OnReceive_Key((Input::KeyEvent*) p->Payload);

				}

				void MouseEvent(const std::shared_ptr<ISocket>& socket, std::shared_ptr<Packet>& p) {
					UNUSED(socket);
					assert(p->Payload_Length == sizeof(Input::MouseEvent));
					_IServerDriver->OnReceive_Mouse((Input::MouseEvent*) p->Payload);
				}
				void ClipboardTextEvent(const std::shared_ptr<ISocket>& socket, std::shared_ptr<Packet>& p) {
					UNUSED(socket);

					_IServerDriver->OnReceive_ClipboardText(p->Payload, p->Payload_Length);
				}


			public:
				ServerNetworkDriverImpl(std::shared_ptr<Network::Server_Config> config, IServerDriver* svrd) : 
                _IServerDriver(svrd), _Config(config), _LastMousePos(0, 0) {

				}
				virtual ~ServerNetworkDriverImpl() {
					Stop();
				}
				virtual bool ValidateUntrustedCert(const std::shared_ptr<ISocket>& socket) override {
					UNUSED(socket);
					return true;
				}

				virtual void OnConnect(const std::shared_ptr<ISocket>& socket) override {
					if (_Clients.size() > static_cast<size_t>(_Config->MaxNumConnections)) {
						socket->close("CLosing due to max number of connections!");
						return;
					}
					_IServerDriver->OnConnect(socket);
					std::lock_guard<std::mutex> lock(_ClientsLock);
					_Clients.push_back(socket);
				}
				virtual void OnClose(const ISocket* socket)override {
					_IServerDriver->OnClose(socket);

					std::lock_guard<std::mutex> lock(_ClientsLock);

					_Clients.erase(std::remove_if(begin(_Clients), end(_Clients), [&socket](const std::shared_ptr<ISocket>& p) { return p.get() == socket; }), _Clients.end());

				}

				virtual void OnReceive(const std::shared_ptr<ISocket>& socket, std::shared_ptr<Packet>& p) override
				{
					switch (p->Packet_Type) {
					case static_cast<unsigned int>(PACKET_TYPES::MOUSEEVENT) :
						MouseEvent(socket, p);
						break;
					case static_cast<unsigned int>(PACKET_TYPES::KEYEVENT) :
						KeyboardEvent(socket, p);
						break;
					case static_cast<unsigned int>(PACKET_TYPES::CLIPBOARDTEXTEVENT) :
						ClipboardTextEvent(socket, p);
						break;
				
					default:
						_IServerDriver->OnReceive(socket, p);//pass up the chain
						break;
					}

				}
				size_t ClientCount() const {
					return _Clients.size();
				}

				std::vector<std::shared_ptr<ISocket>> GetClients() {
					std::lock_guard<std::mutex> lock(_ClientsLock);
					auto tmp = _Clients;
					return tmp;
				}
				void SendScreenDif(ISocket * socket, Utilities::Rect & r, const Utilities::Image & img) {
					auto p(ExtractImageRect(r, img));
					if (socket == nullptr) SendToAll(p);
					else socket->send(p);
				}
				void SendScreenFull(ISocket * socket, const Utilities::Image & img) {
					Utilities::Rect r(Utilities::Point(0, 0), img.Height(), img.Width());
					auto p(ExtractImageRect(r, img));
					p.Packet_Type = static_cast<unsigned int>(PACKET_TYPES::SCREENIMAGE);
					if (socket == nullptr)	SendToAll(p);
					else socket->send(p);
				}
				void SendMouse(ISocket * socket, const Utilities::Image & img) {

					Utilities::Point r(Utilities::Point(img.Width(), img.Height()));
					Packet p(static_cast<unsigned int>(PACKET_TYPES::MOUSEIMAGE), static_cast<unsigned int>(sizeof(r) + img.size()));
					memcpy(p.Payload, &r, sizeof(r));
					memcpy(p.Payload + sizeof(r), img.data(), img.size());
					if (socket == nullptr)	SendToAll(p);
					else socket->send(p);
				}
				void SendMouse(ISocket * socket, const Utilities::Point& pos)
				{
                    if(_LastMousePos == pos) return;//no need to send the same information
                    else _LastMousePos = pos;//copy the last mouse pos
					Packet p(static_cast<unsigned int>(PACKET_TYPES::MOUSEPOS), sizeof(pos));
					memcpy(p.Payload, &pos, sizeof(pos));
					if (socket == nullptr) SendToAll(p);
					else socket->send(p);
				}

				void SendClipboardText(ISocket * socket, const char* data, unsigned int len) {
					Packet p(static_cast<unsigned int>(PACKET_TYPES::CLIPBOARDTEXTEVENT), len, (char*)data, false);
					if (socket == nullptr) SendToAll(p);
					else socket->send(p);

				}

				void SendToAll(Packet& packet) {
					for (auto& c : GetClients()) {
						c->send(packet);
					}
				}
				void SendToAll(std::vector<Packet>& packets) {
					for (auto& c : GetClients()) {
						for (auto& p : packets) {
							c->send(p);
						}
					}
				}

				void Start() {
					Stop();
					if (_Config->WebSocketTLSLPort > 0) {
						_Listener = std::make_unique<Listener>(this, _Config, ListenerTypes::WEBSOCKET);
					}

				}
				void Stop() {
					std::vector<std::shared_ptr<ISocket>> copyclients;
					{
						std::lock_guard<std::mutex> lock(_ClientsLock);
						copyclients = std::move(_Clients);//move all of the clients to a new vector
						_Clients.clear();//clear the internal client listing
					}
					std::for_each(begin(copyclients), end(copyclients), [](const std::shared_ptr<ISocket>& o) { o->close("ShuttingDown"); });
					copyclients.clear();
					if(_Listener) _Listener.reset();

				}
				void ExtractImageRect(Utilities::Rect r, const Utilities::Image & img, std::vector<char>& outbuffer) {

					auto srcbuf = outbuffer.data();
					for (auto row = r.Origin.Y; row < r.bottom(); row++) {

						auto startcopy = img.data() + (row*img.Stride()*img.Width());//advance rows
						startcopy += r.Origin.X*img.Stride();//advance columns
						memcpy(srcbuf, startcopy, r.Width*img.Stride());
						srcbuf += r.Width*img.Stride();
					}

				}
				Packet ExtractImageRect(Utilities::Rect& r, const Utilities::Image & img) {
					auto compfree = [](void* handle) {tjDestroy(handle); };
					auto _jpegCompressor(std::unique_ptr<void, decltype(compfree)>(tjInitCompress(), compfree));

					auto set = _Config->SendGrayScaleImages ? TJSAMP_GRAY : TJSAMP_420;

					auto maxsize = std::max(tjBufSize(r.Width, r.Height, set), static_cast<unsigned long>(r.Width *r.Height* Utilities::Image::DefaultStride()));
					auto _jpegSize = maxsize;

					_CompressBuffer.reserve(r.Width* r.Height*Utilities::Image::DefaultStride());
					ExtractImageRect(r, img, _CompressBuffer);

					auto srcbuf = (unsigned char*)_CompressBuffer.data();
					Packet p(static_cast<unsigned int>(PACKET_TYPES::SCREENIMAGEDIF), sizeof(Utilities::Rect) + maxsize);

					auto dst = (unsigned char*)p.Payload;
					memcpy(dst, &r, sizeof(Utilities::Rect));
					dst += sizeof(Utilities::Rect);
#if __ANDROID__
					auto colorencoding = TJPF_RGBX;
#else 
					auto colorencoding = TJPF_BGRX;
#endif

					if (tjCompress2(_jpegCompressor.get(), srcbuf, r.Width, 0, r.Height, colorencoding, &dst, &_jpegSize, set, _Config->ImageCompressionSetting, TJFLAG_FASTDCT | TJFLAG_NOREALLOC) == -1) {
						SL_RAT_LOG(Utilities::Logging_Levels::ERROR_log_level, tjGetErrorStr());
					}
					//	std::cout << "Sending " << r << std::endl;
					p.Payload_Length = sizeof(Utilities::Rect) + _jpegSize;//adjust the correct size
					return p;
				}
			};
		}
	}
}

SL::Remote_Access_Library::Network::ServerNetworkDriver::ServerNetworkDriver(Network::IServerDriver * r, std::shared_ptr<Network::Server_Config> config) : _ServerNetworkDriverImpl(new ServerNetworkDriverImpl(config, r))
{

}
SL::Remote_Access_Library::Network::ServerNetworkDriver::~ServerNetworkDriver()
{
	Stop();
	delete _ServerNetworkDriverImpl;
}


void SL::Remote_Access_Library::Network::ServerNetworkDriver::Start()
{
	_ServerNetworkDriverImpl->Start();
}

void SL::Remote_Access_Library::Network::ServerNetworkDriver::Stop()
{
	_ServerNetworkDriverImpl->Stop();
}


void SL::Remote_Access_Library::Network::ServerNetworkDriver::SendScreenDif(ISocket * socket, Utilities::Rect & r, const Utilities::Image & img)
{
	_ServerNetworkDriverImpl->SendScreenDif(socket, r, img);
}
void SL::Remote_Access_Library::Network::ServerNetworkDriver::SendScreenFull(ISocket * socket, const Utilities::Image & img)
{
	_ServerNetworkDriverImpl->SendScreenFull(socket, img);
}
void SL::Remote_Access_Library::Network::ServerNetworkDriver::SendMouse(ISocket * socket, const Utilities::Image & img)
{
	_ServerNetworkDriverImpl->SendMouse(socket, img);
}
void SL::Remote_Access_Library::Network::ServerNetworkDriver::SendMouse(ISocket * socket, const Utilities::Point & pos)
{
	_ServerNetworkDriverImpl->SendMouse(socket, pos);
}

void SL::Remote_Access_Library::Network::ServerNetworkDriver::SendClipboardText(ISocket * socket, const char* data, unsigned int len) {
	SL_RAT_LOG(Utilities::Logging_Levels::INFO_log_level, "OnSend_ClipboardText " << len);
	_ServerNetworkDriverImpl->SendClipboardText(socket, data, len);
}

std::vector<std::shared_ptr<SL::Remote_Access_Library::Network::ISocket>> SL::Remote_Access_Library::Network::ServerNetworkDriver::GetClients()
{
	return _ServerNetworkDriverImpl->GetClients();
}

size_t SL::Remote_Access_Library::Network::ServerNetworkDriver::ClientCount() const
{
	return _ServerNetworkDriverImpl->ClientCount();
}

