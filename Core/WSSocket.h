#pragma once
#include <memory>
#include "ISocket.h"
#include "IBaseNetworkDriver.h"


namespace SL {
	namespace Remote_Access_Library {
		namespace Network {

			class WebSocketListinerImpl;
			class Packet;
			struct Server_Config;
			class WSSListener {
				std::shared_ptr<WebSocketListinerImpl> _WebSocketListinerImpl;
			public:

				WSSListener(IBaseNetworkDriver<std::shared_ptr<ISocket>, std::shared_ptr<Packet>>* netevent, std::shared_ptr<Network::Server_Config> config);
				~WSSListener();
			};

			class WSSocketImpl;
			class WSSocket : public ISocket {
				std::shared_ptr<WSSocketImpl> _WSSocketImpl;
			public:

				explicit WSSocket(std::shared_ptr<WSSocketImpl> impl);
				virtual ~WSSocket();

				virtual void send(Packet& pack) override;
				//sends a request that the socket be closed. NetworkEvents::OnClose will be called when the call is successful
				virtual void close(std::string reason) override;
				virtual bool closed() override;

				//Get the statstics for this socket
				virtual SocketStats get_SocketStats() const override;

				//s in in seconds
				virtual void set_ReadTimeout(int s)override;
				//s in in seconds
				virtual void set_WriteTimeout(int s) override;

				virtual std::string get_ipv4_address() const override;
				virtual unsigned short get_port() const override;
			
				static std::shared_ptr<WSSocket> connect(IBaseNetworkDriver<std::shared_ptr<ISocket>, std::shared_ptr<Packet>>* driver, const char* host, const char* port);

			};
		}
	}
}