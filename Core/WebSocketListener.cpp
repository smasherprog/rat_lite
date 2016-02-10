#include "stdafx.h"
#include "WebSocketListener.h"
#include "TCPListener.h"
#include "WebSocket.h"
#include "IBaseNetworkDriver.h"
#include "Packet.h"

namespace SL {
	namespace Remote_Access_Library {
		namespace Network {
			namespace INTERNAL {
				class WebSocketListinerImpl : public IBaseNetworkDriver {
				public:
					std::shared_ptr<TCPListener<socket, WebSocket<socket>>> _TCPListener;
					IBaseNetworkDriver* _IBaseNetworkDriver;
					boost::asio::io_service& _io_service;
					unsigned short _Listenport;
					WebSocketListinerImpl(IBaseNetworkDriver* netevent, boost::asio::io_service& io_service, unsigned short port) : _io_service(io_service), _IBaseNetworkDriver(netevent), _Listenport(port) { }
					virtual ~WebSocketListinerImpl() {
						Stop();
					}

					virtual void OnConnect(const std::shared_ptr<ISocket>& socket) override {
						std::cout << "websocket OnConnect" << std::endl;
						std::string msg = "Hello From Websocket Server";
						Packet p(static_cast<unsigned int>(PACKET_TYPES::WEBSOCKET_MSG), static_cast<unsigned int>(msg.size()));
						memcpy(p.Payload, msg.c_str(), msg.size());
						socket->send(p);
					}
					virtual void OnReceive(const std::shared_ptr<ISocket>& socket, std::shared_ptr<Packet>& packet)  override {
						std::cout << "websocket OnReceive" << std::endl;
					}
					virtual void OnClose(const std::shared_ptr<ISocket>& socket)  override {
						std::cout << "websocket Close" << std::endl;
					}
					void Start() {
						_TCPListener = std::make_shared<TCPListener<socket, WebSocket<socket>>>(this, _Listenport, _io_service);
						_TCPListener->Start();
					}
					void Stop() {
						_TCPListener.reset();
					}
				};
			}
		}
	}
}


SL::Remote_Access_Library::Network::WebSocketListener::WebSocketListener(IBaseNetworkDriver* netevent, boost::asio::io_service& io_service, unsigned short listenport)
{
	_WebSocketListinerImpl = new INTERNAL::WebSocketListinerImpl(netevent, io_service, listenport);
}

SL::Remote_Access_Library::Network::WebSocketListener::~WebSocketListener()
{
	delete _WebSocketListinerImpl;
}

void SL::Remote_Access_Library::Network::WebSocketListener::Start()
{
	_WebSocketListinerImpl->Start();
}

void SL::Remote_Access_Library::Network::WebSocketListener::Stop()
{
	_WebSocketListinerImpl->Stop();
}

