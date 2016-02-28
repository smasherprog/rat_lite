// Client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../Core/Server.h"
#include "../Core/Server_Config.h"
#include "../Core/ApplicationDirectory.h"
#include <string>
#include <assert.h>

int main()
{
	SL::Remote_Access_Library::Network::Server_Config config;
	config.TCPListenPort = 6000;//listen on port 600 for tcp connections
	config.WebSocketListenPort = 6001;// listen for websockets
	config.WWWRoot = "";

	auto searchpath = executable_path(nullptr);
	auto exeindex = searchpath.find_last_of('\\');
	if (exeindex == searchpath.npos) exeindex = searchpath.find_last_of('/');
	if (exeindex != searchpath.npos) {
		config.WWWRoot = searchpath.substr(0, exeindex);
	}
	assert(exeindex != std::string::npos);
	SL::Remote_Access_Library::Server s(config);
	return s.Run();



}

