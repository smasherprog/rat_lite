#include "stdafx.h"
#include "WebSocket.h"
#include <boost\asio.hpp>
#include "Packet.h"
#include "crypto.hpp"
#include "IBaseNetworkDriver.h"

namespace SL {
	namespace Remote_Access_Library {
		namespace Network {
			namespace INTERNAL {

				class WebSocketSocketImpl {
				public:
					WebSocketSocketImpl(IBaseNetworkDriver* nets) : NetworkEvents(nets) {}
					boost::asio::streambuf _IncomingBuffer;
					boost::asio::streambuf _OutGoingBuffer;
					IBaseNetworkDriver* NetworkEvents;

					std::unordered_map<std::string, std::string> Header;

					unsigned int _IncomingContentLength;
					unsigned char _fin_rsv_opcode;



					void parse_handshake()
					{
						std::istream stream(&_IncomingBuffer);
						std::string line;
						getline(stream, line);
						size_t method_end;
						if ((method_end = line.find(' ')) != std::string::npos) {
							size_t path_end;
							if ((path_end = line.find(' ', method_end + 1)) != std::string::npos) {
								Header[HTTP_METHOD] = line.substr(0, method_end);
								Header[HTTP_PATH] = line.substr(method_end + 1, path_end - method_end - 1);
								if ((path_end + 6) < line.size())
									Header[HTTP_VERSION] = line.substr(path_end + 6, line.size() - (path_end + 6) - 1);
								else
									Header[HTTP_VERSION] = "1.1";

								getline(stream, line);
								size_t param_end;
								while ((param_end = line.find(':')) != std::string::npos) {
									size_t value_start = param_end + 1;
									if ((value_start) < line.size()) {
										if (line[value_start] == ' ')
											value_start++;
										if (value_start < line.size())
											Header.insert(std::make_pair(line.substr(0, param_end), line.substr(value_start, line.size() - value_start - 1)));
									}
									getline(stream, line);
								}
							}
						}

					}

				};

			}
		}
	}
}


SL::Remote_Access_Library::Network::WebSocket::WebSocket(IBaseNetworkDriver * netevents, void* socket) : TCPSocket(netevents, socket)
{
	_WebSocketSocketImpl = new INTERNAL::WebSocketSocketImpl(netevents);
}

SL::Remote_Access_Library::Network::WebSocket::~WebSocket()
{
	delete _WebSocketSocketImpl;
}
const std::string ws_magic_string = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

void SL::Remote_Access_Library::Network::WebSocket::handshake()
{
	auto self(shared_from_this());
	boost::asio::async_read_until(get_socket(), _WebSocketSocketImpl->_IncomingBuffer, "\r\n\r\n", [this, self](const boost::system::error_code& ec, size_t /*bytes_transferred*/) {
		if (!ec) {
			_WebSocketSocketImpl->parse_handshake();

			if (_WebSocketSocketImpl->Header.count("Sec-WebSocket-Key") == 0) return close();//close socket and get out malformed
			auto write_buffer(std::make_shared<boost::asio::streambuf>());
			std::ostream handshake(write_buffer.get());

			auto sha1 = Crypto::SHA1(_WebSocketSocketImpl->Header["Sec-WebSocket-Key"] + ws_magic_string);
			handshake << "HTTP/1.1 101 Web Socket Protocol Handshake\r\n";
			handshake << "Upgrade: websocket\r\n";
			handshake << "Connection: Upgrade\r\n";
			handshake << "Sec-WebSocket-Accept: " << Crypto::Base64::encode(sha1) << "\r\n";
			handshake << "\r\n";
			boost::asio::async_write(get_socket(), *write_buffer, [this, self, write_buffer](const boost::system::error_code& ec, size_t /*bytes_transferred*/) {
				if (!ec) {
					_WebSocketSocketImpl->NetworkEvents->OnConnect(self);
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

void SL::Remote_Access_Library::Network::WebSocket::readheader()
{
	auto self(shared_from_this());
	boost::asio::async_read(get_socket(), _WebSocketSocketImpl->_IncomingBuffer, boost::asio::transfer_exactly(2), [this, self](const boost::system::error_code& ec, size_t /*bytes_transferred*/) {
		if (!ec) {
			std::istream stream(&_WebSocketSocketImpl->_IncomingBuffer);

			std::vector<unsigned char> first_bytes;
			first_bytes.resize(2);
			stream.read((char*)&first_bytes[0], 2);

			_WebSocketSocketImpl->_fin_rsv_opcode = first_bytes[0];

			//Close connection if unmasked message from client (protocol error)
			if (first_bytes[1] < 128) return close();

			_WebSocketSocketImpl->_IncomingContentLength = (first_bytes[1] & 127);

			if (_WebSocketSocketImpl->_IncomingContentLength == 126) {
				//2 next bytes is the size of content
				boost::asio::async_read(get_socket(), _WebSocketSocketImpl->_IncomingBuffer, boost::asio::transfer_exactly(2), [this, self](const boost::system::error_code& ec, size_t /*bytes_transferred*/) {
					if (!ec) {
						std::istream stream(&_WebSocketSocketImpl->_IncomingBuffer);

						std::vector<unsigned char> length_bytes;
						length_bytes.resize(2);
						stream.read((char*)&length_bytes[0], 2);

						_WebSocketSocketImpl->_IncomingContentLength = 0;
						int num_bytes = 2;
						for (int c = 0; c < num_bytes; c++)
							_WebSocketSocketImpl->_IncomingContentLength += length_bytes[c] << (8 * (num_bytes - 1 - c));
						readbody();
					}
					else {
						close();
					}
				});
			}
			else if (_WebSocketSocketImpl->_IncomingContentLength == 127) {
				//8 next bytes is the size of content
				boost::asio::async_read(get_socket(), _WebSocketSocketImpl->_IncomingBuffer, boost::asio::transfer_exactly(8), [this, self](const boost::system::error_code& ec, size_t /*bytes_transferred*/) {
					if (!ec) {
						std::istream stream(&_WebSocketSocketImpl->_IncomingBuffer);

						std::vector<unsigned char> length_bytes;
						length_bytes.resize(8);
						stream.read((char*)&length_bytes[0], 8);

						size_t length = 0;
						int num_bytes = 8;
						for (int c = 0; c < num_bytes; c++)
							length += length_bytes[c] << (8 * (num_bytes - 1 - c));
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

void SL::Remote_Access_Library::Network::WebSocket::readbody()
{
	auto self(shared_from_this());
	boost::asio::async_read(get_socket(), _WebSocketSocketImpl->_IncomingBuffer, boost::asio::transfer_exactly(4 + _WebSocketSocketImpl->_IncomingContentLength), [this, self](const boost::system::error_code& ec, size_t /*bytes_transferred*/) {
		if (!ec) {
			std::istream raw_message_data(&_WebSocketSocketImpl->_IncomingBuffer);

			//Read mask
			std::vector<unsigned char> mask;
			mask.resize(4);
			raw_message_data.read((char*)&mask[0], 4);

			auto packet(std::make_shared<Packet>(static_cast<unsigned int>(PACKET_TYPES::WEBSOCKET_MSG), _WebSocketSocketImpl->_IncomingContentLength, std::move(_WebSocketSocketImpl->Header)));
			_WebSocketSocketImpl->Header.clear();//reset after move
			auto startpack = packet->Payload;
			for (size_t c = 0; c < _WebSocketSocketImpl->_IncomingContentLength; c++) {
				startpack[c] = (raw_message_data.get() ^ mask[c % 4]);
			}

			//If connection close
			if ((_WebSocketSocketImpl->_fin_rsv_opcode & 0x0f) == 8) {
				return close();
			}
			else {
				//If ping
				if ((_WebSocketSocketImpl->_fin_rsv_opcode & 0x0f) == 9) {
					auto write_buffer(std::make_shared<boost::asio::streambuf>());
					std::ostream outbuf(write_buffer.get());
					outbuf.put(_WebSocketSocketImpl->_fin_rsv_opcode + 1);
					outbuf.put(0);
					boost::asio::async_write(get_socket(), *write_buffer, [this, self, write_buffer](const boost::system::error_code& ec, size_t /*bytes_transferred*/) {
						if (!ec) {
							readheader();
						}
						else {
							close();
						}
					});
				}
				else {
					_WebSocketSocketImpl->NetworkEvents->OnReceive(self, packet);
				}
				readheader();
			}
		}
		else {
			close();
		}
	});
}

void SL::Remote_Access_Library::Network::WebSocket::writeheader(std::shared_ptr<Packet> packet)
{
	auto self(shared_from_this());
	std::ostream header_stream(&_WebSocketSocketImpl->_OutGoingBuffer);
	
	///fin_rsv_opcode: 129=one fragment, text, 130=one fragment, binary, 136=close connection.
	header_stream.put(129);
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

	boost::asio::async_write(get_socket(), _WebSocketSocketImpl->_OutGoingBuffer, [self, this, packet](boost::system::error_code ec, std::size_t byteswritten)
	{
		if (!ec && !closed())
		{
			writebody(packet);
		}
		else close();
	});


}

//these just make copies for now
SL::Remote_Access_Library::Network::Packet SL::Remote_Access_Library::Network::WebSocket::compress(Packet & packet)
{
	Packet p(packet.Packet_Type, packet.Payload_Length);
	memcpy(p.Payload, packet.Payload, packet.Payload_Length);
	p.Header = packet.Header;
	return p;
}
//these just make copies for now
SL::Remote_Access_Library::Network::Packet SL::Remote_Access_Library::Network::WebSocket::decompress(Packet & packet)
{
	Packet p(packet.Packet_Type, packet.Payload_Length);
	memcpy(p.Payload, packet.Payload, packet.Payload_Length);
	p.Header = packet.Header;
	return p;
}
