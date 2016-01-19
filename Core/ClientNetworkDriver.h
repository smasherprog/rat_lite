#pragma once
#include "IBaseNetworkDriver.h"
#include "TCPSocket.h"
#include "Image.h"
#include "Packet.h"
#include <memory>
#include <string>

namespace SL {
	namespace Remote_Access_Library {

		namespace Network {
		
			template<class T>class ClientNetworkDriver : public IBaseNetworkDriver {
				T* _Receiver;
				std::shared_ptr<Network::ISocket> _Socket;

			public:
				ClientNetworkDriver(T* r, std::string dst_host, std::string dst_port) : _Receiver(r) { 
					_Socket = SL::Remote_Access_Library::Network::TCPSocket::ConnectTo(dst_host.c_str(), dst_port.c_str(), this); 
				}
				virtual ~ClientNetworkDriver() { _Socket->close(); }
				virtual void OnConnect(const std::shared_ptr<ISocket>& socket) override { _Receiver->OnConnect(socket); }
				virtual void OnClose(const ISocket* socket) override { _Receiver->OnClose(socket); }


				virtual void OnReceive(const ISocket* socket, std::shared_ptr<Packet>& p) override {

					switch (p->header()->Packet_Type) {
					case Commands::PACKET_TYPES::IMAGEDIF:
						ImageDif(socket, p);

					}

				}
				void ImageDif(const ISocket* socket, std::shared_ptr<Packet>& p) {
					auto imgrect = (Utilities::Rect*)p->data();
					auto img = Utilities::Image::CreateImage(imgrect->Height, imgrect->Width, p->data() + sizeof(Utilities::Rect), p->header()->PayloadLen - sizeof(Utilities::Rect));
					_Receiver->ImageDif(imgrect, img);

				}
			};
		}
	}
}
