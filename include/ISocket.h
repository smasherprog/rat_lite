#pragma once
#include <memory>
#include <string>

namespace SL {
	namespace RAT {

		enum class PACKET_TYPES : unsigned int {
			INVALID,
			HTTP_MSG,
			SCREENIMAGE,
			SCREENIMAGEDIF,
			MOUSEPOS,
			MOUSEIMAGE,
			KEYEVENT,
			MOUSEEVENT,
			CLIPBOARDTEXTEVENT,
			//use LAST_PACKET_TYPE as the starting point of your custom packet types. Everything before this is used internally by the library
			LAST_PACKET_TYPE
		};

		class ISocket {
		public:

			virtual ~ISocket() {}
			//adds the data to the internal queue, does not block and returns immediately.
			virtual void send(std::shared_ptr<char> data, size_t len) = 0;
			//sends a request that the socket be closed. NetworkEvents::OnClose will be called when the call is successful
			virtual void close(const std::string& reason) = 0;
			virtual bool closed() = 0;

			//s in in seconds
			virtual void set_ReadTimeout(int s) = 0;
			//s in in seconds
			virtual void set_WriteTimeout(int s) = 0;

			virtual std::string get_address() const = 0;
			virtual unsigned short get_port() const = 0;

			virtual bool is_v4() const = 0;
			virtual bool is_v6() const = 0;
			//is the this connection to ourselfs? i.e. 127.0.0.1 or ::1, etc
			virtual bool is_loopback() const = 0;
		};


	}
}
