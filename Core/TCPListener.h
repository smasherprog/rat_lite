#pragma once
#include <memory>
#include <functional>
#include <asio.hpp>
#include <asio/ssl.hpp>
#include "ISocket.h"
#include <type_traits>
#include "Logging.h"

namespace SL {
	namespace Remote_Access_Library {
		namespace Network {

			template<typename BASESOCKET, typename DERIVED_BASESOCKET>class TCPListener : public std::enable_shared_from_this<TCPListener<BASESOCKET, DERIVED_BASESOCKET>> {
			public:
				static_assert(std::is_same<BASESOCKET, asio::ip::tcp::socket>::value || std::is_same<BASESOCKET, asio::ssl::stream<asio::ip::tcp::socket>>::value, "BASESOCKET must be asio::ip::tcp::socket or asio::ssl::stream<asio::ip::tcp::socket>");
				static_assert(std::is_base_of<ISocket, DERIVED_BASESOCKET>::value, "DERIVED_BASESOCKET must be derived from ISocket");

				//MUST BE an std::shared_ptr otherwise it will crash
				TCPListener(IBaseNetworkDriver* driver, unsigned short port, asio::io_service& io_service) :
					_driver(driver), _acceptor(io_service, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)),
					_socket(io_service){

				}
				~TCPListener() {
					_acceptor.close();
				}
				void Start() {
					auto self(this->shared_from_this());
					_acceptor.async_accept(_socket, [self, this](std::error_code ec)
					{
						if (!ec)
						{
							//asio::ip::tcp::no_delay option(true);
							//_socket.set_option(option);
							SL_RAT_LOG("Servicing new connection .. ", Utilities::Logging_Levels::INFO_log_level);
							std::make_shared<DERIVED_BASESOCKET>(_driver, _socket)->connect(nullptr, nullptr);
							Start();
						}
						else {
							SL_RAT_LOG("Exiting asyncaccept", Utilities::Logging_Levels::INFO_log_level);
						}
					});
				}
				//no copy allowed
				TCPListener(const TCPListener&) = delete;
				TCPListener(TCPListener&&) = delete;
				//no copy allowed
				TCPListener& operator=(TCPListener&&) = delete;
				TCPListener& operator=(const TCPListener&) = delete;
			private:
				IBaseNetworkDriver* _driver;
				asio::ip::tcp::acceptor _acceptor;
				BASESOCKET _socket;

			};

			template<typename DERIVED_BASESOCKET>class TCPListener<asio::ssl::stream<asio::ip::tcp::socket>, DERIVED_BASESOCKET> : public std::enable_shared_from_this<TCPListener<asio::ssl::stream<asio::ip::tcp::socket>, DERIVED_BASESOCKET>> {
			public:
				static_assert(std::is_base_of<ISocket, DERIVED_BASESOCKET>::value, "DERIVED_BASESOCKET must be derived from ISocket");

				//MUST BE an std::shared_ptr otherwise it will crash
				TCPListener(IBaseNetworkDriver* driver, unsigned short port, asio::io_service& io_service) :
					_driver(driver), _io_service(io_service), _acceptor(io_service, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)),
					 _context(asio::ssl::context::tlsv12) {

					_context.set_options(
						asio::ssl::context::default_workarounds
						| asio::ssl::context::no_sslv2 | asio::ssl::context::no_sslv3
						| asio::ssl::context::single_dh_use);
					//_context.set_password_callback(bind(&server::get_password, this))
					//_context.use_certificate_chain_file("server.pem");
					//_context.use_private_key_file("server.pem", asio::ssl::context::pem);
					//_context.use_tmp_dh_file("dh2048.pem");

				}
				~TCPListener() {
					_acceptor.close();
				}
				void Start() {
					auto self(this->shared_from_this());

					auto _socket = std::make_shared<DERIVED_BASESOCKET>(_driver, _io_service, _context);
					_acceptor.async_accept(_socket->get_socket().lowest_layer(), [self, this, _socket](std::error_code ec)
					{	
						if (!ec)
						{
							//asio::ip::tcp::no_delay option(true);
							//_socket.set_option(option);
							
							_socket->get_socket().async_handshake(asio::ssl::stream_base::server, [self, this, _socket](const std::error_code& ec) {
								if (!ec) {
									_socket->connect(nullptr, nullptr);
								}
								Start();
							});
						}
						else {
							SL_RAT_LOG("Exiting asyncaccept", Utilities::Logging_Levels::INFO_log_level);
						}
					});
				}
				//no copy allowed
				TCPListener(const TCPListener&) = delete;
				TCPListener(TCPListener&&) = delete;
				//no copy allowed
				TCPListener& operator=(TCPListener&&) = delete;
				TCPListener& operator=(const TCPListener&) = delete;
			private:
				IBaseNetworkDriver* _driver;
				asio::io_service& _io_service;
				asio::ip::tcp::acceptor _acceptor;
				asio::ssl::context _context;
			};
		}

	}
}
