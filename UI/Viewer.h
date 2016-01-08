#pragma once

#include <string>
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <functional>

namespace SL {
	namespace Remote_Access_Library {

		namespace UI {
			class Viewer : public wxApp {
			
				virtual bool OnInit() override;
				
			public:
		
				Viewer();
				virtual ~Viewer() {}
			};
		}
	}
}