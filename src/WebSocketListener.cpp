#include "WebSocketListener.h"
#include "WebSocket.h"
#include "Configs.h"
#include "Logging.h"
#include "ICryptoLoader.h"
#include "ISocket.h"

#include <assert.h>


namespace SL {
	namespace RAT {

		//This can be used in production, the dh params do not need to be kept secret. Below is a 3072 bit dhparams
		const char dhparams[] = R"(-----BEGIN DH PARAMETERS-----
MIIBCAKCAQEAzPjrDCNwq0bYz5xi72GM4EoWjqwmaAXFcY7vR6+nDpeeKpeYg8XS
tdjCwyaIRaYO3tzZxTbjdgnCbksVQrRKscefnba5zguqnqvaGL7rHNwdqaQq7rTp
KGdva8BKIaE9fq2q0OaZpBWE7KtVdraF5+CnvEj4AJqxGgZ/OtP+Y3UPTIcjoIve
2ss+XbvHGvcZ+RmyYeHmmQbWyqNyoUgrMzfGOHUvY6x9fcl/DvCSIpVn8qRK+3+n
64R3OAMcxNK7ONGSL3q6DRVTUXTNch9W+TKOYAppKtdyemoUAMleXD3F282BUwqL
f4pf5b+c+w+99vHpUlkbIzV0tI5vGZo1uwIBAg==
-----END DH PARAMETERS-----)";


		WebSocketListener::WebSocketListener(std::shared_ptr<Server_Config>& c) :context_(boost::asio::ssl::context::tlsv11), acceptor_(ios_), work_(ios_), Server_Config_(c)
		{
			SL_RAT_LOG(Logging_Levels::INFO_log_level, "WebSocket Starting ");
			thread_.reserve(Server_Config_->MaxWebSocketThreads);
			for (std::size_t i = 0; i < Server_Config_->MaxWebSocketThreads; ++i)
				thread_.emplace_back([&] { ios_.run(); });

			boost::asio::ip::tcp::endpoint tcp(boost::asio::ip::tcp::v4(), Server_Config_->WebSocketTLSLPort);
			context_.set_options(
				boost::asio::ssl::context::default_workarounds
				| boost::asio::ssl::context::no_sslv2 | boost::asio::ssl::context::no_sslv3
				| boost::asio::ssl::context::single_dh_use);
			boost::system::error_code ec;
			context_.set_password_callback([=]() { return Server_Config_->PasswordToPrivateKey;  }, ec);
			if (ec) SL_RAT_LOG(Logging_Levels::ERROR_log_level, "set_password_callback error " << ec.message());
			ec.clear();
			boost::asio::const_buffer dhp(dhparams, sizeof(dhparams));

			context_.use_tmp_dh(dhp, ec);
			if (ec) SL_RAT_LOG(Logging_Levels::ERROR_log_level, "use_tmp_dh error " << ec.message());
			ec.clear();

			boost::asio::const_buffer certficatebuffer(Server_Config_->Public_Certficate->get_buffer(), Server_Config_->Public_Certficate->get_size());
			context_.use_certificate_chain(certficatebuffer, ec);
			if (ec) SL_RAT_LOG(Logging_Levels::ERROR_log_level, "use_certificate_chain error " << ec.message());
			ec.clear();

			context_.set_default_verify_paths(ec);
			if (ec) SL_RAT_LOG(Logging_Levels::ERROR_log_level, "set_default_verify_paths error " << ec.message());
			ec.clear();

			boost::asio::const_buffer privkey(Server_Config_->Private_Key->get_buffer(), Server_Config_->Private_Key->get_size());
			context_.use_private_key(privkey, boost::asio::ssl::context::pem, ec);
			if (ec) SL_RAT_LOG(Logging_Levels::ERROR_log_level, "use_private_key error " << ec.message());
			ec.clear();

			acceptor_.open(tcp.protocol(), ec);
			if (ec)
			{
				SL_RAT_LOG(Logging_Levels::ERROR_log_level, "open: " << ec.message());
				return;
			}
			acceptor_.set_option(boost::asio::socket_base::reuse_address{ true });
			acceptor_.bind(tcp, ec);
			if (ec)
			{
				SL_RAT_LOG(Logging_Levels::ERROR_log_level, "bind: " << ec.message());
				return;
			}
			acceptor_.listen(boost::asio::socket_base::max_connections, ec);
			if (ec)
			{
				SL_RAT_LOG(Logging_Levels::ERROR_log_level, "listen: " << ec.message());
				return;
			}

		}
		void WebSocketListener::run() {
			auto self(shared_from_this());
			auto sock = std::make_shared<WebSocket>(context_, ios_);
		
			sock->onDisconnection(onDisconnection_);
			sock->onMessage(onMessage_);
			acceptor_.async_accept(sock->get_Socket().lowest_layer(), [self, sock](boost::system::error_code ec) {
				if (!self->acceptor_.is_open())
					return;
				if (ec == boost::asio::error::operation_aborted)
					return;
				if (ec)
				{
					SL_RAT_LOG(Logging_Levels::ERROR_log_level, "on_accept: " << ec.message());
					return;
				}

				sock->get_Socket().next_layer().async_handshake(boost::asio::ssl::stream_base::server, [self, sock](boost::system::error_code ec) {
					if (!self->acceptor_.is_open())
						return;
					if (ec == boost::asio::error::operation_aborted)
						return;
					if (ec)
					{
						SL_RAT_LOG(Logging_Levels::ERROR_log_level, "on_accept: " << ec.message());
						return;
					}
					self->onConnection_(std::dynamic_pointer_cast<ISocket>(sock));
					sock->read();
					self->run();

				});
			});
		}
		WebSocketListener::~WebSocketListener()
		{
			work_ = boost::none;
			boost::system::error_code ec;
			ios_.dispatch([&] { acceptor_.close(ec); });
			for (auto& t : thread_)
				t.join();
		}
	}
}