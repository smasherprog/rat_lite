#include "stdafx.h"

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Multi_Browser.H>

#include <memory>
#include <string>
#include <assert.h>
#include <stdio.h>
#include <sstream>

#include "LogWindow.h"
#include "ConnectionInfoWindow.h"

namespace SL {
	namespace Remote_Access_Library {
		namespace UI {
			class LogWindowImpl : Fl_Window {
			public:

				Fl_Multi_Browser* Log = nullptr;
				int _MaxLines = 50;

				LogWindowImpl() : Fl_Window(400, 420, 400, 400, "Server Log") {
					Log = new Fl_Multi_Browser(5, 20, this->w() - 10, this->h() - 40, "Events");
					Log->align(FL_ALIGN_TOP);
					this->resizable(Log);
					this->end();
			
				}
				virtual ~LogWindowImpl() {
				
				}

				void AddMessage(std::string msg) {
					Log->add(msg.c_str());
					//trim lines
					while (Log->size() > _MaxLines) {
						Log->remove(0);
					}
				}
				void set_MaxLines(int lines) {
					assert(lines > 10);
					_MaxLines = lines;
				}
				void Show() {
					this->show();
				}
				void Hide() {
					this->hide();
				}
			};

		}
	}
}


SL::Remote_Access_Library::UI::LogWindow::LogWindow()
{
	_LogWindowImpl = new LogWindowImpl();

}
SL::Remote_Access_Library::UI::LogWindow::~LogWindow()
{
	delete _LogWindowImpl;
}

void SL::Remote_Access_Library::UI::LogWindow::Show()
{
	_LogWindowImpl->Show();
}


void SL::Remote_Access_Library::UI::LogWindow::Hide()
{
	_LogWindowImpl->Hide();
}

void SL::Remote_Access_Library::UI::LogWindow::AddMessage(std::string msg)
{
	_LogWindowImpl->AddMessage(msg);
}
void SL::Remote_Access_Library::UI::LogWindow::set_MaxLines(int lines)
{
	_LogWindowImpl->set_MaxLines(lines);
}
