#include "stdafx.h"
#include "Viewer.h"
#include "MainWindow.h"
#include "Image.h"

void SL::Remote_Access_Library::UI::Viewer::onreceive(const SL::Remote_Access_Library::Network::Socket * s, std::shared_ptr<SL::Remote_Access_Library::Network::Packet>& p)
{
	if (p->header()->Packet_Type == SL::Remote_Access_Library::Network::Commands::PACKET_TYPES::IMAGECHANGE) {
		auto imginfo = (SL::Remote_Access_Library::Network::Commands::ImageChange*)p->data();
		auto img = Remote_Access_Library::Utilities::Image::CreateImage(imginfo->height, imginfo->width, p->data() + sizeof(SL::Remote_Access_Library::Network::Commands::ImageChange), p->header()->PayloadLen - sizeof(SL::Remote_Access_Library::Network::Commands::ImageChange), true);
		img->decompress();
		wnd->Update(imginfo, img);
	}
}
void SL::Remote_Access_Library::UI::Viewer::ToggleConnectEvent()
{
	if (sock) {
		sock->close();
	}
	else {
		sock = SL::Remote_Access_Library::Network::Socket::ConnectTo("127.0.0.1", "6000", netevents);
	}
}
SL::Remote_Access_Library::UI::Viewer::Viewer()
{
	wnd = new MainWindow(10, 10, 400, 400, "Viewer Placeholder", std::bind(&SL::Remote_Access_Library::UI::Viewer::ToggleConnectEvent, this));
	wnd->show();
	netevents.OnReceive = std::bind(&SL::Remote_Access_Library::UI::Viewer::onreceive, this, std::placeholders::_1, std::placeholders::_2);

}
