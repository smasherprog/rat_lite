#pragma once
#include <string>
#include <functional>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
#include <thread>
#include <mutex>
#include <atomic>
#include <vector>

namespace SL {
    namespace Clipboard_Lite {
        struct Image;

        //HELPERS
        class ClipWrapper {
            HWND Hwnd;
            BOOL Opened = FALSE;
        public:
            ClipWrapper(HWND h) : Hwnd(h) {
                Opened = OpenClipboard(Hwnd);
            }
            operator bool() {
                return Opened == TRUE;
            }
            ~ClipWrapper() {
                if (Opened == TRUE) {
                    CloseClipboard();
                }
            }
        };
        class GlobalLockWrapper {
        public:
            GlobalLockWrapper(void* ptr) : Ptr(ptr) {}
            ~GlobalLockWrapper() { if (Ptr != nullptr) { GlobalUnlock(Ptr); } }
            operator bool() {
                return Ptr != nullptr;
            }
            void* Ptr;
        };
        class HDCWrapper {
        public:
            HDCWrapper(HDC d) : DC(d) {}
            ~HDCWrapper() { if (DC != nullptr) { DeleteDC(DC); } }
            operator bool() {
                return DC != nullptr;
            }
            HDC DC;
        };
        class HBITMAPWrapper {
        public:
            HBITMAPWrapper(HBITMAP b) : Bitmap(b) {}
            ~HBITMAPWrapper() { if (Bitmap != nullptr) { DeleteObject(Bitmap); } }
            operator bool() {
                return Bitmap != nullptr;
            }
            HBITMAP Bitmap;
        };

        class Clipboard_ManagerImpl {
            std::thread BackGroundWorker;
            std::atomic<bool> Copying;
            HWND Hwnd = nullptr;

            void LoadClipImage();
            void LoadClipText();

        public:

            std::function<void(const std::string& text)> onText;
            std::function<void(const Image& image)> onImage;

            Clipboard_ManagerImpl();
            ~Clipboard_ManagerImpl();

            void run();
            void copy(const std::string& test);
            void copy(const Image& image);
          
    
            template<class T>void RestoreClip(const T& buffer, UINT format) {
                if (OpenClipboard(Hwnd) == TRUE) {
                    if (EmptyClipboard() == TRUE && buffer.size() > 0) {
                        auto hData = GlobalAlloc(GMEM_MOVEABLE, buffer.size());
                        if (hData) {
                            auto pData = GlobalLock(hData);
                            if (pData) {
                                memcpy(pData, buffer.data(), buffer.size());
                                GlobalUnlock(hData);
                                if (::SetClipboardData(format, hData)) {
                                    //clipboard takes ownership of the memory
                                    hData = nullptr;
                                }
                            }
                        }
                        if (hData) {
                            GlobalFree(hData);
                        }
                    }
                    CloseClipboard();
                }
            }
        };
    }
}