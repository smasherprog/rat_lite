#pragma once
#include "BaseNetworkDriver.h"
#include "Socket.h"
#include "Image.h"
#include "Packet.h"
#include <memory>
#include <string>
#include "Listener.h"

namespace SL {
	namespace Remote_Access_Library {

		namespace Network {

			template<class T>class ServerNetworkDriver : public BaseNetworkDriver {
				T* _Receiver;
				std::shared_ptr<Network::Listener> _Listener;

			public:
				ServerNetworkDriver(T* r, unsigned short port) : _Receiver(r) { _Listener = Network::Listener::CreateListener(port, this); }
				virtual ~ServerNetworkDriver() { }
				virtual void OnConnect(const std::shared_ptr<Socket>& socket) override {
					std::lock_guard<std::mutex> lock(ClientsLock);
					Clients.push_back(socket);
					_Receiver->OnConnect(socket); 
				}
				virtual void OnClose(const Socket* socket) override {
					_Receiver->OnClose(socket); 
					std::lock_guard<std::mutex> lock(ClientsLock);
					Clients.erase(std::remove_if(begin(Clients), end(Clients), [socket](const std::shared_ptr<SL::Remote_Access_Library::Network::Socket>& p) { return p.get() == socket; }), Clients.end());
				}


				virtual void OnReceive(const Socket* socket, std::shared_ptr<Packet>& p) override {
				


				}
				void SendToAll(std::shared_ptr<Network::Packet>& packet) {
					std::lock_guard<std::mutex> lock(ClientsLock);
					std::for_each(begin(Clients), end(Clients), [&packet](std::shared_ptr<SL::Remote_Access_Library::Network::Socket>& a) {	a->send(packet); });
				}
				void SendToAll(std::vector<std::shared_ptr<Network::Packet>>& packets) {
					std::lock_guard<std::mutex> lock(ClientsLock);
					std::for_each(begin(Clients), end(Clients), [&packets](std::shared_ptr<SL::Remote_Access_Library::Network::Socket>& a) {
						for (auto& p : packets) {
							a->send(p);
						}
					});
				}

				std::vector<std::shared_ptr<Network::Socket>> Clients;
				std::mutex ClientsLock;

			};

		}

	}

}
