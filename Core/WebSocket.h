#pragma once
#include <memory>
#include "TCPSocket.h"
#include "crypto.hpp"
#include "HttpHeader.h"
#include <random>
#include <string.h>
#include <utility>

namespace SL {
	namespace Remote_Access_Library {
		namespace Network {

			//this class is async so all calls return immediately and are later executed
			template<typename T>class WebSocket : public TCPSocket<T> {

				unsigned char _readheaderbuffer[8];
				#define MASKSIZE 4
				unsigned char _writeheaderbuffer[sizeof(char)/*type*/ + sizeof(char)/*extra*/ + sizeof(unsigned long long)/*largest size*/ + MASKSIZE/*mask*/];
				
				const std::string ws_magic_string = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
			public:
				explicit WebSocket(IBaseNetworkDriver* netevents, T& socket) :TCPSocket<T>(netevents, socket) {}
				//MUST BE CREATED AS A SHARED_PTR OTHERWISE IT WILL CRASH!
				explicit WebSocket(IBaseNetworkDriver* netevents, boost::asio::io_service& io_service) :TCPSocket<T>(netevents, io_service) {}
				explicit WebSocket(IBaseNetworkDriver* netevents, boost::asio::io_service& io_service, boost::asio::ssl::context& context) : TCPSocket<T>(netevents, io_service, context) {}

				virtual ~WebSocket() {
					this->close_Socket("~WebSocket");
				}
				virtual SocketTypes get_type() const override { return SocketTypes::WEBSOCKET; }
				void Android_writeheader(std::shared_ptr<Packet> packet) {
					this->writeexpire_from_now(this->_SocketImpl.writetimeout);
					auto self(this->shared_from_this());

					boost::asio::async_write(this->_socket, boost::asio::buffer(&this->_SocketImpl.WritePacketHeader, sizeof(this->_SocketImpl.WritePacketHeader)), [self, this, packet](const boost::system::error_code& ec, std::size_t byteswritten)
					{
						if (!ec && !this->closed())
						{
							assert(byteswritten == sizeof(this->_SocketImpl.WritePacketHeader));
							this->writebody(packet);
						}
						else this->close_Socket(std::string("writeheader async_write ") + ec.message());
					});
				}
			private:
			
				void sendHandshake() {
					std::shared_ptr<boost::asio::streambuf> write_buffer(new boost::asio::streambuf);
					auto self(this->shared_from_this());
					std::ostream request(write_buffer.get());

					request << "GET /rdpenpoint/ HTTP/1.1" << HttpHeader::HTTP_ENDLINE;
					request << "Host: " << this->_SocketImpl.Host << HttpHeader::HTTP_ENDLINE;
					request << "Upgrade: websocket" << HttpHeader::HTTP_ENDLINE;
					request << "Connection: Upgrade" << HttpHeader::HTTP_ENDLINE;

					//Make random 16-byte nonce
					std::string nonce;
					nonce.resize(16);
					std::uniform_int_distribution<unsigned short> dist(0, 255);
					std::random_device rd;
					for (int c = 0; c < 16; c++)
						nonce[c] = static_cast<unsigned char>(dist(rd));

					std::string nonce_base64 = Crypto::Base64::encode(nonce);
					request << HttpHeader::HTTP_SECWEBSOCKETKEY << HttpHeader::HTTP_KEYVALUEDELIM << nonce_base64 << HttpHeader::HTTP_ENDLINE;
					request << "Sec-WebSocket-Version: 13" << HttpHeader::HTTP_ENDLINE << HttpHeader::HTTP_ENDLINE;
					std::shared_ptr<std::string> accept_sha1(new std::string(Crypto::SHA1(nonce_base64 + ws_magic_string)));
					boost::asio::async_write(this->_socket, *write_buffer, [this, write_buffer, accept_sha1, self](const boost::system::error_code& ec, size_t /*bytes_transferred*/) {
						if (!ec) {
							std::shared_ptr<boost::asio::streambuf> read_buffer(new boost::asio::streambuf);
							boost::asio::async_read_until(this->_socket, *read_buffer, "\r\n\r\n", [this, read_buffer, accept_sha1, self](const boost::system::error_code& ec, size_t /*bytes_transferred*/) {
								if (!ec) {
									std::istream stream(read_buffer.get());
									this->_SocketImpl.Header = std::move(HttpHeader::Parse("1.1", stream));
									if (Crypto::Base64::decode(this->_SocketImpl.Header[HttpHeader::HTTP_SECWEBSOCKETACCEPT]) == *accept_sha1) {
										this->_SocketImpl.get_Driver()->OnConnect(self);
										readheader();
									}
									else {
										this->close_Socket(std::string("WebSocket handshake failed ") + ec.message());
									}
								}
							});
						}
						else {
							this->close_Socket(std::string("Failed sending handshake ") + ec.message());
						}
					});

				}
				void receivehandshake() {
					auto self(this->shared_from_this());
					std::shared_ptr<boost::asio::streambuf> read_buffer(new boost::asio::streambuf);

					boost::asio::async_read_until(this->_socket, *read_buffer, "\r\n\r\n", [self, read_buffer, this](const boost::system::error_code& ec, size_t bytes_transferred) {
						UNUSED(bytes_transferred);
						if (!ec) {


							std::istream stream(read_buffer.get());
							this->_SocketImpl.Header = std::move(HttpHeader::Parse("1.1", stream));

							if (this->_SocketImpl.Header.count(HttpHeader::HTTP_SECWEBSOCKETKEY) == 0) return this->close_Socket("handshake async_read_until Sec-WebSocket-Key not present");//close socket and get out malformed
							auto write_buffer(std::make_shared<boost::asio::streambuf>());
							std::ostream handshake(write_buffer.get());

							handshake << "HTTP/1.1 101 Web Socket Protocol Handshake" << HttpHeader::HTTP_ENDLINE;
							handshake << "Upgrade: websocket" << HttpHeader::HTTP_ENDLINE;
							handshake << "Connection: Upgrade" << HttpHeader::HTTP_ENDLINE;
								
							handshake << HttpHeader::HTTP_SECWEBSOCKETACCEPT << HttpHeader::HTTP_KEYVALUEDELIM << Crypto::Base64::encode(Crypto::SHA1(this->_SocketImpl.Header[HttpHeader::HTTP_SECWEBSOCKETKEY] + ws_magic_string)) << HttpHeader::HTTP_ENDLINE << HttpHeader::HTTP_ENDLINE;
							boost::asio::async_write(this->_socket, *write_buffer, [this, self, write_buffer](const boost::system::error_code& ec, size_t bytes_transferred) {
								UNUSED(bytes_transferred);
								if (!ec) {
									this->_SocketImpl.get_Driver()->OnConnect(self);
									readheader();

								}
								else {
									this->close_Socket(std::string("handshake async_write ") + ec.message());
								}
							});
						}
						else {
							this->close_Socket(std::string("handshake async_read_until ") + ec.message());
						}
					});
				}

				virtual void handshake()  override {
					if (this->_SocketImpl.Server) receivehandshake();
					else sendHandshake();

				}
				virtual void readheader()  override {
					this->readexpire_from_now(0);
					auto self(this->shared_from_this());
					boost::asio::async_read(this->_socket, boost::asio::buffer(_readheaderbuffer, 2), [this, self](const boost::system::error_code& ec, size_t bytes_transferred) {
						UNUSED(bytes_transferred);
						if (!ec) {
							this->_SocketImpl.ReadPacketHeader.Payload_Length = 0;
							_recv_fin_rsv_opcode = _readheaderbuffer[0];
							//Close connection if unmasked message from client (protocol error)
							if (this->_SocketImpl.Server) {
								if (_readheaderbuffer[1] < 128) return send_close(1002, "Closing connection because mask was not received");
							}
							else {
								if (_readheaderbuffer[1] >= 128) return send_close(1002, "Closing connection because the message was masked");
							}
							auto readbytes = (_readheaderbuffer[1] & 127) == 126 ? 2 : ((_readheaderbuffer[1] & 127) == 127 ? 8 : 0);
							if (readbytes != 0) {
								boost::asio::async_read(this->_socket, boost::asio::buffer(_readheaderbuffer, readbytes), [this, self, readbytes](const boost::system::error_code& ec, size_t bytes_transferred) {
									UNUSED(bytes_transferred);
									if (!ec) {

										for (int c = 0; c <readbytes; c++) {
											this->_SocketImpl.ReadPacketHeader.Payload_Length += _readheaderbuffer[c] << (8 * (readbytes - 1 - c));
										}
										readbody();
									}
									else {
										return this->close_Socket(std::string("readheader async_read ") + ec.message());
									}
								});
							}
							else {
								this->_SocketImpl.ReadPacketHeader.Payload_Length = _readheaderbuffer[1] & 127;
								readbody();
							}
						}
						else {
							this->close_Socket(std::string("readheader async_read ") + ec.message());
						}
					});
				}
				virtual void readbody() override {

					this->readexpire_from_now(this->_SocketImpl.readtimeout);
					auto self(this->shared_from_this());

					this->_SocketImpl.ReadPacketHeader.Payload_Length += this->_SocketImpl.Server ? MASKSIZE : 0;//if this is a server, it receives 4 extra bytes

					auto p(this->_SocketImpl.get_ReadBuffer());
					auto size(this->_SocketImpl.get_ReadBufferSize());
					boost::asio::async_read(this->_socket, boost::asio::buffer(p, size), [this, self](const boost::system::error_code& ec, size_t bytes_transferred) {
						UNUSED(bytes_transferred);
						if (!ec) {
							auto packet(this->_SocketImpl.GetNextReadPacket());
							//If connection close
							if ((_recv_fin_rsv_opcode & 0x0f) == 8) {
								int status = 0;
								if (bytes_transferred >= 2) {
									unsigned char byte1 = packet.Payload[0];
									unsigned char byte2 = packet.Payload[1];
									status = (byte1 << 8) + byte2;
								}
								std::string msg(packet.Payload + 2);
								send_close(status, msg);
							}
							else if ((_recv_fin_rsv_opcode & 0x0f) == 9) {//ping
								std::cout << "PING RECIVED" << std::endl;

								auto writeheader(std::make_shared<std::vector<unsigned char>>());
								writeheader->resize(2);
								auto p(writeheader->data());
								*p++ = 0x1A;
								*p++ = 0;
								boost::asio::async_write(this->_socket, boost::asio::buffer(writeheader->data(), writeheader->size()), [self, this, writeheader](const boost::system::error_code& ec, std::size_t)
								{
									if (ec) this->close_Socket(std::string("ping send failed ") + ec.message());
									this->readheader();
								});
							}
							else {
								if (this->_SocketImpl.Server) {
									//servers receive data masked, so it needs to be unmasked
									unsigned char mask[MASKSIZE];
									memcpy(mask, packet.Payload, sizeof(mask));
									auto startpack = packet.Payload;

									for (size_t c = 0; c < packet.Payload_Length - sizeof(mask); c++) {
										startpack[c] = startpack[c + sizeof(mask)] ^ mask[c % sizeof(mask)];
									}
									memcpy(&this->_SocketImpl.ReadPacketHeader, packet.Payload + MASKSIZE/*sizeof(mask)*/, sizeof(this->_SocketImpl.ReadPacketHeader));
									memmove(packet.Payload, packet.Payload + MASKSIZE + sizeof(this->_SocketImpl.ReadPacketHeader), this->_SocketImpl.ReadPacketHeader.Payload_Length);
								}
								else {//client receives no mask data from the server
									memcpy(&this->_SocketImpl.ReadPacketHeader, packet.Payload, sizeof(this->_SocketImpl.ReadPacketHeader));
									memmove(packet.Payload, packet.Payload + sizeof(this->_SocketImpl.ReadPacketHeader), this->_SocketImpl.ReadPacketHeader.Payload_Length);

								}
								packet.Packet_Type = this->_SocketImpl.ReadPacketHeader.Packet_Type;
								packet.Payload_Length = this->_SocketImpl.ReadPacketHeader.Payload_Length;
								auto spac(std::make_shared<Packet>(std::move(this->decompress(packet))));

								this->_SocketImpl.get_Driver()->OnReceive(self, spac);
								this->readheader();
							}
						}
						else {
							this->close_Socket(std::string("readheader async_read ") + ec.message());
						}
					});
				}
				virtual void writeheader(std::shared_ptr<Packet> packet) override {
					auto p(_writeheaderbuffer);
					*p++ = 130;//binary payload type
					size_t length = packet->Payload_Length + sizeof(this->_SocketImpl.WritePacketHeader);
					if (length >= 126) {
						if (length > 0xffff) {
							*p++ = 127 + (this->_SocketImpl.Server ? 0 : 128);
							auto s = static_cast<unsigned long long>(length);
							for (int c = sizeof(s) - 1; c >= 0; c--) {
								*p++ = (s >> (8 * c)) % 256;
							}
						}
						else {
							*p++ = 126 + (this->_SocketImpl.Server ? 0 : 128);
							auto s = static_cast<unsigned short int>(length);
							for (int c = sizeof(s) - 1; c >= 0; c--) {
								*p++ = (s >> (8 * c)) % 256;
							}
						}
					}
					else {
						*p++ = static_cast<unsigned char>(length + (this->_SocketImpl.Server ? 0 : 128));
					}
					if (!this->_SocketImpl.Server) {//if this is a client, mask the data
						std::uniform_int_distribution<unsigned short> dist(0, 255);
						std::random_device rd;
						unsigned char mask[MASKSIZE];
						for (int c = 0; c < MASKSIZE; c++) {
							mask[c]=*p++ = static_cast<unsigned char>(dist(rd));
						}
						size_t c = 0;
						auto pheader = reinterpret_cast<unsigned char*>(&this->_SocketImpl.WritePacketHeader);
						for (; c < sizeof(this->_SocketImpl.WritePacketHeader); c++) {
							*pheader++ ^= mask[c % MASKSIZE];
						}
						pheader = reinterpret_cast<unsigned char*>(packet->Payload);
						for (; c < packet->Payload_Length; c++) {
							*pheader++ ^= mask[c % MASKSIZE];
						}
					}

					auto self(this->shared_from_this());
					boost::asio::async_write(this->_socket, boost::asio::buffer(_writeheaderbuffer, p - _writeheaderbuffer), [self, this, packet](const boost::system::error_code& ec, std::size_t byteswritten)
					{
						UNUSED(byteswritten);
						if (!ec && !this->closed())
						{
							this->Android_writeheader(packet);//call base
						}
						else this->close_Socket(std::string("writeheader async_write ") + ec.message());
					});

				}
	
				void send_close(int status_code, std::string reason) {
					auto writeheader(std::make_shared<std::vector<unsigned char>>());
					writeheader->resize(2 + reason.size());
					auto p(writeheader->data());

					*(unsigned short int*)p = htons(static_cast<unsigned short int>(status_code));
					memcpy(p + 2, reason.c_str(), reason.size());
					auto self(this->shared_from_this());
					boost::asio::async_write(this->_socket, boost::asio::buffer(writeheader->data(), writeheader->size()), [self, this, writeheader](const boost::system::error_code& ec, std::size_t)
					{
						if (ec) this->close_Socket(std::string("send_close ") + ec.message());
					});
				}
				unsigned char _recv_fin_rsv_opcode = 0;

			};
		}
	}
}
