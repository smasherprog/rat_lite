#include "stdafx.h"
#include "HttpsServerNetworkDriver.h"
#include "IHttpsDriver.h"
#include "HttpsSocket.h"
#include "HttpHeader.h"
#include "Server_Config.h"
#include "Logging.h"
#include "Packet.h"
#include "MediaTypes.h"

#ifndef __ANDROID__
#include <boost/filesystem.hpp>
#endif

#include <string>
#include <algorithm>

namespace SL {
	namespace Remote_Access_Library {
		namespace Network {


			class HttpsServerNetworkDriverImpl : public IHttpsDriver {
			public:

				std::shared_ptr<HttpsListener> _Listener;

				IHttpsDriver* _IHttpsDriver;
				std::shared_ptr<Network::Server_Config> _config;
		
				HttpsServerNetworkDriverImpl(IHttpsDriver* netevent, std::shared_ptr<Network::Server_Config> config) : _IHttpsDriver(netevent),_config(config) {
					if (_config->WWWRoot.empty()) {
						_config->WWWRoot = "/wwwwroot/";
					}
					else if (_config->WWWRoot.back() == '/' || _config->WWWRoot.back() == '\\') {
						_config->WWWRoot.pop_back();
					}
#ifndef __ANDROID__
					try {
						boost::filesystem::path p(boost::filesystem::canonical(_config->WWWRoot));
						_config->WWWRoot = p.string();
					}
					catch (std::exception ex) {
						SL_RAT_LOG(Utilities::FATAL_log_level, "No wwwroot Folder Found!");
					}
#endif
				}
				virtual ~HttpsServerNetworkDriverImpl() {
					Stop();
				}
				virtual bool ValidateUntrustedCert(const std::shared_ptr<ISocket>& socket) override {
					UNUSED(socket);
					return true;
				}
				virtual void OnConnect(const std::shared_ptr<ISocket>& socket) override {
					SL_RAT_LOG(Utilities::Logging_Levels::INFO_log_level, "HTTP OnConnect");
				}
				//below will need to be moved out into its own class, but for now this is faster
				virtual void OnReceive(const std::shared_ptr<ISocket>& socket, std::shared_ptr<Packet>& packet)  override {
					UNUSED(socket);
					auto requestedpath = packet->get_HeaderValue(HTTP_PATH);
					
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
				virtual void OnClose(const ISocket* socket)  override {
					UNUSED(socket);
					SL_RAT_LOG(Utilities::Logging_Levels::INFO_log_level, "HTTP Close");
				}
#ifdef __ANDROID__

				Packet GetContent(std::string path) {
					return Get404Page();
				}

#else 

				Packet GetContent(std::string path) {
					SL_RAT_LOG(Utilities::Logging_Levels::INFO_log_level, "HTTP GetContent " << path);
					if (path == "/") path = _config->WWWRoot + "/index.html";
					else path = _config->WWWRoot + path;
					try {
						boost::filesystem::path p(boost::filesystem::canonical(path, _config->WWWRoot));
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

								pack.Header[HTTP_STATUSCODE] = "200 OK";
								pack.Header[HTTP_VERSION] = "HTTP/1.1";
								pack.Header[HTTP_CONTENTTYPE] = Utilities::GetMimeType(path);

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
								pack.Header[HTTP_LASTMODIFIED] = buffer;
								auto ext = p.extension();
								if (ext == ".png" || ext == ".jpg") {
									pack.Header[HTTP_CACHECONTROL] = "max-age=3600";//set a longer cache timeout for images
								}
								return pack;
							}

						}
					}
					catch (std::exception e) {
						SL_RAT_LOG(Utilities::Logging_Levels::ERROR_log_level, e.what());
					}
					return Get404Page();
				}
#endif
				Packet Get404Page() {
					SL_RAT_LOG(Utilities::Logging_Levels::INFO_log_level, "HTTP OnReceive Get404Page");

					std::string sHTML = "<html><body><h1>404 Not Found</h1><p>There's nothing here.</p></body></html>";
					Packet pack(static_cast<unsigned int>(PACKET_TYPES::HTTP_MSG), static_cast<unsigned int>(sHTML.size()));
					memcpy(pack.Payload, sHTML.c_str(), sHTML.size());
					pack.Header[HTTP_STATUSCODE] = "404 Not Found";
					pack.Header[HTTP_VERSION] = "HTTP/1.1";
					pack.Header[HTTP_CONTENTTYPE] = "text/html";
					return pack;
				}
				void Start() {
					if (_config->HttpTLSPort > 0) {
						SL_RAT_LOG(Utilities::Logging_Levels::INFO_log_level, "Starting TLS http socket Listening on port " << _config->HttpTLSPort);
						_Listener = std::make_shared<HttpsListener>(this, _config);
					}
				}
				void Stop() {
					if(_Listener) _Listener.reset();
				}
			};
		}
	}
}



SL::Remote_Access_Library::Network::HttpsServerNetworkDriver::HttpsServerNetworkDriver(IHttpsDriver* netevent, std::shared_ptr<Network::Server_Config> config)
{
	_HttpsServerNetworkDriverImpl = new HttpsServerNetworkDriverImpl(netevent, config);
}

SL::Remote_Access_Library::Network::HttpsServerNetworkDriver::~HttpsServerNetworkDriver()
{
	_HttpsServerNetworkDriverImpl->Stop();
}

void SL::Remote_Access_Library::Network::HttpsServerNetworkDriver::Start()
{
	_HttpsServerNetworkDriverImpl->Start();
}

void SL::Remote_Access_Library::Network::HttpsServerNetworkDriver::Stop()
{
	_HttpsServerNetworkDriverImpl->Stop();
}
