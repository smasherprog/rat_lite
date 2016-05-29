#pragma once
#include <memory>
#include "ISocket.h"
#include "IBaseNetworkDriver.h"


namespace SL {
	namespace Remote_Access_Library {
		namespace Network {

			class Packet;
			class HttpsServerImpl;
			struct Server_Config;
			class HttpsListener {
				std::shared_ptr<HttpsServerImpl> _HttpsServerImpl;
			public:

				HttpsListener(IBaseNetworkDriver<std::shared_ptr<ISocket>, std::shared_ptr<Packet>>* netevent,  std::shared_ptr<Network::Server_Config> config);
				//no copy allowed
				HttpsListener(const HttpsListener&) = delete;
				HttpsListener(HttpsListener&&) = delete;
				//no copy allowed
				HttpsListener& operator=(HttpsListener&&) = delete;
				HttpsListener& operator=(const HttpsListener&) = delete;
				~HttpsListener();



			};




			class HttpsSocketImpl;

			//this class is async so all calls return immediately and are later executed
			class HttpsSocket : public ISocket{
				std::shared_ptr<HttpsSocketImpl> _HttpsSocketImpl;
			public:

				HttpsSocket(std::shared_ptr<HttpsSocketImpl> impl);
				virtual ~HttpsSocket();

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


			};


		}
	}
}