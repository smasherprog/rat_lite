#pragma once
#include <atomic>
#include <functional>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
typedef unsigned char uchar;

namespace SL
{
namespace Clipboard_Lite
{
    struct Image;

    class Clipboard_ManagerImpl
    {
        enum ClipboardDataType { NONE, TEXT, IMAGE };
        std::thread BackGroundWorker;
        std::atomic<bool> Pasting;
        Display* Display_;
        Window Window_ = 0;

        int xfixes_event_base = 0;

        Atom TARGETS;
        Atom CLIPBOARD;
        Atom TIMESTAMP;
        Atom PRIMARY_TIMESTAMP;
        Atom CLIPBOARD_TIMESTAMP;
        Atom Xatextplainutf;
        Atom Xatextplainutf2; // STR#2930
        Atom Xatextplain;
        Atom XaText;
        Atom XaCompoundText;
        Atom XaUtf8String;
        Atom XaTextUriList;
        Atom XaImageBmp;
        Atom XaImagePNG;
        Atom INCR;

        int atom_bits = 32;
        Time LastTImestamp = CurrentTime;
        bool OwnsClipboard = false;

        std::vector<unsigned char> Data;
        ClipboardDataType ClipboardDataType_ = ClipboardDataType::NONE;

        long getIncrData(const XSelectionEvent& selevent, long lower_bound);
        bool getNextEvent(XEvent* event_return);

        void handle(const XEvent& thisevent);

        void handle_clipboard_timestamp(Time time);
        // this is called by the OS to notify us to clear any data we have on the clipboard
        void handle_SelectionClear(const XEvent& e);
        // this is called by the OS to notify us of possible data available.
        void handle_SelectionNotify(const XEvent& e);
        void handle_SelectionRequest(const XEvent& ex);
        void handle_ClipboardType(const XEvent& e, const Atom types[], unsigned long count);

    public:
        std::function<void(const std::string& text)> onText;
        std::function<void(const Image& image)> onImage;

        Clipboard_ManagerImpl();
        ~Clipboard_ManagerImpl();
        void run();
        void copy(const std::string& text);
    };
}
}