#pragma once

#include "Socket.h"
#include "Packet.h"
#include "ThreadPool.h"
#include "Commands.h"

namespace SL {
	namespace Remote_Access_Library {

		namespace UI {
			class MainWindow;
			class Viewer{
				Network::NetworkEvents netevents;
				std::shared_ptr<Network::Socket> sock;
				void onreceive(const SL::Remote_Access_Library::Network::Socket* s, std::shared_ptr<SL::Remote_Access_Library::Network::Packet>& p);
				MainWindow* wnd = nullptr;
				void ToggleConnectEvent();
			public:
				Viewer();


			};
		}
	}
}