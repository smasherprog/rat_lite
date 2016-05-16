#pragma once


namespace boost { namespace asio { class io_service; } }
namespace SL {
	namespace Remote_Access_Library {
		namespace Network {
			namespace INTERNAL {
				class HttpServerImpl;
			}
			class IBaseNetworkDriver;
			struct Server_Config;
			class HttpListener {
				INTERNAL::HttpServerImpl* _HttpServerImpl;
			public:

				HttpListener(IBaseNetworkDriver* netevent, boost::asio::io_service& io_service, std::shared_ptr<Network::Server_Config> config);
				//no copy allowed
				HttpListener(const HttpListener&) = delete;
				HttpListener(HttpListener&&) = delete;
				//no copy allowed
				HttpListener& operator=(HttpListener&&) = delete;
				HttpListener& operator=(const HttpListener&) = delete;
				~HttpListener();
				void Start();
				void Stop();
			
			};
		}
	}
}
