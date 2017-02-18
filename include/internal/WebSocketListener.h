#pragma once
#include <memory>
#include <functional>
#include <vector>

#include "uWS.h"
#include "Configs.h"

namespace SL {
	namespace RAT {
		class ISocket;
	

		class WebSocketListener {
			std::vector<std::thread> thread_;
			uS::TLS::Context Context_;

			std::shared_ptr<Server_Config> Server_Config_;

			std::function<void(const std::shared_ptr<ISocket>&)> onConnection_;
			std::function<void(const std::shared_ptr<ISocket>&, const char*, size_t)> onMessage_;
			std::function<void(const ISocket* socket)> onDisconnection_;
		public:

			WebSocketListener(std::shared_ptr<Server_Config>& c):Server_Config_(c){
				uS::TLS::Context c = uS::TLS::createContext("ssl/cert.pem",
					"ssl/key.pem",
					"1234");

			}
			~WebSocketListener();
		
			void onConnection(const std::function<void(const std::shared_ptr<ISocket>&)>& func) { onConnection_ = func; }
			void onMessage(const std::function<void(const std::shared_ptr<ISocket>&, const char*, size_t)>& func) { onMessage_ = func; }
			void onDisconnection(const std::function<void(const ISocket* socket)>& func) { onDisconnection_ = func; }
			void run();

		};



	}
}