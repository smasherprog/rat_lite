#pragma once
#include <memory>
#include "TCPSocket.h"
#include "crypto.hpp"
#include "HttpHeader.h"

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
				virtual void handshake()  override {

					auto self(this->shared_from_this());
					std::shared_ptr<boost::asio::streambuf> read_buffer(new boost::asio::streambuf);

					boost::asio::async_read_until(_socket, *read_buffer, "\r\n\r\n", [this, self, read_buffer](const boost::system::error_code& ec, size_t bytes_transferred) {
						if (!ec) {

						
							std::istream stream(read_buffer.get());
							_SocketImpl.Header = std::move(HttpHeader::Parse("1.1", stream));

							if (_SocketImpl.Header.count("Sec-WebSocket-Key") == 0) return close("handshake async_read_until Sec-WebSocket-Key not present");//close socket and get out malformed
							auto write_buffer(std::make_shared<boost::asio::streambuf>());
							std::ostream handshake(write_buffer.get());

							handshake << "HTTP/1.1 101 Web Socket Protocol Handshake\r\n";
							handshake << "Upgrade: websocket\r\n";
							handshake << "Connection: Upgrade\r\n";
							handshake << "Sec-WebSocket-Accept: " << Crypto::Base64::encode(Crypto::SHA1(_SocketImpl.Header["Sec-WebSocket-Key"] + ws_magic_string)) << "\r\n\r\n";
							boost::asio::async_write(_socket, *write_buffer, [this, self, write_buffer](const boost::system::error_code& ec, size_t bytes_transferred) {
								if (!ec) {
									_SocketImpl.get_Driver()->OnConnect(self);
									readheader();

								}
								else {
									close(std::string("handshake async_write ") + ec.message());
								}
							});
						}
						else {
							close(std::string("handshake async_read_until ") + ec.message());
						}
					});
				}
				virtual void readheader()  override {

					auto self(this->shared_from_this());
					boost::asio::async_read(_socket, boost::asio::buffer(_readheaderbuffer2, sizeof(_readheaderbuffer2)), [this, self](const boost::system::error_code& ec, size_t bytes_transferred) {
						if (!ec) {
							_SocketImpl.ReadPacketHeader.Payload_Length = 0;
							_recv_fin_rsv_opcode = _readheaderbuffer2[0];
							//Close connection if unmasked message from client (protocol error)
							if (_readheaderbuffer2[1] < 128) return send_close(1002, "Closing connection because mask was not received");

							if (_SocketImpl.ReadPacketHeader.Payload_Length == 126) {
								//2 next bytes is the size of content
								boost::asio::async_read(_socket, boost::asio::buffer(_readheaderbuffer2, sizeof(_readheaderbuffer2)), [this, self](const boost::system::error_code& ec, size_t bytes_transferred) {
									if (!ec) {
										for (int c = 0; c < 2; c++) {
											_SocketImpl.ReadPacketHeader.Payload_Length += _readheaderbuffer2[c] << (8 * (2 - 1 - c));
										}
										readbody();
									}
									else {
										return close(std::string("readheader async_read ") + ec.message());
									}
								});
							}
							else if (_SocketImpl.ReadPacketHeader.Payload_Length == 127) {
								//8 next bytes is the size of content
								boost::asio::async_read(_socket, boost::asio::buffer(_readheaderbuffer8, sizeof(_readheaderbuffer8)), [this, self](const boost::system::error_code& ec, size_t bytes_transferred) {
									if (!ec) {
										for (int c = 0; c < 8; c++) {
											_SocketImpl.ReadPacketHeader.Payload_Length += _readheaderbuffer2[c] << (8 * (8 - 1 - c));
										}
										readbody();
									}
									else {
										close(std::string("readheader async_read ") + ec.message());
									}
								});
							}
							else {
								_SocketImpl.ReadPacketHeader.Payload_Length = _readheaderbuffer2[1] & 127;
								readbody();
							}
						}
						else {
							close(std::string("readheader async_read ") + ec.message());
						}
					});
				}
				virtual void readbody() override {

					auto self(this->shared_from_this());
					_SocketImpl.ReadPacketHeader.Payload_Length += 4;//always 4 more because of the mask
					auto p(_SocketImpl.get_ReadBuffer());
					auto size(_SocketImpl.get_ReadBufferSize());
					boost::asio::async_read(_socket, boost::asio::buffer(p, size), [this, self](const boost::system::error_code& ec, size_t bytes_transferred) {
						if (!ec) {

							auto packet(std::make_shared<Packet>(std::move(_SocketImpl.GetNextReadPacket())));
							packet->Packet_Type = 0;
							auto mask = reinterpret_cast<unsigned char*>(packet->Payload);

							auto startpack = packet->Payload + 4;
							for (size_t c = 0; c < packet->Payload_Length - 4; c++) {
								startpack[c] ^= mask[c % 4];
							}

							//If connection close
							if ((_recv_fin_rsv_opcode & 0x0f) == 8) {
								int status = 0;
								if (bytes_transferred >= 2) {
									unsigned char byte1 = startpack[0];
									unsigned char byte2 = startpack[1];
									status = (byte1 << 8) + byte2;
								}
								std::string msg(startpack + 2);
								send_close(status, msg);
								return  close("readheader (_recv_fin_rsv_opcode & 0x0f) == 8 " + msg);
							} else if ((_recv_fin_rsv_opcode & 0x0f) == 9) {//ping
								std::cout << "PING RECIVED" << std::endl;
								Packet pingpacket(static_cast<unsigned int>(PACKET_TYPES::WEBSOCKET_PING));
								send(pingpacket);
							}
							else {
								_SocketImpl.get_Driver()->OnReceive(self, packet);
							}
						}
						else {
							close(std::string("readheader async_read ") + ec.message());
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
						extralength += sizeof(_SocketImpl.WritePacketHeader);
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


					boost::asio::async_write(_socket, boost::asio::buffer(p, size), [self, this, packet](const boost::system::error_code& ec, std::size_t byteswritten)
					{
						if (!ec && !closed())
						{
							if (packet->Packet_Type == static_cast<unsigned int>(PACKET_TYPES::WEBSOCKET_CLOSE) ||
								packet->Packet_Type == static_cast<unsigned int>(PACKET_TYPES::WEBSOCKET_PING)) {// in this case, there is only a payload
								TCPSocket::writebody(packet);//write payload
							}
							else {
								TCPSocket::writeheader(packet);//call base
							}
						}
						else close(std::string("writeheader async_write ") + ec.message());
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
					send(p);
					auto self(this->shared_from_this());
					_socket.get_io_service().post([this, self]()
					{
						close("send_close");//close this socket after the send has completed
					});
				}
				unsigned char _recv_fin_rsv_opcode = 0;

			};
		}
	}
}
