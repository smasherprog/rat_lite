#pragma once

namespace asio { class io_service; } 
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

				asio::io_service& get_io_service();
			private:
				IO_RunnerImpl* _IO_RunnerImpl;
			};
		}
	}
}