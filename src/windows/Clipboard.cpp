#include "Clipboard.h"
#include "Clipboard_Lite.h"
#include <string>
#include <functional>
#include <memory>
#include <iostream>
#include <assert.h>

namespace SL {
    namespace Clipboard_Lite {
        Clipboard_ManagerImpl::Clipboard_ManagerImpl() {
            Copying = false;
        }
        Clipboard_ManagerImpl::~Clipboard_ManagerImpl() {
            if (Hwnd) {
                PostMessage(Hwnd, WM_QUIT, 0, 0);
            }
            if (BackGroundWorker.joinable()) {
                BackGroundWorker.join();
            }
        }

        void Clipboard_ManagerImpl::LoadClipImage() {
            auto c = ClipWrapper(Hwnd);
            if (!c)  return;
            if (IsClipboardFormatAvailable(CF_BITMAP) || IsClipboardFormatAvailable(CF_DIB) || IsClipboardFormatAvailable(CF_DIBV5))
            {
                HANDLE hClipboard = (BITMAPINFO*)GetClipboardData(CF_DIB);
                if (hClipboard)
                {
                    GlobalLockWrapper dib(GlobalLock(hClipboard));
                    if (dib)
                    {
                        auto info = reinterpret_cast<LPBITMAPINFO>(dib.Ptr);
                        Image img;
                        img.Width = info->bmiHeader.biWidth;
                        img.Height = info->bmiHeader.biHeight;
                        img.PixelStride = info->bmiHeader.biBitCount / 8;

                        if ((info->bmiHeader.biBitCount == 24 || info->bmiHeader.biBitCount == 32) &&
                            info->bmiHeader.biCompression == BI_RGB && info->bmiHeader.biClrUsed == 0) {

                            img.Data = std::shared_ptr<unsigned char>(new unsigned char[info->bmiHeader.biSizeImage], [](auto p) {if (p) delete[] p; });
                            memcpy(img.Data.get(), (info + 1), info->bmiHeader.biSizeImage);

                            auto linewidth = 0;
                            auto depth = info->bmiHeader.biBitCount / 8;
                            if (depth == 3) linewidth = 4 * ((3 * img.Width + 3) / 4);
                            else linewidth = 4 * img.Width;

                            auto *p = img.Data.get();

                            for (int i = img.Height - 1; i >= 0; i--) {
                                auto r = img.Data.get() + (img.Width*img.PixelStride*i);
                                for (int j = 0; j < img.Width; j++) {
                                    auto bb = *r++;
                                    auto gg = *r++;
                                    auto rr = *r++;
                                    *p++ = rr;
                                    *p++ = gg;
                                    *p++ = bb;
                                    if (img.PixelStride == 4) *p++ = *r++;
                                }
                            }

                        }
                        else {
                            HDCWrapper dc(GetDC(NULL));
                            HDCWrapper capturedc(CreateCompatibleDC(dc.DC));
                            HBITMAPWrapper bitmap(CreateCompatibleBitmap(dc.DC, img.Width, img.Height));

                            auto originalBmp = SelectObject(capturedc.DC, bitmap.Bitmap);

                            void *pDIBBits = (void*)(info->bmiColors);
                            if (info->bmiHeader.biCompression == BI_BITFIELDS) pDIBBits = (void*)(info->bmiColors + 3);
                            else if (info->bmiHeader.biClrUsed > 0) pDIBBits = (void*)(info->bmiColors + info->bmiHeader.biClrUsed);

                            SetDIBitsToDevice(capturedc.DC, 0, 0, img.Width, img.Height, 0, 0, 0, img.Height, pDIBBits, info, DIB_RGB_COLORS);

                            BITMAPINFOHEADER bi;
                            memset(&bi, 0, sizeof(bi));

                            bi.biSize = sizeof(BITMAPINFOHEADER);
                            bi.biWidth = img.Width;
                            bi.biHeight = -img.Height;
                            bi.biPlanes = 1;
                            bi.biBitCount = static_cast<WORD>(img.PixelStride * 8);
                            bi.biCompression = BI_RGB;
                            bi.biSizeImage = ((img.Width * bi.biBitCount + 31) / (img.PixelStride * 8)) * img.PixelStride * img.Height;
                            img.Data = std::shared_ptr<unsigned char>(new unsigned char[bi.biSizeImage], [](auto p) {if (p) delete[] p; });

                            GetDIBits(capturedc.DC, bitmap.Bitmap, 0, (UINT)img.Height, img.Data.get(), (BITMAPINFO*)&bi, DIB_RGB_COLORS);

                            SelectObject(capturedc.DC, originalBmp);

                            auto *p = img.Data.get();

                            for (int i = 0; i < img.Height; i++) {
                                auto r = img.Data.get() + (img.Width*img.PixelStride*i);
                                for (int j = 0; j < img.Width; j++) {
                                    auto bb = *r++;
                                    auto gg = *r++;
                                    auto rr = *r++;
                                    *p++ = rr; // we want RGB
                                    *p++ = gg;
                                    *p++ = bb;
                                    if (img.PixelStride == 4) *p++ = *r++;
                                }
                            }
                        }
                        onImage(img);
                    }

                }
            }
        }
        void Clipboard_ManagerImpl::LoadClipText() {
            auto c = ClipWrapper(Hwnd);
            if (!c) return;
            if (IsClipboardFormatAvailable(CF_TEXT)) {
                auto h = ::GetClipboardData(CF_TEXT);
                if (h) {
                    auto pData = GlobalLock(h);
                    auto nLength = GlobalSize(h);
                    if (pData && nLength > 0) {
                        std::string buffer;
                        buffer.resize(nLength);
                        memcpy((void*)buffer.data(), pData, nLength);
                        GlobalUnlock(h);
                        onText(buffer);
                    }
                }
            }
        }
        void Clipboard_ManagerImpl::run() {
            BackGroundWorker = std::thread([&] {
                WNDCLASSEX wndclass = {};
                memset(&wndclass, 0, sizeof(wndclass));
                wndclass.cbSize = sizeof(WNDCLASSEX);
                wndclass.lpfnWndProc = DefWindowProc;
                wndclass.lpszClassName = _T("myclass");
                if (RegisterClassEx(&wndclass))
                {
                    Hwnd = CreateWindowEx(0, wndclass.lpszClassName, _T("clipwatcher"), 0, 0, 0, 0, 0, HWND_MESSAGE, 0, 0, 0);
                    if (Hwnd) {
                        if (AddClipboardFormatListener(Hwnd)) {
                            MSG msg;
                            while (GetMessage(&msg, Hwnd, 0, 0) != 0) {
                                if (msg.message == WM_CLIPBOARDUPDATE) {
                                    if (!Copying) {
                                        if (onText) {
                                            LoadClipText();
                                        }
                                        if (onImage) {
                                            LoadClipImage();
                                        }
                                    }
                                    Copying = false;
                                }
                                else
                                {
                                    TranslateMessage(&msg);
                                    DispatchMessage(&msg);
                                }
                            }
                            RemoveClipboardFormatListener(Hwnd);
                        }
                        UnregisterClass(wndclass.lpszClassName, nullptr);
                    }
                }
            });
        }

        void Clipboard_ManagerImpl::copy(const std::string& text) {
            Copying = true;
            RestoreClip(text, CF_TEXT);
        }

        void Clipboard_ManagerImpl::copy(const Image& image) {
            Copying = true;
            BITMAPINFOHEADER   bmih;
            memset(&bmih, 0, sizeof(BITMAPINFOHEADER));

            bmih.biWidth = image.Width;
            bmih.biHeight = -image.Height;
            bmih.biBitCount = image.PixelStride*8;
            bmih.biCompression = BI_RGB;
            bmih.biSize = sizeof(BITMAPINFOHEADER);
            bmih.biPlanes = 1;

            BITMAPINFO dbmi;
            ZeroMemory(&dbmi, sizeof(dbmi));
            dbmi.bmiHeader = bmih;
            dbmi.bmiColors->rgbBlue = 0;
            dbmi.bmiColors->rgbGreen = 0;
            dbmi.bmiColors->rgbRed = 0;
            dbmi.bmiColors->rgbReserved = 0;

            HDC hdc = ::GetDC(NULL);
            CreateDIBitmap(hdc, &bmih, CBM_INIT, image.Data.get(), &dbmi, DIB_RGB_COLORS);
            ::ReleaseDC(NULL, hdc);

            //HANDLE hData = GlobalAlloc(GHND | GMEM_SHARE,  sizeof(BITMAPINFO) + m_bmi.bmiHeader.biWidth*m_bmi.bmiHeader.biHeight * 3);
            //LPVOID pData = (LPVOID)GlobalLock(hData);
            //memcpy(pData, &m_bmi, sizeof(BITMAPINFO));
            //memcpy((UCHAR *)pData + sizeof(BITMAPINFO), (LPVOID)m_pUTDispData, m_bmi.bmiHeader.biWidth*m_bmi.bmiHeader.biHeight * 3);
            //GlobalUnlock(hData);
        
            //// cleanup
            //DeleteObject(hbmp);
            //if (OpenClipboard(Hwnd) == TRUE) {

            //    if (EmptyClipboard() == TRUE && (image.Height > 0 && image.Width >0)) {
            //        auto hData = GlobalAlloc(GMEM_MOVEABLE, buffer.size());
            //        if (hData) {
            //            auto pData = GlobalLock(hData);
            //            if (pData) {
            //                memcpy(pData, buffer.data(), buffer.size());
            //                GlobalUnlock(hData);
            //                if (::SetClipboardData(CF_BITMAP, hData)) {
            //                    //clipboard takes ownership of the memory
            //                    hData = nullptr;
            //                }
            //            }
            //        }
            //        if (hData) {
            //            GlobalFree(hData);
            //        }
            //    }
            //    CloseClipboard();

            //}
        }
    };
}