#pragma once
#include <memory>
#include "TCPSocket.h"


namespace SL {
	namespace Remote_Access_Library {
		namespace Network {

			//this class is async so all calls return immediately and are later executed
			template<typename T>class HttpSocket : public TCPSocket<T> {
			public:
				explicit HttpSocket(IBaseNetworkDriver* netevents, T& socket) :TCPSocket(netevents, socket) {}
				//MUST BE CREATED AS A SHARED_PTR OTHERWISE IT WILL CRASH!
				explicit HttpSocket(IBaseNetworkDriver* netevents, boost::asio::io_service& io_service) :TCPSocket(netevents, io_service) {}
				virtual ~HttpSocket() {

				}
				virtual SocketTypes get_type() const override { return SocketTypes::HTTPSOCKET; }

			private:


				virtual void readheader()  override {
					auto self(shared_from_this());
					boost::asio::async_read_until(_socket, _SocketImpl.get_IncomingStreamBuffer(), "\r\n\r\n", [this, self](const boost::system::error_code ec, std::size_t s)
					{
						if (!ec) {
							_SocketImpl.parse_header("1.0", s);
							readbody();
						}
						else {
							close();
						}
					});
				}
			
				virtual void writeheader(std::shared_ptr<Packet> pack) override {
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
					os << HTTP_CONTENTLENGTH << HTTP_KEYVALUEDELIM << std::to_string(pack->Payload_Length) << HTTP_ENDLINE;
					os << HTTP_ENDLINE;//marks the end of the header

					auto self(shared_from_this());
					boost::asio::async_write(_socket, *outpackbuff, [self, this, outpackbuff, pack](boost::system::error_code ec, std::size_t byteswritten)
					{
						if (!ec && !closed())
						{
							writebody(pack);
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

			

			};


		}
	}
}