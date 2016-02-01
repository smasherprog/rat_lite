#pragma once
#include <memory>
#include "ISocket.h"
#include <vector>

namespace SL {
	namespace Remote_Access_Library {

		namespace Network {
			namespace INTERNAL {
				class TCPSocketImpl;
			}

			class Packet;
			class IBaseNetworkDriver;

			//this class is async so all calls return immediately and are later executed
			class TCPSocket : public ISocket {
			public:
				//factory to create a socket and connect to the destination address

				static std::shared_ptr<TCPSocket> Create(SL::Remote_Access_Library::Network::IBaseNetworkDriver* netevents, const char * dst_host, const char * dst_port, void* io_service);
				static std::shared_ptr<TCPSocket> Create(SL::Remote_Access_Library::Network::IBaseNetworkDriver* netevents, void* socket);

				TCPSocket(INTERNAL::TCPSocketImpl* impl);

				virtual ~TCPSocket();
				//adds the data to the internal queue, does not block and returns immediately.
				virtual void send(std::shared_ptr<Packet>& pack) override;
				//sends a request that the socket be closed. NetworkEvents::OnClose will be called when the call is successful
				virtual void close() override;

				//pending packets which are queued up and waiting to be sent
				virtual SocketStats get_SocketStats() const override;


			private:


				INTERNAL::TCPSocketImpl* _SocketImpl;
				void Read();
				void Write();
				void connect(const char* host, const char* port);

			};


		}
	}
}