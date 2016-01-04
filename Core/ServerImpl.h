#pragma once
#include "BaseNetworkDriver.h"
#include "Socket.h"
#include "Packet.h"
#include "Listener.h"
#include <mutex>
#include <thread>
#include <vector>

namespace SL {
	namespace Remote_Access_Library {
	
			namespace INTERNAL
			{
				class ServerImpl : public Network::BaseNetworkDriver {
				public:
					ServerImpl(unsigned short port);
					virtual ~ServerImpl();
					virtual void OnConnect(const std::shared_ptr<Network::Socket>& socket) override;
					virtual void OnReceive(const Network::Socket* socket, std::shared_ptr<Network::Packet>& packet);
					virtual void OnClose(const Network::Socket* socket);

					void SendToAll(std::shared_ptr<Network::Packet>& packet);
					void SendToAll(std::vector<std::shared_ptr<Network::Packet>>& packets);
					void Run();
					std::shared_ptr<Network::Listener> _Listener;
					std::vector<std::shared_ptr<Network::Socket>> Clients;
					std::mutex ClientsLock;
					bool _Keepgoing;
					std::thread _Runner;
				};
			
		}
	}
}