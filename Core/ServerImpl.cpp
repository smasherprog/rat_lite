#include "stdafx.h"
#include "ServerImpl.h"
#include "Screen.h"
#include "Mouse.h"
#include "Image.h"

using namespace std::literals;

SL::Remote_Access_Library::INTERNAL::ServerImpl::ServerImpl(unsigned short port)
{
//	_Listener = Network::Listener::CreateListener(port, this);
	_Keepgoing = true;
	_Runner = std::thread(&SL::Remote_Access_Library::INTERNAL::ServerImpl::Run, this);
}

SL::Remote_Access_Library::INTERNAL::ServerImpl::~ServerImpl()
{
	_Keepgoing = false;
	_Runner.join();
}

void SL::Remote_Access_Library::INTERNAL::ServerImpl::OnConnect(const std::shared_ptr<Network::Socket>& socket)
{
	std::lock_guard<std::mutex> lock(ClientsLock);
	Clients.push_back(socket);
}

void SL::Remote_Access_Library::INTERNAL::ServerImpl::OnReceive(const Network::Socket * socket, std::shared_ptr<Network::Packet>& packet)
{

}

void SL::Remote_Access_Library::INTERNAL::ServerImpl::OnClose(const Network::Socket * socket)
{
	std::lock_guard<std::mutex> lock(ClientsLock);
	Clients.erase(std::remove_if(begin(Clients), end(Clients), [socket](const std::shared_ptr<SL::Remote_Access_Library::Network::Socket>& p) { return p.get() == socket; }), Clients.end());
}

void SL::Remote_Access_Library::INTERNAL::ServerImpl::SendToAll(std::shared_ptr<Network::Packet>& packet)
{
	std::lock_guard<std::mutex> lock(ClientsLock);
	std::for_each(begin(Clients), end(Clients), [&packet](std::shared_ptr<SL::Remote_Access_Library::Network::Socket>& a) {	a->send(packet); });
}

void SL::Remote_Access_Library::INTERNAL::ServerImpl::SendToAll(std::vector<std::shared_ptr<Network::Packet>>& packets)
{
	std::lock_guard<std::mutex> lock(ClientsLock);
	std::for_each(begin(Clients), end(Clients), [&packets](std::shared_ptr<SL::Remote_Access_Library::Network::Socket>& a) {	
		for (auto& p : packets) {
			a->send(p); 
		}
	});
}

std::shared_ptr<SL::Remote_Access_Library::Network::Packet> ProcessScreen(const std::shared_ptr<SL::Remote_Access_Library::Utilities::Image>& oldimg, const std::shared_ptr<SL::Remote_Access_Library::Utilities::Image>& newimg)
{
	auto screen = SL::Remote_Access_Library::Utilities::Image::CreateImage(newimg->Height(), newimg->Width(), newimg->data(), newimg->size());

	SL::Remote_Access_Library::Network::Commands::ImageChange imgheader;

	imgheader.top = imgheader.left = 0;
	imgheader.height = screen->Height();
	imgheader.width = screen->Width();

	SL::Remote_Access_Library::Network::PacketHeader p;
	p.Packet_Type = SL::Remote_Access_Library::Network::Commands::PACKET_TYPES::RESOLUTIONCHANGE;
	p.PayloadLen = screen->size() + sizeof(imgheader);

	auto pack = SL::Remote_Access_Library::Network::Packet::CreatePacket(p, imgheader, screen->data(), screen->size());

	
	auto rs = SL::Remote_Access_Library::Utilities::Image::GetDifs(*oldimg, *newimg);
	std::cout << "packet Compressed size is: " << pack->header()->PayloadLen << std::endl;
	return pack;
}


void SL::Remote_Access_Library::INTERNAL::ServerImpl::Run()
{
	//_Listener->Start();

	auto screen(SL::Remote_Access_Library::Capturing::CaptureDesktop());
	while (_Keepgoing) {

		auto tempscreen(SL::Remote_Access_Library::Capturing::CaptureDesktop());
		auto start = std::chrono::steady_clock::now();

		auto rs = SL::Remote_Access_Library::Utilities::Image::GetDifs(*screen, *tempscreen);
		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start);
		std::cout << "It took " << elapsed.count() << " milliseconds to compare screens " << rs.size() <<" size"<< std::endl;

		//wxBitmap bmp1(screen->data(), screen->Width(), screen->Height(), 32);
		//bmp1.SaveFile("c:\\users\\scott\\desktop\\Before.jpg", wxBitmapType::wxBITMAP_TYPE_JPEG);

		//wxBitmap bmp2(tempscreen->data(), tempscreen->Width(), tempscreen->Height(), 32);
		//bmp2.SaveFile("c:\\users\\scott\\desktop\\New.png", wxBitmapType::wxBITMAP_TYPE_PNG);

		//auto oldimg_ptr = (unsigned int*)screen->data();
		//for (auto& rec : rs) {
		//	for (auto row = rec.Origin.X; row < rec.Width+ rec.Origin.X; row ++) {
		//		for (auto col = rec.Origin.Y; col < rec.Height + rec.Origin.Y; col++) {
		//			oldimg_ptr[col*screen->Width() + row] = 0xffffff00;
		//		}
		//	}
		//}
		//wxBitmap bmp(screen->data(), screen->Width(), screen->Height(), 32);
		//bmp.SaveFile("c:\\users\\scott\\desktop\\somefile.png", wxBitmapType::wxBITMAP_TYPE_PNG);
		//ProcessScreen(screen, tempscreen);
		//SendToAll();
		std::this_thread::sleep_for(200ms);
		screen = tempscreen;
		

	}
}
