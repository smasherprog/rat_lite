#include "stdafx.h"
#include "ServerNetworkDriver.h"
#include "Image.h"
#include "Shapes.h"

void SL::Remote_Access_Library::Network::ServerNetworkDriverImpl::OnConnect(const std::shared_ptr<Socket>& socket)
{
	std::lock_guard<std::mutex> lock(_ClientsLock);
	_Clients.push_back(socket);
}

void SL::Remote_Access_Library::Network::ServerNetworkDriverImpl::OnClose(const Socket * socket)
{
	std::lock_guard<std::mutex> lock(_ClientsLock);
	_Clients.erase(std::remove_if(begin(_Clients), end(_Clients), [socket](const std::shared_ptr<SL::Remote_Access_Library::Network::Socket>& p) { return p.get() == socket; }), _Clients.end());
}

void SL::Remote_Access_Library::Network::ServerNetworkDriverImpl::SendToAll(std::shared_ptr<Network::Packet>& packet) {
	for (auto& c : GetClients()) {
		c->send(packet);
	}
}
void SL::Remote_Access_Library::Network::ServerNetworkDriverImpl::SendToAll(std::vector<std::shared_ptr<Network::Packet>>& packets) {
	for (auto& c : GetClients()) {
		for (auto& p : packets) {
			c->send(p);
		}
	}
}
std::shared_ptr<SL::Remote_Access_Library::Network::Packet> GenerateDifs(SL::Remote_Access_Library::Utilities::Rect& r, const SL::Remote_Access_Library::Utilities::Image & img) {
	SL::Remote_Access_Library::Network::PacketHeader header;
	header.Packet_Type = SL::Remote_Access_Library::Network::Commands::PACKET_TYPES::IMAGEDIF;
	header.PayloadLen = sizeof(SL::Remote_Access_Library::Utilities::Rect) + (r.Width * r.Height* img.Stride());
	auto p = SL::Remote_Access_Library::Network::Packet::CreatePacket(header);
	auto start = p->data();
	memcpy(start, &r, sizeof(SL::Remote_Access_Library::Utilities::Rect));
	start += sizeof(SL::Remote_Access_Library::Utilities::Rect);
	for (unsigned int row = r.Origin.Y; row < r.Height + r.Origin.Y; row++) {

		auto startcopy = img.data() + (row*img.Stride()*img.Width());//advance rows
		startcopy += r.Origin.Y*img.Stride();//advance columns
		memcpy(start, startcopy, r.Width*img.Stride() );
		start += r.Width*img.Stride();
	}
	return p;
}
void SL::Remote_Access_Library::Network::ServerNetworkDriverImpl::Send(Socket * socket, SL::Remote_Access_Library::Utilities::Rect & r, const Utilities::Image & img)
{
	socket->send(GenerateDifs(r, img));
}


void SL::Remote_Access_Library::Network::ServerNetworkDriverImpl::Send(SL::Remote_Access_Library::Utilities::Rect& r, const Utilities::Image & img) {
	SendToAll(GenerateDifs(r, img));
}

std::vector<std::shared_ptr<SL::Remote_Access_Library::Network::Socket>> SL::Remote_Access_Library::Network::ServerNetworkDriverImpl::GetClients() {
	//make a copy of the list
	std::lock_guard<std::mutex> lock(_ClientsLock);
	return _Clients;
}