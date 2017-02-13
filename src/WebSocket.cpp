#include "internal/WebSocket.h"
#include "Logging.h"
#include "ISocket.h"
#include "RAT.h"

namespace SL {
	namespace RAT {


		WebSocket::WebSocket(boost::asio::ssl::context& context, boost::asio::io_service& io_service) :
			_socket(io_service, context),
			_read_deadline(io_service),
			_write_deadline(io_service)
		{
			_read_deadline.expires_at(boost::posix_time::pos_infin);
			_write_deadline.expires_at(boost::posix_time::pos_infin);
			_Closed = false;
			_readtimeout = _writetimeout = 5;
		}


		WebSocket::~WebSocket() {
			SL_RAT_LOG(Logging_Levels::INFO_log_level, "~WSSocketImpl");
			close("~WSSocketImpl");
		}

		void WebSocket::send(std::shared_ptr<char> data, size_t len) {
			auto self(shared_from_this());

			_socket.async_write(boost::asio::buffer(data.get(), len), [data, self](const boost::system::error_code& ec) {
				if (ec)
				{
					self->close(std::string("readheader async_read ") + ec.message());
				}
			});

		}
		void WebSocket::close(const std::string& reason) {
			if (closed()) return;
			SL_RAT_LOG(Logging_Levels::INFO_log_level, "Closing socket: " << reason);
			_Closed = true;
			_read_deadline.cancel();
			_write_deadline.cancel();

			onDisconnection_(this);
			boost::system::error_code ec;
			_socket.close(beast::websocket::close_code::normal, ec);

			if (ec) SL_RAT_LOG(Logging_Levels::ERROR_log_level, ec.message());
			ec.clear();
			SL_RAT_LOG(Logging_Levels::INFO_log_level, "Socket Closed");
		}
		void WebSocket::read() {
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
					self->onMessage_(self, boost::asio::buffer_cast<const char*>(self->db.data()), self->db.size());
					self->db.consume(self->db.size());
					self->read();
				}
			});
		}
		class WebSocketDataHolder {
		public:
			WebSocketDataHolder(Client_Config* config) :context_(boost::asio::ssl::context::tlsv11), work_(ios_) {
				thread_ = std::thread([&] {  ios_.run(); });

				boost::asio::ip::tcp::endpoint tcp(boost::asio::ip::tcp::v4(), config->WebSocketTLSLPort);
				context_.set_options(
					boost::asio::ssl::context::default_workarounds
					| boost::asio::ssl::context::no_sslv2 | boost::asio::ssl::context::no_sslv3
					| boost::asio::ssl::context::single_dh_use);
				boost::system::error_code ec;


				boost::asio::const_buffer certficatebuffer(config->Public_Certficate->get_buffer(), config->Public_Certficate->get_size());
				context_.add_certificate_authority(certficatebuffer, ec);
				if (ec) SL_RAT_LOG(Logging_Levels::ERROR_log_level, "use_certificate_chain error " << ec.message());
				ec.clear();

				context_.set_default_verify_paths(ec);
				if (ec) {
					SL_RAT_LOG(Logging_Levels::ERROR_log_level, "set_default_verify_paths error " << ec.message());
					return;
				}

			}
			~WebSocketDataHolder() {
				work_ = boost::none;
				thread_.join();
			}
			boost::asio::io_service ios_;
			std::thread thread_;
			boost::optional<boost::asio::io_service::work> work_;
			boost::asio::ssl::context context_;
		};
		void testp() {

		}

		void Connect(Client_Config* config, INetworkHandlers* driver, const char* host) {
			static std::unique_ptr<WebSocketDataHolder> io_runner;
			if (!io_runner) io_runner = std::make_unique<WebSocketDataHolder>(config);

			auto sock = std::make_shared<WebSocket>(io_runner->context_, io_runner->ios_);
			auto test = [=]() {};
			sock->onDisconnection([=](const ISocket* socket) { driver->onDisconnection(socket); });
			sock->onMessage([=](const std::shared_ptr<ISocket>& s, const char* d, size_t l) { driver->onMessage(s, d, l); });

			std::string _host = host != nullptr ? host : "";
			auto port = std::to_string(config->WebSocketTLSLPort);
			assert(_host.size() > 2);
			assert(port.size() > 0);

			boost::asio::ip::tcp::resolver resolver(io_runner->ios_);

			boost::asio::ip::tcp::resolver::query query(_host, port);
			boost::system::error_code ercode;
			auto endpoint = resolver.resolve(query, ercode);
			if (ercode) {
				sock->close(std::string("resolve ") + ercode.message());
				driver->onDisconnection(sock.get());
			}
			else {
				boost::asio::connect(sock->get_Socket().lowest_layer(), endpoint, ercode);
				if (ercode) {
					sock->close(std::string("connect ") + ercode.message());
					driver->onDisconnection(sock.get());
				}
				else {
					sock->get_Socket().next_layer().set_verify_mode(boost::asio::ssl::verify_none);
					sock->get_Socket().next_layer().handshake(boost::asio::ssl::stream_base::client, ercode);
					if (ercode) {
						sock->close(std::string("handshake ") + ercode.message());
						driver->onDisconnection(sock.get());
					}
					else {
						SL_RAT_LOG(Logging_Levels::INFO_log_level, "Connected to "<<host<<" Starting Read");
						sock->read();
					}
				}
			}
		}
	}
}