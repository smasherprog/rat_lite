#pragma once
#include <memory>
#include "TCPSocket.h"

namespace SL {
	namespace Remote_Access_Library {

		namespace Network {
			namespace INTERNAL {
				class HttpSocketImpl;
			}

			class Packet;
			class IBaseNetworkDriver;

			//this class is async so all calls return immediately and are later executed
			class HttpSocket : public TCPSocket {
			public:
				//MUST BE CREATED AS A SHARED_PTR OTHERWISE IT WILL CRASH!
				HttpSocket(IBaseNetworkDriver* netevents, void* socket);
				virtual ~HttpSocket();

			private:


				INTERNAL::HttpSocketImpl* _HttpSocketImpl;

				virtual void readheader()  override;
				virtual void readbody() override;
				virtual void writeheader() override;

				virtual std::shared_ptr<Packet> decompress(PacketHeader& header, char* buffer) override;
				virtual std::shared_ptr<Packet> compress(std::shared_ptr<Packet>& packet)  override;
			};


		}
	}
}