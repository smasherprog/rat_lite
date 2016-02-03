#include "stdafx.h"
#include "HttpListener.h"
#include "ApplicationDirectory.h"
#include "TCPListener.h"
#include "HttpSocket.h"
#include "IBaseNetworkDriver.h"
#include "Packet.h"


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
						auto htmlfile = executable_path(nullptr);
						auto exeindex = htmlfile.find_last_of('\\');
						if (exeindex == htmlfile.npos) exeindex = htmlfile.find_last_of('/');
						if (exeindex == htmlfile.npos) return socket->send(Get404Page());
						socket->send(GetIndexPage(htmlfile.substr(0, exeindex)));


					}
					virtual void OnReceive(const std::shared_ptr<ISocket>& socket, std::shared_ptr<Packet>& packet)  override {
						std::cout << "HTTP OnReceive" << std::endl;
					}
					virtual void OnClose(const std::shared_ptr<ISocket>& socket)  override {
						std::cout << "HTTP Close" << std::endl;
					}
					std::shared_ptr<Packet> GetIndexPage(std::string path) {
						path = path + "\\index.html";
						std::ifstream file(path.c_str());
						if (file.is_open()) {
							std::string sHTML((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
							std::string secondpart = "HTTP/1.1 200 OK\r\n";
							secondpart += "content-type: text/html\r\n";
							secondpart += "content-length: " + std::to_string(sHTML.length()) + "\r\n\r\n";
							secondpart += sHTML;
							PacketHeader h;
							h.PayloadLen =static_cast<unsigned int>( sHTML.size());
							auto pack = Packet::CreatePacket(h);
							memcpy(pack->data(), sHTML.c_str(), sHTML.size());
							return pack;
						}
						else return Get404Page();

					}
					std::shared_ptr<Packet> Get404Page() {

						std::string sHTML =
							"<html><body><h1>404 Not Found</h1><p>There's nothing here.</p></body></html>";
						std::string secondpart = "HTTP/1.1 404 Not Found\r\n";
						secondpart += "content-type: text/html\r\n";
						secondpart += "content-length: " + std::to_string(sHTML.length()) + "\r\n\r\n";
						secondpart += sHTML;
						PacketHeader h;
						h.PayloadLen = static_cast<unsigned int>(sHTML.size());
						auto pack = Packet::CreatePacket(h);
						memcpy(pack->data(), sHTML.c_str(), sHTML.size());
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

