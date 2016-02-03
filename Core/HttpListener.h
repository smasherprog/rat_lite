#pragma once


namespace boost { namespace asio { class io_service; } }
namespace SL {
	namespace Remote_Access_Library {
		namespace Network {
			namespace INTERNAL {
				class HttpServerImpl;
			}
			class IBaseNetworkDriver;
			class HttpListener {
				INTERNAL::HttpServerImpl* _HttpServerImpl;
			public:

				HttpListener(IBaseNetworkDriver* netevent, boost::asio::io_service& io_service, unsigned short listenport=8080);	
				HttpListener(const HttpListener&) = delete;
				HttpListener& operator=(const HttpListener&) = delete;
				~HttpListener();
				void Start();
				void Stop();
			
			};
		}
	}
}
