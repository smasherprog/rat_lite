#include "stdafx.h"
#include "HttpListener.h"
#include "TCPListener.h"
#include "HttpSocket.h"
#include "Packet.h"
#include "IBaseNetworkDriver.h"
#include "HttpHeader.h"
#include "MediaTypes.h"
#include "Server_Config.h"
#include "Packet.h"

#ifndef __ANDROID__
#include <boost/filesystem.hpp>
#endif

namespace SL {
	namespace Remote_Access_Library {
		namespace Network {
			namespace INTERNAL {

				class HttpServerImpl : public IBaseNetworkDriver {
				public:
					std::shared_ptr<TCPListener<socket, HttpSocket<socket>>> _TCPListener;
					std::shared_ptr<TCPListener<ssl_socket, HttpSocket<ssl_socket>>> _TLSTCPListener;
					IBaseNetworkDriver* _IBaseNetworkDriver;
					asio::io_service& _io_service;
					Server_Config _config;

					HttpServerImpl(IBaseNetworkDriver* netevent, asio::io_service& io_service, Server_Config& config) :
						_IBaseNetworkDriver(netevent), _io_service(io_service), _config(config) {
						if (_config.WWWRoot.empty()) {
							_config.WWWRoot = "/wwwwroot/";
						}
						else if (_config.WWWRoot.back() == '/' || _config.WWWRoot.back() == '\\') {
							_config.WWWRoot.pop_back();
						}
#ifndef __ANDROID__
						boost::filesystem::path p(boost::filesystem::canonical(_config.WWWRoot));
						_config.WWWRoot = p.string();
#endif
					}
					virtual ~HttpServerImpl() {
						Stop();
					}

					virtual void OnConnect(const std::shared_ptr<ISocket>& socket) override {
						socket->set_ReadTimeout(_config.Read_Timeout);
						socket->set_WriteTimeout(_config.Write_Timeout);
						SL_RAT_LOG("HTTP OnConnect", Utilities::Logging_Levels::INFO_log_level);
					}
					//below will need to be moved out into its own class, but for now this is faster
					virtual void OnReceive(const std::shared_ptr<ISocket>& socket, std::shared_ptr<Packet>& packet)  override {
						UNUSED(socket);
						auto requestedpath = packet->Header[HttpHeader::HTTP_PATH];
						
						auto params = requestedpath.find('?');
						if (params != requestedpath.npos) {
							requestedpath = requestedpath.substr(0, params);
						}
						
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
						SL_RAT_LOG("HTTP Close", Utilities::Logging_Levels::INFO_log_level);
					}
#ifdef __ANDROID__

					Packet GetContent(std::string path) {
						return Get404Page();
					}

#else 

					Packet GetContent(std::string path) {
						SL_RAT_LOG(std::string("HTTP GetContent ") + path, Utilities::Logging_Levels::INFO_log_level);
						if (path == "/") path = _config.WWWRoot + "/index.html";
						else path = _config.WWWRoot + path;
						try {
							boost::filesystem::path p(boost::filesystem::canonical(path, _config.WWWRoot));
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

									std::time_t t = boost::filesystem::last_write_time(path);

									struct tm buf;
									char buffer[80];
									memset(buffer, 0, sizeof(buffer));
#if _WIN32
									gmtime_s(&buf, &t);
#else 
									gmtime_r(&t, &buf);
#endif


									strftime(buffer, 80, "%a, %d %b %G %R GMT", &buf);
									pack.Header[HttpHeader::HTTP_LASTMODIFIED] = buffer;
									auto ext = p.extension();
									if (ext == ".png" || ext == ".jpg") {
										pack.Header[HttpHeader::HTTP_CACHECONTROL] = "max-age=3600";//set a longer cache timeout for images
									}
									return pack;
								}

							}
						}
						catch (std::exception e) {
							SL_RAT_LOG(e.what(), Utilities::Logging_Levels::ERROR_log_level);
						}
						return Get404Page();
					}
#endif
					Packet Get404Page() {
						SL_RAT_LOG("HTTP OnReceive Get404Page", Utilities::Logging_Levels::INFO_log_level);

						std::string sHTML = "<html><body><h1>404 Not Found</h1><p>There's nothing here.</p></body></html>";
						Packet pack(static_cast<unsigned int>(PACKET_TYPES::HTTP_MSG), static_cast<unsigned int>(sHTML.size()));
						memcpy(pack.Payload, sHTML.c_str(), sHTML.size());
						pack.Header[HttpHeader::HTTP_STATUSCODE] = "404 Not Found";
						pack.Header[HttpHeader::HTTP_VERSION] = "HTTP/1.1";
						pack.Header[HttpHeader::HTTP_CONTENTTYPE] = "text/html";
						return pack;
					}
					void Start() {
						if (_config.HttpListenPort > 0) {
							SL_RAT_LOG(std::string("Starting http socket Listening on port ") + std::to_string(_config.HttpListenPort), Utilities::Logging_Levels::INFO_log_level);
							_TCPListener = std::make_shared<TCPListener<socket, HttpSocket<socket>>>(this, _config.HttpListenPort, _io_service);
							_TCPListener->Start();
						}
						if (_config.HttpTLSListenPort > 0) {
							SL_RAT_LOG(std::string("Starting TLS http socket Listening on port ") + std::to_string( _config.HttpTLSListenPort), Utilities::Logging_Levels::INFO_log_level);

							_TLSTCPListener = std::make_shared<TCPListener<ssl_socket, HttpSocket<ssl_socket>>>(this, _config.HttpTLSListenPort, _io_service);
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


SL::Remote_Access_Library::Network::HttpListener::HttpListener(IBaseNetworkDriver* netevent, asio::io_service& io_service, Server_Config& config)
{
	_HttpServerImpl = new INTERNAL::HttpServerImpl(netevent, io_service, config);
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

