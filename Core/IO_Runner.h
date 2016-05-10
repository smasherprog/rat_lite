#pragma once
#include <memory>

namespace boost { namespace asio { class io_service; } }
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

				boost::asio::io_service& get_io_service();
			private:
				std::unique_ptr<IO_RunnerImpl> _IO_RunnerImpl;
			};
		}
	}
}