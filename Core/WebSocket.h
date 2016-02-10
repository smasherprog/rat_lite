#pragma once
#include <memory>
#include "TCPSocket.h"
#include "crypto.hpp"

namespace SL {
	namespace Remote_Access_Library {
		namespace Network {

			//this class is async so all calls return immediately and are later executed
			template<typename T>class WebSocket : public TCPSocket<T> {

			public:
				explicit WebSocket(IBaseNetworkDriver* netevents, T& socket) :TCPSocket(netevents, socket) {}
				//MUST BE CREATED AS A SHARED_PTR OTHERWISE IT WILL CRASH!
				explicit WebSocket(IBaseNetworkDriver* netevents, boost::asio::io_service& io_service) :TCPSocket(netevents, io_service) {}
				virtual ~WebSocket() {

				}
				virtual SocketTypes get_type() const override { return SocketTypes::WEBSOCKET; }

			private:
				const std::string ws_magic_string = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

				virtual void handshake()  override {

					auto self(shared_from_this());
					boost::asio::async_read_until(_socket, _SocketImpl.get_IncomingStreamBuffer(), "\r\n\r\n", [this, self](const boost::system::error_code& ec, size_t s/*bytes_transferred*/) {
						if (!ec) {
							_SocketImpl.parse_header("1.1", s);
							if (_SocketImpl.get_Header().count("Sec-WebSocket-Key") == 0) return close();//close socket and get out malformed
							auto write_buffer(std::make_shared<boost::asio::streambuf>());
							std::ostream handshake(write_buffer.get());
							auto sha1 = Crypto::SHA1(_SocketImpl.get_Header()["Sec-WebSocket-Key"] + ws_magic_string);
							handshake << "HTTP/1.1 101 Web Socket Protocol Handshake\r\n";
							handshake << "Upgrade: websocket\r\n";
							handshake << "Connection: Upgrade\r\n";
							handshake << "Sec-WebSocket-Accept: " << Crypto::Base64::encode(sha1) << "\r\n";
							handshake << "\r\n";
							boost::asio::async_write(_socket, *write_buffer, [this, self, write_buffer](const boost::system::error_code& ec, size_t /*bytes_transferred*/) {
								if (!ec) {
									_SocketImpl.get_Driver()->OnConnect(self);
									readheader();
								}
								else {
									close();
								}
							});
						}
						else {
							close();
						}
					});
				}
				virtual void readheader()  override {

					auto self(shared_from_this());
					boost::asio::async_read(_socket, _SocketImpl.get_IncomingStreamBuffer(), boost::asio::transfer_exactly(2), [this, self](const boost::system::error_code& ec, size_t /*bytes_transferred*/) {
						if (!ec) {

							std::istream stream(&_SocketImpl.get_IncomingStreamBuffer());

							unsigned char first_bytes[2];
						
							stream.read((char*)&first_bytes[0], 2);

							_fin_rsv_opcode = first_bytes[0];
							//Close connection if unmasked message from client (protocol error)
							if (first_bytes[1] < 128) return close();
							_IncomingContentLength = (first_bytes[1] & 127);

							if (_IncomingContentLength == 126) {
								//2 next bytes is the size of content
								boost::asio::async_read(_socket, _SocketImpl.get_IncomingStreamBuffer(), boost::asio::transfer_exactly(2), [this, self](const boost::system::error_code& ec, size_t /*bytes_transferred*/) {
									if (!ec) {
										std::istream stream(&_SocketImpl.get_IncomingStreamBuffer());

										unsigned char length_bytes[2];
										stream.read((char*)&length_bytes[0], 2);

										_IncomingContentLength = 0;
										int num_bytes = 2;
										for (int c = 0; c < num_bytes; c++)
											_IncomingContentLength += length_bytes[c] << (8 * (num_bytes - 1 - c));
										readbody();
									}
									else {
										close();
									}
								});
							}
							else if (_IncomingContentLength == 127) {
								//8 next bytes is the size of content
								boost::asio::async_read(_socket, _SocketImpl.get_IncomingStreamBuffer(), boost::asio::transfer_exactly(8), [this, self](const boost::system::error_code& ec, size_t /*bytes_transferred*/) {
									if (!ec) {
										std::istream stream(&_SocketImpl.get_IncomingStreamBuffer());

										unsigned char length_bytes[8];
										stream.read((char*)&length_bytes[0], 8);

										_IncomingContentLength = 0;
										int num_bytes = 8;
										for (int c = 0; c < num_bytes; c++)
											_IncomingContentLength += length_bytes[c] << (8 * (num_bytes - 1 - c));
										readbody();
									}
									else {
										close();
									}
								});
							}
							else {
								readbody();
							}
						}
						else {
							close();
						}
					});
				}
				virtual void readbody() override {

					auto self(shared_from_this());
					boost::asio::async_read(_socket, _SocketImpl.get_IncomingStreamBuffer(), boost::asio::transfer_exactly(4 + _IncomingContentLength), [this, self](const boost::system::error_code& ec, size_t /*bytes_transferred*/) {
						if (!ec) {
							std::istream raw_message_data(&_SocketImpl.get_IncomingStreamBuffer());

							//Read mask
							unsigned char mask[4];
							raw_message_data.read((char*)&mask[0], 4);

							auto packet(std::make_shared<Packet>(static_cast<unsigned int>(PACKET_TYPES::WEBSOCKET_MSG), _IncomingContentLength, std::move(_SocketImpl.get_Header())));
							_SocketImpl.get_Header().clear();//reset after move
							auto startpack = packet->Payload;
							for (size_t c = 0; c < _IncomingContentLength; c++) {
								startpack[c] = (raw_message_data.get() ^ mask[c % 4]);
							}

							//If connection close
							if ((_fin_rsv_opcode & 0x0f) == 8) {
								return close();
							}
							else {
								//If ping
								if ((_fin_rsv_opcode & 0x0f) == 9) {
									auto write_buffer(std::make_shared<boost::asio::streambuf>());
									std::ostream outbuf(write_buffer.get());
									outbuf.put(_fin_rsv_opcode + 1);
									outbuf.put(0);
									boost::asio::async_write(_socket, *write_buffer, [this, self, write_buffer](const boost::system::error_code& ec, size_t /*bytes_transferred*/) {
										if (!ec) {
											readheader();
										}
										else {
											close();
										}
									});
								}
								else {
									_SocketImpl.get_Driver()->OnReceive(self, packet);
								}
								readheader();
							}
						}
						else {
							close();
						}
					});
				}
				virtual void writeheader(std::shared_ptr<Packet> packet) override {
					auto self(shared_from_this());
					auto write_buffer(std::make_shared<boost::asio::streambuf>());
					std::ostream header_stream(write_buffer.get());

					///fin_rsv_opcode: 129=one fragment, text, 130=one fragment, binary, 136=close connection.
					header_stream.put((char)129);
					size_t length = packet->Payload_Length;
					if (length >= 126) {
						int num_bytes;
						if (length > 0xffff) {
							num_bytes = 8;
							header_stream.put(127);
						}
						else {
							num_bytes = 2;
							header_stream.put(126);
						}

						for (int c = num_bytes - 1; c >= 0; c--) {
							header_stream.put((length >> (8 * c)) % 256);
						}
					}
					else {
						header_stream.put(static_cast<unsigned char>(length));
					}

					boost::asio::async_write(_socket, *write_buffer, [self, this, packet, write_buffer](boost::system::error_code ec, std::size_t byteswritten)
					{
						if (!ec && !closed())
						{
							writebody(packet);
						}
						else close();
					});

				}
				virtual Packet compress(Packet& packet)  override {
					Packet p(packet.Packet_Type, packet.Payload_Length);
					memcpy(p.Payload, packet.Payload, packet.Payload_Length);
					p.Header = packet.Header;
					return p;
				}
				virtual Packet decompress(Packet& packet)  override {
					Packet p(packet.Packet_Type, packet.Payload_Length);
					memcpy(p.Payload, packet.Payload, packet.Payload_Length);
					p.Header = packet.Header;
					return p;
				}


				unsigned int _IncomingContentLength = 0;
				unsigned char _fin_rsv_opcode = 0;
				
			};
		}
	}
}
