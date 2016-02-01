// Client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "..\Core\Server.h"
#include "..\Core\Server_Config.h"
#include <thread>
#include <chrono>

using namespace std::literals;


int main()
{
	SL::Remote_Access_Library::Network::Server_Config config;
	config.TCPListenPort = 6000;//listen on port 600 for tcp connections
	config.WebSocketListenPort = 6001;// listen for websockets
	SL::Remote_Access_Library::Server s(config);
	return s.Run();



}

