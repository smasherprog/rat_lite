#pragma once
#include <memory>
#include "ISocket.h"
#include <vector>

namespace boost { namespace asio { class io_service; } }
namespace SL {
	namespace Remote_Access_Library {

		namespace Network {
			namespace INTERNAL {
				class TCPSocketImpl;
			}

			class Packet;
			class IBaseNetworkDriver;
			struct PacketHeader;

			//this class is async so all calls return immediately and are later executed
			class TCPSocket : public ISocket {
			public:
				//MUST BE CREATED AS A SHARED_PTR OTHERWISE IT WILL CRASH!
				explicit TCPSocket(IBaseNetworkDriver* netevents, void* socket);//cant forward declare a tcpsocket because its a template
				//MUST BE CREATED AS A SHARED_PTR OTHERWISE IT WILL CRASH!
				explicit TCPSocket(IBaseNetworkDriver* netevents, boost::asio::io_service& io_service);

				virtual ~TCPSocket();
				//adds the data to the internal queue, does not block and returns immediately.
				virtual void send(std::shared_ptr<Packet>& pack) override;
				//sends a request that the socket be closed. NetworkEvents::OnClose will be called when the call is successful
				virtual void close() override;

				//pending packets which are queued up and waiting to be sent
				virtual SocketStats get_SocketStats() const override;
				//calling connected with a host and port will attempt an async connection returns immediatly and executes the OnConnect Callback when connected. If connection fails, the OnClose callback is called
				virtual void connect(const char* host, const char* port) override;
			protected:

				INTERNAL::TCPSocketImpl* _SocketImpl;
				virtual void readheader()  override;
				virtual void readbody() override;
				virtual void writeheader() override;
				virtual void writebody() override;
				virtual std::shared_ptr<Packet> decompress(PacketHeader& header, char* buffer) override;
				virtual std::shared_ptr<Packet> compress(std::shared_ptr<Packet>& packet)  override;
		

			};
		}
	}
}