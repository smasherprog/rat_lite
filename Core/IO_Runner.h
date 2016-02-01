#pragma once




namespace SL {
	namespace Remote_Access_Library {
		namespace Network {
			class IO_RunnerImpl;
			class IO_Runner {
			public:
				IO_Runner();
				~IO_Runner();
				void Start();
				void Stop();
				//this is really boost::asio::io_service, but I dont want to expose that to the outside world.. its used internally only
				void* get_io_service();
			private:
				IO_RunnerImpl* _IO_RunnerImpl;
			};
		}
	}
}