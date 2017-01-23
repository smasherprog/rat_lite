#pragma once
#include <uWS/uWS.h>
namespace SL {
	namespace Remote_Access_Library {
		//pure virtual
		template <bool isServer>
		class INetworkHandlers {
		public:
			virtual ~INetworkHandlers() {}

			virtual void onConnection(uWS::WebSocket<isServer> ws, uWS::UpgradeInfo ui) = 0;
			virtual void onDisconnection(uWS::WebSocket<isServer> ws, int code, char *message, size_t length) = 0;
			virtual void onMessage(uWS::WebSocket<isServer> ws, char *message, size_t length, uWS::OpCode opCode) = 0;
		};
	}
}
