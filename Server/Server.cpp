
#include "stdafx.h"
#include "../Core/Server.h"
#include "../Core/Server_Config.h"
#include "../Core/ApplicationDirectory.h"
#include <string>
#include <assert.h>

#if __linux__
	#include <gtk/gtk.h>
#endif
int main(int argc, char **argv)
{
		
#if __linux__
	gdk_init(&argc, &argv);
#endif
	
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

