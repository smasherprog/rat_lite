#pragma once



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

				HttpListener(IBaseNetworkDriver* netevent, void* io_service);
				~HttpListener();
				void Start();
				void Stop();

			};
		}
	}
}
