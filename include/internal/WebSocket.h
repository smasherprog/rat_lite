#pragma once
#include <string>
#include <memory>
#include <functional>
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
#include <beast/core/streambuf.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include "ISocket.h"

namespace SL {
	namespace RAT {

		class WebSocket : public ISocket, public std::enable_shared_from_this<WebSocket> {
			beast::websocket::stream<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>> _socket;

			std::function<void(const std::shared_ptr<ISocket>&, const char*, size_t)> onMessage_;
			std::function<void(const ISocket* socket)> onDisconnection_;

			boost::asio::deadline_timer _read_deadline;
			boost::asio::deadline_timer _write_deadline;
			boost::asio::streambuf db;
			beast::websocket::frame_info frame_;

			bool _Closed = true;
			bool _Writing = false;

			int _readtimeout = 5;
			int _writetimeout = 5;

		public:

			WebSocket(boost::asio::ssl::context& context, boost::asio::io_service& io_service);
			virtual ~WebSocket();

			void onMessage(std::function<void(const std::shared_ptr<ISocket>&, const char*, size_t)>& cb) { onMessage_ = cb; }
			void onDisconnection(std::function<void(const ISocket* socket)>& cb) { onDisconnection_ = cb; }
			virtual void send(std::shared_ptr<char> data, size_t len) override;
			beast::websocket::stream<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>>& get_Socket() { return _socket; }
			virtual void close(const std::string& reason) override;
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
			void read();
		};
	}
}