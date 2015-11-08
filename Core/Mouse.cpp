#include "stdafx.h"
#include "Mouse.h"
#include <memory>

#if _WIN32

#define RAIIHDC(handle) std::unique_ptr<std::remove_pointer<HDC>::type, decltype(&::DeleteDC)>(handle, &::DeleteDC)
#define RAIIHBITMAP(handle) std::unique_ptr<std::remove_pointer<HBITMAP>::type, decltype(&::DeleteObject)>(handle, &::DeleteObject)

#define RAIIHANDLE(handle) std::unique_ptr<std::remove_pointer<HANDLE>::type, decltype(&::CloseHandle)>(handle, &::CloseHandle)
void SaveBMP(BITMAPINFOHEADER bi, char* imgdata, std::string dst) {
	BITMAPFILEHEADER   bmfHeader;
	// A file is created, this is where we will save the screen capture.
	auto hFile(RAIIHANDLE(CreateFileA(dst.c_str(),GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, NULL)));
	// Add the size of the headers to the size of the bitmap to get the total file size
	DWORD dwSizeofDIB = bi.biSizeImage + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	//Offset to where the actual bitmap bits start.
	bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);
	//Size of the file
	bmfHeader.bfSize = dwSizeofDIB;
	//bfType must always be BM for Bitmaps
	bmfHeader.bfType = 0x4D42; //BM   
	DWORD dwBytesWritten = 0;
	WriteFile(hFile.get(), (LPSTR)&bmfHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
	WriteFile(hFile.get(), (LPSTR)&bi, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);
	WriteFile(hFile.get(), (LPSTR)imgdata, bi.biSizeImage, &dwBytesWritten, NULL);
}


SL::Mouse_Capture::Mouse SL::Mouse_Capture::GetMouse()
{
	auto desktopdc = RAIIHDC(GetDC(NULL));
	if (desktopdc.get() == NULL) return Mouse();
	auto capturedc = RAIIHDC(CreateCompatibleDC(desktopdc.get()));
	if (capturedc.get() == NULL) return Mouse();
	auto capturebmp = RAIIHBITMAP(CreateCompatibleBitmap(desktopdc.get(), 32, 32));//32 x 32 is the biggest allowed for windows...
	if (capturebmp.get() == NULL)return Mouse();

	std::vector<char> retdata;
	auto originalBmp = SelectObject(capturedc.get(), capturebmp.get());

	CURSORINFO cursorInfo;
	cursorInfo.cbSize = sizeof(cursorInfo);
	GetCursorInfo(&cursorInfo);

	ICONINFO ii = { 0 };
	GetIconInfo(cursorInfo.hCursor, &ii);
	auto colorbmp = RAIIHBITMAP(ii.hbmColor);//make sure this is cleaned up properly
	auto maskbmp = RAIIHBITMAP(ii.hbmMask);//make sure this is cleaned up properly
	DrawIcon(capturedc.get(), 0,0, cursorInfo.hCursor);
	BITMAP bm;
	int height = 0;

	if (ii.hbmColor != NULL) {
		GetObject(colorbmp.get(), sizeof(bm), &bm);	
		height = bm.bmHeight;
	}
	else if (ii.hbmMask != NULL) {
		GetObject(maskbmp.get(), sizeof(bm), &bm);
		height = bm.bmHeight/2;
	}
	else return Mouse();
	int width = bm.bmWidth;

	BITMAPINFOHEADER   bi;
	memset(&bi, 0, sizeof(bi));

	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = width;
	bi.biHeight = -height;
	bi.biPlanes = 1;
	bi.biBitCount = 32;//always 32 bits damnit!!!
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;
	bi.biSizeImage = ((width * bi.biBitCount + 31) / 32) * 4 * height;
	retdata.resize(width*height * 4);

	GetDIBits(desktopdc.get(), capturebmp.get(), 0, (UINT)height, retdata.data(), (BITMAPINFO *)&bi, DIB_RGB_COLORS);

	SelectObject(capturedc.get(), originalBmp);

	//Sanity check to ensure the data is correct
	//SaveBMP(bi, retdata.data(), "c:\\users\\scott\\desktop\\one.bmp");

	Mouse_Info m;
	m.Hidden = cursorInfo.flags & CURSOR_SHOWING;
	m.left = cursorInfo.ptScreenPos.x - ii.xHotspot;
	m.top = cursorInfo.ptScreenPos.y - ii.yHotspot;
	m.height = height;
	m.width = width;
	return Mouse(m, retdata);
}


#else



#endif

