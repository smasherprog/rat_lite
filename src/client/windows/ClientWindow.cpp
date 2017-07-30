#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#
#include "resource.h"
#include "../ClientWindow.h"
#include "Configs.h"

// Global Variables:
HINSTANCE hInst;                                // current instance


const char WindowClass[] = "RAT_PROJECT";

namespace SL {
    namespace RAT_Client {
        class ClientWindowImpl {
            HWND hWnd = nullptr;
            HINSTANCE hInstance = nullptr;
            std::shared_ptr<SL::RAT::Client_Config> Config;
        public:

            LRESULT WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

                switch (uMsg)
                {
             
                case WM_PAINT:
                {
                    PAINTSTRUCT ps;
                    HDC hdc = BeginPaint(hwnd, &ps);
                    // TODO: Add any drawing code that uses hdc here...
                    EndPaint(hwnd, &ps);
                }
                break;
                case WM_DESTROY:
                    PostQuitMessage(0);
                    break;
                default:
                    return DefWindowProc(hwnd, uMsg, wParam, lParam);
                }
                return 0;


            }
            static LRESULT CALLBACK WindowProcStatic(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

                if (uMsg == WM_NCCREATE)
                {
                    auto cs = (CREATESTRUCT*)lParam;
                    auto p = (ClientWindowImpl*)cs->lpCreateParams;
                    SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)p);
                    return TRUE;
                }
                else
                {
                    auto thisclass = (ClientWindowImpl*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
                    if (thisclass) {
                        return thisclass->WindowProc(hwnd, uMsg, wParam, lParam);
                    }
                }
                return DefWindowProc(hwnd, uMsg, wParam, lParam);
            }
      
            ClientWindowImpl(std::shared_ptr<SL::RAT::Client_Config>& config, const std::string& host) :Config(config) {
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
                HWND hWnd = CreateWindowA(WindowClass, WindowTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, this);
                if (hWnd)
                {
                    ShowWindow(hWnd, SW_SHOW);
                    UpdateWindow(hWnd);
                }
            }
            void Run() {

                MSG msg;
                BOOL bRet;
                while ((bRet = GetMessage(&msg, hWnd, 0, 0)) != 0)
                {
                    if (bRet == -1)
                    {
                        // handle the error and possibly exit
                    }
                    else
                    {
                        TranslateMessage(&msg);
                        DispatchMessage(&msg);
                    }
                }
            }


        };


        ClientWindow::ClientWindow(std::shared_ptr<SL::RAT::Client_Config>& config, const std::string& host) {
            ClientWindowImpl_ = new ClientWindowImpl(config, host);
        }
        ClientWindow::~ClientWindow() {
            delete ClientWindowImpl_;
        }

        void ClientWindow::Run() {
            ClientWindowImpl_->Run();
        }
    }
}
