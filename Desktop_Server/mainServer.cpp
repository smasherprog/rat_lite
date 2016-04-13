
#include "stdafx.h"
#include "Server.h"
#include "Server_Config.h"
#include "ApplicationDirectory.h"
#include <string>
#include <assert.h>
#include <stdio.h>

int main(int argc, char **argv)
{

	SL::Remote_Access_Library::Network::Server_Config config;
	config.WebSocketListenPort = 6001;// listen for websockets
	config.HttpListenPort = 8080;
	auto searchpath = executable_path(nullptr);
	auto exeindex = searchpath.find_last_of('\\');
	if (exeindex == searchpath.npos) exeindex = searchpath.find_last_of('/');
	if (exeindex != searchpath.npos) {
		config.WWWRoot = searchpath.substr(0, exeindex) + "/wwwroot/";
	}
	assert(exeindex != std::string::npos);
	SL::Remote_Access_Library::Server s(config);
	return s.Run();
}

