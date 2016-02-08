#include "stdafx.h"
#include "HttpSocket.h"
#include "IBaseNetworkDriver.h"
#include "Packet.h"
#include "zstd.h"
#include <boost/asio.hpp>
#include <boost/iostreams/stream.hpp>


namespace SL {
	namespace Remote_Access_Library {
		namespace Network {

			//this namespace should only be accessed by the Core library and NEVER by a consumer of this library
			namespace INTERNAL {

				class HttpSocketImpl {
					std::vector<char> _IncomingBuffer;
					unsigned long long _content_length;
					size_t ExtraBytesRead = 0;
					IBaseNetworkDriver* _IBaseNetworkDriver;
				public:

					boost::asio::streambuf _IncomingStreamBuffer;
					std::unordered_map<std::string, std::string> Header;

					HttpSocketImpl(IBaseNetworkDriver* net) :_IBaseNetworkDriver(net) {

					}
					Network::IBaseNetworkDriver* get_IBaseNetworkDriver() {
						return _IBaseNetworkDriver;
					}
					char* get_Buffer()
					{
						_IncomingBuffer.reserve(_content_length);
						return _IncomingBuffer.data();
					}
					size_t get_BufferSize()
					{
						const auto it = Header.find(HTTP_CONTENTLENGTH);
						_content_length = 0;
						if (it != Header.end()) {
							try {
								_content_length = stoull(it->second);
							}
							catch (const std::exception &e) {
								std::cout << e.what() << std::endl;
								return _content_length;
							}
						}
						if (_content_length > ExtraBytesRead) _content_length -= ExtraBytesRead;
						return _content_length;
					}
					std::shared_ptr<Packet> get_IncomingPacket() {
						auto pack(std::make_shared<Packet>(static_cast<unsigned int>(PACKET_TYPES::HTTP_MSG), _content_length, std::move(Header)));
						Header.clear();//reset
						memcpy(pack->Payload, _IncomingBuffer.data(), _content_length);
						return pack;
					}
					void parse_header(size_t bytesready)
					{
						std::string line;
						std::istream stream(&_IncomingStreamBuffer);
						std::getline(stream, line);
						size_t method_end = line.find(' ');
						if (method_end != std::string::npos) {
							size_t path_end = line.find(' ', method_end + 1);
							if (path_end != std::string::npos) {
								Header[HTTP_METHOD] = line.substr(0, method_end);
								Header[HTTP_PATH] = line.substr(method_end + 1, path_end - method_end - 1);
								if ((path_end + 6) < line.size())
									Header[HTTP_VERSION] = line.substr(path_end + 6, line.size() - (path_end + 6) - 1);
								else
									Header[HTTP_VERSION] = "1.0";

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
						}//read the rest of the data
						ExtraBytesRead = _IncomingStreamBuffer.size() - bytesready;
						_IncomingBuffer.assign(std::istreambuf_iterator<char>(stream), {});
					}
				};
			}
		}
	}
}
SL::Remote_Access_Library::Network::HttpSocket::HttpSocket(IBaseNetworkDriver* netevents, void* socket) : TCPSocket(netevents, socket)
{

	std::cout << "HttpSocket()" << std::endl;
	_HttpSocketImpl = new INTERNAL::HttpSocketImpl(netevents);
}

SL::Remote_Access_Library::Network::HttpSocket::~HttpSocket()
{
	delete _HttpSocketImpl;
	std::cout << "~HttpSocket()" << std::endl;
}


void SL::Remote_Access_Library::Network::HttpSocket::readheader()
{
	auto self(shared_from_this());
	boost::asio::async_read_until(get_socket(), _HttpSocketImpl->_IncomingStreamBuffer, "\r\n\r\n", [this, self](const boost::system::error_code ec, std::size_t s)
	{
		if (!ec) {
			_HttpSocketImpl->parse_header(s);
			readbody();
		}
		else {
			close();
		}
	});
}

void SL::Remote_Access_Library::Network::HttpSocket::readbody()
{
	auto self(shared_from_this());
	auto size(_HttpSocketImpl->get_BufferSize());
	auto p(_HttpSocketImpl->get_Buffer());

	if (size > 0) {//still more to be read!
		boost::asio::async_read(get_socket(), boost::asio::buffer(p, size), boost::asio::transfer_exactly(size), [this, self, size](const boost::system::error_code& ec, size_t s/*bytes_transferred*/) {
			if (!ec) {
				assert(s == size);
				_HttpSocketImpl->get_IBaseNetworkDriver()->OnReceive(self, _HttpSocketImpl->get_IncomingPacket());
				readheader();
			}
			else {
				close();
			}

		});
	}
	else {//all bytes already read.. good to go!
		_HttpSocketImpl->get_IBaseNetworkDriver()->OnReceive(self, _HttpSocketImpl->get_IncomingPacket());
		readheader();
	}
}

void SL::Remote_Access_Library::Network::HttpSocket::writeheader(std::shared_ptr<Packet> pack)
{
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
	boost::asio::async_write(get_socket(), *outpackbuff, [self, this, outpackbuff, pack](boost::system::error_code ec, std::size_t byteswritten)
	{
		if (!ec && !closed())
		{
			writebody(pack);
		}
		else close();
	});
}
//these just make copies for now
SL::Remote_Access_Library::Network::Packet SL::Remote_Access_Library::Network::HttpSocket::compress(Packet & packet)
{
	Packet p(packet.Packet_Type, packet.Payload_Length);
	memcpy(p.Payload, packet.Payload, packet.Payload_Length);
	p.Header = packet.Header;
	return p;
}
//these just make copies for now
SL::Remote_Access_Library::Network::Packet SL::Remote_Access_Library::Network::HttpSocket::decompress(Packet & packet)
{
	Packet p(packet.Packet_Type, packet.Payload_Length);
	memcpy(p.Payload, packet.Payload, packet.Payload_Length);
	p.Header = packet.Header;
	return p;
}

