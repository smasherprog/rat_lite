#include "WebSocketListener.h"
#include "Server_Config.h"
#include "Logging.h"
#include "ICryptoLoader.h"
#include "ISocket.h"

#include <vector>
#include <assert.h>
#include <beast/websocket.hpp>
#include <beast/websocket/ssl.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

#include <beast/core/placeholders.hpp>
#include <beast/core/streambuf.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>

namespace SL {
	namespace Remote_Access_Library {

		class WSSocketImpl : public ISocket, public std::enable_shared_from_this<WSSocketImpl> {
		private:

		public:

			WSSocketImpl(INetworkHandlers* netdriver, boost::asio::ssl::context& context, boost::asio::io_service& io_service) :
				_sslstream(io_service, context),
				_INetworkHandlers(netdriver),
				_read_deadline(io_service),
				_write_deadline(io_service),
				_socket(_sslstream)
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
			boost::asio::ssl::stream<boost::asio::ip::tcp::socket> _sslstream;
			beast::websocket::stream<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>&> _socket;

			INetworkHandlers* _INetworkHandlers;

			boost::asio::deadline_timer _read_deadline;
			boost::asio::deadline_timer _write_deadline;
			beast::streambuf db;

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
				boost::asio::async_write(_socket, boost::asio::buffer(data.get(), len), [data, self](const boost::system::error_code& ec, std::size_t byteswritten) {
					if (ec)
					{
						self->close(std::string("readheader async_read ") + ec.message());
					}
				});

			}
			virtual void close(std::string reason) override {
				if (closed()) return;
				SL_RAT_LOG(Logging_Levels::INFO_log_level, "Closing socket: " << reason);
				_Closed = true;
				CancelTimers();

				_INetworkHandlers->onDisconnection(this);
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
				else return -1;
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
				beast::websocket::opcode op;
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

				_socket.async_read(op, db, [self](const boost::system::error_code& ec, std::size_t len) {
					if (ec) {
						SL_RAT_LOG(Logging_Levels::ERROR_log_level, ec.message());
						self->close("async_read error");
					}
					else {
						self->_INetworkHandlers->onMessage(self, nullptr, len);
						self->read();
					}
				});
			}
			void operator()(boost::system::error_code ec, std::size_t)
			{
				(*this)(ec);
			}
			template<class DynamicBuffer, std::size_t N>
			static
				bool
				match(DynamicBuffer& db, char const(&s)[N])
			{
				using boost::asio::buffer;
				using boost::asio::buffer_copy;
				if (db.size() < N - 1)
					return false;
				static_string<N - 1> t;
				t.resize(N - 1);
				buffer_copy(buffer(t.data(), t.size()),
					db.data());
				if (t != s)
					return false;
				db.consume(N - 1);
				return true;
			}
			void operator()(boost::system::error_code ec)
			{

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


#if defined(DEBUG)  || defined(_DEBUG) || !defined(NDEBUG)

		//THIS SHOULD ONLY BE USED IN DEBUG MODE FOR EASE OF TESTING!!!!
		//DO NOT USE THIS IN PRODUCTION!!
		const char cert[] = R"(-----BEGIN CERTIFICATE-----
MIIEaDCCAtCgAwIBAgIBADANBgkqhkiG9w0BAQ0FADA3MRowGAYDVQQKDBFUZXN0
IENvbXBhbnkgTmFtZTEZMBcGA1UEAwwQVGVzdCBDb21tb24gTmFtZTAeFw0xNjA1
MzAwMjA1MzRaFw0xNzA1MzAwMjA1MzRaMDcxGjAYBgNVBAoMEVRlc3QgQ29tcGFu
eSBOYW1lMRkwFwYDVQQDDBBUZXN0IENvbW1vbiBOYW1lMIIBojANBgkqhkiG9w0B
AQEFAAOCAY8AMIIBigKCAYEA1E7RvjjPxjo85frpN6lBOW5bK3ZXfr1lsx609001
xUXz7/aX9j+to8GkOqyGapfeidg2E6WMK4HiBCiqO9Zba3EDMEGoppQJ6ntR2zIZ
SYOZfnIWyZQesC5BaL2ha5h96PVC2vYsEAglqYzrc1dvxFx0EA2EArabFcW/l5nS
ztAACGPr/LCsDSMcbLzFO6SDSPCuRaOTUzbzz5mfcarFib6B35OHK/TvqCBH0/Ev
FfCgJL4IgsglvhN7TXht8f/s0NQtsFPFA3g4t7byrxPJKkKgVWhtxOUcKdkTKmqj
c+EsF+punJJ8egXdP4nJ74Corpx0BNHPHnkJUk8ptYrZiHRGU1UqjdA1VdT19Zdu
O7KBCTfL/TaXbVxrhZIRMmoOMZfxqznZ4tNiyOOPxbNOFr1Du13XFXrOrJBkm21t
5sN42hTfmQy6hv7squwidjcEWcsKpbKkbJV7CWEr3HZSZwvn679FWwrzEwJwE5sw
O2W8I3zay62KgEBAEHyNiKGbAgMBAAGjfzB9MA8GA1UdEwEB/wQFMAMBAf8wDgYD
VR0PAQH/BAQDAgEGMB0GA1UdDgQWBBQCXy9tA++sUxkDnnLhypnVqKqwWjARBglg
hkgBhvhCAQEEBAMCAgQwKAYJYIZIAYb4QgENBBsWGWV4YW1wbGUgY29tbWVudCBl
eHRlbnNpb24wDQYJKoZIhvcNAQENBQADggGBAE5wQrvWslZ1W3wlcgSEx4dhe7xJ
P8QcDR56FldEDL50VfcPPMkkm1b3t7OmhC+oo64MFRD/c/dNTtMMq4fWc4g3Laap
rOX6iTgXxfK4JpwFWf+dOazrEBThf3MhV7uQJZeuMr2WSJXCS5Lw9mTd8Q/nK5AB
BZDY0dCwTIGQA1a/p+PZ6ZJMt4GUJuy6rN7Qc8ihRrMx+lp3JUxgVDHJ7feZASNG
2pM/UsuDCvNagyr5A58n/gwdW164UNZHXb6WYoBhRh2Q13wBI2Ejuse8xlvL5vEV
WMUils+cKTzb7wo3fcu/llVcuFhr9z0CfbgK2aiylARJB3xwgPpyb6Qe3+q1foKw
fdu+Rwo59fmX4dsTkySs7ZTKC4VSChAM0ac2RxSCRvPcqkHxxezrk/t+XDL6wSsL
glN6yxU1w+vsYbiayCK4Pl9SHhd9yzx1Li5XYtTmjcIieTa+tmv9IR5cltMqCoSw
ZydqCFmG4VbNpR42y+zIk8vcpJ8Y3u5xQJV3Lg==
-----END CERTIFICATE-----)";

		//THIS SHOULD ONLY BE USED IN DEBUG MODE FOR EASE OF TESTING!!!!
		//DO NOT USE THIS IN PRODUCTION!!
		const char private_key[] = R"(-----BEGIN ENCRYPTED PRIVATE KEY-----
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
dHEcfRl35ANQC0j95KsdzTw/Cg==
-----END ENCRYPTED PRIVATE KEY-----)";
		//THIS SHOULD ONLY BE USED IN DEBUG MODE FOR EASE OF TESTING!!!!
		//DO NOT USE THIS IN PRODUCTION!!
		const std::string private_key_password = "Test pass";

#endif
		class WebSocketListenerImpl : public std::enable_shared_from_this<WebSocketListenerImpl> {

			boost::asio::io_service ios_;
			boost::asio::ip::tcp::acceptor acceptor_;
			std::vector<std::thread> thread_;
			boost::optional<boost::asio::io_service::work> work_;
			boost::asio::ssl::context context_;
			std::shared_ptr<Server_Config> Server_Config_;
			INetworkHandlers* handlers;

		public:

			WebSocketListenerImpl(INetworkHandlers* netevent, std::shared_ptr<Server_Config> config) : handlers(netevent), context_(boost::asio::ssl::context::tlsv11), acceptor_(ios_), work_(ios_), Server_Config_(config)
			{
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
				run();
			}
			void run() {
				auto self(shared_from_this());
				auto sock = std::make_shared<WSSocketImpl>(handlers, ios_, context_);
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

					sock->_sslstream.async_handshake(boost::asio::ssl::stream_base::server, [self, sock](boost::system::error_code ec) {
						if (!self->acceptor_.is_open())
							return;
						if (ec == boost::asio::error::operation_aborted)
							return;
						if (ec)
						{
							SL_RAT_LOG(Logging_Levels::ERROR_log_level, "on_accept: " << ec.message());
							return;
						}
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

		WebSocketListener::WebSocketListener(INetworkHandlers* netevent, std::shared_ptr<Server_Config> config) {
			_WebSocketListenerImpl = std::make_unique<WebSocketListenerImpl>(netevent, config);
		}
		WebSocketListener::~WebSocketListener() {

		}

	}
}