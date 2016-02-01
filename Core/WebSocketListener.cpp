#include "stdafx.h"
#include "WebSocketListener.h"

#include <memory>

namespace SL {
	namespace Remote_Access_Library {
		namespace Network {
			namespace INTERNAL {
				class WebSocketListinerImpl {
			
					Network::IBaseNetworkDriver * _IBaseNetworkDriver;
				public:
					WebSocketListinerImpl(unsigned short port, Network::IBaseNetworkDriver *n) : _IBaseNetworkDriver(n) {

					}
					~WebSocketListinerImpl() {
					
					}

					void Start() {
			
		
					}
				};
			}
		}
	}
}
std::shared_ptr<SL::Remote_Access_Library::Network::WebSocketListener> SL::Remote_Access_Library::Network::WebSocketListener::CreateListener(unsigned short port, Network::IBaseNetworkDriver * netevents)
{
	return std::make_shared<WebSocketListener>(new INTERNAL::WebSocketListinerImpl(port, netevents));
}

SL::Remote_Access_Library::Network::WebSocketListener::WebSocketListener(INTERNAL::WebSocketListinerImpl* impl) :_ListinerImpl(impl)
{
}

SL::Remote_Access_Library::Network::WebSocketListener::~WebSocketListener()
{
	Stop();
}

void SL::Remote_Access_Library::Network::WebSocketListener::Start()
{
	if (!_ListinerImpl) _ListinerImpl = new INTERNAL::WebSocketListinerImpl(_ListenPort, _IBaseNetworkDriver);
	_ListinerImpl->Start();
}
void SL::Remote_Access_Library::Network::WebSocketListener::Stop()
{
	delete _ListinerImpl;
}
