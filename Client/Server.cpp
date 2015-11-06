#include "stdafx.h"
#include "..\Core\CommonNetwork.h"
#include "Server.h"
#include "..\Core\Listener.h"
#include <vector>
#include <algorithm>
#include <thread>
#include "..\..\Screen_Capture\Screen_Capture\Screen.h"
#include <mutex>
#include <chrono>
#include "..\Core\Socket.h"
#include "..\Core\Packet.h"

using namespace std::literals;

struct SL::Remote_Access_Library::ServerImpl {
	std::shared_ptr<Network::Listener> _Listener;
	Network::NetworkEvents _NetworkEvents;
	std::vector<std::shared_ptr<Network::Socket>> Clients;
	std::mutex ClientsLock;
	bool Keepgoing;
	std::thread _Runner;
};

void OnConnect(const std::shared_ptr<SL::Remote_Access_Library::Network::Socket> ptr, std::shared_ptr<SL::Remote_Access_Library::ServerImpl>& s) {
	{
		std::lock_guard<std::mutex> lock(s->ClientsLock);
		s->Clients.push_back(ptr);
	}
	s->_NetworkEvents.OnConnect(ptr);
}
void OnReceive(const std::shared_ptr<SL::Remote_Access_Library::Network::Socket> ptr, std::shared_ptr<SL::Remote_Access_Library::Network::Packet>& pac, std::shared_ptr<SL::Remote_Access_Library::ServerImpl>& s) {
	s->_NetworkEvents.OnReceive(ptr, pac);
}
void OnClose(const std::shared_ptr<SL::Remote_Access_Library::Network::Socket> ptr, std::shared_ptr<SL::Remote_Access_Library::ServerImpl>& s) {
	{
		std::lock_guard<std::mutex> lock(s->ClientsLock);
		std::remove(begin(s->Clients), end(s->Clients), ptr);
	}
	s->_NetworkEvents.OnClose(ptr);
}
void Run(std::shared_ptr<SL::Remote_Access_Library::ServerImpl> serverimpl);
SL::Remote_Access_Library::Server::Server(unsigned short port, Network::NetworkEvents& netevents)
{
	_ServerImpl = std::make_shared<SL::Remote_Access_Library::ServerImpl>();
	Network::NetworkEvents lowerevents;
	lowerevents.OnConnect = [this](const std::shared_ptr<SL::Remote_Access_Library::Network::Socket> ptr) { OnConnect(ptr, _ServerImpl); };
	lowerevents.OnReceive = [this](const std::shared_ptr<SL::Remote_Access_Library::Network::Socket> ptr, std::shared_ptr<SL::Remote_Access_Library::Network::Packet>& pac) {OnReceive(ptr, pac, _ServerImpl); };
	lowerevents.OnClose = [this](const std::shared_ptr<SL::Remote_Access_Library::Network::Socket> ptr) { OnClose(ptr, _ServerImpl); };
	_ServerImpl->_NetworkEvents = netevents;
	_ServerImpl->_Listener = std::make_shared<Network::Listener>(port, lowerevents);
	_ServerImpl->Keepgoing = true;
	_ServerImpl->_Runner = std::thread(Run, _ServerImpl);
}

SL::Remote_Access_Library::Server::~Server()
{
	_ServerImpl->Keepgoing = false;
	_ServerImpl->_Runner.detach();//deatch it will stop eventually
}

//keep the data ALIVE!
void Run(std::shared_ptr<SL::Remote_Access_Library::ServerImpl> serverimpl) {
	serverimpl->_Listener->Start();
	auto screen(SL::Screen_Capture::CaptureDesktop(false));
	std::cout << "Testing Screen Gran and Compression methods right meow! " << std::endl;
	while (serverimpl->Keepgoing) {
		std::cout << "Grabbing Screen " << std::endl;
		screen = SL::Screen_Capture::CaptureDesktop(false);
		auto pack = SL::Remote_Access_Library::Network::Packet::CreatePacket(SL::Remote_Access_Library::Network::PACKET_TYPES::RESOLUTIONCHANGE, screen.getDataSize());
		memcpy(pack->get_Payload(), screen.getData(), screen.getDataSize());
		std::cout << "Got Screen, uncompressed size is: " << screen.getDataSize()<< std::endl;
		auto start = std::chrono::high_resolution_clock::now();
		pack->compress();

		std::cout << "Got Screen, compressed size is: " << pack->get_Payloadsize() << " Took: "<< std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - start).count()<< std::endl;
		
		auto pack1 = SL::Remote_Access_Library::Network::Packet::CreatePacket(SL::Remote_Access_Library::Network::PACKET_TYPES::RESOLUTIONCHANGE, screen.getDataSize());
		memcpy(pack1->get_Payload(), screen.getData(), screen.getDataSize());
		start = std::chrono::high_resolution_clock::now();
		pack1->compress1();
		std::cout << "Got Screen, compressed1 size is: " << pack1->get_Payloadsize() << " Took: " << std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - start).count() << std::endl;

		for (auto& a : serverimpl->Clients) {
			//a->send(pack);
		}
		std::this_thread::sleep_for(10s);

	}
	serverimpl->_Listener->Stop();
}