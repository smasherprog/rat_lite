#pragma once

namespace SL {
	namespace RAT {

		enum class PACKET_TYPES : unsigned int {
			INVALID,
			HTTP_MSG,
			MONITORINFO,
			SCREENIMAGEDIF,
			MOUSEPOS,
			MOUSEIMAGE,
			KEYEVENT,
			MOUSEEVENT,
			CLIPBOARDTEXTEVENT,
			//use LAST_PACKET_TYPE as the starting point of your custom packet types. Everything before this is used internally by the library
			LAST_PACKET_TYPE
		};
		struct SocketStats {
			long long TotalBytesSent = 0;
			long long TotalPacketSent = 0;
			long long TotalBytesReceived = 0;
			long long TotalPacketReceived = 0;
		};
		class IWebSocket {
		public:
			virtual ~IWebSocket() {}
			//the beginning of the payload must contain a PACKET_TYPE of your custom type
			virtual void send(const char* data, size_t len) = 0;
			//sends a request that the socket be closed. NetworkEvents::OnClose will be called when the call is successful
			virtual void close(int code, const char* message, size_t length) = 0;

			virtual const char* get_address() = 0;
			virtual unsigned short get_port() = 0;
			virtual const char* get_address_family() = 0;
			virtual SocketStats* get_Stats() = 0;
			//is the this connection to ourselfs? i.e. 127.0.0.1 or ::1, etc
			virtual bool is_loopback() = 0;
		};
	}
}
