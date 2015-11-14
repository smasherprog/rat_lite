#include "stdafx.h"
#include "Screen.h"
#include "Internal_Impls.h"



struct SL::Remote_Access_Library::Screen_Capture::Image_Impl {
	unsigned int height;
	unsigned int width;
};

std::shared_ptr<SL::Remote_Access_Library::Screen_Capture::Image> SL::Remote_Access_Library::Screen_Capture::Image::CreateImage(unsigned int h, unsigned int w) {
	Image_Impl i;
	i.height = h;
	i.width = w;
	return std::make_shared<SL::Remote_Access_Library::Screen_Capture::Image>(i);
}
SL::Remote_Access_Library::Screen_Capture::Image::Image(Image_Impl& impl) : _Height(impl.height), _Width(impl.width) {

	_Data= 	Remote_Access_Library::INTERNAL::_Buffer.AquireBuffer(impl.height*impl.width *4);//stride is always 4 bytes
}
SL::Remote_Access_Library::Screen_Capture::Image::~Image() {

	Remote_Access_Library::INTERNAL::_Buffer.ReleaseBuffer(_Data);
}
char* SL::Remote_Access_Library::Screen_Capture::Image::data() const { return _Data.data; }
size_t SL::Remote_Access_Library::Screen_Capture::Image::size() const {
	return _Height*_Width * 4;
}