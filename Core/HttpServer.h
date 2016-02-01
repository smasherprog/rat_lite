#pragma once



namespace SL {
	namespace Remote_Access_Library {
		namespace Network {
			namespace INTERNAL {
				class HttpServerImpl;
			}
			class IBaseNetworkDriver;
			class HttpServer {
				INTERNAL::HttpServerImpl* _HttpServerImpl;
			public:

				HttpServer(IBaseNetworkDriver* netevent, void* io_service);
				~HttpServer();
				void Start();
				void Stop();

			};
		}
	}
}
