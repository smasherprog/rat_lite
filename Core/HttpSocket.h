#pragma once
#include <memory>
#include "ISocket.h"
#include <vector>

namespace SL {
	namespace Remote_Access_Library {

		namespace Network {
			namespace INTERNAL {
				class HttpSocketImpl;
			}

			class Packet;
			class IBaseNetworkDriver;

			//this class is async so all calls return immediately and are later executed
			class HttpSocket : public ISocket {
			public:
				//factory to create a socket and connect to the destination address

				static std::shared_ptr<HttpSocket> Create(SL::Remote_Access_Library::Network::IBaseNetworkDriver* netevents, void* socket);

				HttpSocket(INTERNAL::HttpSocketImpl* impl);

				virtual ~HttpSocket();
				//adds the data to the internal queue, does not block and returns immediately.
				virtual void send(std::shared_ptr<Packet>& pack) override;
				//sends a request that the socket be closed. NetworkEvents::OnClose will be called when the call is successful
				virtual void close() override;

				//pending packets which are queued up and waiting to be sent
				virtual SocketStats get_SocketStats() const override;


			private:


				INTERNAL::HttpSocketImpl* _SocketImpl;


				void Start();

				void write();
				void write_response();
				void read_body();
				void read_header();

			};


		}
	}
}