#include "stdafx.h"
#include "HttpListener.h"
#include <fstream>
#include <boost/filesystem.hpp>
#include "ApplicationDirectory.h"
#include <string>
#include "TCPListener.h"
#include "HttpSocket.h"
#include "IBaseNetworkDriver.h"

namespace SL {
	namespace Remote_Access_Library {
		namespace Network {
			namespace INTERNAL {

				class HttpServerImpl : public IBaseNetworkDriver {
				public:
					std::shared_ptr<TCPListener> _TCPListener;
					IBaseNetworkDriver* _IBaseNetworkDriver;
					void* _io_service;
					HttpServerImpl(IBaseNetworkDriver* netevent, void *io_service) : _io_service(io_service), _IBaseNetworkDriver(netevent) { }
					virtual ~HttpServerImpl() {
						Stop();
					}

					virtual void OnConnect(const std::shared_ptr<ISocket>& socket) override {
						std::cout << "HTTP Connect" << std::endl;
					}
					virtual void OnReceive(const std::shared_ptr<ISocket>& socket, std::shared_ptr<Packet>& packet)  override {
						std::cout << "HTTP OnReceive" << std::endl;
					}
					virtual void OnClose(const std::shared_ptr<ISocket>& socket)  override {
						std::cout << "HTTP Close" << std::endl;
					}
					void Start() {
						_TCPListener = TCPListener::Create(8080, _io_service, [this](void* socket) {
							SL::Remote_Access_Library::Network::HttpSocket::Create(this, socket);
						});
						auto st = executable_path(nullptr);
						auto fd = st.find_last_of('\\');
						if (fd == st.npos) fd = st.find('/');
						st = st.substr(0, fd);

						_TCPListener->Start();
					}
					void Stop() {
						_TCPListener->Stop();
					}

				};

			}
		}
	}
}


SL::Remote_Access_Library::Network::HttpListener::HttpListener(IBaseNetworkDriver* netevent, void *io_service)
{
	_HttpServerImpl = new INTERNAL::HttpServerImpl(netevent, io_service);
}

SL::Remote_Access_Library::Network::HttpListener::~HttpListener()
{
	delete _HttpServerImpl;
}

void SL::Remote_Access_Library::Network::HttpListener::Start()
{
	_HttpServerImpl->Start();
}

void SL::Remote_Access_Library::Network::HttpListener::Stop()
{
	_HttpServerImpl->Stop();
}

