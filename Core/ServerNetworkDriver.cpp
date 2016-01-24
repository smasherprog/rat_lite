#include "stdafx.h"
#include "ServerNetworkDriver.h"
#include "Image.h"
#include "Shapes.h"
#include "Packet.h"
#include "TCPSocket.h"
#include "IServerDriver.h"

namespace SL {
	namespace Remote_Access_Library {
		namespace Network {

			class ServerNetworkDriverImpl {
		
				std::shared_ptr<Network::Listener> _Listener;
				IServerDriver* _IServerDriver;

				std::vector<std::shared_ptr<Network::ISocket>> _Clients;
				std::mutex _ClientsLock;

				public:
				ServerNetworkDriverImpl(IBaseNetworkDriver* parent, unsigned short port, IServerDriver* svrd) :_Listener(Network::Listener::CreateListener(port, parent)), _IServerDriver(svrd) {}
				~ServerNetworkDriverImpl() { }
				void OnConnect(const std::shared_ptr<ISocket>& socket) {
					_IServerDriver->OnConnect(socket);
					std::lock_guard<std::mutex> lock(_ClientsLock);
					_Clients.push_back(socket);
				}
				void OnClose(const ISocket* socket) {
					_IServerDriver->OnClose(socket);
					std::lock_guard<std::mutex> lock(_ClientsLock);
					_Clients.erase(std::remove_if(begin(_Clients), end(_Clients), [socket](const std::shared_ptr<SL::Remote_Access_Library::Network::ISocket>& p) { return p.get() == socket; }), _Clients.end());
				}
				std::vector<std::shared_ptr<Network::ISocket>> GetClients() {
					std::lock_guard<std::mutex> lock(_ClientsLock);
					return _Clients;
				}

				void Send(ISocket* socket, SL::Remote_Access_Library::Utilities::Rect& r, const Utilities::Image & img) {
					socket->send(GenerateDifs(r, img));
				}
				void Send(SL::Remote_Access_Library::Utilities::Rect& r, const Utilities::Image & img) {
					SendToAll(GenerateDifs(r, img));
				}

				void SendToAll(std::shared_ptr<Network::Packet>& packet) {
					for (auto& c : GetClients()) {
						c->send(packet);
					}
				}
				void SendToAll(std::vector<std::shared_ptr<Network::Packet>>& packets) {
					for (auto& c : GetClients()) {
						for (auto& p : packets) {
							c->send(p);
						}
					}
				}

				void StartListening() { _Listener->Start(); }

				std::shared_ptr<Packet> GenerateDifs(SL::Remote_Access_Library::Utilities::Rect& r, const SL::Remote_Access_Library::Utilities::Image & img) {
					PacketHeader header;
					header.Packet_Type = static_cast<unsigned int>(Commands::PACKET_TYPES::IMAGEDIF);
					header.PayloadLen = sizeof(Utilities::Rect) + (r.Width * r.Height* img.Stride());
					auto p = Packet::CreatePacket(header);
					auto start = p->data();
					memcpy(start, &r, sizeof(Utilities::Rect));
					start += sizeof(SL::Remote_Access_Library::Utilities::Rect);

					for (auto row = r.Origin.Y; row < r.bottom(); row++) {

						auto startcopy = img.data() + (row*img.Stride()*img.Width());//advance rows
						startcopy += r.Origin.X*img.Stride();//advance columns
						memcpy(start, startcopy, r.Width*img.Stride());
						start += r.Width*img.Stride();
					}
					return p;
				}
			};
		}
	}
}

SL::Remote_Access_Library::Network::ServerNetworkDriver::ServerNetworkDriver(Network::IServerDriver * r, unsigned short port) : _ServerNetworkDriverImpl(std::make_unique<ServerNetworkDriverImpl>(this, port, r))
{

}
SL::Remote_Access_Library::Network::ServerNetworkDriver::~ServerNetworkDriver()
{
}
void SL::Remote_Access_Library::Network::ServerNetworkDriver::OnConnect(const std::shared_ptr<ISocket>& socket)
{
	_ServerNetworkDriverImpl->OnConnect(socket);

}
void SL::Remote_Access_Library::Network::ServerNetworkDriver::OnClose(const ISocket * socket)
{
	_ServerNetworkDriverImpl->OnClose(socket);
}
void SL::Remote_Access_Library::Network::ServerNetworkDriver::OnReceive(const ISocket * socket, std::shared_ptr<Packet>& p)
{
	




}
void SL::Remote_Access_Library::Network::ServerNetworkDriver::StartNetworkProcessing()
{
	_ServerNetworkDriverImpl->StartListening();
}

void SL::Remote_Access_Library::Network::ServerNetworkDriver::Send(SL::Remote_Access_Library::Utilities::Rect & r, const Utilities::Image & img)
{
	_ServerNetworkDriverImpl->Send(r, img);
}
void SL::Remote_Access_Library::Network::ServerNetworkDriver::Send(ISocket * socket, SL::Remote_Access_Library::Utilities::Rect & r, const Utilities::Image & img)
{
	_ServerNetworkDriverImpl->Send(socket, r, img);
}
std::vector<std::shared_ptr<SL::Remote_Access_Library::Network::ISocket>> SL::Remote_Access_Library::Network::ServerNetworkDriver::GetClients()
{
	return _ServerNetworkDriverImpl->GetClients();
}

