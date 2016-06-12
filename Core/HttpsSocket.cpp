#include "stdafx.h"
#include "HttpsSocket.h"
#include "IBaseNetworkDriver.h"
#include "HttpHeader.h"
#include "MediaTypes.h"
#include "Server_Config.h"
#include "HttpHeader.h"
#include "Logging.h"
#include "Packet.h"
#include "crypto.h"
#include "ICrypoLoader.h"

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

				HTTPSAsio_Context() : work(io_service) {
					io_servicethread = std::thread([this]() {
						SL_RAT_LOG(Utilities::Logging_Levels::INFO_log_level, "Starting io_service Factory");

						boost::system::error_code ec;
						this->io_service.run(ec);
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


			};



			class HttpsSocketImpl : public ISocket, public std::enable_shared_from_this<HttpsSocketImpl> {
			private:
				std::shared_ptr<HTTPSAsio_Context> _WSSAsio_Context;
			public:

				HttpsSocketImpl(IBaseNetworkDriver* netdriver, boost::asio::io_service& io_service, std::shared_ptr<boost::asio::ssl::context> sslcontext) :
					_socket(io_service, *sslcontext),
                    _IBaseNetworkDriver(netdriver),
					_read_deadline(io_service),
					_write_deadline(io_service)
					
				{
					_read_deadline.expires_at(boost::posix_time::pos_infin);
					_write_deadline.expires_at(boost::posix_time::pos_infin);
					memset(&_SocketStats, 0, sizeof(_SocketStats));
					_Closed = false;
					_readtimeout = _writetimeout = 5;
				}


				virtual ~HttpsSocketImpl() {
					SL_RAT_LOG(Utilities::Logging_Levels::INFO_log_level, "~HttpsSocketImpl");
					CancelTimers();
					close("~HttpsSocketImpl");
				}

				std::shared_ptr<boost::asio::ssl::context> _ssl_context;
				boost::asio::ssl::stream<boost::asio::ip::tcp::socket> _socket;

				IBaseNetworkDriver* _IBaseNetworkDriver;

				boost::asio::deadline_timer _read_deadline;
				boost::asio::deadline_timer _write_deadline;
                
				std::deque<OutgoingPacket> _OutgoingPackets;
				std::vector<char> _IncomingBuffer;
				std::unordered_map<std::string, std::string> _Header;
				bool _Server = false;
				std::string _Host;
				std::string _Port;
				bool _Closed = true;
				bool _Writing = false;
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
					_socket.get_io_service().post([self, compack, beforesize]()
					{
						self->_OutgoingPackets.push_back({ compack, beforesize });
						self->writeheader();
						
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
					_IBaseNetworkDriver->OnClose(this);
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
					if (_Writing) return;//already writing
					_Writing = true;
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
							self->_Writing = false;
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


namespace SL {
	namespace Remote_Access_Library {
		namespace Network {


			class HttpsServerImpl : public std::enable_shared_from_this<HttpsServerImpl> {
			public:

				boost::asio::ip::tcp::acceptor _acceptor;
				std::shared_ptr<Network::Server_Config> _config;
				std::shared_ptr<HTTPSAsio_Context> _HTTPSAsio_Context;
				std::shared_ptr<boost::asio::ssl::context> sslcontext;
				IBaseNetworkDriver* _IBaseNetworkDriver;

				HttpsServerImpl(IBaseNetworkDriver* netevent, std::shared_ptr<HTTPSAsio_Context> asiocontext, std::shared_ptr<Network::Server_Config> config) :
					_acceptor(asiocontext->io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), config->HttpTLSPort)),
					_config(config),
					_HTTPSAsio_Context(asiocontext),
					sslcontext(std::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::tlsv11_server)),
					_IBaseNetworkDriver(netevent)
				{

					sslcontext->set_options(
						boost::asio::ssl::context::default_workarounds
						| boost::asio::ssl::context::no_sslv3
						| boost::asio::ssl::context::single_dh_use);
					boost::system::error_code ec;
					sslcontext->set_password_callback(bind(&HttpsServerImpl::get_password, this), ec);
					if (ec) SL_RAT_LOG(Utilities::Logging_Levels::ERROR_log_level, "set_password_callback error " << ec.message());
					ec.clear();
					boost::asio::const_buffer dhparams(Crypto::dhparams.data(), Crypto::dhparams.size());

					sslcontext->use_tmp_dh(dhparams, ec);
					if (ec) SL_RAT_LOG(Utilities::Logging_Levels::ERROR_log_level, "use_tmp_dh error " << ec.message());
					ec.clear();
					boost::asio::const_buffer cert(config->Public_Certficate->get_buffer(), config->Public_Certficate->get_size());
					sslcontext->use_certificate_chain(cert, ec);

					if (ec) SL_RAT_LOG(Utilities::Logging_Levels::ERROR_log_level, "use_certificate_chain_file error " << ec.message());
					ec.clear();

					sslcontext->set_default_verify_paths(ec);
					if (ec) SL_RAT_LOG(Utilities::Logging_Levels::ERROR_log_level, "set_default_verify_paths error " << ec.message());
					ec.clear();

					boost::asio::const_buffer privkey(config->Private_Key->get_buffer(), config->Private_Key->get_size());
					sslcontext->use_private_key(privkey, boost::asio::ssl::context::pem, ec);
					if (ec) SL_RAT_LOG(Utilities::Logging_Levels::ERROR_log_level, "use_private_key_file error " << ec.message());
					ec.clear();
				}
				virtual ~HttpsServerImpl() {
					Stop();
				}
				std::string get_password() const
				{
					return _config->PasswordToPrivateKey;
				}


				void Start() {
					SL_RAT_LOG(Utilities::Logging_Levels::INFO_log_level, "Starting Accept");
					auto self(shared_from_this());
					auto sock = std::make_shared<HttpsSocketImpl>(_IBaseNetworkDriver, _HTTPSAsio_Context->io_service, sslcontext);
					sock->_Server = true;
					_acceptor.async_accept(sock->_socket.lowest_layer(), [self, sock](const boost::system::error_code& ec)
					{
						if (!ec)
						{
							sock->_socket.async_handshake(boost::asio::ssl::stream_base::server, [self, sock](const boost::system::error_code& ec) {
								if (!ec) {
									sock->start();
								} else {
									
									SL_RAT_LOG(Utilities::Logging_Levels::INFO_log_level, "async_handshake Error: " << ec.message());
								}
								self->Start();
							});
						}
						else {
							SL_RAT_LOG(Utilities::Logging_Levels::INFO_log_level, "Exiting asyncaccept "<< ec.message());
						}
					});
				}
				void Stop() {
					_acceptor.close();
					_HTTPSAsio_Context->Stop();
				}
			};
		}
	}
}


SL::Remote_Access_Library::Network::HttpsListener::HttpsListener(IBaseNetworkDriver* netevent, std::shared_ptr<Network::Server_Config> config)
{
	_HttpsServerImpl = std::make_shared<HttpsServerImpl>(netevent, std::make_shared<HTTPSAsio_Context>(), config);
	_HttpsServerImpl->Start();
}

SL::Remote_Access_Library::Network::HttpsListener::~HttpsListener()
{
	_HttpsServerImpl->Stop();
}
