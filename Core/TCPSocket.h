#pragma once
#include <memory>
#include "ISocket.h"
#include "IBaseNetworkDriver.h"
#include "Packet.h"
#include <asio.hpp>
#include <asio/deadline_timer.hpp>
#include <asio/ssl.hpp>
#include "zstd.h"
#include "SocketImpl.h"
#include <string>
#include "Logging.h"

namespace SL {
	namespace Remote_Access_Library {

		namespace Network {

			typedef asio::ssl::stream<asio::ip::tcp::socket> ssl_socket;
			typedef asio::ip::tcp::socket socket;

			//this class is async so all calls return immediately and are later executed
			template<typename BASESOCKET>class TCPSocket : public ISocket {
			protected:
				BASESOCKET _socket;
				SocketImpl _SocketImpl;
				asio::io_service& _io_service;
			public:
				static_assert(std::is_same<BASESOCKET, asio::ip::tcp::socket>::value || std::is_same<BASESOCKET, asio::ssl::stream<asio::ip::tcp::socket>>::value, "BASESOCKET must be asio::ip::tcp::socket or asio::ssl::stream<asio::ip::tcp::socket>");

				//MUST BE CREATED AS A SHARED_PTR OTHERWISE IT WILL CRASH!
				explicit TCPSocket(IBaseNetworkDriver* netevents, BASESOCKET& socket) :_socket(std::move(socket)), _SocketImpl(_socket.get_io_service(), netevents), _io_service(_socket.get_io_service()) { }
				//MUST BE CREATED AS A SHARED_PTR OTHERWISE IT WILL CRASH!
				explicit TCPSocket(IBaseNetworkDriver* netevents, asio::io_service& io_service) : _socket(io_service), _SocketImpl(io_service, netevents), _io_service(io_service) { }
				explicit TCPSocket(IBaseNetworkDriver* netevents, asio::io_service& io_service, asio::ssl::context& context) : _socket(io_service, context), _SocketImpl(io_service, netevents), _io_service(io_service) { }

				virtual ~TCPSocket() {
					close_Socket("~TCPSocket");
				}
				BASESOCKET& get_socket() { return _socket; }
				//adds the data to the internal queue, does not block and returns immediately.
				virtual void send(Packet& pack) override
				{
					auto self(this->shared_from_this());
					auto compack(std::make_shared<Packet>(std::move(compress(pack))));
					auto beforesize = pack.Payload_Length;
					_io_service.post([this, self, compack, beforesize]()
					{
						_SocketImpl.AddOutgoingPacket(compack, beforesize);
						if (!_SocketImpl.writing())
						{
							_SocketImpl.writing(true);
							writeexpire_from_now(30);
							writeheader(_SocketImpl.GetNextWritePacket());
						}
					});
				}

				virtual void set_ReadTimeout(int s)override {
					assert(s > 0);
					_SocketImpl.readtimeout = s;
				}

				virtual void set_WriteTimeout(int s)override {
					assert(s > 0);
					_SocketImpl.writetimeout = s;
				}

				//sends a request that the socket be closed. NetworkEvents::OnClose will be called when the call is successful
				virtual bool closed() override {
					return _SocketImpl.Closed || !_socket.lowest_layer().is_open();
				}
				virtual void close_Socket(std::string reason) override {
				
					SL_RAT_LOG("Closing socket: " + reason, Utilities::Logging_Levels::INFO_log_level);
					if (closed()) return;
					_SocketImpl.Closed = true;
					_SocketImpl.CancelTimers();
					_SocketImpl.get_Driver()->OnClose(this->shared_from_this());
					try
					{
						std::error_code ec;

						_socket.lowest_layer().shutdown(asio::ip::tcp::socket::shutdown_both, ec);
						_socket.lowest_layer().close();
					}
					catch (std::exception e) {
						auto st = e.what();
						SL_RAT_LOG(e.what(), Utilities::Logging_Levels::ERROR_log_level);
					}//I dont care about exceptions when the socket is being closed!


				}
				//pending packets which are queued up and waiting to be sent
				virtual SocketStats get_SocketStats() const override {
					return _SocketImpl.get_Socketstats();
				}
				//calling connected with a host and port will attempt an async connection returns immediatly and executes the OnConnect Callback when connected. If connection fails, the OnClose callback is called
				virtual void connect(const char* host, const char* port) override {

					_SocketImpl.Host = host != nullptr ? host : "";
					_SocketImpl.Port = port != nullptr ? port : "";
					auto self(this->shared_from_this());
					if (host == nullptr && port == nullptr) {
						_SocketImpl.Server = true;
						handshake();
					}
					else {
						_SocketImpl.Server = false;
						try
						{
							asio::ip::tcp::resolver resolver(_io_service);
							asio::ip::tcp::resolver::query query(host, port);
							auto endpoint = resolver.resolve(query);

							asio::async_connect(_socket.lowest_layer(), endpoint, [self, this](const std::error_code& ec, asio::ip::tcp::resolver::iterator)
							{
								if (!closed()) {
									handshake();
								}
								else close_Socket(std::string("async_connect ") + ec.message());
							});
						}
						catch (std::exception ex) {
							close_Socket(std::string("async_connect ") + ex.what());
						}
					}
				}
				virtual SocketTypes get_type() const override { return SocketTypes::TCPSOCKET; }


			protected:


				virtual void handshake() override {

					auto self(this->shared_from_this());
					_SocketImpl.get_Driver()->OnConnect(self);
					readheader();

				}
				virtual void readheader()  override {
					readexpire_from_now(0);
					auto self(this->shared_from_this());
					asio::async_read(_socket, asio::buffer(&_SocketImpl.ReadPacketHeader, sizeof(_SocketImpl.ReadPacketHeader)), [self, this](const std::error_code& ec, std::size_t len/*length*/)
					{
						if (!ec && !closed()) {
							assert(len == sizeof(_SocketImpl.ReadPacketHeader));
							readbody();
						}
						else close_Socket(std::string("readheader async_read ") + ec.message());
					});

				}
				virtual void readbody() override {
					readexpire_from_now(_SocketImpl.readtimeout);
					auto self(this->shared_from_this());
					auto p(_SocketImpl.get_ReadBuffer());
					auto size(_SocketImpl.get_ReadBufferSize());
					asio::async_read(_socket, asio::buffer(p, size), [self, this](const std::error_code& ec, std::size_t len/*length*/)
					{
						if (!ec && !closed()) {
							assert(len == _SocketImpl.get_ReadBufferSize());
							auto pac(_SocketImpl.GetNextReadPacket());
							auto spac(std::make_shared<Packet>(std::move(decompress(pac))));
							_SocketImpl.get_Driver()->OnReceive(self, spac);
							readheader();
						}
						else close_Socket(std::string("readbody async_read ") + ec.message());
					});


				}
				virtual void writeheader(std::shared_ptr<Packet> packet) override
				{
					writeexpire_from_now(_SocketImpl.writetimeout);
					auto self(this->shared_from_this());

					asio::async_write(_socket, asio::buffer(&_SocketImpl.WritePacketHeader, sizeof(_SocketImpl.WritePacketHeader)), [self, this, packet](const std::error_code& ec, std::size_t byteswritten)
					{
						if (!ec && !closed())
						{
							assert(byteswritten == sizeof(_SocketImpl.WritePacketHeader));
							writebody(packet);
						}
						else close_Socket(std::string("writeheader async_write ") + ec.message());
					});
				}
				virtual void writebody(std::shared_ptr<Packet> packet) override {

					auto self(this->shared_from_this());
					asio::async_write(_socket, asio::buffer(packet->Payload, packet->Payload_Length), [self, this, packet](const std::error_code& ec, std::size_t byteswritten)
					{
						if (!ec && !closed())
						{
							assert(byteswritten == packet->Payload_Length);

							if (!_SocketImpl.OutGoingBuffer_empty()) {
								writeheader(_SocketImpl.GetNextWritePacket());
							}
							else {
								writeexpire_from_now(0);
								_SocketImpl.writing(false);
							}
						}
						else close_Socket(std::string("writebody async_write ") + ec.message());
					});
				}


				virtual Packet compress(Packet& packet) override {
					auto maxsize = ZSTD_compressBound(packet.Payload_Length);
					Packet p(packet.Packet_Type, static_cast<unsigned int>(maxsize), std::move(packet.Header));
					p.Payload_Length = static_cast<unsigned int>(ZSTD_compress(p.Payload, maxsize, packet.Payload, static_cast<unsigned int>(packet.Payload_Length), 3));
					_SocketImpl.UpdateWriteStats(p, packet.Payload_Length);
					return p;
				}
				virtual Packet decompress(Packet& packet)  override {
					Packet p(packet.Packet_Type, _SocketImpl.ReadPacketHeader.UncompressedLength, std::move(packet.Header));
					p.Payload_Length = static_cast<unsigned int>(ZSTD_decompress(p.Payload, _SocketImpl.ReadPacketHeader.UncompressedLength, packet.Payload, packet.Payload_Length));
					if (ZSTD_isError(p.Payload_Length) > 0) {
						return Packet(static_cast<unsigned int>(PACKET_TYPES::INVALID));//empty packet
					}
					_SocketImpl.UpdateReadStats();
					return p;
				}


				void readexpire_from_now(int seconds) {


					_SocketImpl.StartReadTimer(seconds);
					if (seconds >= 0) {
						auto self(this->shared_from_this());

						_SocketImpl.read_deadline_.async_wait([this, self, seconds](std::error_code ec) {
							if (ec != asio::error::operation_aborted) {
								close_Socket("read timer expired. Time waited: ");
								//close_Socket("read timer expired. Time waited: " + std::to_string(seconds));
							}
						});
					}
				}
				void writeexpire_from_now(int seconds) {

					_SocketImpl.StartWriteTimer(seconds);
					if (seconds >= 0) {
						auto self(this->shared_from_this());
						_SocketImpl.write_deadline_.async_wait([this, self, seconds](std::error_code ec) {
							if (ec != asio::error::operation_aborted) {
								//close_Socket("write timer expired. Time waited: " + std::to_string(seconds));
								close_Socket("write timer expired. Time waited: ");
							}
						});
					}
				}
			};
		};
	};
};