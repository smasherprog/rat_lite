#pragma once
#include <memory>
#include "TCPSocket.h"
#include "HttpHeader.h"

namespace SL {
	namespace Remote_Access_Library {
		namespace Network {

			//this class is async so all calls return immediately and are later executed
			template<typename T>class HttpSocket : public TCPSocket<T> {
			public:
				explicit HttpSocket(IBaseNetworkDriver* netevents, T& socket) :TCPSocket<T>(netevents, socket) {}
				//MUST BE CREATED AS A SHARED_PTR OTHERWISE IT WILL CRASH!
				explicit HttpSocket(IBaseNetworkDriver* netevents, boost::asio::io_service& io_service) :TCPSocket<T>(netevents, io_service) {}
				virtual ~HttpSocket() {

				}
				virtual SocketTypes get_type() const override { return SocketTypes::HTTPSOCKET; }

			private:


				virtual void readheader()  override {
					auto self(this->shared_from_this());	
					std::shared_ptr<boost::asio::streambuf> read_buffer(new boost::asio::streambuf);
					boost::asio::async_read_until(_socket, *read_buffer, "\r\n\r\n", [this, self, read_buffer](const boost::system::error_code& ec, std::size_t s)
					{
						if (!ec) {
							auto beforesize = read_buffer->size();
							std::istream stream(read_buffer.get());
							_SocketImpl.Header = std::move(HttpHeader::Parse("1.0", stream));

							const auto it = _SocketImpl.Header.find(HttpHeader::HTTP_CONTENTLENGTH);
							_SocketImpl.ReadPacketHeader.Payload_Length = 0;
							if (it != _SocketImpl.Header.end()) {
								try {
									_SocketImpl.ReadPacketHeader.Payload_Length = static_cast<unsigned int>(stoull(it->second));
								}
								catch (const std::exception &e) {
									std::cout << e.what() << std::endl;
								}
							}
							auto extrabytesread = static_cast<unsigned int>(beforesize - s);
							if (_SocketImpl.ReadPacketHeader.Payload_Length > extrabytesread) _SocketImpl.ReadPacketHeader.Payload_Length -= extrabytesread;
							if (extrabytesread > 0) {
								_SocketImpl._IncomingBuffer.assign(std::istreambuf_iterator<char>(stream), {});
							}
							readbody();
						}
						else {
							close(std::string("readheader async_read_until ") + ec.message());
						}
					});
				}
			
				virtual void writeheader(std::shared_ptr<Packet> pack) override {
					//the headers below are required... 
					assert(pack->Header.find(HttpHeader::HTTP_CONTENTTYPE) != pack->Header.end());
					assert(pack->Header.find(HttpHeader::HTTP_STATUSCODE) != pack->Header.end());
					assert(pack->Header.find(HttpHeader::HTTP_VERSION) != pack->Header.end());
					//Code below isnt optimal... Will have to think about this
					auto outpackbuff(std::make_shared<boost::asio::streambuf>());
					//outpackbuff->prepare(std::accumulate(begin(pack->Header), end(pack->Header), static_cast<size_t>(0), [](size_t a, const decltype(pack->Header)::reference second) { return a + second.first.size() + second.second.size(); }));//try to allocate enouh space for the header
					std::ostream os(outpackbuff.get());
					os << pack->Header[HttpHeader::HTTP_VERSION] << " " << pack->Header[HttpHeader::HTTP_STATUSCODE] << HttpHeader::HTTP_ENDLINE;
					os << HttpHeader::HTTP_CONTENTTYPE << HttpHeader::HTTP_KEYVALUEDELIM << pack->Header[HttpHeader::HTTP_CONTENTTYPE] << HttpHeader::HTTP_ENDLINE;
					pack->Header.erase(HttpHeader::HTTP_VERSION);
					pack->Header.erase(HttpHeader::HTTP_STATUSCODE);
					pack->Header.erase(HttpHeader::HTTP_CONTENTTYPE);
					for (auto& a : pack->Header) {//write the other headers
						os << a.first << HttpHeader::HTTP_KEYVALUEDELIM << a.second << HttpHeader::HTTP_ENDLINE;
					}
					os << HttpHeader::HTTP_CONTENTLENGTH << HttpHeader::HTTP_KEYVALUEDELIM << std::to_string(pack->Payload_Length) << HttpHeader::HTTP_ENDLINE;
					os << HttpHeader::HTTP_ENDLINE;//marks the end of the header

					auto self(this->shared_from_this());
					boost::asio::async_write(_socket, *outpackbuff, [self, this, outpackbuff, pack](const boost::system::error_code& ec, std::size_t byteswritten)
					{
						if (!ec && !closed())
						{
							writebody(pack);
						}
						else close(std::string("writeheader async_write ") + ec.message());
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

			

			};


		}
	}
}