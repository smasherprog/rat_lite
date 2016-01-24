#include "stdafx.h"
#include "WebSocketListener.h"



namespace SL {
	namespace Remote_Access_Library {
		namespace INTERNAL {
			class WebSocketListinerImpl {
			public:
				WebSocketListinerImpl() {}
			};
		}
	}
}
std::shared_ptr<SL::Remote_Access_Library::Network::WebSocketListener> SL::Remote_Access_Library::Network::WebSocketListener::CreateListener(unsigned short port, Network::IBaseNetworkDriver * netevents)
{
	return std::make_shared<WebSocketListener>(new INTERNAL::WebSocketListinerImpl());
}

SL::Remote_Access_Library::Network::WebSocketListener::WebSocketListener(INTERNAL::WebSocketListinerImpl* impl) :_ListinerImpl(impl)
{
}

SL::Remote_Access_Library::Network::WebSocketListener::~WebSocketListener()
{
	delete _ListinerImpl;
}

void SL::Remote_Access_Library::Network::WebSocketListener::Start()
{

}
