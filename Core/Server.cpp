#include "stdafx.h"
#include <iostream>
#include "Server.h"
#include "Listener.h"
#include <vector>
#include <algorithm>
#include <thread>
#include "Screen.h"
#include <mutex>
#include <chrono>
#include "Socket.h"
#include "Packet.h"
#include "Image.h"

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
void OnReceive(const SL::Remote_Access_Library::Network::Socket* ptr, std::shared_ptr<SL::Remote_Access_Library::Network::Packet>& pac, std::shared_ptr<SL::Remote_Access_Library::ServerImpl>& s) {
	s->_NetworkEvents.OnReceive(ptr, pac);
}
void OnClose(const SL::Remote_Access_Library::Network::Socket* ptr, std::shared_ptr<SL::Remote_Access_Library::ServerImpl>& s) {
	{
		std::lock_guard<std::mutex> lock(s->ClientsLock);
	
		auto cl = std::remove_if(begin(s->Clients), end(s->Clients), [ptr](const std::shared_ptr<SL::Remote_Access_Library::Network::Socket>& p) { return p.get() == ptr; });
		if (cl != s->Clients.end()) s->Clients.erase(cl);
	}
	std::cout << "Client Size: " << s->Clients.size() << std::endl;
	s->_NetworkEvents.OnClose(ptr);
}
void Run(std::shared_ptr<SL::Remote_Access_Library::ServerImpl> serverimpl);
SL::Remote_Access_Library::Server::Server(unsigned short port, Network::NetworkEvents& netevents)
{
	_ServerImpl = std::make_shared<SL::Remote_Access_Library::ServerImpl>();
	Network::NetworkEvents lowerevents;
	lowerevents.OnConnect = [this](const std::shared_ptr<SL::Remote_Access_Library::Network::Socket> ptr) { OnConnect(ptr, _ServerImpl); };
	lowerevents.OnReceive = [this](const SL::Remote_Access_Library::Network::Socket* ptr, std::shared_ptr<SL::Remote_Access_Library::Network::Packet>& pac) {OnReceive(ptr, pac, _ServerImpl); };
	lowerevents.OnClose = [this](const SL::Remote_Access_Library::Network::Socket* ptr) { OnClose(ptr, _ServerImpl); };
	_ServerImpl->_NetworkEvents = netevents;
	_ServerImpl->_Listener = Network::Listener::CreateListener(port, lowerevents);
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

	auto screen(SL::Remote_Access_Library::Screen_Capture::CaptureDesktop(false));
	std::cout << "Testing Screen Gran and Compression methods right meow! " << std::endl;
	auto serverimplementation = serverimpl.get();
	while (serverimplementation->Keepgoing) {
		std::cout << "Grabbing Screen " << std::endl;
		screen = SL::Remote_Access_Library::Screen_Capture::CaptureDesktop(false);
		std::cout << "Got Screen, uncompressed RGBA size is: " << screen->size() << std::endl;

		SL::Remote_Access_Library::Network::Commands::ImageChange imgheader;
		imgheader.top = imgheader.left = 0;
		imgheader.height = screen->Height();
		imgheader.width = screen->Width();

		SL::Remote_Access_Library::Network::PacketHeader p;
		p.Packet_Type = SL::Remote_Access_Library::Network::Commands::PACKET_TYPES::IMAGECHANGE;
		p.PayloadLen = screen->size() + sizeof(imgheader);

		auto pack = SL::Remote_Access_Library::Network::Packet::CreatePacket(p, imgheader, screen->data(), screen->size());

		{
			std::lock_guard<std::mutex> lock(serverimplementation->ClientsLock);
			for (auto& a : serverimplementation->Clients) {
				a->send(pack);
			}
		}
		std::this_thread::sleep_for(1s);

	}
	serverimpl->_Listener->Stop();
}