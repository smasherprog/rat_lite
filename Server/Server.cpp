// Client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "..\Core\Server.h"
#include <thread>
#include <chrono>

using namespace std::literals;


int main()
{

	SL::Remote_Access_Library::Server s(6000);
	return s.Run();
}

