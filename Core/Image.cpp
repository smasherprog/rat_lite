#include "stdafx.h"
#include "Image.h"
#include "Internal_Impls.h"
#include "turbojpeg.h"

struct SL::Remote_Access_Library::Utilities::Image_Impl {
	unsigned int height;
	unsigned int width;
};

std::shared_ptr<SL::Remote_Access_Library::Utilities::Image> SL::Remote_Access_Library::Utilities::Image::CreateImage(unsigned int h, unsigned int w) {
	Image_Impl i;
	i.height = h;
	i.width = w;
	return std::make_shared<SL::Remote_Access_Library::Utilities::Image>(i);
}
std::shared_ptr<SL::Remote_Access_Library::Utilities::Image> SL::Remote_Access_Library::Utilities::Image::CreateImage(unsigned int h, unsigned int w, char * data, size_t len, bool compressed)
{
	auto img = CreateImage(h, w);
	memcpy(img->data(), data, len);
	img->Compressed = compressed;
	img->Size = len;
	return img;
}
SL::Remote_Access_Library::Utilities::Image::Image(Image_Impl& impl) : _Height(impl.height), _Width(impl.width), Compressed(false), Size(impl.height*impl.width * 4) {
	_Data = Remote_Access_Library::INTERNAL::_ImageBuffer.AquireBuffer(impl.height*impl.width * 4);//stride is always 4 bytes
}
SL::Remote_Access_Library::Utilities::Image::~Image() {

	Remote_Access_Library::INTERNAL::_ImageBuffer.ReleaseBuffer(_Data);
}
void SL::Remote_Access_Library::Utilities::Image::compress(int quality)
{
	if (Compressed) return;//allready compressed

	auto set = TJSAMP_420;
	auto maxsize = tjBufSize(Width(), Height(), set);
	auto buf = Remote_Access_Library::INTERNAL::_ImageBuffer.AquireBuffer(maxsize);
	auto _jpegCompressor(tjInitCompress());
	auto start = std::chrono::high_resolution_clock::now();

	if (tjCompress2(_jpegCompressor, (unsigned char*)data(), Width(), 0, Height(), TJPF_BGRX, (unsigned char**)&buf.data, &maxsize, set, quality, TJFLAG_FASTDCT | TJFLAG_NOREALLOC) == -1) {
		std::cout << "Err msg " << tjGetErrorStr() << std::endl;
		Remote_Access_Library::INTERNAL::_ImageBuffer.ReleaseBuffer(buf);
	}
	else {		
		std::cout << "JEPG Compression  " << maxsize << " Took: " << std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - start).count() << std::endl;
		Remote_Access_Library::INTERNAL::_ImageBuffer.ReleaseBuffer(_Data);
		_Data = buf;
		Size = maxsize;
		Compressed = true;
	}
	tjDestroy(_jpegCompressor);
}
void SL::Remote_Access_Library::Utilities::Image::decompress()
{
	if (!Compressed) return;
	auto _jpegDecompressor(tjInitDecompress());
	
	int jpegSubsamp = 0;
	auto outwidth = 0;
	auto outheight = 0;
	if (tjDecompressHeader2(_jpegDecompressor, (unsigned char*)data(), size(), &outwidth, &outheight, &jpegSubsamp) >=0) {
		auto buf = Remote_Access_Library::INTERNAL::_ImageBuffer.AquireBuffer(outwidth *outheight* 4);
		if (tjDecompress(_jpegDecompressor, (unsigned char*)data(), size(), (unsigned char*)buf.data, outwidth, 0, outheight, TJPF_BGRX, TJFLAG_FASTDCT | TJFLAG_NOREALLOC) >= 0) {
			_Width = outwidth;
			_Height = outheight;
			Size = _Width * _Height*4;
			Compressed = false;
			Remote_Access_Library::INTERNAL::_ImageBuffer.ReleaseBuffer(_Data);
			_Data = buf;
		}
		else {
			Remote_Access_Library::INTERNAL::_ImageBuffer.ReleaseBuffer(buf);
		}
	}
	tjDestroy(_jpegDecompressor);
}
