#pragma once
#include "SocketStats.h"
#include <string>

namespace SL {
	namespace Remote_Access_Library {
		namespace Network {


			class Packet;
			class ISocket {
			public:

				virtual ~ISocket() {}
				//adds the data to the internal queue, does not block and returns immediately. Library takes a COPY of the packet
				virtual void send(Packet& pack) = 0;
				//sends a request that the socket be closed. NetworkEvents::OnClose will be called when the call is successful
				virtual void close(std::string reason) = 0;
				virtual bool closed() = 0;

				//Get the statstics for this socket
				virtual SocketStats get_SocketStats() const = 0;

				//s in in seconds
				virtual void set_ReadTimeout(int s) = 0;
				//s in in seconds
				virtual void set_WriteTimeout(int s) = 0;

				virtual std::string get_ipv4_address() const = 0;
				virtual unsigned short get_port() const = 0;

			};

		}
	}
}