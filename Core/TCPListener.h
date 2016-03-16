#pragma once
#include <memory>
#include <functional>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include "ISocket.h"
#include <type_traits>

namespace SL {
	namespace Remote_Access_Library {
		namespace Network {

			template<typename BASESOCKET, typename DERIVED_BASESOCKET>class TCPListener : public std::enable_shared_from_this<TCPListener<BASESOCKET, DERIVED_BASESOCKET>> {
			public:
				static_assert(std::is_same<BASESOCKET, boost::asio::ip::tcp::socket>::value || std::is_same<BASESOCKET, boost::asio::ssl::stream<boost::asio::ip::tcp::socket>>::value, "BASESOCKET must be boost::asio::ip::tcp::socket or boost::asio::ssl::stream<boost::asio::ip::tcp::socket>");
				static_assert(std::is_base_of<ISocket, DERIVED_BASESOCKET>::value, "DERIVED_BASESOCKET must be derived from ISocket");

				//MUST BE an std::shared_ptr otherwise it will crash
				TCPListener(IBaseNetworkDriver* driver, unsigned short port, boost::asio::io_service& io_service) :
					_driver(driver), _acceptor(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
					_socket(io_service){

				}
				~TCPListener() {
					_acceptor.close();
				}
				void Start() {
					auto self(this->shared_from_this());
					_acceptor.async_accept(_socket, [self, this](boost::system::error_code ec)
					{
						if (!ec)
						{
							//boost::asio::ip::tcp::no_delay option(true);
							//_socket.set_option(option);
							std::cout << "Servicing new connection .. " << std::endl;
							std::make_shared<DERIVED_BASESOCKET>(_driver, _socket)->connect(nullptr, nullptr);
							Start();
						}
						else {
							std::cout << "Exiting asyncaccept" << std::endl;
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
				boost::asio::ip::tcp::acceptor _acceptor;
				BASESOCKET _socket;

			};

			template<typename DERIVED_BASESOCKET>class TCPListener<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>, DERIVED_BASESOCKET> : public std::enable_shared_from_this<TCPListener<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>, DERIVED_BASESOCKET>> {
			public:
				static_assert(std::is_base_of<ISocket, DERIVED_BASESOCKET>::value, "DERIVED_BASESOCKET must be derived from ISocket");

				//MUST BE an std::shared_ptr otherwise it will crash
				TCPListener(IBaseNetworkDriver* driver, unsigned short port, boost::asio::io_service& io_service) :
					_driver(driver), _io_service(io_service), _acceptor(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
					 _context(boost::asio::ssl::context::tlsv12) {

					_context.set_options(
						boost::asio::ssl::context::default_workarounds
						| boost::asio::ssl::context::no_sslv2 | boost::asio::ssl::context::no_sslv3
						| boost::asio::ssl::context::single_dh_use);
					//_context.set_password_callback(boost::bind(&server::get_password, this))
					//_context.use_certificate_chain_file("server.pem");
					//_context.use_private_key_file("server.pem", boost::asio::ssl::context::pem);
					//_context.use_tmp_dh_file("dh2048.pem");

				}
				~TCPListener() {
					_acceptor.close();
				}
				void Start() {
					auto self(this->shared_from_this());

					auto _socket = std::make_shared<DERIVED_BASESOCKET>(_driver, _io_service, _context);
					_acceptor.async_accept(_socket->get_socket().lowest_layer(), [self, this, _socket](boost::system::error_code ec)
					{	
						if (!ec)
						{
							//boost::asio::ip::tcp::no_delay option(true);
							//_socket.set_option(option);
							_socket->get_socket().async_handshake(boost::asio::ssl::stream_base::server, [self, this, _socket](const boost::system::error_code& ec) {
								if (!ec) {
									_socket->connect(nullptr, nullptr);
								}
								Start();
							});
						}
						else {
							std::cout << "Exiting asyncaccept" << std::endl;
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
				boost::asio::io_service& _io_service;
				boost::asio::ip::tcp::acceptor _acceptor;
				boost::asio::ssl::context _context;
			};
		}

	}
}
