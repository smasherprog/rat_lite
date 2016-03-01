#include "stdafx.h"
#include "ServerNetworkDriver.h"
#include "Image.h"
#include "Shapes.h"
#include "Packet.h"
#include "TCPSocket.h"
#include "IServerDriver.h"
#include "Server_Config.h"
#include "WebSocketListener.h"
#include "IO_Runner.h"
#include "ThreadPool.h"
#include "TCPListener.h"
#include "HttpListener.h"
#include "turbojpeg.h"
#include "Mouse.h"

namespace SL {
	namespace Remote_Access_Library {
		namespace Network {

			class ServerNetworkDriverImpl : public IBaseNetworkDriver {

				std::shared_ptr<TCPListener<socket, TCPSocket<socket>>> _TCPListener;
				std::shared_ptr<WebSocketListener> _WebSocketListener;
				std::unique_ptr<HttpListener> _HttptListener;

				std::unique_ptr<IO_Runner> _IO_Runner;
				Utilities::ThreadPool _SendPool, _ReceivePool;
				IServerDriver* _IServerDriver;

				std::vector<std::shared_ptr<ISocket>> _Clients;
				std::mutex _ClientsLock;
				Server_Config _Config;
				std::vector<char> _CompressBuffer;

			public:
				ServerNetworkDriverImpl(Server_Config& config, IServerDriver* svrd) : _IServerDriver(svrd), _Config(config) {
				
				}
				virtual ~ServerNetworkDriverImpl() {
					Stop();
				}
				virtual void OnConnect(const std::shared_ptr<ISocket>& socket) override {
					_IServerDriver->OnConnect(socket);
					std::lock_guard<std::mutex> lock(_ClientsLock);
					_Clients.push_back(socket);
				}
				virtual void OnClose(const std::shared_ptr<ISocket>& socket)override {
					_IServerDriver->OnClose(socket);
					std::lock_guard<std::mutex> lock(_ClientsLock);
					_Clients.erase(std::remove_if(begin(_Clients), end(_Clients), [socket](const std::shared_ptr<ISocket>& p) { return p == socket; }), _Clients.end());
				}

				virtual void OnReceive(const std::shared_ptr<ISocket>& socket, std::shared_ptr<Packet>& p) override
				{
					_IServerDriver->OnReceive(socket, p);//pass up the chain
				}


				std::vector<std::shared_ptr<ISocket>> GetClients() {
					std::lock_guard<std::mutex> lock(_ClientsLock);
					return _Clients;
				}

				void Send(ISocket* socket, const Utilities::Image & img) {
					Utilities::Rect r(Utilities::Point(0, 0),  img.Height(), img.Width());
					auto p(ExtractImageRect(r, img));
					p.Packet_Type = static_cast<unsigned int>(PACKET_TYPES::IMAGE);
					if(socket==nullptr)	SendToAll(p);
					else socket->send(p);
				}

				void Send(ISocket* socket, Utilities::Rect& r, const Utilities::Image & img) {
					auto p(ExtractImageRect(r, img));
					if (socket == nullptr) SendToAll(p);
					else socket->send(p);
				}
				void Send(ISocket* socket, Capturing::MouseInfo mouseinfo) {
					Packet p(static_cast<unsigned int>(PACKET_TYPES::MOUSEINFO), static_cast<unsigned int>(sizeof(mouseinfo)));
					memcpy(p.Payload, &mouseinfo, sizeof(mouseinfo));
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

					_IO_Runner = std::make_unique<IO_Runner>();
					_TCPListener = std::make_shared<TCPListener<socket, TCPSocket<socket>>>(this, _Config.TCPListenPort, _IO_Runner->get_io_service());

					if (_Config.WebSocketListenPort > 0) {

						_HttptListener = std::make_unique<HttpListener>(this, _IO_Runner->get_io_service(), _Config.WWWRoot);
						_WebSocketListener = std::make_unique<WebSocketListener>(this, _IO_Runner->get_io_service());
					}


					_HttptListener->Start();
					_TCPListener->Start();
					_WebSocketListener->Start();

				}
				void Stop() {
					{
						std::lock_guard<std::mutex> lock(_ClientsLock);
						_Clients.clear();//destroy all clients
					}

					_HttptListener.reset();
					_TCPListener.reset();
					_WebSocketListener.reset();
					_IO_Runner.reset();

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
					auto set = TJSAMP_420;
					auto maxsize = std::max(tjBufSize(r.Width, r.Height, TJSAMP_420), static_cast<unsigned long>(r.Width *r.Height* Utilities::Image::DefaultStride()));
					auto _jpegSize = maxsize;

					_CompressBuffer.reserve(r.Width* r.Height*Utilities::Image::DefaultStride());
					ExtractImageRect(r, img, _CompressBuffer);

					auto srcbuf = (unsigned char*)_CompressBuffer.data();
					Packet p(static_cast<unsigned int>(PACKET_TYPES::IMAGEDIF), sizeof(Utilities::Rect) + maxsize);

					auto dst = (unsigned char*)p.Payload;
					memcpy(dst, &r, sizeof(Utilities::Rect));
					dst += sizeof(Utilities::Rect);


					if (tjCompress2(_jpegCompressor.get(), srcbuf, r.Width, 0, r.Height, TJPF_BGRX, &dst, &_jpegSize, set, 70, TJFLAG_FASTDCT | TJFLAG_NOREALLOC) == -1) {
						std::cout << "Err msg " << tjGetErrorStr() << std::endl;
					}
				//	std::cout << "Sending " << r << std::endl;
					p.Payload_Length = sizeof(Utilities::Rect) + _jpegSize;//adjust the correct size
					return p;
				}
			};
		}
	}
}

SL::Remote_Access_Library::Network::ServerNetworkDriver::ServerNetworkDriver(Network::IServerDriver * r, Server_Config& config) : _ServerNetworkDriverImpl(std::make_unique<ServerNetworkDriverImpl>(config, r))
{

}
SL::Remote_Access_Library::Network::ServerNetworkDriver::~ServerNetworkDriver()
{
	_ServerNetworkDriverImpl.reset();
}


void SL::Remote_Access_Library::Network::ServerNetworkDriver::Start()
{
	_ServerNetworkDriverImpl->Start();
}

void SL::Remote_Access_Library::Network::ServerNetworkDriver::Stop()
{
	_ServerNetworkDriverImpl->Stop();
}


void SL::Remote_Access_Library::Network::ServerNetworkDriver::Send(ISocket * socket, Utilities::Rect & r, const Utilities::Image & img)
{
	_ServerNetworkDriverImpl->Send(socket, r, img);
}
void SL::Remote_Access_Library::Network::ServerNetworkDriver::Send(ISocket * socket, const Utilities::Image & img)
{
	_ServerNetworkDriverImpl->Send(socket, img);
}

void SL::Remote_Access_Library::Network::ServerNetworkDriver::Send(ISocket * socket, Capturing::MouseInfo& mousetype)
{
	_ServerNetworkDriverImpl->Send(socket, mousetype);
}


std::vector<std::shared_ptr<SL::Remote_Access_Library::Network::ISocket>> SL::Remote_Access_Library::Network::ServerNetworkDriver::GetClients()
{
	return _ServerNetworkDriverImpl->GetClients();
}

