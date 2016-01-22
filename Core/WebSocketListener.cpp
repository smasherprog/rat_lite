#include "stdafx.h"
#include "WebSocketListener.h"
#include "libwebsockets.h"


namespace SL {
	namespace Remote_Access_Library {
		namespace INTERNAL {
			class WebSocketListinerImpl {
			public:

				Network::IBaseNetworkDriver* NetworkEvents = nullptr;
				lws_context *context = nullptr;
				WebSocketListinerImpl() {
				
				}
				~WebSocketListinerImpl() {
					if (context != nullptr) {
						lws_context_destroy(context);
					}
				}


				void on_connect(lws *wsi) {

				}
				void on_message(lws *wsi, void *in, size_t len) {

				}
				void on_close(lws *wsi) {

				}
				void on_http_req(lws *wsi, char *request) {

				}

			};
		}
	}
}
static int callback_http(lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len) {
	return 0;
}
static int callback_websockets(lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len) {
	auto self = (SL::Remote_Access_Library::INTERNAL::WebSocketListinerImpl *)lws_wsi_user(wsi);

	switch (reason) {
	case LWS_CALLBACK_ESTABLISHED:
		self->on_connect(wsi);
		break;
	case LWS_CALLBACK_RECEIVE:
		self->on_message(wsi, in, len);
		break;

	case LWS_CALLBACK_CLOSED:
		self->on_close(wsi);
		break;
	}
	return 0;
}

static struct lws_protocols server_protocols[] = {
	{ "http-only", callback_http, 0 },
	{ NULL, callback_websockets, sizeof(int), 1024 * 1024 },
	{ NULL, NULL, 0 /* End of list */ }
};
std::shared_ptr<SL::Remote_Access_Library::Network::WebSocketListener> SL::Remote_Access_Library::Network::WebSocketListener::CreateListener(unsigned short port, Network::IBaseNetworkDriver * netevents)
{
	lws_set_log_level(LLL_ERR | LLL_WARN, NULL);

	lws_context_creation_info info = { 0 };
	info.port = port;
	info.gid = -1;
	info.uid = -1;
	info.protocols = server_protocols;

	auto selfint = new INTERNAL::WebSocketListinerImpl();
	info.user = selfint;
	selfint->context = lws_create_context(&info);
	selfint->NetworkEvents = netevents;
	return std::make_shared<WebSocketListener>(selfint);

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
