// Client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include "..\Core\Server.h"
#include "..\Core\Socket.h"
#include <thread>
#include <chrono>

using namespace std::literals;


int main()
{

	SL::Remote_Access_Library::Server s(6000);
	while(true){
		std::this_thread::sleep_for(5s);
	}


    return 0;
}

