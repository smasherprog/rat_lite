#include "stdafx.h"
#include "MainWindow.h"
#include "Image.h"
#include <FL/fl_draw.H>
#include "Image.h"

void button_cb(Fl_Widget *w, void *d) {
	auto wnd = (SL::Remote_Access_Library::UI::MainWindow*)d;
	wnd->ToggleConnect();
}

SL::Remote_Access_Library::UI::MainWindow::MainWindow(int x, int y, int w, int h, const char * l, std::function<void()> onToggleConnection) : Fl_Double_Window(x, y, w, h, l), _OnToggleConnection(onToggleConnection)
{
	resizable(this);
	
	scroller = new Fl_Scroll(0, 0, w, h);
	overb = new Fl_Button(5, 5, 100, 25, "Connect");
	img_box = new Fl_Box(5, 5, w - 5, h - 5);
	overb->callback(button_cb, this);
	resizable(img_box);
	resizable(scroller);
	end();
	Fl::visual(FL_RGB);
}

void SL::Remote_Access_Library::UI::MainWindow::Update(Network::Commands::ImageChange * info, std::shared_ptr<Utilities::Image>& img)
{
	auto h(0), w(0);
	if (image != nullptr) {
		h = image->h();
		w = image->w();
	}
	if (image == nullptr || info->height >= h || info->width >= w) {	
		delete image;
		image = new Fl_RGB_Image((unsigned char*)img->data(), info->width, info->height, 3, info->width * 3);
		img_box->image(image);
		img_box->size(info->width, info->height);
	}
	else if (info->height < h && info->width < w) {
		for (auto i = 0; i < info->width; i++) {
			auto ptr = image->array + (info->left * 4) + (info->top*image->w() * 4);
			memcpy((void*)ptr, img->data() + (i*info->width * 4), info->width * 4);
		}
	}
	img_box->redraw();
	Fl::check();

}

void SL::Remote_Access_Library::UI::MainWindow::ToggleConnect()
{
	_OnToggleConnection();
}


