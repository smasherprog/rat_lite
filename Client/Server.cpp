#include "stdafx.h"
#include "..\Core\CommonNetwork.h"
#include "Server.h"
#include "..\Core\Listener.h"
#include <vector>
#include <algorithm>
#include <thread>
#include "..\Core\Screen.h"
#include <mutex>
#include <chrono>
#include "..\Core\Socket.h"
#include "..\Core\Packet.h"
#include "turbojpeg.h"

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
		auto socketptr = ptr.get();
		s->Clients.erase(std::remove_if(begin(s->Clients), end(s->Clients), [socketptr](const std::shared_ptr<SL::Remote_Access_Library::Network::Socket>& p) { return p.get() == socketptr; }));
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
	lowerevents.OnReceive = [this](const std::shared_ptr<SL::Remote_Access_Library::Network::Socket> ptr, std::shared_ptr<SL::Remote_Access_Library::Network::Packet>& pac) {OnReceive(ptr, pac, _ServerImpl); };
	lowerevents.OnClose = [this](const std::shared_ptr<SL::Remote_Access_Library::Network::Socket> ptr) { OnClose(ptr, _ServerImpl); };
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

	auto compfree = [](void* handle) {tjDestroy(handle); };

	auto _jpegCompressor(std::unique_ptr<void, decltype(compfree)>(tjInitCompress(), compfree));
	std::vector<char> compressBuffer;

	auto screen(SL::Remote_Access_Library::Screen_Capture::CaptureDesktop(false));
	std::cout << "Testing Screen Gran and Compression methods right meow! " << std::endl;
	while (serverimpl->Keepgoing) {
		std::cout << "Grabbing Screen " << std::endl;
		screen = SL::Remote_Access_Library::Screen_Capture::CaptureDesktop(false);
		std::cout << "Got Screen, uncompressed RGBA size is: " << screen->size() << std::endl;
		SL::Remote_Access_Library::Network::PacketHeader p;
		p.Packet_Type = SL::Remote_Access_Library::Network::PACKET_TYPES::RESOLUTIONCHANGE;
		p.PayloadLen = screen->size();
		auto pack = SL::Remote_Access_Library::Network::Packet::CreatePacket(p);
		memcpy(pack->data(), screen->data(), screen->size());

		/*	auto GOGRAY = false;
			auto imgquality = 70;
			auto set = GOGRAY ? TJSAMP_GRAY : TJSAMP_420;

			auto maxsize = tjBufSize(screen->Width(), screen->Height(), set);
			long unsigned int _jpegSize = maxsize;

			if (compressBuffer.capacity() < maxsize) compressBuffer.reserve(maxsize + 16);
			auto ptr = (unsigned char*)compressBuffer.data();

			start = std::chrono::high_resolution_clock::now();

			if (tjCompress2(_jpegCompressor.get(), (unsigned char*)screen->data(), screen->Width(), 0, screen->Height(), TJPF_BGRX, &ptr, &_jpegSize, set, imgquality, TJFLAG_FASTDCT | TJFLAG_NOREALLOC) == -1) {
				std::cout << "Err msg " << tjGetErrorStr() << std::endl;
			}
			else {
				std::cout << "JEPG Compression  " << _jpegSize << " Took: " << std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - start).count() << std::endl;
			}

			auto pack2 = SL::Remote_Access_Library::Network::Packet::CreatePacket(SL::Remote_Access_Library::Network::PACKET_TYPES::RESOLUTIONCHANGE, _jpegSize);
			memcpy(pack2->get_Payload(), ptr, _jpegSize);
			start = std::chrono::high_resolution_clock::now();
			pack2->compress();
			std::cout << "Got Screen, JPEG + compressed size is: " << pack2->get_Payloadsize() << " Took: " << std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - start).count() << std::endl;


	*/

		for (auto& a : serverimpl->Clients) {
			a->send(pack);
		}
		std::this_thread::sleep_for(10s);

	}
	serverimpl->_Listener->Stop();
}