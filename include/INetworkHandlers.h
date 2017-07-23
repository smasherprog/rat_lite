#pragma once
#include <memory>
#include <string>

namespace SL {
    namespace WS_LITE {
        class IWSocket;
        struct WSMessage;
    }
	namespace RAT {
        enum class PACKET_TYPES : unsigned int {
            INVALID,
            HTTP_MSG,
            ONMONITORSCHANGED,
            ONNEWFRAME,
            ONFRAMECHANGED,
            ONMOUSEIMAGECHANGED,
            ONMOUSEPOSITIONCHANGED,
            ONKEYCHANGED,
            ONMOUSECHANGED,
            ONCLIPBOARDTEXTCHANGED,
            //use LAST_PACKET_TYPE as the starting point of your custom packet types. Everything before this is used internally by the library
            LAST_PACKET_TYPE
        };
		class INetworkHandlers {
		public:
			virtual ~INetworkHandlers() {}

			virtual void onConnection(const std::shared_ptr<SL::WS_LITE::IWSocket>& socket) = 0;
			virtual void onMessage(const std::shared_ptr<SL::WS_LITE::IWSocket>& socket, const WS_LITE::WSMessage& msg) = 0;
			virtual void onDisconnection(const std::shared_ptr<SL::WS_LITE::IWSocket>& socket, unsigned short code, const std::string& msg) = 0;
		};
	}
}
