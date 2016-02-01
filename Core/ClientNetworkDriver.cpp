#include "stdafx.h"
#include "ClientNetworkDriver.h"
#include "TCPSocket.h"
#include "Commands.h"
#include "Shapes.h"
#include "IClientDriver.h"
#include "Image.h"
#include "Packet.h"
#include "IO_Runner.h"

namespace SL {
	namespace Remote_Access_Library {
		namespace Network {

			class ClientNetworkDriverImpl : public IBaseNetworkDriver {
				IClientDriver* _IClientDriver;
				std::shared_ptr<Network::TCPSocket> _Socket;
				std::unique_ptr<IO_Runner> _IO_Runner;

			public:
				ClientNetworkDriverImpl(IClientDriver* r, const char * dst_host, const char * dst_port) : _IClientDriver(r), _IO_Runner(std::make_unique<IO_Runner>()){
				
					_Socket = SL::Remote_Access_Library::Network::TCPSocket::Create(this, dst_host, dst_port, _IO_Runner->get_io_service());

				}

				virtual ~ClientNetworkDriverImpl() { _Socket->close(); }
				virtual void OnConnect(const std::shared_ptr<ISocket>& socket) override { _IClientDriver->OnConnect(socket); }
				virtual void OnClose(const std::shared_ptr<ISocket>& socket) override { _IClientDriver->OnClose(socket); }

				virtual void OnReceive(const std::shared_ptr<ISocket>& socket, std::shared_ptr<Packet>& p) override {

					switch (p->header()->Packet_Type) {
					case static_cast<unsigned int>(Commands::PACKET_TYPES::IMAGEDIF) :
						ImageDif(socket, p);
						break;
					default:
						_IClientDriver->OnReceive(socket, p);//pass up the chain
						break;
					}

				}
				void ImageDif(const std::shared_ptr<ISocket>& socket, std::shared_ptr<Packet>& p) {
					auto imgrect = (Utilities::Rect*)p->data();
					auto img = Utilities::Image::CreateImage(imgrect->Height, imgrect->Width, p->data() + sizeof(Utilities::Rect), p->header()->PayloadLen - sizeof(Utilities::Rect));
					_IClientDriver->OnReceive_Image(socket, imgrect, img);

				}
			};


		}
	}
}


SL::Remote_Access_Library::Network::ClientNetworkDriver::ClientNetworkDriver(IClientDriver * r, const char * dst_host, const char * dst_port) : _ClientNetworkDriverImpl(std::make_unique<ClientNetworkDriverImpl>(r, dst_host, dst_port))
{

}

SL::Remote_Access_Library::Network::ClientNetworkDriver::~ClientNetworkDriver()
{
	Close();
}

void SL::Remote_Access_Library::Network::ClientNetworkDriver::Close()
{
	_ClientNetworkDriverImpl.reset();
}
