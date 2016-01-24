#pragma once
#include "IBaseNetworkDriver.h"
#include <memory>


namespace SL {
	namespace Remote_Access_Library {
		namespace Utilities {
			class Image;
			class Rect;
		}
		namespace Network {
			class ServerNetworkDriverImpl;
			class IServerDriver;

			class ServerNetworkDriver : public IBaseNetworkDriver {
				std::unique_ptr<ServerNetworkDriverImpl> _ServerNetworkDriverImpl;

			public:
				ServerNetworkDriver(Network::IServerDriver * r, unsigned short port);
				virtual ~ServerNetworkDriver();

				virtual void OnConnect(const std::shared_ptr<ISocket>& socket) override;
				virtual void OnClose(const ISocket* socket) override;
				virtual void OnReceive(const ISocket* socket, std::shared_ptr<Packet>& p);

				void StartNetworkProcessing();
	

				void Send(SL::Remote_Access_Library::Utilities::Rect& r, const Utilities::Image & img);
				void Send(ISocket* socket, SL::Remote_Access_Library::Utilities::Rect& r, const Utilities::Image & img);

				

				std::vector<std::shared_ptr<Network::ISocket>> GetClients();

			};
		
		}
	}
}
