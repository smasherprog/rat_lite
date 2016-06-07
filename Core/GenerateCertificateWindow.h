#pragma once

namespace SL {
	namespace Remote_Access_Library {
		namespace Server {
			namespace UI {
				class GenerateCertificateWindowImpl;
				class GenerateCertificateWindow {
					GenerateCertificateWindowImpl* _GenerateCertificateWindowImpl = nullptr;
				public:
					GenerateCertificateWindow();
					~GenerateCertificateWindow();
					void Show();
					void Hide();

				};
			}
		}
	}
}
