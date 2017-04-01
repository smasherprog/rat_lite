#pragma once
#include "uWS.h"
#include "IWebSocket.h"
namespace SL {
	namespace RAT {


		template<class SOCKET_TYPE>class WebSocket :public IWebSocket {
			SOCKET_TYPE ws;
			std::mutex* Lock;
		public:
			WebSocket(SOCKET_TYPE w, std::mutex* m) : ws(w), Lock(m) {	
		
			}
			virtual ~WebSocket() {}
			virtual void send(const char* data, size_t len) override {
				std::lock_guard<std::mutex> lock(*Lock);
				auto s = (SocketStats*)ws->getUserData();
				s->TotalBytesSent += len;
				s->TotalPacketSent += 1;
				ws->send(data, len, uWS::OpCode::BINARY);
			}
			virtual void close(int code, const char* message, size_t length)  override {
				std::lock_guard<std::mutex> lock(*Lock);
				ws->close(code, message, length);
			}
			virtual SocketStats* get_Stats() override {
				return (SocketStats*)ws->getUserData();
			}
			virtual const char* get_address() override {
				return ws->getAddress().address;
			}
			virtual unsigned short get_port() override {
				return static_cast<unsigned short>(ws->getAddress().port);
			}

			virtual const char* get_address_family() override {
				return ws->getAddress().family;
			}

			virtual bool is_loopback() override {
				auto str(ws->getAddress().address);
				return strcmp(str, "::1") == 0 || strcmp(str, "127.0.0.1") == 0 || strcmp(str, "localhost") == 0;
			}
		};

	}
}