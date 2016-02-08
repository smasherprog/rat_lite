#include "stdafx.h"
#include "HttpListener.h"
#include "ApplicationDirectory.h"
#include "TCPListener.h"
#include "HttpSocket.h"
#include "Packet.h"
#include "IBaseNetworkDriver.h"

namespace SL {
	namespace Remote_Access_Library {
		namespace Network {
			namespace INTERNAL {

				class HttpServerImpl : public IBaseNetworkDriver {
				public:
					std::shared_ptr<TCPListener> _TCPListener;
					IBaseNetworkDriver* _IBaseNetworkDriver;
					boost::asio::io_service& _io_service;
					unsigned short _Listenport;
					HttpServerImpl(IBaseNetworkDriver* netevent, boost::asio::io_service& io_service, unsigned short port) : _io_service(io_service), _IBaseNetworkDriver(netevent), _Listenport(port) { }
					virtual ~HttpServerImpl() {
						Stop();
					}

					virtual void OnConnect(const std::shared_ptr<ISocket>& socket) override {
						std::cout << "HTTP OnConnect" << std::endl;

					}
					virtual void OnReceive(const std::shared_ptr<ISocket>& socket, std::shared_ptr<Packet>& packet)  override {
						auto searchpath = executable_path(nullptr);
						auto exeindex = searchpath.find_last_of('\\');
						if (exeindex == searchpath.npos) exeindex = searchpath.find_last_of('/');
						if (exeindex == searchpath.npos) return socket->send(Get404Page());
						socket->send(GetIndexPage(searchpath.substr(0, exeindex)));

						std::cout << "HTTP OnReceive" << std::endl;
					}
					virtual void OnClose(const std::shared_ptr<ISocket>& socket)  override {
						std::cout << "HTTP Close" << std::endl;
					}
					
					Packet GetIndexPage(std::string path) {
						path = path + "\\index.html";
						std::ifstream file(path.c_str());
						if (file.is_open()) {
							file.seekg(0, std::ios_base::end);
							Packet pack(static_cast<unsigned int>(PACKET_TYPES::HTTP_MSG), file.tellg());
							file.seekg(0);//goto begining
							file.read(pack.Payload, pack.Payload_Length);
							std::string rd(pack.Payload, pack.Payload_Length);
							pack.Header[HTTP_STATUSCODE] = "200 OK";
							pack.Header[HTTP_VERSION] = "HTTP/1.1";
							pack.Header[HTTP_CONTENTTYPE] = "text/html";
							return pack;
						}
						else return Get404Page();

					}
					Packet Get404Page() {

						std::string sHTML ="<html><body><h1>404 Not Found</h1><p>There's nothing here.</p></body></html>";
						Packet pack(static_cast<unsigned int>(PACKET_TYPES::HTTP_MSG), sHTML.size());
						std::copy(begin(sHTML), end(sHTML), pack.Payload);
						pack.Header[HTTP_STATUSCODE] = "404 Not Found";
						pack.Header[HTTP_VERSION] = "HTTP/1.1";
						pack.Header[HTTP_CONTENTTYPE] = "text/html";
						return pack;
					}
					void Start() {
						_TCPListener = TCPListener::Create(_Listenport, _io_service, [this](void* socket) {
							std::make_shared<HttpSocket>(this, socket)->connect(nullptr, nullptr);
						});
						_TCPListener->Start();
					}
					void Stop() {
						_TCPListener->Stop();
					}

				};

			}
		}
	}
}


SL::Remote_Access_Library::Network::HttpListener::HttpListener(IBaseNetworkDriver* netevent, boost::asio::io_service& io_service, unsigned short listenport)
{
	_HttpServerImpl = new INTERNAL::HttpServerImpl(netevent, io_service, listenport);
}

SL::Remote_Access_Library::Network::HttpListener::~HttpListener()
{
	delete _HttpServerImpl;
}

void SL::Remote_Access_Library::Network::HttpListener::Start()
{
	_HttpServerImpl->Start();
}

void SL::Remote_Access_Library::Network::HttpListener::Stop()
{
	_HttpServerImpl->Stop();
}

