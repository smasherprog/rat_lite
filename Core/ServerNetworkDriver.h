#pragma once
#include "IBaseNetworkDriver.h"
#include "ISocket.h"
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

				std::vector<std::shared_ptr<Network::ISocket>> _Clients;
				std::mutex _ClientsLock;

			public:
				ServerNetworkDriverImpl(std::shared_ptr<Network::Listener> listener) :_Listener(listener) {}
				~ServerNetworkDriverImpl() { }
				void OnConnect(const std::shared_ptr<ISocket>& socket);
				void OnClose(const ISocket* socket);
				std::vector<std::shared_ptr<Network::ISocket>> GetClients();

				void Send(ISocket* socket, SL::Remote_Access_Library::Utilities::Rect& r, const Utilities::Image & img);
				void Send(SL::Remote_Access_Library::Utilities::Rect& r, const Utilities::Image & img);
				void StartListening() { _Listener->Start(); }
			};

			template<class T>class ServerNetworkDriver : public IBaseNetworkDriver {
				T* _Receiver;
				ServerNetworkDriverImpl _ServerNetworkDriverImpl;

			public:
				ServerNetworkDriver(T* r, unsigned short port) : _Receiver(r), _ServerNetworkDriverImpl(Network::Listener::CreateListener(port, this)) { 
					_ServerNetworkDriverImpl.StartListening();
				}
				virtual ~ServerNetworkDriver() { }
				virtual void OnConnect(const std::shared_ptr<ISocket>& socket) override {
					_Receiver->OnConnect(socket); 
					_ServerNetworkDriverImpl.OnConnect(socket);
				}
				virtual void OnClose(const ISocket* socket) override {
					_Receiver->OnClose(socket); 
					_ServerNetworkDriverImpl.OnClose(socket);
				}


				virtual void OnReceive(const ISocket* socket, std::shared_ptr<Packet>& p) override {
				


				}

				//forward calls along...
				template<class  ... Ts>
				void Send(Ts&& ... args) {
					_ServerNetworkDriverImpl.Send(std::forward<Ts>(args) ...);
				}

				std::vector<std::shared_ptr<Network::ISocket>> GetClients() {
					return _ServerNetworkDriverImpl.GetClients();
				}
			};
		}
	}
}
