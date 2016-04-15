#include "stdafx.h"
#include "WebSocketListener.h"
#include "TCPListener.h"
#include "WebSocket.h"
#include "IBaseNetworkDriver.h"
#include "Packet.h"
#include "Server_Config.h"
#include "Logging.h"

namespace SL {
	namespace Remote_Access_Library {
		namespace Network {
			namespace INTERNAL {
				class WebSocketListinerImpl : public IBaseNetworkDriver {
				public:
					std::shared_ptr<TCPListener<socket, WebSocket<socket>>> _TCPListener;
					std::shared_ptr<TCPListener<ssl_socket, WebSocket<ssl_socket>>> _TLSTCPListener;
					Server_Config _config;
					IBaseNetworkDriver* _IBaseNetworkDriver;
					asio::io_service& _io_service;
					WebSocketListinerImpl(IBaseNetworkDriver* netevent, asio::io_service& io_service, Server_Config& config) :
						_config(config), _IBaseNetworkDriver(netevent),  _io_service(io_service){ }
					virtual ~WebSocketListinerImpl() {
						Stop();
					}

					virtual void OnConnect(const std::shared_ptr<ISocket>& socket) override {
						SL_RAT_LOG("websocket OnConnect", Utilities::Logging_Levels::INFO_log_level);
						socket->set_ReadTimeout(_config.Read_Timeout);
						socket->set_WriteTimeout(_config.Write_Timeout);
						_IBaseNetworkDriver->OnConnect(socket);
				
						/*	std::string testing = "sdfdf";
							Packet p(static_cast<unsigned int>(PACKET_TYPES::WEBSOCKET_TEXT), testing.size());
							memcpy(p.Payload, testing.c_str(), testing.size());
							socket->send(p);*/
					}
					virtual void OnReceive(const std::shared_ptr<ISocket>& socket, std::shared_ptr<Packet>& packet)  override {
					_IBaseNetworkDriver->OnReceive(socket, packet);
					}
					virtual void OnClose(const std::shared_ptr<ISocket>& socket)  override {
						SL_RAT_LOG("websocket Close", Utilities::Logging_Levels::INFO_log_level);
						_IBaseNetworkDriver->OnClose(socket);
					}
					void Start() {
						if (_config.WebSocketListenPort > 0) {
							SL_RAT_LOG(std::string("Starting Web socket Listening on port ") + std::to_string(_config.WebSocketListenPort), Utilities::Logging_Levels::INFO_log_level);
							_TCPListener = std::make_shared<TCPListener<socket, WebSocket<socket>>>(this, _config.WebSocketListenPort, _io_service);
							_TCPListener->Start();
						}
						if (_config.WebSocketTLSListenPort > 0) {
							SL_RAT_LOG(std::string("Starting TLS Web socket Listening on port ") + std::to_string( _config.WebSocketTLSListenPort), Utilities::Logging_Levels::INFO_log_level);
	
							_TLSTCPListener = std::make_shared<TCPListener<ssl_socket, WebSocket<ssl_socket>>>(this, _config.WebSocketTLSListenPort, _io_service);
							_TLSTCPListener->Start();
						}

					}
					void Stop() {
						_TCPListener.reset();
						_TLSTCPListener.reset();
					}
				};
			}
		}
	}
}


SL::Remote_Access_Library::Network::WebSocketListener::WebSocketListener(IBaseNetworkDriver* netevent, asio::io_service& io_service, Server_Config& config)
{
	_WebSocketListinerImpl = new INTERNAL::WebSocketListinerImpl(netevent, io_service, config);
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

