#pragma once
#include <memory>
#include <functional>

namespace SL {
	namespace Remote_Access_Library {
		namespace INTERNAL {
			struct SocketImpl;
		}
		namespace Network {
			class Packet;
			struct NetworkEvents;
	

			//this class is async so all calls return immediately and are later executed, which is why callbacks are passed to the factory so you can be notified 
			class Socket: public std::enable_shared_from_this<Socket> {
			public:
				//factory to create a socket and connect to the destination address
				static std::shared_ptr<Socket> ConnectTo(const char* host, const char* port, NetworkEvents& netevents);
				//constructed with opaque type for internal use only
				Socket(std::shared_ptr<INTERNAL::SocketImpl>& impl);
				
				~Socket();
				//adds the data to the internal queue, does not block and returns immediately. This calls compress before sending, and calls on_sent_callback once the packet has completed sending the packet
				void send(std::shared_ptr<Packet>& pack, std::function<void()> on_sent_callback);
				//adds the data to the internal queue, does not block and returns immediately. This calls compress before sending 
				void send(std::shared_ptr<Packet>& pack);
				//sends a request that the socket be closed. NetworkEvents::OnClose will be called when the call is successful
				void close();
				void connect(const char* host, const char* port);


				//pending packets which are queued up and waiting to be sent
				unsigned int get_OutgoingPacketCount() const;
				//pending data which is queued up and waiting to be sent
				unsigned int get_OutgoingByteCount() const;

			private:
				std::shared_ptr<INTERNAL::SocketImpl> _SocketImpl;

				void do_read_header();
				void do_write();
				void do_write_header();
				void do_read_body();
				

			};


			void DefaultOnConnect(const std::shared_ptr<SL::Remote_Access_Library::Network::Socket> ptr);
			void DefaultOnReceive(const Socket* ptr, std::shared_ptr<SL::Remote_Access_Library::Network::Packet>& pac);
			void DefaultOnClose(const Socket* ptr);

			struct NetworkEvents {
				std::function<void(const std::shared_ptr<Socket>)> OnConnect = DefaultOnConnect;
				std::function<void(const Socket*, std::shared_ptr<SL::Remote_Access_Library::Network::Packet>&)> OnReceive = DefaultOnReceive;
				std::function<void(const Socket*)> OnClose = DefaultOnClose;
			};
		}
	}
}