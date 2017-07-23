#include "Clipboard.h"

#include <string>
#include <string.h>
#include <functional>
#include <assert.h>
#include <iostream>
#include <chrono>

#include <X11/extensions/Xfixes.h>

using namespace std::chrono_literals;

namespace SL
{
    namespace Clipboard_Lite
    {

        Clipboard_ManagerImpl::Clipboard_ManagerImpl()
        {

            Pasting = false;
            auto d = Display_ = XOpenDisplay(NULL);
            assert(Display_);
            Window_ = XCreateWindow(Display_,
                RootWindow(Display_, XDefaultScreen(Display_)),
                10,
                10,
                200,
                200,
                0,
                CopyFromParent,
                InputOnly,
                0,
                0,
                NULL);
            assert(Window_);

            TARGETS = XInternAtom(d, "TARGETS", 0);
            CLIPBOARD = XInternAtom(d, "CLIPBOARD", 0);
            TIMESTAMP = XInternAtom(d, "TIMESTAMP", 0);
            CLIPBOARD_TIMESTAMP = XInternAtom(d, "CLIPBOARD_TIMESTAMP", 0);


            Xatextplainutf = XInternAtom(d, "text/plain;charset=UTF-8", 0);
            Xatextplainutf2 = XInternAtom(d, "text/plain;charset=utf-8", 0);	// Firefox/Thunderbird needs this - See STR#2930
            Xatextplain = XInternAtom(d, "text/plain", 0);
            XaText = XInternAtom(d, "TEXT", 0);
            XaCompoundText = XInternAtom(d, "COMPOUND_TEXT", 0);
            XaUtf8String = XInternAtom(d, "UTF8_STRING", 0);
            XaTextUriList = XInternAtom(d, "text/uri-list", 0);
            XaImageBmp = XInternAtom(d, "image/bmp", 0);
            XaImagePNG = XInternAtom(d, "image/png", 0);
            INCR = XInternAtom(d, "INCR", 0);

            if (sizeof(Atom) < 4)
                atom_bits = sizeof(Atom) * 8;

            int error_base;
            if (XFixesQueryExtension(Display_, &xfixes_event_base, &error_base)) {
                XFixesSelectSelectionInput(Display_, Window_, CLIPBOARD, XFixesSetSelectionOwnerNotifyMask);
                std::cout << "have_xfixes Supported!" << std::endl;
            }
            else {
                assert(false);
                std::cout << "XFixes is required for this library to function properly!" << std::endl;
            }
        }
        Clipboard_ManagerImpl::~Clipboard_ManagerImpl()
        {
            if (Display_ && Window_) {
                XDestroyWindowEvent e = { 0 };
                e.display = Display_;
                e.send_event = false;
                e.type = DestroyNotify;
                e.window = Window_;
                XSendEvent(Display_, Window_, 0, 0, (XEvent *)&e);
            }
            if (BackGroundWorker.joinable()) {
                BackGroundWorker.join();
            }
            if (Display_) {
                XDestroyWindow(Display_, Window_);
                if (Window_) {
                    XCloseDisplay(Display_);
                }
            }
        }

        void Clipboard_ManagerImpl::handle_clipboard_timestamp(Time time)
        {
            if (time > LastTImestamp) {
                LastTImestamp = time;
                std::cout << "CLIPBOARD CHANGED " << std::endl;
                if (!OwnsClipboard) {
                    //ask for an update on the clipboard
                    XConvertSelection(Display_, CLIPBOARD, TARGETS, CLIPBOARD, Window_, LastTImestamp);
                }
                else if (onText) {
                    //I own the clipboard so just return the data locally
                    std::string str(reinterpret_cast<char*>(Data.data()), Data.size());
                    onText(str);
                }
                else {

                }
            }
        }

        bool Clipboard_ManagerImpl::getNextEvent(XEvent *event_return)
        {
            auto start = std::chrono::high_resolution_clock::now();
            while (!XPending(Display_))
            {
                if (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - start).count() > 10)
                {
                    return false;
                }
            }
            XNextEvent(Display_, event_return);
            return true;
        }
        long Clipboard_ManagerImpl::getIncrData(const XSelectionEvent& selevent, long lower_bound)
        {

            //fprintf(stderr,"Incremental transfer starting due to INCR property\n");
            size_t total = 0;
            XEvent event;
            XDeleteProperty(Display_, selevent.requestor, selevent.property);
            Data.resize(lower_bound);
            for (;;)
            {
                if (!getNextEvent(&event)) break;
                if (event.type == PropertyNotify)
                {
                    if (event.xproperty.state != PropertyNewValue) continue;
                    Atom actual_type;
                    int actual_format;
                    unsigned long nitems;
                    unsigned long bytes_after;
                    unsigned char* prop = 0;
                    long offset = 0;
                    size_t num_bytes;
                    //size_t slice_size = 0;
                    do
                    {
                        XGetWindowProperty(Display_, selevent.requestor, selevent.property, offset, 70000, True,
                            AnyPropertyType, &actual_type, &actual_format, &nitems, &bytes_after, &prop);
                        num_bytes = nitems * (actual_format / 8);
                        offset += num_bytes / 4;
                        //slice_size += num_bytes;
                        if (total + num_bytes > (size_t)lower_bound) {
                            Data.resize(total + num_bytes);
                        }
                        memcpy(Data.data() + total, prop, num_bytes); total += num_bytes;
                        if (prop) XFree(prop);
                    } while (bytes_after != 0);
                    //fprintf(stderr,"INCR data size:%ld\n", slice_size);
                    if (num_bytes == 0) break;
                }
                else break;
            }
            XDeleteProperty(Display_, selevent.requestor, selevent.property);
            return (long)total;
        }
        // replace \r\n by \n
        void convert_crlf(char *string, long& len) {
            char *a, *b;
            a = b = string;
            while (*a) {
                if (*a == '\r' && a[1] == '\n') { a++; len--; }
                else *b++ = *a++;
            }
            *b = 0;
        }
        void Clipboard_ManagerImpl::handle_SelectionClear(const XEvent& e) {
            OwnsClipboard = !(e.xselectionclear.selection == CLIPBOARD);
            if (OwnsClipboard) {
                std::cout << "OwnsClipboard" << std::endl;
            }
            else {
                std::cout << "!OwnsClipboard" << std::endl;
            }
            Data.clear();
        }
        //helper class to cleanup memory
        class XFreeBuffer {
        public:
            unsigned char* p = nullptr;
            ~XFreeBuffer() {
                if (p) XFree(p);
            }
        };
        void Clipboard_ManagerImpl::handle_ClipboardType(const XEvent& e, const Atom types[], unsigned long count) {
            ClipboardDataType_ = ClipboardDataType::NONE;
            for (unsigned long i = 0; i < count; i++) {
                auto t = types[i];
                if (t == XaImageBmp ||
                    t == XaImagePNG) {

                    ClipboardDataType_ = ClipboardDataType::IMAGE;
                    XConvertSelection(Display_, e.xselection.property, t, e.xselection.property, Window_, LastTImestamp);
                    return;
                }
                else if (t == Xatextplainutf ||
                    t == Xatextplainutf2 ||
                    t == Xatextplain ||
                    t == XaUtf8String ||
                    t == XaText ||
                    t == XaTextUriList ||
                    t == XaCompoundText) {
                    ClipboardDataType_ = ClipboardDataType::TEXT;
                    XConvertSelection(Display_, e.xselection.property, t, e.xselection.property, Window_, LastTImestamp);
                    return;
                }

            }
        }
        void Clipboard_ManagerImpl::handle_SelectionNotify(const XEvent& e) {
            //clear any previous data
            Data.clear();
            long bytesread = 0;
            if (e.xselection.property != None) {

                unsigned long  remaining = 0;
                do {
                    Atom actual = 0;
                    int format = 0;
                    unsigned long count = 0;
                    XFreeBuffer xbuf;
                    if (XGetWindowProperty(Display_,
                        e.xselection.requestor,
                        e.xselection.property,
                        bytesread / 4, 65536, True, AnyPropertyType,
                        &actual, &format, &count, &remaining,
                        &xbuf.p)) {
                        break; // quit on error
                    }
                    if (e.xselection.property == CLIPBOARD_TIMESTAMP && xbuf.p && format == 32 && count == 1) {
                        return handle_clipboard_timestamp(*reinterpret_cast<Time*>(xbuf.p));
                    }
                    else if (actual == TARGETS || actual == XA_ATOM) {
                        return handle_ClipboardType(e, reinterpret_cast<Atom*>(xbuf.p), count);
                    }
                    else if (actual == INCR) {
                        bytesread = getIncrData(e.xselection, *reinterpret_cast<long*>(xbuf.p));
                        break;
                    }
                    else {
                        Data.resize(bytesread + count + remaining + 1);
                        memcpy(Data.data() + bytesread, xbuf.p, count);
                        bytesread += count;
                        // Cannot trust data to be null terminated
                        Data[bytesread] = '\0';
                    }
                } while (remaining > 0);
            }
            if (!Data.empty()) {

                if (ClipboardDataType_ == ClipboardDataType::TEXT) {
                    convert_crlf(reinterpret_cast<char*>(Data.data()), bytesread);
                    Data.resize(bytesread);
                    if (onText) {
                        std::string str(reinterpret_cast<char*>(Data.data()), static_cast<size_t>(bytesread));
                        onText(str);
                    }
                }
                else if (ClipboardDataType_ == ClipboardDataType::IMAGE) {

                    /* if (onText) {
                         onText(Data.data(), static_cast<size_t>(bytesread));
                     }*/

                }

            }
        }
        void Clipboard_ManagerImpl::handle_SelectionRequest(const XEvent& ex)
        {

            XSelectionEvent e;
            e.type = SelectionNotify;
            e.requestor = ex.xselectionrequest.requestor;
            e.selection = ex.xselectionrequest.selection;
            e.target = ex.xselectionrequest.target;
            e.time = ex.xselectionrequest.time;
            e.property = ex.xselectionrequest.property;

            if (ClipboardDataType_ == ClipboardDataType::TEXT) {
                if (e.target == TARGETS) {
                    Atom a[3] = { XaUtf8String, XA_STRING, XaText };
                    XChangeProperty(Display_, e.requestor, e.property, XA_ATOM, atom_bits, 0, (unsigned char*)a, 3);
                }
                else {
                    if (!Data.empty()) {
                        if (e.target == XaUtf8String ||
                            e.target == XA_STRING ||
                            e.target == XaCompoundText ||
                            e.target == XaText ||
                            e.target == Xatextplain ||
                            e.target == Xatextplainutf ||
                            e.target == Xatextplainutf2) {
                            if (e.target != XA_STRING) e.target = XaUtf8String;
                            XChangeProperty(Display_, e.requestor, e.property, e.target, 8, 0, Data.data(), Data.size());
                        }
                    }
                    else {
                        e.property = 0;
                    }
                }
            }
            else { // image in clipboard
                if (e.target == TARGETS) {
                    Atom a[1] = { XaImageBmp };
                    XChangeProperty(Display_, e.requestor, e.property, XA_ATOM, atom_bits, 0, (unsigned char*)a, 1);
                }
                else {
                    if (e.target == XaImageBmp && !Data.empty()) {
                        XChangeProperty(Display_, e.requestor, e.property, e.target, 8, 0, Data.data(), Data.size());
                    }
                    else {
                        e.property = 0;
                    }
                }
            }
            XSendEvent(Display_, e.requestor, 0, 0, (XEvent *)&e);
        }
        void  Clipboard_ManagerImpl::handle(const XEvent& ex) {

            std::cout << "EVENT" << ex.type << std::endl;
            switch (ex.type) {

            case SelectionNotify:
                handle_SelectionNotify(ex);
                break;
            case SelectionClear:
                handle_SelectionClear(ex);
                break;
            case SelectionRequest:
                handle_SelectionRequest(ex);
                break;
            default:
                if (ex.type - xfixes_event_base == XFixesSelectionNotify) {
                    auto selection_notify = reinterpret_cast<const XFixesSelectionNotifyEvent *>(&ex);
                    if ((selection_notify->selection == CLIPBOARD) && !OwnsClipboard) {
                        handle_clipboard_timestamp(selection_notify->selection_timestamp);
                    }
                }
                break;
            }

        }

        void Clipboard_ManagerImpl::run()
        {
            BackGroundWorker = std::thread([&] {
                while (true) {
                    while (XPending(Display_) > 0) {
                        XEvent xevent;
                        XNextEvent(Display_, &xevent);
                        if (xevent.type == DestroyNotify) {
                            std::cout << "Destroy Called! " << std::endl;
                            return false;
                        }
                        handle(xevent);
                    }
                    std::this_thread::sleep_for(50ms);
                }
            });
        }
        void Clipboard_ManagerImpl::copy(const std::string& text)
        {
            if (text.empty()) return;
            Data.resize(text.size() + 1);
            memcpy(Data.data(), text.c_str(), text.size() + 1);
            OwnsClipboard = true;
            ClipboardDataType_ = ClipboardDataType::TEXT;
            XSetSelectionOwner(Display_, CLIPBOARD, Window_, LastTImestamp);
        }
    }
}