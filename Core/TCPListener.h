#pragma once
#include <memory>
#include <functional>
namespace boost { namespace asio { class io_service; } }

namespace SL {
	namespace Remote_Access_Library {
		namespace Network {
			namespace INTERNAL {
				class ListinerImpl;
				class Listiner_DataImpl;
			};
			class TCPListener : public std::enable_shared_from_this<TCPListener> {
			public:
				//factory for listener creation
				static std::shared_ptr<TCPListener> Create(unsigned short port, boost::asio::io_service& io_service, std::function<void(void*)> onaccept);
				//Must use static factory TCPListener::Create to create a listener
				TCPListener(INTERNAL::Listiner_DataImpl* data);
				~TCPListener();
				//you must call start before network processing will begin
				void Start();
				void Stop();

				//no copy allowed
				TCPListener(const TCPListener&) = delete;
				TCPListener(TCPListener&&) = delete;
				//no copy allowed
				TCPListener& operator=(TCPListener&&) = delete;
				TCPListener& operator=(const TCPListener&) = delete;
			private:
				INTERNAL::ListinerImpl* _ListinerImpl;
				INTERNAL::Listiner_DataImpl* _Listiner_DataImpl;

			};
		}

	}
}
