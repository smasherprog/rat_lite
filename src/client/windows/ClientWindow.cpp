#include "targetver.h"
#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

#include "ClientWindow.h"
#include "Clipboard_Lite.h"
#include "Input_Lite.h"
#include "RAT.h"
#include "ScreenCapture.h"
#include "resource.h"
#include <iostream>

const char WindowClass[] = "RAT_PROJECT";

namespace SL {
namespace RAT_Client {

    struct MonitorInfo {
        Screen_Capture::Monitor Monitor;
        std::shared_ptr<HBITMAP__> Bitmap;
    };
    class ClientWindowImpl {
      public:
        std::shared_ptr<Clipboard_Lite::IClipboard_Manager> Clipboard_Manager_;
        std::vector<MonitorInfo> Monitors;

        std::shared_ptr<RAT_Lite::IClientDriver> ClientDriver_;
        std::shared_ptr<SL::WS_LITE::IWSHub> IWSHub_;
        std::shared_ptr<SL::WS_LITE::IWSocket> Socket_;

        unsigned short Port;
        std::string PathToCert;
        bool ShareClip = false;

        HWND hWnd = nullptr;
        HWND ConnecthWnd = nullptr;
        HINSTANCE hInstance = nullptr;

        static INT_PTR CALLBACK DlgProc(HWND hWndDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
        {
            // static so it lasts for ever!!
            static char Hostname[256];
            switch (Msg) {
            case WM_INITDIALOG:
                return TRUE;
            case WM_COMMAND:
                switch (wParam) {
                case ID_TRYCONNTECT:
                    GetDlgItemTextA(hWndDlg, IDC_HOSTTEXTBOX, Hostname, sizeof(Hostname));
                    PostMessage(GetParent(hWndDlg), WM_COMMAND, ID_TRYCONNTECT, (LPARAM)Hostname);
                    return TRUE;
                case ID_CANCELCONNTECT:
                    PostMessage(GetParent(hWndDlg), WM_COMMAND, ID_TRYCONNTECTCLOSING, 0);
                    EndDialog(hWndDlg, 0);
                    return TRUE;
                }
                break;
            case WM_CLOSE:
            case WM_DESTROY:
                PostMessage(GetParent(hWndDlg), WM_COMMAND, ID_TRYCONNTECTCLOSING, 0);
                EndDialog(hWndDlg, 0);
                return TRUE;
            default:
                return FALSE;
            }
            return FALSE;
        }
        LRESULT WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
        {

            switch (uMsg) {
            case WM_COMMAND:
                switch (wParam) {
                case ID_TRYCONNTECT:
                    TryConnect(reinterpret_cast<const char *>(lParam));
                    break;
                case IDD_CONNECTTODIALOG:
                    if (!IsWindow(ConnecthWnd)) {
                        ConnecthWnd = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_CONNECTTO), hwnd, DlgProc);
                        ShowWindow(ConnecthWnd, SW_SHOW);
                    }
                    break;
                case ID_TRYCONNTECTCLOSING:
                    ConnecthWnd = nullptr;
                    if (IsWindowVisible(hwnd) == FALSE) {
                        PostQuitMessage(0);
                        break;
                    }
                    break;
                }
                break;
            case WM_MOUSEMOVE:
                ClientDriver_->SendMousePosition(RAT_Lite::Point(LOWORD(lParam), HIWORD(lParam)));
                break;
            case WM_LBUTTONDOWN:
                ClientDriver_->SendMouseDown(Input_Lite::MouseButtons::LEFT);
                break;
            case WM_LBUTTONUP:
                ClientDriver_->SendMouseUp(Input_Lite::MouseButtons::LEFT);
                break;
            case WM_RBUTTONDOWN:
                ClientDriver_->SendMouseDown(Input_Lite::MouseButtons::RIGHT);
                break;
            case WM_RBUTTONUP:
                ClientDriver_->SendMouseUp(Input_Lite::MouseButtons::RIGHT);
                break;
            case WM_MBUTTONDOWN:
                ClientDriver_->SendMouseDown(Input_Lite::MouseButtons::MIDDLE);
                break;
            case WM_MBUTTONUP:
                ClientDriver_->SendMouseUp(Input_Lite::MouseButtons::MIDDLE);
                break;
            case WM_MOUSEWHEEL:
                ClientDriver_->SendMouseScroll(GET_WHEEL_DELTA_WPARAM(wParam) / 120);
                break;
            case WM_KEYUP:
                ClientDriver_->SendKeyUp(SL::Input_Lite::ConvertToKeyCode(wParam));
                break;
            case WM_KEYDOWN:
                ClientDriver_->SendKeyDown(SL::Input_Lite::ConvertToKeyCode(wParam));
                break;
            case WM_PAINT: {
                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hwnd, &ps);

                for (size_t i = 0; i < Monitors.size(); i++) {
                    if (!Monitors[i].Bitmap)
                        continue;
                    auto b = Monitors[i].Bitmap.get();
                    auto hdcMem = CreateCompatibleDC(hdc);
                    auto oldBitmap = SelectObject(hdcMem, b);
                    BITMAP bitmap;

                    GetObject(b, sizeof(bitmap), &bitmap);
                    BitBlt(hdc, Monitors[i].Monitor.OffsetX, Monitors[i].Monitor.OffsetY, bitmap.bmWidth, bitmap.bmHeight, hdcMem, 0, 0, SRCCOPY);

                    SelectObject(hdcMem, oldBitmap);
                    DeleteDC(hdcMem);
                }
                EndPaint(hwnd, &ps);
            } break;
            case WM_DESTROY:
                PostQuitMessage(0);
                break;
            default:
                return DefWindowProc(hwnd, uMsg, wParam, lParam);
            }
            return 0;
        }
        static LRESULT CALLBACK WindowProcStatic(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
        {

            if (uMsg == WM_NCCREATE) {
                auto cs = (CREATESTRUCT *)lParam;
                auto p = (ClientWindowImpl *)cs->lpCreateParams;
                SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)p);
                return TRUE;
            }
            else {
                auto thisclass = (ClientWindowImpl *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
                if (thisclass) {
                    return thisclass->WindowProc(hwnd, uMsg, wParam, lParam);
                }
            }
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
        ClientWindowImpl(std::string host, unsigned short port, std::string pathtocertificate)
        {

            hInstance = GetModuleHandle(NULL);
            WNDCLASSEXA wcex;
            wcex.cbSize = sizeof(wcex);

            wcex.style = CS_HREDRAW | CS_VREDRAW;
            wcex.lpfnWndProc = WindowProcStatic;
            wcex.cbClsExtra = 0;
            wcex.cbWndExtra = 0;
            wcex.hInstance = hInstance;
            wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PROJECTICON));
            wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
            wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
            wcex.lpszMenuName = 0;
            wcex.lpszClassName = WindowClass;
            wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_PROJECTICON));

            RegisterClassExA(&wcex);
            hWnd =
                CreateWindowA(WindowClass, WindowTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, this);
            if (hWnd) {
                if (host.empty() || host.size() < 2) {
                    PostMessage(hWnd, WM_COMMAND, IDD_CONNECTTODIALOG, 0);
                }
                else {
                    TryConnect(host.c_str());
                }
            }
            Clipboard_Manager_ = Clipboard_Lite::CreateClipboard()->onText([&](const std::string &text) {})->run();
        }
        virtual ~ClientWindowImpl() { Clipboard_Manager_.reset(); }
        void Run()
        {

            MSG msg;
            BOOL bRet;
            while ((bRet = GetMessage(&msg, 0, 0, 0)) != 0) {
                if (bRet == -1) {
                    break; // get out of the loop
                }
                else {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
            }
        }
        void CleanUp()
        {
            Monitors.clear(); // free memory
            if (Socket_) {
                Socket_->close();
            }
            Socket_.reset();
            ClientDriver_.reset();
            IWSHub_.reset();
            Clipboard_Manager_.reset();
        }
        void TryConnect(const char *hostname)
        {
            CleanUp();
            auto clientctx = SL::WS_LITE::CreateContext(SL::WS_LITE::ThreadCount(1))->NoTLS()->CreateClient();
            ClientDriver_ =
                RAT_Lite::CreateClientDriverConfiguration()
                    ->onConnection([&](const std::shared_ptr<SL::WS_LITE::IWSocket> &socket) {
                        Socket_ = socket;
                        // make sure to show the window
                        ShowWindow(hWnd, SW_SHOW);
                        UpdateWindow(hWnd);
                        PostMessage(ConnecthWnd, WM_CLOSE, 0, 0);
                        ConnecthWnd = nullptr;
                        std::cout << "onConnection" << std::endl;
                    })
                    ->onMessage([&](const std::shared_ptr<SL::WS_LITE::IWSocket> &socket, const WS_LITE::WSMessage &msg) {
                        std::cout << "onMessage" << std::endl;
                    })
                    ->onDisconnection([&](const std::shared_ptr<SL::WS_LITE::IWSocket> &socket, unsigned short code, const std::string &msg) {
                        ShowWindow(hWnd, SW_HIDE);
                        UpdateWindow(hWnd);
                        PostMessage(hWnd, WM_COMMAND, IDD_CONNECTTODIALOG, 0);
                        std::cout << "Client disconnected, reason: " << msg << std::endl;
                    })
                    ->onMonitorsChanged([&](const std::vector<Screen_Capture::Monitor> &monitors) {
                        Monitors.clear(); // free memory
                        Monitors.resize(monitors.size());
                        for (size_t i = 0; i < Monitors.size(); i++) {
                            Monitors[i].Monitor = monitors[i];
                        }
                        std::cout << "onMonitorsChanged" << std::endl;
                    })
                    ->onFrameChanged([&](const RAT_Lite::Image &img, const SL::Screen_Capture::Monitor &monitor) {
                        // std::cout << "onFrameChanged" << std::endl;
                        HDC hdc = GetDC(hWnd);

                        BITMAPINFO info;
                        ZeroMemory(&info, sizeof(BITMAPINFO));
                        info.bmiHeader.biBitCount = 32;
                        info.bmiHeader.biWidth = img.Rect_.Width;
                        info.bmiHeader.biHeight = -img.Rect_.Height;
                        info.bmiHeader.biPlanes = 1;
                        info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
                        info.bmiHeader.biSizeImage = img.Length;
                        info.bmiHeader.biCompression = BI_RGB;

                        auto mon = std::find_if(begin(Monitors), end(Monitors), [&](const MonitorInfo &m) { return monitor.Id == m.Monitor.Id; });
                        if (mon != end(Monitors)) {

                            auto bitmaptodraw = CreateDIBitmap(hdc, &info.bmiHeader, CBM_INIT, (void *)img.Data, &info, DIB_RGB_COLORS);

                            auto srcmem = CreateCompatibleDC(hdc);
                            auto srcbitmap = SelectObject(srcmem, bitmaptodraw);

                            auto hdcMem = CreateCompatibleDC(hdc);
                            auto oldBitmap = SelectObject(hdcMem, mon->Bitmap.get());

                            BitBlt(hdcMem, img.Rect_.left(), img.Rect_.top(), img.Rect_.Width, img.Rect_.Height, srcmem, 0, 0, SRCCOPY);

                            SelectObject(srcmem, srcbitmap);
                            DeleteObject(bitmaptodraw);

                            SelectObject(hdcMem, oldBitmap);
                            DeleteDC(hdcMem);
                            RECT RECT_ImageUpdate_Window;
                            RECT_ImageUpdate_Window.left = img.Rect_.left() + mon->Monitor.OffsetX;
                            RECT_ImageUpdate_Window.top = img.Rect_.top() + mon->Monitor.OffsetY;
                            RECT_ImageUpdate_Window.bottom = img.Rect_.bottom() + mon->Monitor.OffsetY;
                            RECT_ImageUpdate_Window.right = img.Rect_.right() + mon->Monitor.OffsetX;

                            InvalidateRect(hWnd, &RECT_ImageUpdate_Window, FALSE);
                            UpdateWindow(hWnd);
                        }

                        // StretchDIBits(hdc, img.Rect_.left(), img.Rect_.top(), img.Rect_.Width, img.Rect_.Height, 0, 0, img.Rect_.Width,
                        // img.Rect_.Height, img.Data, &info, DIB_RGB_COLORS, SRCCOPY);
                        ReleaseDC(hWnd, hdc);
                    })
                    ->onNewFrame([&](const RAT_Lite::Image &img, const SL::Screen_Capture::Monitor &monitor) {
                        std::cout << "onNewFrame" << std::endl;
                        HDC hdc = GetDC(hWnd);

                        BITMAPINFO info;
                        ZeroMemory(&info, sizeof(BITMAPINFO));
                        info.bmiHeader.biBitCount = 32;
                        info.bmiHeader.biWidth = img.Rect_.Width;
                        info.bmiHeader.biHeight = -img.Rect_.Height;
                        info.bmiHeader.biPlanes = 1;
                        info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
                        info.bmiHeader.biSizeImage = img.Length;
                        info.bmiHeader.biCompression = BI_RGB;
                        auto mon = std::find_if(begin(Monitors), end(Monitors), [&](const MonitorInfo &m) { return monitor.Id == m.Monitor.Id; });
                        if (mon != end(Monitors)) {
                            mon->Bitmap =
                                std::shared_ptr<HBITMAP__>(CreateDIBitmap(hdc, &info.bmiHeader, CBM_INIT, (void *)img.Data, &info, DIB_RGB_COLORS),
                                                           [](HBITMAP__ *p) { DeleteObject(p); });
                            RECT RECT_ImageUpdate_Window;
                            RECT_ImageUpdate_Window.left = img.Rect_.left() + mon->Monitor.OffsetX;
                            RECT_ImageUpdate_Window.top = img.Rect_.top() + mon->Monitor.OffsetY;
                            RECT_ImageUpdate_Window.bottom = img.Rect_.bottom() + mon->Monitor.OffsetY;
                            RECT_ImageUpdate_Window.right = img.Rect_.right() + mon->Monitor.OffsetX;
                            InvalidateRect(hWnd, &RECT_ImageUpdate_Window, FALSE);
                            UpdateWindow(hWnd);
                        }
                        ReleaseDC(hWnd, hdc);
                    })
                    ->Build(clientctx);
            ClientDriver_->ShareClipboard(ShareClip);
            IWSHub_ = clientctx->connect(hostname, Port);
        }
    };

    ClientWindow::ClientWindow(std::string host, unsigned short port, std::string pathtocertificate)
    {
        ClientWindowImpl_ = new ClientWindowImpl(host, port, pathtocertificate);
    }
    ClientWindow::~ClientWindow() { delete ClientWindowImpl_; }
    void ClientWindow::ShareClipboard(bool share)
    {
        ClientWindowImpl_->ShareClip = share;
        if (ClientWindowImpl_->ClientDriver_) {
            ClientWindowImpl_->ClientDriver_->ShareClipboard(share);
        }
    }
    bool ClientWindow::ShareClipboard() const { return ClientWindowImpl_->ShareClip; }
    void ClientWindow::Run() { ClientWindowImpl_->Run(); }
} // namespace RAT_Client
} // namespace SL
