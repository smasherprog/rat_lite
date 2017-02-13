#pragma once
#include <memory>
#include <functional>
#include <vector>
#if defined(_WIN32)
#if (_MSC_VER >= 1700) && defined(_USING_V110_SDK71_)
//windows xp
#define _WIN32_WINNT 0x0501
#else
#define _WIN32_WINNT 0x0601
#endif
#endif


#include <beast/websocket.hpp>
#include <beast/websocket/ssl.hpp>
#include <beast/core/placeholders.hpp>
#include <beast/core/streambuf.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/optional.hpp>

#include "Configs.h"

namespace SL {
	namespace RAT {
		class ISocket;
	

		class WebSocketListener : public std::enable_shared_from_this<WebSocketListener> {
			boost::asio::io_service ios_;
			boost::asio::ip::tcp::acceptor acceptor_;
			std::vector<std::thread> thread_;
			boost::optional<boost::asio::io_service::work> work_;
			boost::asio::ssl::context context_;

			std::shared_ptr<Server_Config> Server_Config_;

			std::function<void(const std::shared_ptr<ISocket>&)> onConnection_;
			std::function<void(const std::shared_ptr<ISocket>&, const char*, size_t)> onMessage_;
			std::function<void(const ISocket* socket)> onDisconnection_;
		public:

			WebSocketListener(std::shared_ptr<Server_Config>& c);
			~WebSocketListener();
		
			void onConnection(const std::function<void(const std::shared_ptr<ISocket>&)>& func) { onConnection_ = func; }
			void onMessage(const std::function<void(const std::shared_ptr<ISocket>&, const char*, size_t)>& func) { onMessage_ = func; }
			void onDisconnection(const std::function<void(const ISocket* socket)>& func) { onDisconnection_ = func; }
			void run();

		};



	}
}