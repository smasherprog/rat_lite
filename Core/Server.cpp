#include "stdafx.h"
#include "Server.h"
#include "ServerImpl.h"


SL::Remote_Access_Library::Server::Server(unsigned short port)
{
	wxInitAllImageHandlers();
	_ServerImpl = std::make_shared<INTERNAL::ServerImpl>(port);
}

SL::Remote_Access_Library::Server::~Server()
{

}
