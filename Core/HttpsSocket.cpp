#pragma once
#include "stdafx.h"
#include "HttpsSocket.h"
#include "IBaseNetworkDriver.h"
#include "HttpHeader.h"
#include "MediaTypes.h"
#include "Server_Config.h"
#include "HttpHeader.h"
#include "Logging.h"
#include "Packet.h"

#ifndef __ANDROID__
#include <boost/filesystem.hpp>
#endif

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/asio/deadline_timer.hpp>

#include <memory>
#include <utility>
#include <string>
#include <fstream>

namespace SL {
	namespace Remote_Access_Library {
		namespace Network {

			class HTTPSAsio_Context {

			public:

				HTTPSAsio_Context() : work(io_service), ssl_context(boost::asio::ssl::context::tlsv12) {
					io_servicethread = std::thread([this]() {
						SL_RAT_LOG(Utilities::Logging_Levels::INFO_log_level, "Starting io_service Factory");
						boost::system::error_code ec;
						io_service.run(ec);
						if (ec) {
							SL_RAT_LOG(Utilities::Logging_Levels::ERROR_log_level, ec.message());
						}
						SL_RAT_LOG(Utilities::Logging_Levels::INFO_log_level, "stopping io_service Factory");
					});
				}
				~HTTPSAsio_Context()
				{
					Stop();
				}
				void Stop() {
					io_service.stop();
					if (io_servicethread.joinable()) io_servicethread.join();
				}

				boost::asio::io_service io_service;
				std::thread io_servicethread;
				boost::asio::io_service::work work;
				boost::asio::ssl::context ssl_context;

			};





			class HttpsSocketImpl : public ISocket, public std::enable_shared_from_this<HttpsSocketImpl> {
			private:
				std::shared_ptr<HTTPSAsio_Context> _WSSAsio_Context;
			public:

				HttpsSocketImpl(IBaseNetworkDriver<std::shared_ptr<ISocket>, std::shared_ptr<Packet>>* netdriver, std::shared_ptr<HTTPSAsio_Context> impl) :
					_socket(_WSSAsio_Context->io_service, _WSSAsio_Context->ssl_context),
					_read_deadline(_WSSAsio_Context->io_service),
					_write_deadline(_WSSAsio_Context->io_service),
					_io_service(_WSSAsio_Context->io_service),
					_IBaseNetworkDriver(netdriver)
				{
					_read_deadline.expires_at(boost::posix_time::pos_infin);
					_write_deadline.expires_at(boost::posix_time::pos_infin);
					memset(&_SocketStats, 0, sizeof(_SocketStats));
					_Closed = false;
					_readtimeout = _writetimeout = 5;
				}


				~HttpsSocketImpl() {
					CancelTimers();
				}


				boost::asio::ssl::stream<boost::asio::ip::tcp::socket> _socket;
				boost::asio::io_service& _io_service;
				IBaseNetworkDriver<std::shared_ptr<ISocket>, std::shared_ptr<Packet>>* _IBaseNetworkDriver = nullptr;

				std::deque<OutgoingPacket> _OutgoingPackets;
				std::vector<char> _IncomingBuffer;
				boost::asio::deadline_timer _read_deadline;
				boost::asio::deadline_timer _write_deadline;
				std::unordered_map<std::string, std::string> _Header;
				bool _Server = false;
				std::string _Host;
				std::string _Port;
				bool _Closed = true;
				int _readtimeout = 5;
				int _writetimeout = 5;
				unsigned int _ReadPayload_Length;
				SocketStats _SocketStats;


				void CancelTimers()
				{
					_read_deadline.cancel();
					_write_deadline.cancel();
				}
				Packet GetNextReadPacket()
				{
					//dont free the memory on this... Owned internally
					Packet p(static_cast<unsigned int>(PACKET_TYPES::HTTP_MSG), static_cast<size_t>(_ReadPayload_Length), std::move(_Header), _IncomingBuffer.data(), false);
					_Header.clear();//reset the header after move
					return p;
				}

				virtual void send(Packet & pack) override
				{
					auto self(shared_from_this());
					auto beforesize = pack.Payload_Length;
					auto compack(std::make_shared<Packet>(std::move(pack)));
					_io_service.post([self, compack, beforesize]()
					{
						auto outgoingempty = self->_OutgoingPackets.empty();
						self->_OutgoingPackets.push_back({ compack, beforesize });
						if (outgoingempty)
						{
							self->writeexpire_from_now(30);
							self->writeheader();
						}
					});
				}

				virtual bool closed() override
				{
					return _Closed || !_socket.lowest_layer().is_open();
				}

				virtual SocketStats get_SocketStats() const override
				{
					return _SocketStats;
				}

				virtual void set_ReadTimeout(int s) override
				{
					assert(s > 0);
					_readtimeout = s;
				}

				virtual void set_WriteTimeout(int s) override
				{
					assert(s > 0);
					_writetimeout = s;
				}

				virtual std::string get_ipv4_address() const override
				{
					return _socket.lowest_layer().remote_endpoint().address().to_string();
				}

				virtual unsigned short get_port() const override
				{
					return _socket.lowest_layer().remote_endpoint().port();
				}



				virtual void close(std::string reason) override
				{
					if (closed()) return;
					SL_RAT_LOG(Utilities::Logging_Levels::INFO_log_level, "Closing socket: " << reason);
					_Closed = true;
					CancelTimers();
					_IBaseNetworkDriver->OnClose(shared_from_this());
					boost::system::error_code ec;
					_socket.lowest_layer().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
					_socket.lowest_layer().close();
					if (ec) {
						SL_RAT_LOG(Utilities::Logging_Levels::ERROR_log_level, ec.message());
					}
				}
				void start()
				{
					_Server = true;
					_IBaseNetworkDriver->OnConnect(shared_from_this());
					readheader();
				}
				void readheader()
				{
					auto self(shared_from_this());
					std::shared_ptr<boost::asio::streambuf> read_buffer(new boost::asio::streambuf);
					boost::asio::async_read_until(_socket, *read_buffer, "\r\n\r\n", [self, read_buffer](const boost::system::error_code& ec, std::size_t s)
					{
						if (!ec) {
							auto beforesize = read_buffer->size();
							std::istream stream(read_buffer.get());
							self->_Header = std::move(Parse("1.0", stream));

							const auto it = self->_Header.find(HTTP_CONTENTLENGTH);
							self->_ReadPayload_Length = 0;
							if (it != self->_Header.end()) {
								self->_ReadPayload_Length = static_cast<unsigned int>(std::stoi(it->second.c_str()));
							}
							auto extrabytesread = static_cast<unsigned int>(beforesize - s);
							if (self->_ReadPayload_Length > extrabytesread) self->_ReadPayload_Length -= extrabytesread;
							if (extrabytesread > 0) {
								self->_IncomingBuffer.assign(std::istreambuf_iterator<char>(stream), {});
							}
							self->readbody();
						}
						else {
							self->close(std::string("readheader async_read_until ") + ec.message());
						}
					});
				}

				void readbody()
				{
					readexpire_from_now(_readtimeout);
					auto self(shared_from_this());
					auto p(_IncomingBuffer.data());
					auto size(_ReadPayload_Length);

					boost::asio::async_read(_socket, boost::asio::buffer(p, size), [self](const boost::system::error_code& ec, std::size_t len/*length*/)
					{
						if (!ec && !self->closed()) {
							assert(len == self->_ReadPayload_Length);
							auto pac(std::make_shared<Packet>(std::move(self->GetNextReadPacket())));
							self->_IBaseNetworkDriver->OnReceive(self, pac);
							self->readheader();
						}
						else self->close(std::string("readbody async_read ") + ec.message());
					});
				}

				void writeheader()
				{
					writeexpire_from_now(_writetimeout);
					auto& pack = _OutgoingPackets.front().Pack;
					//the headers below are required... 
					assert(pack->Header.find(HTTP_CONTENTTYPE) != pack->Header.end());
					assert(pack->Header.find(HTTP_STATUSCODE) != pack->Header.end());
					assert(pack->Header.find(HTTP_VERSION) != pack->Header.end());
					//Code below isnt optimal... Will have to think about this
					auto outpackbuff(std::make_shared<boost::asio::streambuf>());
					//outpackbuff->prepare(std::accumulate(begin(pack->Header), end(pack->Header), static_cast<size_t>(0), [](size_t a, const decltype(pack->Header)::reference second) { return a + second.first.size() + second.second.size(); }));//try to allocate enouh space for the header
					std::ostream os(outpackbuff.get());
					os << pack->Header[HTTP_VERSION] << " " << pack->Header[HTTP_STATUSCODE] << HTTP_ENDLINE;
					os << HTTP_CONTENTTYPE << HTTP_KEYVALUEDELIM << pack->Header[HTTP_CONTENTTYPE] << HTTP_ENDLINE;
					pack->Header.erase(HTTP_VERSION);
					pack->Header.erase(HTTP_STATUSCODE);
					pack->Header.erase(HTTP_CONTENTTYPE);
					for (auto& a : pack->Header) {//write the other headers
						os << a.first << HTTP_KEYVALUEDELIM << a.second << HTTP_ENDLINE;
					}
					os << HTTP_CONTENTLENGTH << HTTP_KEYVALUEDELIM << pack->Payload_Length << HTTP_ENDLINE;
					os << HTTP_ENDLINE;//marks the end of the header

					auto self(shared_from_this());
					boost::asio::async_write(self->_socket, *outpackbuff, [self, outpackbuff, pack](const boost::system::error_code& ec, std::size_t byteswritten)
					{
						UNUSED(byteswritten);
						if (!ec && !self->closed())
						{
							self->writebody();
						}
						else self->close(std::string("writeheader async_write ") + ec.message());
					});
				}

				void writebody()
				{
					writeexpire_from_now(_writetimeout);
					auto packet = _OutgoingPackets.front().Pack;
					_OutgoingPackets.pop_front();//remove the packet

					auto self(shared_from_this());
					boost::asio::async_write(_socket, boost::asio::buffer(packet->Payload, packet->Payload_Length), [self, packet](const boost::system::error_code& ec, std::size_t byteswritten)
					{
						if (!ec && !self->closed())
						{
							assert(byteswritten == packet->Payload_Length);
							if (!self->_OutgoingPackets.empty()) {
								self->writeheader();
							}
							else {
								self->writeexpire_from_now(0);
							}
						}
						else self->close(std::string("writebody async_write ") + ec.message());
					});
				}

				void readexpire_from_now(int seconds)
				{
					if (seconds <= 0) _read_deadline.expires_at(boost::posix_time::pos_infin);
					else  _read_deadline.expires_from_now(boost::posix_time::seconds(seconds));

					if (seconds >= 0) {
						auto self(shared_from_this());

						_read_deadline.async_wait([self, seconds](const boost::system::error_code& ec) {
							if (ec != boost::asio::error::operation_aborted) {
								self->close("read timer expired. Time waited: ");
							}
						});
					}
				}
				void writeexpire_from_now(int seconds)
				{
					if (seconds <= 0) _write_deadline.expires_at(boost::posix_time::pos_infin);
					else  _write_deadline.expires_from_now(boost::posix_time::seconds(seconds));
					if (seconds >= 0) {
						auto self(shared_from_this());
						_write_deadline.async_wait([self, seconds](const boost::system::error_code& ec) {
							if (ec != boost::asio::error::operation_aborted) {
								//close("write timer expired. Time waited: " + std::to_string(seconds));
								self->close("write timer expired. Time waited: ");
							}
						});
					}
				}


			};
		}
	}
}

SL::Remote_Access_Library::Network::HttpsSocket::HttpsSocket(std::shared_ptr<HttpsSocketImpl> impl)
	: _HttpsSocketImpl(impl) { }

SL::Remote_Access_Library::Network::HttpsSocket::~HttpsSocket()
{
	_HttpsSocketImpl->close("~WSSocket");//make sure the lower level closes
}

void SL::Remote_Access_Library::Network::HttpsSocket::send(Packet & pack)
{
	_HttpsSocketImpl->send(pack);
}

bool SL::Remote_Access_Library::Network::HttpsSocket::closed()
{
	return 	_HttpsSocketImpl->closed();
}

SL::Remote_Access_Library::Network::SocketStats SL::Remote_Access_Library::Network::HttpsSocket::get_SocketStats() const
{
	return 	_HttpsSocketImpl->get_SocketStats();
}

void SL::Remote_Access_Library::Network::HttpsSocket::set_ReadTimeout(int s)
{
	_HttpsSocketImpl->set_ReadTimeout(s);
}

void SL::Remote_Access_Library::Network::HttpsSocket::set_WriteTimeout(int s)
{
	_HttpsSocketImpl->set_WriteTimeout(s);
}

std::string SL::Remote_Access_Library::Network::HttpsSocket::get_ipv4_address() const
{
	return _HttpsSocketImpl->get_ipv4_address();
}

unsigned short SL::Remote_Access_Library::Network::HttpsSocket::get_port() const
{
	return _HttpsSocketImpl->get_port();
}

void SL::Remote_Access_Library::Network::HttpsSocket::close(std::string reason)
{
	_HttpsSocketImpl->close(reason);
}



namespace SL {
	namespace Remote_Access_Library {
		namespace Network {

			class HttpsServerImpl : public std::enable_shared_from_this<HttpsServerImpl> {
			public:

				boost::asio::ip::tcp::acceptor _acceptor;
				std::shared_ptr<Network::Server_Config> _config;
				std::shared_ptr<HTTPSAsio_Context> _Asio_Context;
				IBaseNetworkDriver<std::shared_ptr<ISocket>, std::shared_ptr<Packet>>* _IBaseNetworkDriver;

				HttpsServerImpl(IBaseNetworkDriver<std::shared_ptr<ISocket>, std::shared_ptr<Packet>>* netevent, std::shared_ptr<HTTPSAsio_Context> asiocontext, std::shared_ptr<Network::Server_Config> config) :
					_IBaseNetworkDriver(netevent),
					_config(config),
					_acceptor(asiocontext->io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), config->HttpTLSListenPort))
				{
					asiocontext->ssl_context.set_options(
						boost::asio::ssl::context::default_workarounds
						| boost::asio::ssl::context::no_sslv2 | boost::asio::ssl::context::no_sslv3
						| boost::asio::ssl::context::single_dh_use);

					//_context.set_password_callback(bind(&server::get_password, this))
					//_context.use_certificate_chain_file("server.pem");
					//_context.use_private_key_file("server.pem", boost::asio::ssl::context::pem);
					//_context.use_tmp_dh_file("dh2048.pem");
				}
				std::string get_password() const
				{
					return "test";
				}
				virtual ~HttpsServerImpl() {
					Stop();
				}

				void Start() {

					auto self(shared_from_this());
					auto sock = std::make_shared<HttpsSocketImpl>(_IBaseNetworkDriver, _Asio_Context);
					auto _socket = std::make_shared<HttpsSocket>(sock);
					_acceptor.async_accept(sock->_socket.lowest_layer(), [self, _socket, sock](const boost::system::error_code& ec)
					{
						if (!ec)
						{
							sock->_socket.async_handshake(boost::asio::ssl::stream_base::server, [self, _socket, sock](const boost::system::error_code& ec) {
								if (!ec) {
									sock->start();
								}
								self->Start();
							});
						}
						else {
							SL_RAT_LOG(Utilities::Logging_Levels::INFO_log_level, "Exiting asyncaccept");
						}
					});
				}
				void Stop() {
					_acceptor.close();
					_Asio_Context->Stop();
				}
			};

		}
	}
}


SL::Remote_Access_Library::Network::HttpsListener::HttpsListener(IBaseNetworkDriver<std::shared_ptr<ISocket>, std::shared_ptr<Packet>>* netevent, std::shared_ptr<Network::Server_Config> config)
{
	_HttpsServerImpl = std::make_shared<HttpsServerImpl>(netevent, std::make_shared<HTTPSAsio_Context>(), config);
	_HttpsServerImpl->Start();
}

SL::Remote_Access_Library::Network::HttpsListener::~HttpsListener()
{
	_HttpsServerImpl->Stop();
}
