#pragma once
#include <string>
#include <functional>

class wxFrame;
namespace SL {
	namespace Remote_Access_Library {
		namespace UI {
			class ConnectWindowImpl;
			
			class ConnectWindow 
			{
			public:
				ConnectWindow(std::function<void(std::string)> connfunc, std::function<void()> onquit);
				wxFrame* get_Frame();
			private:
				std::unique_ptr<ConnectWindowImpl> _Impl;
			};


		}
	}
}
