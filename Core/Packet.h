#pragma once
namespace SL {
	namespace Remote_Access_Library {
		namespace Network {

			enum class PACKET_TYPES : unsigned int {
				INVALID,
				WEBSOCKET_TEXT,
				WEBSOCKET_BINARY,
				WEBSOCKET_CLOSE,
				WEBSOCKET_PING,
				HTTP_MSG,
				IMAGEDIF,
				MOUSEEVENT,
				KEYEVENT,
				//use LAST_PACKET_TYPE as the starting point of your custom packet types. Everything before this is used internally by the library
				LAST_PACKET_TYPE
			};
			//used in the header for http/websocket requests


			class Packet {
				//no copy allowed
				Packet(const Packet&) = delete;
				Packet& operator=(const Packet&) = delete;
				bool _owns_Payload;
				void _clearaftermove() {//makes sure to clear all data after a move
					_owns_Payload = false;
					Packet_Type = 0;
					Payload_Length = 0;
					Payload = nullptr;
				}
			public:
				Packet(unsigned int packet_type) : Packet_Type(packet_type), Payload(nullptr), _owns_Payload(true) {}
				Packet(unsigned int packet_type, unsigned int payloadsize) : Packet_Type(packet_type), Payload_Length(payloadsize), Payload(new char[payloadsize]), _owns_Payload(true) {}
				Packet(unsigned int packet_type, unsigned int payloadsize, char* buf, bool take_ownership = true) : Packet_Type(packet_type), Payload_Length(payloadsize), Payload(std::move(buf)), _owns_Payload(take_ownership) {}
				Packet(unsigned int packet_type, unsigned int payloadsize, std::unordered_map<std::string, std::string>&& header, char* buf, bool take_ownership = true) : Packet_Type(packet_type), Payload_Length(payloadsize), Payload(buf), _owns_Payload(take_ownership), Header(std::move(header)) {}
				Packet(unsigned int packet_type, unsigned int payloadsize, std::unordered_map<std::string, std::string>&& header) : Packet_Type(packet_type), Payload_Length(payloadsize), Payload(new char[payloadsize]), _owns_Payload(true), Header(std::move(header)) {}
				Packet(Packet&& other) {
					operator=(std::move(other));
				}
				Packet& operator=(Packet&& other) {
					Packet_Type = other.Packet_Type;
					Payload_Length = other.Payload_Length;
					Payload = other.Payload;
					Header = std::move(other.Header);
					_owns_Payload = other._owns_Payload;
					other._clearaftermove();
					return *this;
				}
				~Packet() {
					if (_owns_Payload) delete[] Payload;
				}
				unsigned int Packet_Type;
				unsigned int Payload_Length;
				char* Payload;
				std::unordered_map<std::string, std::string> Header;
			};

		}
	}
}