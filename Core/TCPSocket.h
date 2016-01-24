#pragma once
#include <memory>
#include <functional>
#include "ISocket.h"

namespace SL {
	namespace Remote_Access_Library {
		namespace INTERNAL {
			class TCPSocketImpl;
			class ListinerImpl;
		}
		namespace Network {
			class Packet;
			class IBaseNetworkDriver;

			//this class is async so all calls return immediately and are later executed
			class TCPSocket: public ISocket {
			public:
				//factory to create a socket and connect to the destination address

				static std::shared_ptr<ISocket> ConnectTo(const char* host, const char* port, SL::Remote_Access_Library::Network::IBaseNetworkDriver* netevents);
				TCPSocket(std::shared_ptr<INTERNAL::TCPSocketImpl>& impl);
				
				virtual ~TCPSocket();
				//adds the data to the internal queue, does not block and returns immediately.
				virtual void send(std::shared_ptr<Packet>& pack) override;
				//sends a request that the socket be closed. NetworkEvents::OnClose will be called when the call is successful
				virtual void close() override;
				void connect(const char* host, const char* port);


				//pending packets which are queued up and waiting to be sent
				virtual SocketStats get_SocketStats() const override;
	

			private:
				std::shared_ptr<INTERNAL::TCPSocketImpl> _SocketImpl;

				void do_read_header();
				void do_write(std::shared_ptr<Network::Packet> packet);
				void do_write_header();
				void do_read_body();
				

			};

	
			class Listener : public std::enable_shared_from_this<Listener> {
			public:
				//factory to create a listening socket
				static std::shared_ptr<Listener> CreateListener(unsigned short port, Network::IBaseNetworkDriver* netevents);

				Listener(INTERNAL::ListinerImpl* impl);
				~Listener();

				void Start();

			private:
				INTERNAL::ListinerImpl* _ListinerImpl;
			};
		}
	}
}