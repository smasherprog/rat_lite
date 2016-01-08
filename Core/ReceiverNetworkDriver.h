#pragma once
#include "BaseNetworkDriver.h"
#include "Socket.h"
#include "Image.h"
#include "Packet.h"
#include <memory>
#include <string>

namespace SL {
	namespace Remote_Access_Library {

		namespace Network {
			
			template<class T>class ReceiverNetworkDriver : public BaseNetworkDriver {
				T* _Receiver;
				std::shared_ptr<Network::Socket> _Socket;

			public:
				ReceiverNetworkDriver(T* r, std::string dst_host, std::string dst_port) : _Receiver(r){_Socket = SL::Remote_Access_Library::Network::Socket::ConnectTo(dst_host.c_str(), dst_port.c_str(), this);}
				virtual ~ReceiverNetworkDriver() { }
				virtual void OnConnect(const std::shared_ptr<Socket>& socket) override { _Receiver->OnConnect(socket); }
				virtual void OnClose(const Socket* socket) override { _Receiver->OnClose(socket); }


				virtual void OnReceive(const Socket* socket, std::shared_ptr<Packet>& p) override {
					if (p->header()->Packet_Type == SL::Remote_Access_Library::Network::Commands::PACKET_TYPES::IMAGECHANGE) {
						auto imginfo = (SL::Remote_Access_Library::Network::Commands::ImageChange*)p->data();
						auto img = Remote_Access_Library::Utilities::Image::CreateImage(imginfo->height, imginfo->width, p->data() + sizeof(SL::Remote_Access_Library::Network::Commands::ImageChange), p->header()->PayloadLen - sizeof(SL::Remote_Access_Library::Network::Commands::ImageChange));

						//	Update(imginfo, img);
					}
					else if (p->header()->Packet_Type == SL::Remote_Access_Library::Network::Commands::PACKET_TYPES::RESOLUTIONCHANGE) {
						auto imginfo = (SL::Remote_Access_Library::Network::Commands::ImageChange*)p->data();
						auto img = Remote_Access_Library::Utilities::Image::CreateImage(imginfo->height, imginfo->width, p->data() + sizeof(SL::Remote_Access_Library::Network::Commands::ImageChange), p->header()->PayloadLen - sizeof(SL::Remote_Access_Library::Network::Commands::ImageChange));
						_Receiver->NewImage(imginfo, img);
					}



				}
			};

		}

	}

}
