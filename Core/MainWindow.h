#pragma once

#include "Socket.h"
#include "Packet.h"
#include "ThreadPool.h"
#include "Commands.h"
#include <vector>

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Toggle_Button.H>
#include <FL/Fl_RGB_Image.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Box.H>

namespace SL {
	namespace Remote_Access_Library {
		namespace Utilities {
			class Image;
		}
		namespace UI {
			class MainWindow : public Fl_Double_Window {
				Fl_Button* overb;
				Fl_Scroll* scroller = nullptr;
				Fl_RGB_Image* image = nullptr;
				Fl_Box* img_box = nullptr;
				std::vector<char> Data;
				std::function<void()> _OnToggleConnection;
				
			public:
				MainWindow(int x, int y, int w, int h, const char *l, std::function<void()> ontoggleconnection);

				void Update(Network::Commands::ImageChange* info, std::shared_ptr<Utilities::Image>& img);
				void ToggleConnect();
			};

		}
	}
}