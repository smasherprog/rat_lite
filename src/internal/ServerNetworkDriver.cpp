#include "ServerNetworkDriver.h"
#include "Shapes.h"
#include "IServerDriver.h"
#include "Server_Config.h"
#include "turbojpeg.h"
#include "Input.h"
#include "ICryptoLoader.h"
#include "Logging.h"
#include "ScreenCapture.h"

#include <atomic>
#include <mutex>
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
		class NetworkImpl {

			boost::asio::io_service ios_;
			boost::asio::ip::tcp::socket sock_;
			boost::asio::ip::tcp::acceptor acceptor_;
			std::vector<std::thread> thread_;
			boost::optional<boost::asio::io_service::work> work_;
			boost::asio::ssl::context context_;
			std::shared_ptr<Server_Config> Server_Config_;

		public:
			NetworkImpl(NetworkImpl const&) = delete;
			NetworkImpl& operator=(NetworkImpl const&) = delete;

			NetworkImpl(std::shared_ptr<Server_Config> config, int threads=2): sock_(ios_), acceptor_(ios_), work_(ios_), context_(boost::asio::ssl::context::tlsv11), Server_Config_(config)
			{
				thread_.reserve(threads);
				for (std::size_t i = 0; i < threads; ++i)
					thread_.emplace_back([&] { ios_.run(); });
			}

			~NetworkImpl()
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
			void open(bool server, boost::asio::ip::tcp::endpoint const& ep, boost::system::error_code& ec)
			{
				context_.set_options(
					boost::asio::ssl::context::default_workarounds
					| boost::asio::ssl::context::no_sslv2 | boost::asio::ssl::context::no_sslv3
					| boost::asio::ssl::context::single_dh_use);
				boost::system::error_code ec;
				context_.set_password_callback(std::bind(&NetworkImpl::get_password, this), ec);
				if (ec) SL_RAT_LOG(Logging_Levels::ERROR_log_level, "set_password_callback error " << ec.message());
				ec.clear();
				boost::asio::const_buffer dhparams(dhparams, sizeof(dhparams));

				context_.use_tmp_dh(dhparams, ec);
				if (ec) SL_RAT_LOG(Logging_Levels::ERROR_log_level, "use_tmp_dh error " << ec.message());
				ec.clear();

				boost::asio::const_buffer cert(Server_Config_->Public_Certficate->get_buffer(), Server_Config_->Public_Certficate->get_size());
				context_.use_certificate_chain(cert, ec);
				if (ec) SL_RAT_LOG(Logging_Levels::ERROR_log_level, "use_certificate_chain error " << ec.message());
				ec.clear();

				context_.set_default_verify_paths(ec);
				if (ec) SL_RAT_LOG(Utilities::Logging_Levels::ERROR_log_level, "set_default_verify_paths error " << ec.message());
				ec.clear();

				boost::asio::const_buffer privkey(Server_Config_->Private_Key->get_buffer(), Server_Config_->Private_Key->get_size());
				context_.use_private_key(privkey, boost::asio::ssl::context::pem, ec);
				if (ec) SL_RAT_LOG(Logging_Levels::ERROR_log_level, "use_private_key error " << ec.message());
				ec.clear();

				if (server)
				{
					acceptor_.open(ep.protocol(), ec);
					if (ec)
					{
						SL_RAT_LOG(Logging_Levels::ERROR_log_level, "open: " << ec.message());
						return;
					}
					acceptor_.set_option(boost::asio::socket_base::reuse_address{ true });
					acceptor_.bind(ep, ec);
					if (ec)
					{
						SL_RAT_LOG(Logging_Levels::ERROR_log_level, "bind: " << ec.message());
						return;
					}
					acceptor_.listen( boost::asio::socket_base::max_connections, ec);
					if (ec)
					{
						SL_RAT_LOG(Logging_Levels::ERROR_log_level, "listen: " << ec.message());
						return;
					}
					acceptor_.async_accept(sock_, std::bind(&NetworkImpl::on_accept, this, beast::asio::placeholders::error));
				}
				else
				{
					Peer{ *this, std::move(sock_), ep };
				}
			}

			boost::asio::ip::tcp::endpoint local_endpoint() const
			{
				return acceptor_.local_endpoint();
			}

			void on_accept(boost::system::error_code ec)
			{
				if (!acceptor_.is_open())
					return;
				if (ec == boost::asio::error::operation_aborted)
					return;
				if (ec)
				{
					SL_RAT_LOG(Logging_Levels::ERROR_log_level, "on_accept: " << ec.message());
					return;
				}
				boost::asio::ip::tcp::socket sock(std::move(sock_));
				acceptor_.async_accept(sock_, std::bind(&NetworkImpl::on_accept, this, beast::asio::placeholders::error));
				Peer{ *this, std::move(sock) };
			}

		};


		class ServerNetworkDriverImpl {

			IServerDriver* _IServerDriver;

			std::shared_ptr<Server_Config> _Config;
			std::atomic_int ClientCount;


		public:
			ServerNetworkDriverImpl(std::shared_ptr<Server_Config> config, IServerDriver* svrd) :
				_IServerDriver(svrd), _Config(config) {

				_Listener.onConnection([&](uWS::WebSocket<uWS::CLIENT> ws, uWS::UpgradeInfo ui) {
					if (ClientCount >= static_cast<size_t>(_Config->MaxNumConnections)) {
						char *closeMessage = "Closing due to max number of connections!";
						size_t closeMessageLength = strlen(closeMessage);
						ws.close(1000, closeMessage, closeMessageLength);
					}
					else {
						ClientCount += 1;
						_IServerDriver->onConnection(ws, ui);
					}
				});
				_Listener.onDisconnection([&](uWS::WebSocket<uWS::CLIENT> ws, int code, char *message, size_t length) {
					ClientCount -= 1;
					_IServerDriver->onDisconnection(ws, code, message, length);

				});
				_Listener.onMessage([&](uWS::WebSocket<uWS::CLIENT> ws, char *message, size_t length, uWS::OpCode opCode) {
					auto pactype = PACKET_TYPES::INVALID;
					assert(length >= sizeof(pactype));
					pactype = *reinterpret_cast<PACKET_TYPES*>(message);
					length -= sizeof(pactype);
					message += sizeof(pactype);

					switch (pactype) {
					case PACKET_TYPES::MOUSEEVENT:
						assert(length == sizeof(MouseEvent));
						_IServerDriver->OnReceive_Mouse(reinterpret_cast<MouseEvent*>(message));
						break;
					case PACKET_TYPES::KEYEVENT:
						assert(length == sizeof(KeyEvent));
						_IServerDriver->OnReceive_Key(reinterpret_cast<KeyEvent*>(message));
						break;
					case PACKET_TYPES::CLIPBOARDTEXTEVENT:
						_IServerDriver->OnReceive_ClipboardText(message, length);
						break;
					default:
						_IServerDriver->onMessage(ws, message - sizeof(pactype), length + sizeof(pactype), opCode);
						break;
					}

					_IServerDriver->onMessage(ws, message, length, opCode);
				});

				//auto c = uS::TLS::createContext(_Config->Certficate_Public_FilePath, _Config->Certficate_Private_FilePath, _Config->PasswordToPrivateKey);

				_Listener.listen(_Config->WebSocketTLSLPort);
				_Listener.run();
				int k = 0;
			}
			virtual ~ServerNetworkDriverImpl() {

			}
			void Start() {
			
			}
			void Stop() {
				_Listener.getDefaultGroup<uWS::SERVER>().close();
			}
			

			void SendScreen(ISocket* socket, const Screen_Capture::Image & img) {
				Rect r(Point(0, 0), Height(img), Width(img));
				auto p = static_cast<unsigned int>(PACKET_TYPES::SCREENIMAGE);

				auto compfree = [](void* handle) {tjDestroy(handle); };
				auto _jpegCompressor(std::unique_ptr<void, decltype(compfree)>(tjInitCompress(), compfree));

				auto set = _Config->SendGrayScaleImages ? TJSAMP_GRAY : TJSAMP_420;

				auto maxsize = std::max(tjBufSize(Screen_Capture::Width(img), Screen_Capture::Height(img), set), static_cast<unsigned long>((Screen_Capture::RowStride(img) + Screen_Capture::RowPadding(img)) * Screen_Capture::Height(img))) + sizeof(r) + sizeof(p);
				auto _jpegSize = maxsize;
				auto buffer1 = std::make_unique<char[]>(maxsize);

				auto dst = (unsigned char*)buffer1.get();
				memcpy(dst, &p, sizeof(p));
				dst += sizeof(p);
				memcpy(dst, &r, sizeof(r));
				dst += sizeof(r);

				auto srcbuffer = std::make_unique<char[]>(RowStride(img)*Height(img));
				Screen_Capture::Extract(img, srcbuffer.get(), RowStride(img)*Height(img));
				auto srcbuf = (const unsigned char*)srcbuffer.get();

#if __ANDROID__
				auto colorencoding = TJPF_RGBX;
#else 
				auto colorencoding = TJPF_BGRX;
#endif

				if (tjCompress2(_jpegCompressor.get(), srcbuf, r.Width, 0, r.Height, colorencoding, &dst, &_jpegSize, set, _Config->ImageCompressionSetting, TJFLAG_FASTDCT | TJFLAG_NOREALLOC) == -1) {
					SL_RAT_LOG(Logging_Levels::ERROR_log_level, tjGetErrorStr());
				}
				//	std::cout << "Sending " << r << std::endl;
				auto finalsize = sizeof(p) + sizeof(r) + _jpegSize;//adjust the correct size

				if (socket == nullptr)	_Listener.getDefaultGroup<uWS::SERVER>().broadcast(buffer1.get(), finalsize, uWS::OpCode::BINARY);
				else socket->send(buffer1.get(), finalsize, uWS::OpCode::BINARY);
			}
			void SendMouse(ISocket* socket, const Screen_Capture::Image & img) {

				Point r(Width(img), Height(img));
				auto p = static_cast<unsigned int>(PACKET_TYPES::MOUSEIMAGE);
				auto finalsize = (Screen_Capture::RowStride(img) * Screen_Capture::Height(img)) + sizeof(p) + sizeof(r);
				auto buffer = std::make_unique<char[]>(finalsize);

				auto dst = buffer.get();
				memcpy(dst, &p, sizeof(p));
				dst += sizeof(p);
				memcpy(dst, &r, sizeof(r));
				dst += sizeof(r);
				Screen_Capture::Extract(img, dst, Screen_Capture::RowStride(img) * Screen_Capture::Height(img));

				if (socket == nullptr)	_Listener.getDefaultGroup<uWS::SERVER>().broadcast(buffer.get(), finalsize, uWS::OpCode::BINARY);
				else socket->send(buffer.get(), finalsize, uWS::OpCode::BINARY);

			}
			void SendMouse(ISocket* socket, const Point& pos)
			{
				auto p = static_cast<unsigned int>(PACKET_TYPES::MOUSEPOS);
				const auto size = sizeof(pos) + sizeof(p);

				char buffer[size];
				memcpy(buffer, &p, sizeof(p));
				memcpy(buffer + sizeof(p), &pos, sizeof(pos));

				if (socket == nullptr)	_Listener.getDefaultGroup<uWS::SERVER>().broadcast(buffer, size, uWS::OpCode::BINARY);
				else socket->send(buffer, size, uWS::OpCode::BINARY);
			}

			void SendClipboardText(ISocket* socket, const char* data, unsigned int len) {
				auto p = static_cast<unsigned int>(PACKET_TYPES::CLIPBOARDTEXTEVENT);
				auto size = len + sizeof(p);
				auto buffer = std::make_unique<char[]>(size);

				if (socket == nullptr)	_Listener.getDefaultGroup<uWS::SERVER>().broadcast(buffer.get(), size, uWS::OpCode::BINARY);
				else socket->send(buffer.get(), size, uWS::OpCode::BINARY);

			}
		};
		ServerNetworkDriver::ServerNetworkDriver(IServerDriver * r, std::shared_ptr<Server_Config> config) :_ServerNetworkDriverImpl(std::make_unique<ServerNetworkDriverImpl>(config, r))
		{
		}
		ServerNetworkDriver::~ServerNetworkDriver()
		{

		}
		void ServerNetworkDriver::Start() {
			_ServerNetworkDriverImpl->Start();
		}
		void ServerNetworkDriver::Stop() {
			_ServerNetworkDriverImpl->Stop();
		}

		void ServerNetworkDriver::SendScreen(ISocket* socket, const Screen_Capture::Image & img)
		{
			_ServerNetworkDriverImpl->SendScreen(socket, img);
		}
		void ServerNetworkDriver::SendMouse(ISocket* socket, const Screen_Capture::Image & img)
		{
			_ServerNetworkDriverImpl->SendMouse(socket, img);
		}
		void ServerNetworkDriver::SendMouse(ISocket* socket, const Point & pos)
		{
			_ServerNetworkDriverImpl->SendMouse(socket, pos);
		}

		void ServerNetworkDriver::SendClipboardText(ISocket* socket, const char* data, unsigned int len) {
			SL_RAT_LOG(Logging_Levels::INFO_log_level, "OnSend_ClipboardText " << len);
			_ServerNetworkDriverImpl->SendClipboardText(socket, data, len);
		}


	}
}
