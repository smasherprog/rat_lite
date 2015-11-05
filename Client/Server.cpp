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
	_ServerImpl->_NetworkEvents.OnConnect = [this](const std::shared_ptr<SL::Remote_Access_Library::Network::Socket> ptr) {OnConnect(ptr, _ServerImpl); };
	_ServerImpl->_NetworkEvents.OnReceive = [this](const std::shared_ptr<SL::Remote_Access_Library::Network::Socket> ptr, std::shared_ptr<SL::Remote_Access_Library::Network::Packet>& pac) {OnReceive(ptr, pac, _ServerImpl); };
	_ServerImpl->_NetworkEvents.OnClose = [this](const std::shared_ptr<SL::Remote_Access_Library::Network::Socket> ptr) { OnClose(ptr, _ServerImpl); };
	_ServerImpl->_Listener = std::make_shared<Network::Listener>(port, _ServerImpl->_NetworkEvents);
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

	while (serverimpl->Keepgoing) {

		std::this_thread::sleep_for(100ms);

	}
	serverimpl->_Listener->Stop();
}