#include "WebSocketListener.h"
#include "Configs.h"
#include "Logging.h"
#include "ICryptoLoader.h"
#include "ISocket.h"

#include <vector>
#include <assert.h>
#include <beast/websocket.hpp>
#include <beast/websocket/ssl.hpp>
#include <beast/core/placeholders.hpp>
#include <beast/core/streambuf.hpp>

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>


namespace SL {
	namespace RAT {

		class WSSocketImpl : public ISocket, public std::enable_shared_from_this<WSSocketImpl> {
		private:

		public:

			WSSocketImpl(boost::asio::ssl::context& context, boost::asio::io_service& io_service) :
				_socket(io_service, context),
				_read_deadline(io_service),
				_write_deadline(io_service)
			{
				_read_deadline.expires_at(boost::posix_time::pos_infin);
				_write_deadline.expires_at(boost::posix_time::pos_infin);
				_Closed = false;
				_readtimeout = _writetimeout = 5;
			}


			virtual ~WSSocketImpl() {
				SL_RAT_LOG(Logging_Levels::INFO_log_level, "~WSSocketImpl");
				CancelTimers();
				close("~WSSocketImpl");
			}
			beast::websocket::stream<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>> _socket;

			std::function<void(const std::shared_ptr<ISocket>&, const char*, size_t)> onMessage;
			std::function<void(const ISocket* socket)> onDisconnection;

			boost::asio::deadline_timer _read_deadline;
			boost::asio::deadline_timer _write_deadline;
			boost::asio::streambuf db;
			beast::websocket::frame_info frame_;

			bool _Closed = true;
			bool _Writing = false;

			int _readtimeout = 5;
			int _writetimeout = 5;


			void CancelTimers()
			{
				_read_deadline.cancel();
				_write_deadline.cancel();
			}

			virtual void send(std::shared_ptr<char> data, size_t len) override {
				auto self(shared_from_this());

				_socket.async_write(boost::asio::buffer(data.get(), len), [data, self](const boost::system::error_code& ec) {
					if (ec)
					{
						self->close(std::string("readheader async_read ") + ec.message());
					}
				});

			}
			virtual void close(const std::string& reason) override {
				if (closed()) return;
				SL_RAT_LOG(Logging_Levels::INFO_log_level, "Closing socket: " << reason);
				_Closed = true;
				CancelTimers();

				onDisconnection(this);
				boost::system::error_code ec;
				_socket.close(beast::websocket::close_code::normal, ec);

				if (ec) SL_RAT_LOG(Logging_Levels::ERROR_log_level, ec.message());
				ec.clear();
				SL_RAT_LOG(Logging_Levels::INFO_log_level, "Socket Closed");
			}
			virtual bool closed() override {
				return _Closed || !_socket.lowest_layer().is_open();
			}


			//s in in seconds
			virtual void set_ReadTimeout(int s)  override {
				assert(s > 0);
				_readtimeout = s;

			}
			//s in in seconds
			virtual void set_WriteTimeout(int s) override {
				assert(s > 0);
				_writetimeout = s;

			}
			virtual std::string get_address() const override
			{
				boost::system::error_code ec;
				auto rt(_socket.lowest_layer().remote_endpoint(ec));
				if (!ec) return rt.address().to_string();
				else return "";
			}
			virtual unsigned short get_port() const override
			{
				boost::system::error_code ec;
				auto rt(_socket.lowest_layer().remote_endpoint(ec));
				if (!ec) return rt.port();
				else return 0;
			}
			virtual bool is_v4() const override
			{
				boost::system::error_code ec;
				auto rt(_socket.lowest_layer().remote_endpoint(ec));
				if (!ec) return rt.address().is_v4();
				else return true;
			}
			virtual bool is_v6() const override
			{
				boost::system::error_code ec;
				auto rt(_socket.lowest_layer().remote_endpoint(ec));
				if (!ec) return rt.address().is_v6();
				else return true;
			}
			virtual bool is_loopback() const override {
				boost::system::error_code ec;
				auto rt(_socket.lowest_layer().remote_endpoint(ec));
				if (!ec) return rt.address().is_loopback();
				else return true;
			}
			void read() {
				auto self(shared_from_this());

				boost::system::error_code ec;
				if (_readtimeout <= 0) _read_deadline.expires_at(boost::posix_time::pos_infin, ec);
				else  _read_deadline.expires_from_now(boost::posix_time::seconds(_readtimeout), ec);
				if (ec) {
					SL_RAT_LOG(Logging_Levels::ERROR_log_level, ec.message());
				}
				else if (_readtimeout >= 0) {
					_read_deadline.async_wait([self](const boost::system::error_code& ec) {
						if (ec != boost::asio::error::operation_aborted) {
							self->close("read timer expired. Time waited: ");
						}
					});
				}

				_socket.async_read_frame(frame_, db, [self](const boost::system::error_code& ec) {
					if (ec) {
						SL_RAT_LOG(Logging_Levels::ERROR_log_level, ec.message());
						self->close("async_read error");
					}
					else {
						self->onMessage(self, boost::asio::buffer_cast<const char*>(self->db.data()), self->db.size());
						self->db.consume(self->db.size());
						self->read();
					}
				});
			}


		};


		//This can be used in production, the dh params do not need to be kept secret. Below is a 3072 bit dhparams
		const char dhparams[] = R"(-----BEGIN DH PARAMETERS-----
MIIBCAKCAQEAzPjrDCNwq0bYz5xi72GM4EoWjqwmaAXFcY7vR6+nDpeeKpeYg8XS
tdjCwyaIRaYO3tzZxTbjdgnCbksVQrRKscefnba5zguqnqvaGL7rHNwdqaQq7rTp
KGdva8BKIaE9fq2q0OaZpBWE7KtVdraF5+CnvEj4AJqxGgZ/OtP+Y3UPTIcjoIve
2ss+XbvHGvcZ+RmyYeHmmQbWyqNyoUgrMzfGOHUvY6x9fcl/DvCSIpVn8qRK+3+n
64R3OAMcxNK7ONGSL3q6DRVTUXTNch9W+TKOYAppKtdyemoUAMleXD3F282BUwqL
f4pf5b+c+w+99vHpUlkbIzV0tI5vGZo1uwIBAg==
-----END DH PARAMETERS-----)";

		class WebSocketListenerImpl : public std::enable_shared_from_this<WebSocketListenerImpl> {

			boost::asio::io_service ios_;
			boost::asio::ip::tcp::acceptor acceptor_;
			std::vector<std::thread> thread_;
			boost::optional<boost::asio::io_service::work> work_;
			boost::asio::ssl::context context_;
			std::shared_ptr<Server_Config> Server_Config_;

		public:
			std::function<void(const std::shared_ptr<ISocket>&)> onConnection;
			std::function<void(const std::shared_ptr<ISocket>&, const char*, size_t)> onMessage;
			std::function<void(const ISocket* socket)> onDisconnection;

			WebSocketListenerImpl(std::shared_ptr<Server_Config> config) :context_(boost::asio::ssl::context::tlsv11), acceptor_(ios_), work_(ios_), Server_Config_(config)
			{
				SL_RAT_LOG(Logging_Levels::INFO_log_level, "WebSocket Starting " );
				thread_.reserve(config->MaxWebSocketThreads);
				for (std::size_t i = 0; i < config->MaxWebSocketThreads; ++i)
					thread_.emplace_back([&] { ios_.run(); });

				boost::asio::ip::tcp::endpoint tcp(boost::asio::ip::tcp::v4(), Server_Config_->WebSocketTLSLPort);
				context_.set_options(
					boost::asio::ssl::context::default_workarounds
					| boost::asio::ssl::context::no_sslv2 | boost::asio::ssl::context::no_sslv3
					| boost::asio::ssl::context::single_dh_use);
				boost::system::error_code ec;
				context_.set_password_callback(std::bind(&WebSocketListenerImpl::get_password, this), ec);
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
			void run() {
				auto self(shared_from_this());
				auto sock = std::make_shared<WSSocketImpl>(context_, ios_);
				sock->onDisconnection = onDisconnection;
				sock->onMessage = onMessage;
				acceptor_.async_accept(sock->_socket.lowest_layer(), [self, sock](boost::system::error_code ec) {
					if (!self->acceptor_.is_open())
						return;
					if (ec == boost::asio::error::operation_aborted)
						return;
					if (ec)
					{
						SL_RAT_LOG(Logging_Levels::ERROR_log_level, "on_accept: " << ec.message());
						return;
					}

					sock->_socket.next_layer().async_handshake(boost::asio::ssl::stream_base::server, [self, sock](boost::system::error_code ec) {
						if (!self->acceptor_.is_open())
							return;
						if (ec == boost::asio::error::operation_aborted)
							return;
						if (ec)
						{
							SL_RAT_LOG(Logging_Levels::ERROR_log_level, "on_accept: " << ec.message());
							return;
						}
						self->onConnection(std::dynamic_pointer_cast<ISocket>(sock));
						sock->read();
						self->run();

					});
				});
			}
			~WebSocketListenerImpl()
			{
				work_ = boost::none;
				boost::system::error_code ec;
				ios_.dispatch([&] { acceptor_.close(ec); });
				for (auto& t : thread_)
					t.join();
			}

			std::string get_password() const
			{
				return Server_Config_->PasswordToPrivateKey;
			}
			boost::asio::ip::tcp::endpoint local_endpoint() const
			{
				return acceptor_.local_endpoint();
			}



		};

		WebSocketListener::WebSocketListener(std::shared_ptr<Server_Config> config) {
			_WebSocketListenerImpl = std::make_shared<WebSocketListenerImpl>(config);
			_WebSocketListenerImpl->run();
		}
		WebSocketListener::~WebSocketListener() {

		}

		void WebSocketListener::onConnection(const std::function<void(const std::shared_ptr<ISocket>&)>& func) {
			_WebSocketListenerImpl->onConnection = func;
		}
		void WebSocketListener::onMessage(const std::function<void(const std::shared_ptr<ISocket>&, const char*, size_t)>& func) {
			_WebSocketListenerImpl->onMessage = func;
		}
		void WebSocketListener::onDisconnection(const std::function<void(const ISocket* socket)>& func) {
			_WebSocketListenerImpl->onDisconnection = func;
		}



	}
}