#pragma once
#include <memory>

class wxFrame;
namespace SL {
	namespace Remote_Access_Library {
		namespace UI {
			class ConnectWindowImpl;
			
			class ConnectWindow 
			{
			public:
				ConnectWindow();
				wxFrame* get_Frame();
			private:
				std::unique_ptr<ConnectWindowImpl> _Impl;
			};


		}
	}
}
