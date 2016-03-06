#pragma once
#include <memory>
#include "TCPSocket.h"
#include "crypto.hpp"
#include "HttpHeader.h"
#include <random>

namespace SL {
	namespace Remote_Access_Library {
		namespace Network {

			//this class is async so all calls return immediately and are later executed
			template<typename T>class WebSocket : public TCPSocket<T> {

				unsigned char _readheaderbuffer2[2];
				unsigned char _readheaderbuffer8[8];

				unsigned char _writeheaderbuffer[sizeof(char) + sizeof(char) + sizeof(unsigned long long)];

			public:
				explicit WebSocket(IBaseNetworkDriver* netevents, T& socket) :TCPSocket<T>(netevents, socket) {}
				//MUST BE CREATED AS A SHARED_PTR OTHERWISE IT WILL CRASH!
				explicit WebSocket(IBaseNetworkDriver* netevents, boost::asio::io_service& io_service) :TCPSocket<T>(netevents, io_service) {}
				virtual ~WebSocket() {

				}
				virtual SocketTypes get_type() const override { return SocketTypes::WEBSOCKET; }

			private:
				const std::string ws_magic_string = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
				void parse_header() {

				}
				void sendHandshake() {
					std::shared_ptr<boost::asio::streambuf> write_buffer(new boost::asio::streambuf);
					auto self(this->shared_from_this());
					std::ostream request(write_buffer.get());

					request << "GET /rdpenpoint/ HTTP/1.1" << "\r\n";
					request << "Host: " << this->_SocketImpl.Host << "\r\n";
					request << "Upgrade: websocket\r\n";
					request << "Connection: Upgrade\r\n";

					//Make random 16-byte nonce
					std::string nonce;
					nonce.resize(16);
					std::uniform_int_distribution<unsigned short> dist(0, 255);
					std::random_device rd;
					for (int c = 0; c<16; c++)
						nonce[c] = static_cast<unsigned char>(dist(rd));

					std::string nonce_base64 = Crypto::Base64::encode(nonce);
					request << HttpHeader::HTTP_SECWEBSOCKETKEY << HttpHeader::HTTP_KEYVALUEDELIM << nonce_base64 << HttpHeader::HTTP_ENDLINE;
					request << "Sec-WebSocket-Version: 13"<< HttpHeader::HTTP_ENDLINE << HttpHeader::HTTP_ENDLINE;
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

							handshake << "HTTP/1.1 101 Web Socket Protocol Handshake\r\n";
							handshake << "Upgrade: websocket\r\n";
							handshake << "Connection: Upgrade\r\n";

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
					auto self(this->shared_from_this());
					boost::asio::async_read(this->_socket, boost::asio::buffer(_readheaderbuffer2, sizeof(_readheaderbuffer2)), [this, self](const boost::system::error_code& ec, size_t bytes_transferred) {
						UNUSED(bytes_transferred);
						if (!ec) {
							this->_SocketImpl.ReadPacketHeader.Payload_Length = 0;
							_recv_fin_rsv_opcode = _readheaderbuffer2[0];
							//Close connection if unmasked message from client (protocol error)
							if (this->_SocketImpl.Server) {
								if (_readheaderbuffer2[1] < 128) return send_close(1002, "Closing connection because mask was not received");
							}
							else {
								if (_readheaderbuffer2[1] >= 128) return send_close(1002, "Closing connection because the message was masked");
							}
							this->_SocketImpl.ReadPacketHeader.Payload_Length = (_readheaderbuffer2[1] & 127);

							if (this->_SocketImpl.ReadPacketHeader.Payload_Length == 126) {
								//2 next bytes is the size of content
								boost::asio::async_read(this->_socket, boost::asio::buffer(_readheaderbuffer2, sizeof(_readheaderbuffer2)), [this, self](const boost::system::error_code& ec, size_t bytes_transferred) {
									UNUSED(bytes_transferred);
									if (!ec) {
										for (int c = 0; c < 2; c++) {
											this->_SocketImpl.ReadPacketHeader.Payload_Length += _readheaderbuffer2[c] << (8 * (2 - 1 - c));
										}
										readbody();
									}
									else {
										return this->close_Socket(std::string("readheader async_read ") + ec.message());
									}
								});
							}
							else if (this->_SocketImpl.ReadPacketHeader.Payload_Length == 127) {
								//8 next bytes is the size of content
								boost::asio::async_read(this->_socket, boost::asio::buffer(_readheaderbuffer8, sizeof(_readheaderbuffer8)), [this, self](const boost::system::error_code& ec, size_t bytes_transferred) {
									UNUSED(bytes_transferred);
									if (!ec) {
										for (int c = 0; c < 8; c++) {
											this->_SocketImpl.ReadPacketHeader.Payload_Length += _readheaderbuffer2[c] << (8 * (8 - 1 - c));
										}
										readbody();
									}
									else {
										this->close_Socket(std::string("readheader async_read ") + ec.message());
									}
								});
							}
							else {
								this->_SocketImpl.ReadPacketHeader.Payload_Length = _readheaderbuffer2[1] & 127;
								readbody();
							}
						}
						else {
							this->close_Socket(std::string("readheader async_read ") + ec.message());
						}
					});
				}
				virtual void readbody() override {

					auto self(this->shared_from_this());

					this->_SocketImpl.ReadPacketHeader.Payload_Length += this->_SocketImpl.Server ? 4 : 0;//if this is a server, it receives 4 extra bytes

					auto p(this->_SocketImpl.get_ReadBuffer());
					auto size(this->_SocketImpl.get_ReadBufferSize());
					boost::asio::async_read(this->_socket, boost::asio::buffer(p, size), [this, self](const boost::system::error_code& ec, size_t bytes_transferred) {
						UNUSED(bytes_transferred);
						if (!ec) {
							auto packet(std::make_shared<Packet>(std::move(this->_SocketImpl.GetNextReadPacket())));
							//If connection close
							if ((_recv_fin_rsv_opcode & 0x0f) == 8) {
								int status = 0;
								if (bytes_transferred >= 2) {
									unsigned char byte1 = packet->Payload[0];
									unsigned char byte2 = packet->Payload[1];
									status = (byte1 << 8) + byte2;
								}
								std::string msg(packet->Payload + 2);
								send_close(status, msg);
								return this->close_Socket("readheader (_recv_fin_rsv_opcode & 0x0f) == 8 " + msg);
							}
							else if ((_recv_fin_rsv_opcode & 0x0f) == 9) {//ping
								std::cout << "PING RECIVED" << std::endl;
							
								auto pingreponse(std::make_shared<char[]>(2));
								auto p(pingreponse->get())
								*p++ = 0x1A;
								*p++ = 0;
								boost::asio::async_write(this->_socket, boost::asio::buffer(p, 2), [self, this, pingreponse](const boost::system::error_code& ec, std::size_t )
								{
									if (ec) this->close_Socket(std::string("ping send failed ") + ec.message());
								});
								Packet pingpacket(static_cast<unsigned int>(PACKET_TYPES::WEBSOCKET_PING));
								this->send(pingpacket);
							}
							else {
								
								
								if (this->_SocketImpl.Server) {
									//servers receive data masked, so it needs to be unmasked
									unsigned char mask[4];
									memcpy(mask, packet->Payload, sizeof(mask));
									auto startpack = packet->Payload;
									for (size_t c = 0; c < packet->Payload_Length - sizeof(mask); c++) {
										startpack[c] = startpack[c+ sizeof(mask)] ^ mask[c % sizeof(mask)];
									}
									packet->Payload_Length -= sizeof(mask);//adjust the size of the packet correctly
								}


								this->_SocketImpl.get_Driver()->OnReceive(self, packet);
							}
						}
						else {
							this->close_Socket(std::string("readheader async_read ") + ec.message());
						}
					});
				}
				virtual void writeheader(std::shared_ptr<Packet> packet) override {

					unsigned char* p(_writeheaderbuffer);
					///fin_rsv_opcode: 129=one fragment, text, 130=one fragment, binary, 136=close connection.
					unsigned int extralength = 0;
					switch (packet->Packet_Type) {
					case(static_cast<unsigned int>(PACKET_TYPES::WEBSOCKET_PING)) :
						*p++ = 0x1A;
						break;
					case(static_cast<unsigned int>(PACKET_TYPES::WEBSOCKET_CLOSE)) :
						*p++ = 136;
						break;
					default://default is binary
						*p++ = 130;
						extralength += sizeof(this->_SocketImpl.WritePacketHeader);
						break;
					}
					size_t length = packet->Payload_Length + extralength;
					if (length >= 126) {
						if (length > 0xffff) {
							*p++ = 127;
							auto s = static_cast<unsigned long long>(length);
							for (int c = sizeof(s) - 1; c >= 0; c--) {
								*p++ = (s >> (8 * c)) % 256;
							}
						}
						else {
							*p++ = 126;
							auto s = static_cast<unsigned short int>(length);
							for (int c = sizeof(s) - 1; c >= 0; c--) {
								*p++ = (s >> (8 * c)) % 256;
							}
						}
					}
					else {
						*p++ = static_cast<unsigned char>(length);
					}

					auto self(this->shared_from_this());
					auto size(p - _writeheaderbuffer);
					p = _writeheaderbuffer;


					boost::asio::async_write(this->_socket, boost::asio::buffer(p, size), [self, this, packet](const boost::system::error_code& ec, std::size_t byteswritten)
					{
						UNUSED(byteswritten);
						if (!ec && !this->closed())
						{
							if (packet->Packet_Type == static_cast<unsigned int>(PACKET_TYPES::WEBSOCKET_CLOSE) ||
								packet->Packet_Type == static_cast<unsigned int>(PACKET_TYPES::WEBSOCKET_PING)) {// in this case, there is only a payload
								TCPSocket<T>::writebody(packet);//write payload
							}
							else {
								TCPSocket<T>::writeheader(packet);//call base
							}
						}
						else this->close_Socket(std::string("writeheader async_write ") + ec.message());
					});

				}
				virtual Packet decompress(Packet& packet)  override {
					Packet p(packet.Packet_Type, packet.Payload_Length);
					memcpy(p.Payload, packet.Payload, packet.Payload_Length);
					p.Header = packet.Header;
					return p;
				}
				void send_close(int status_code, std::string reason) {
					Packet p(static_cast<unsigned int>(PACKET_TYPES::WEBSOCKET_CLOSE), static_cast<unsigned int>(2 + reason.size()));
					*(unsigned short int*)p.Payload = htons(static_cast<unsigned short int>(status_code));
					memcpy(p.Payload + 2, reason.c_str(), reason.size());
					this->send(p);
					auto self(this->shared_from_this());
					this->_socket.get_io_service().post([this, self, reason]()
					{

						this->close_Socket(reason + "send_close");//close this socket after the send has completed
					});
				}
				unsigned char _recv_fin_rsv_opcode = 0;

			};
		}
	}
}
