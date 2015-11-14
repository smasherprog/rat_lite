// Client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include "Server.h"
#include "..\Core\CommonNetwork.h"
#include <thread>
#include <chrono>

using namespace std::literals;
SL::Remote_Access_Library::Network::NetworkEvents netevents;

int main()
{
	netevents.OnConnect = [](const std::shared_ptr<SL::Remote_Access_Library::Network::Socket> ptr) { std::cout << "Connected" << std::endl; };
	netevents.OnReceive = [](const SL::Remote_Access_Library::Network::Socket* ptr, std::shared_ptr<SL::Remote_Access_Library::Network::Packet>& pac) { std::cout << "OnReceive" << std::endl; };
	netevents.OnClose = [](const SL::Remote_Access_Library::Network::Socket* ptr) { std::cout << "Disconnected" << std::endl; };
	SL::Remote_Access_Library::Server s(6000, netevents);
		while(true){
		
		std::this_thread::sleep_for(5s);
	}


    return 0;
}

