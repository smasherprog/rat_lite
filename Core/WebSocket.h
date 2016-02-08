#pragma once
#include <memory>
#include "TCPSocket.h"

namespace SL {
	namespace Remote_Access_Library {
		namespace Network {
			namespace INTERNAL {
				class WebSocketSocketImpl;
			}
			class Packet;
			class IBaseNetworkDriver;

			//this class is async so all calls return immediately and are later executed
			class WebSocket : public TCPSocket {

			public:
				//MUST BE CREATED AS A SHARED_PTR OTHERWISE IT WILL CRASH!
				WebSocket(IBaseNetworkDriver* netevents, void* socket);
				virtual ~WebSocket();
				virtual SocketTypes get_type() const override { return SocketTypes::WEBSOCKET; }

			private:
				INTERNAL::WebSocketSocketImpl* _WebSocketSocketImpl;

				virtual void handshake()  override;
				virtual void readheader()  override;
				virtual void readbody() override;
				virtual void writeheader(std::shared_ptr<Packet> packet) override;
				virtual Packet compress(Packet& packet)  override;
				virtual Packet decompress(Packet& packet)  override;

			};
		}
	}
}
