#include "stdafx.h"
#include "HttpListener.h"
#include "TCPListener.h"
#include "HttpSocket.h"
#include "Packet.h"
#include "IBaseNetworkDriver.h"
#include "HttpHeader.h"
#include "MediaTypes.h"
#include <boost/filesystem.hpp>

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
					std::string WWWRoot;//this is the search folder for files.. EVERYTHING WILL BE ISSUED OUT IN THE FOLDER SO BE CAREFUL!
					HttpServerImpl(IBaseNetworkDriver* netevent, boost::asio::io_service& io_service, unsigned short port, std::string wwwroot) :
						_IBaseNetworkDriver(netevent),_io_service(io_service),  _Listenport(port), WWWRoot(wwwroot) {
						if (WWWRoot.back() == '/' || WWWRoot.back() == '\\') {
							WWWRoot.pop_back();
						}
					}
					virtual ~HttpServerImpl() {
						Stop();
					}

					virtual void OnConnect(const std::shared_ptr<ISocket>& socket) override {
						UNUSED(socket);
						std::cout << "HTTP OnConnect" << std::endl;

					}
					//below will need to be moved out into its own class, but for now this is faster
					virtual void OnReceive(const std::shared_ptr<ISocket>& socket, std::shared_ptr<Packet>& packet)  override {
						UNUSED(socket);
						auto requestedpath = packet->Header[HttpHeader::HTTP_PATH];
						//sanitize path below. SImple for right now, 
						std::transform(begin(requestedpath), end(requestedpath), begin(requestedpath), [](const char& elem) {
							if ((elem >= 'a' && elem <= 'z') || (elem >= 'A' && elem <= 'Z') || (elem >= '0' && elem <= '9') || elem == ' ' || elem == '_' || elem == '.' || elem == '/' || elem == '\\') {
								return elem;
							}
							else return '_';
						});
						auto sendpack(GetContent(requestedpath));
						return socket->send(sendpack);

					}
					virtual void OnClose(const std::shared_ptr<ISocket>& socket)  override {
						UNUSED(socket);
						std::cout << "HTTP Close" << std::endl;
					}
					Packet GetContent(std::string path) {
						std::cout << "HTTP GetContent " << path << std::endl;
						if (path == "/") path = WWWRoot+"/index.html";
						else path = WWWRoot + path;
						try {
							boost::filesystem::path p(boost::filesystem::canonical(path, WWWRoot));
							auto tesert = p.string();
							if (boost::filesystem::exists(p) && boost::filesystem::is_regular_file(p))
							{

								std::ifstream file(path.c_str(), std::ios::binary);
								if (file.is_open()) {
									file.seekg(0, std::ios_base::end);
									Packet pack(static_cast<unsigned int>(PACKET_TYPES::HTTP_MSG), static_cast<unsigned int>(file.tellg()));
									file.seekg(0);//goto begining
									file.read(pack.Payload, pack.Payload_Length);
									std::string rd(pack.Payload, pack.Payload_Length);
									pack.Header[HttpHeader::HTTP_STATUSCODE] = "200 OK";
									pack.Header[HttpHeader::HTTP_VERSION] = "HTTP/1.1";
									pack.Header[HttpHeader::HTTP_CONTENTTYPE] = Utilities::GetMimeType(path);
									auto ext = p.extension();
									if (ext == ".png" || ext == ".jpg") {
										pack.Header[HttpHeader::HTTP_CACHECONTROL] = "max-age=3600";//set a longer cache timeout for images
									}
									return pack;
								}

							}
						}
						catch (std::exception e) {
							std::cout << __FILE__ << e.what() << std::endl;
						}
						return Get404Page();
					}
					Packet Get404Page() {
						std::cout << "HTTP OnReceive Get404Page" << std::endl;
						std::string sHTML = "<html><body><h1>404 Not Found</h1><p>There's nothing here.</p></body></html>";
						Packet pack(static_cast<unsigned int>(PACKET_TYPES::HTTP_MSG), static_cast<unsigned int>(sHTML.size()));
						memcpy(pack.Payload, sHTML.c_str(), sHTML.size());
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


SL::Remote_Access_Library::Network::HttpListener::HttpListener(IBaseNetworkDriver* netevent, boost::asio::io_service& io_service, std::string wwwroot, unsigned short listenport)
{
	_HttpServerImpl = new INTERNAL::HttpServerImpl(netevent, io_service, listenport, wwwroot);
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

