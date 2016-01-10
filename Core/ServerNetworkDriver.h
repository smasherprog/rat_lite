#pragma once
#include "BaseNetworkDriver.h"
#include "Socket.h"
#include "Packet.h"
#include <memory>
#include <string>
#include "Listener.h"

namespace SL {
	namespace Remote_Access_Library {
		namespace Utilities {
			class Image;
			class Rect;
		}
		namespace Network {

			class ServerNetworkDriverImpl {

				std::shared_ptr<Network::Listener> _Listener;

				void SendToAll(std::shared_ptr<Network::Packet>& packet);
				void SendToAll(std::vector<std::shared_ptr<Network::Packet>>& packets);

				std::vector<std::shared_ptr<Network::Socket>> _Clients;
				std::mutex _ClientsLock;

			public:
				ServerNetworkDriverImpl(std::shared_ptr<Network::Listener> listener) :_Listener(listener) {}
				~ServerNetworkDriverImpl() { }
				void OnConnect(const std::shared_ptr<Socket>& socket);
				void OnClose(const Socket* socket);
				std::vector<std::shared_ptr<Network::Socket>> GetClients();

				void Send(Socket* socket, SL::Remote_Access_Library::Utilities::Rect& r, const Utilities::Image & img);
				void Send(SL::Remote_Access_Library::Utilities::Rect& r, const Utilities::Image & img);
				void StartListening() { _Listener->Start(); }
			};

			template<class T>class ServerNetworkDriver : public BaseNetworkDriver {
				T* _Receiver;
				ServerNetworkDriverImpl _ServerNetworkDriverImpl;

			public:
				ServerNetworkDriver(T* r, unsigned short port) : _Receiver(r), _ServerNetworkDriverImpl(Network::Listener::CreateListener(port, this)) { 
					_ServerNetworkDriverImpl.StartListening();
				}
				virtual ~ServerNetworkDriver() { }
				virtual void OnConnect(const std::shared_ptr<Socket>& socket) override {
					_ServerNetworkDriverImpl.OnConnect(socket);
					_Receiver->OnConnect(socket); 
				}
				virtual void OnClose(const Socket* socket) override {
					_Receiver->OnClose(socket); 
					_ServerNetworkDriverImpl.OnClose(socket);
				}


				virtual void OnReceive(const Socket* socket, std::shared_ptr<Packet>& p) override {
				


				}

				//forward calls along...
				template<class  ... Ts>
				void Send(Ts&& ... args) {
					_ServerNetworkDriverImpl.Send(std::forward<Ts>(args) ...);
				}

				std::vector<std::shared_ptr<Network::Socket>> GetClients() {
					return _ServerNetworkDriverImpl.GetClients();
				}
			};
		}
	}
}
