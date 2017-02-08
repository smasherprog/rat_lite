#include "WebSocket.h"
#include "Logging.h"
#include "ISocket.h"

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

			onDisconnection(this);
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
					self->onMessage(self, boost::asio::buffer_cast<const char*>(self->db.data()), self->db.size());
					self->db.consume(self->db.size());
					self->read();
				}
			});
		}
	}
}