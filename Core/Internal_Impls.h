#pragma once
#include "BufferManager.h"
#include <boost/asio.hpp>
#include <thread>
#include "Packet.h"
#include "ThreadPool.h"

namespace SL {
	namespace Remote_Access_Library {
		namespace Network{
			struct NetworkEvents;
		}
	
		//this namespace should only be accessed by the Core library and NEVER by a consumer of this library
		namespace INTERNAL {
			extern Utilities::BufferManager _PacketBuffer;
			extern Utilities::BufferManager _ImageBuffer;
			class io_runner {
			public:
				boost::asio::io_service io_service;
				Utilities::ThreadPool ThreadPool;

				io_runner(int numofthreads=2);
				~io_runner();
			private:
				std::thread io_servicethread;
				boost::asio::io_service::work work;
			
			};

			struct SocketImpl;
			std::shared_ptr<SocketImpl> Create(boost::asio::io_service& io, Network::NetworkEvents& netevents, Utilities::ThreadPool& t, boost::asio::ip::tcp::socket&& s);
			std::shared_ptr<SocketImpl> Create(boost::asio::io_service& io, Network::NetworkEvents& netevents, Utilities::ThreadPool& t);

		}
	}
}