#include "stdafx.h"
#include "HttpListener.h"
#include "ApplicationDirectory.h"
#include "TCPListener.h"
#include "HttpSocket.h"
#include "Packet.h"
#include "IBaseNetworkDriver.h"
#include "HttpHeader.h"

namespace SL {
	namespace Remote_Access_Library {
		namespace Network {
			namespace INTERNAL {

				class HttpServerImpl : public IBaseNetworkDriver {
				public:
					std::shared_ptr<TCPListener<socket, HttpSocket<socket>>> _TCPListener;
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
						if (exeindex == searchpath.npos) {
							auto p(Get404Page());
							return socket->send(p);
						}

						auto requestedpath = packet->Header[HttpHeader::HTTP_PATH];
						if (requestedpath.find_last_of(".ico") != std::string::npos) {
							auto p(GetFavIcon(searchpath.substr(0, exeindex)));
							return socket->send(p);
						}
						else {
							auto p(GetIndexPage(searchpath.substr(0, exeindex)));
							return socket->send(p);
						}
					
					}
					virtual void OnClose(const std::shared_ptr<ISocket>& socket)  override {
						std::cout << "HTTP Close" << std::endl;
					}

					Packet GetIndexPage(std::string path) {
						std::cout << "HTTP OnReceive GetIndexPage" << std::endl;
						path = path + "\\index.html";
						std::ifstream file(path.c_str(), std::ios::binary);
						if (file.is_open()) {
							file.seekg(0, std::ios_base::end);
							Packet pack(static_cast<unsigned int>(PACKET_TYPES::HTTP_MSG), static_cast<unsigned int>(file.tellg()));
							file.seekg(0);//goto begining
							file.read(pack.Payload, pack.Payload_Length);
							std::string rd(pack.Payload, pack.Payload_Length);
							pack.Header[HttpHeader::HTTP_STATUSCODE] = "200 OK";
							pack.Header[HttpHeader::HTTP_VERSION] = "HTTP/1.1";
							pack.Header[HttpHeader::HTTP_CONTENTTYPE] = "text/html";
							return pack;
						}
						else return Get404Page();
					}
					Packet GetFavIcon(std::string path) {
						std::cout << "HTTP OnReceive favicon" << std::endl;
						path = path + "\\favicon.ico";
						std::ifstream file(path.c_str(), std::ios::binary);
						if (file.is_open()) {
							file.seekg(0, std::ios_base::end);
							Packet pack(static_cast<unsigned int>(PACKET_TYPES::HTTP_MSG), static_cast<unsigned int>(file.tellg()));
							file.seekg(0);//goto begining
							file.read(pack.Payload, pack.Payload_Length);
							std::string rd(pack.Payload, pack.Payload_Length);
							pack.Header[HttpHeader::HTTP_STATUSCODE] = "200 OK";
							pack.Header[HttpHeader::HTTP_VERSION] = "HTTP/1.1";
							pack.Header[HttpHeader::HTTP_CONTENTTYPE] = "image/vnd.microsoft.icon";
							return pack;
						}
						else return Get404Page();

					}
					Packet Get404Page() {
						std::cout << "HTTP OnReceive Get404Page" << std::endl;
						std::string sHTML = "<html><body><h1>404 Not Found</h1><p>There's nothing here.</p></body></html>";
						Packet pack(static_cast<unsigned int>(PACKET_TYPES::HTTP_MSG), static_cast<unsigned int>(sHTML.size()));
						std::copy(begin(sHTML), end(sHTML), pack.Payload);
						pack.Header[HttpHeader::HTTP_STATUSCODE] = "404 Not Found";
						pack.Header[HttpHeader::HTTP_VERSION] = "HTTP/1.1";
						pack.Header[HttpHeader::HTTP_CONTENTTYPE] = "text/html";
						return pack;
					}
					void Start() {
						_TCPListener = std::make_shared<TCPListener<socket, HttpSocket<socket>>>(this, _Listenport, _io_service);
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

