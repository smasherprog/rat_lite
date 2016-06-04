#pragma once
#include <memory>
#include <functional>

namespace SL {
	namespace Remote_Access_Library {
		namespace Network {
			struct Server_Config;
		}
		namespace Server {
			namespace UI {
				class GenerateCertificateWindowImpl;
				class GenerateCertificateWindow {
					GenerateCertificateWindowImpl* _GenerateCertificateWindowImpl = nullptr;
				public:
					GenerateCertificateWindow(std::shared_ptr<Network::Server_Config> ptr, std::function<void(bool)> certgen /*bool indicating whether infor was generated or not*/);
					~GenerateCertificateWindow();
					void Show();
					void Hide();

				};
			}
		}
	}
}
