#pragma once
#include <memory>
#include "SocketStats.h"

namespace SL {
	namespace Remote_Access_Library {
		namespace Network {
			enum SocketTypes {
				TCPSOCKET, HTTPSOCKET, WEBSOCKET
			};
			class Packet;
			//this class is async so all calls return immediately and are later executed
			class ISocket : public std::enable_shared_from_this<ISocket> {
			public:

				virtual ~ISocket() { }
				//adds the data to the internal queue, does not block and returns immediately. Library takes a COPY of the packet
				virtual void send(Packet& pack) = 0;
				//sends a request that the socket be closed. NetworkEvents::OnClose will be called when the call is successful
				virtual void close() = 0;
				virtual bool closed() const = 0;

				//Get the statstics for this socket
				virtual SocketStats get_SocketStats() const = 0;
				//calling connected with a host and port will attempt an async connection returns immediatly and executes the OnConnect Callback when connected. If connection fails, the OnClose callback is called
				virtual void connect(const char* host, const char* port) = 0;
				virtual SocketTypes get_type() const = 0;

				

			protected:
				//called before OnConnect Called
				virtual void handshake() = 0;
				virtual void readheader() = 0;
				virtual void readbody() = 0;
				virtual void writeheader(std::shared_ptr<Packet> outboundpacket) = 0;
				virtual void writebody(std::shared_ptr<Packet> outboundpacket) = 0;
				virtual Packet compress(Packet& packet) = 0;
				virtual Packet decompress(Packet& packet) = 0;
			};

		}
	}
}