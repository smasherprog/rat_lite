#pragma once
#include "Input_Lite.h"
#include "ScreenCapture.h"
#include "WS_Lite.h"
#include <algorithm>
#include <memory>
#include <string>
#include <vector>
#include <math.h>

#if defined(WINDOWS) || defined(WIN32)
#if defined(RAT_LITE_DLL)
#define RAT_LITE_EXTERN __declspec(dllexport)
#define RAT_EXPIMP_TEMPLATE
#else
#define RAT_LITE_EXTERN
#define RAT_EXPIMP_TEMPLATE extern
#endif
#else
#define RAT_LITE_EXTERN
#define RAT_EXPIMP_TEMPLATE
#endif

namespace SL {
namespace RAT {

    const int PixelStride = 4;
    struct Point {
        Point() : X(0), Y(0) {}
        Point(const Point &p) : Point(p.X, p.Y) {}
        Point(int x, int y) : X(x), Y(y) {}
        int X, Y;
    };

    typedef Point Size;
    class Rect {

      public:
        Rect() : Height(0), Width(0) {}
        Rect(const Rect &rect) : Rect(rect.Origin, rect.Height, rect.Width) {}
        Rect(Point origin, int height, int width) : Origin(origin), Height(height), Width(width) {}
        auto Center() const { return Point(Origin.X + (Width / 2), Origin.Y + (Height / 2)); }

        Point Origin;
        int Height, Width;
        auto top() const { return Origin.Y; }
        auto bottom() const { return Origin.Y + Height; }
        void bottom(int b) { Height = b - Origin.Y; }
        auto left() const { return Origin.X; }
        auto right() const { return Origin.X + Width; }
        void right(int r) { Width = r - Origin.X; }
        auto Contains(const Point &p) const
        {
            if (p.X < left())
                return false;
            if (p.Y < top())
                return false;
            if (p.X >= right())
                return false;
            if (p.Y >= bottom())
                return false;
            return true;
        }
        void Expand_To_Include(const Point &p)
        {
            if (p.X <= left())
                Origin.X = p.X;
            if (p.Y <= top())
                Origin.Y = p.Y;
            if (right() <= p.X)
                Width = p.X - left();
            if (bottom() <= p.Y)
                Height = p.Y - top();
        }
    };
    inline auto operator==(const Point &p1, const Point &p2) { return p1.X == p2.X && p1.Y == p2.Y; }
    inline auto operator!=(const Point &p1, const Point &p2) { return !(p1 == p2); }
    inline auto operator==(const Rect &p1, const Rect &p2) { return p1.Origin == p2.Origin && p1.Height == p2.Height && p1.Width == p2.Width; }
    inline auto SquaredDistance(const Point &p1, const Point &p2)
    {
        auto dx = abs(p1.X - p2.X);
        auto dy = abs(p1.Y - p2.Y);
        return dx * dx + dy * dy;
    }
    inline auto SquaredDistance(const Point &p, const Rect &r)
    {
        auto cx = std::max(std::min(p.X, r.right()), r.left());
        auto cy = std::max(std::min(p.Y, r.bottom()), r.top());
        return SquaredDistance(Point(cx, cy), p);
    }
    inline auto Distance(const Point &p1, const Point &p2) { return sqrt(SquaredDistance(p1, p2)); }

    inline auto Distance(const Point &p, const Rect &r) { return Distance(p, r.Center()); }
    struct Image {
        Image() {}
        Image(const Rect &r, const unsigned char *d, const size_t &l) : Rect_(r), Data(d), Length(l) {}
        Rect Rect_;
        const unsigned char *Data = nullptr;
        size_t Length = 0;
    };

    enum class PACKET_TYPES : unsigned int {
        INVALID,
        HTTP_MSG,
        ONMONITORSCHANGED,
        ONFRAMECHANGED,
        ONNEWFRAME,
        ONMOUSEIMAGECHANGED,
        ONMOUSEPOSITIONCHANGED,
        ONKEYUP,
        ONKEYDOWN,
        ONMOUSEUP,
        ONMOUSEDOWN,
        ONMOUSESCROLL,
        ONCLIPBOARDTEXTCHANGED,
        // use LAST_PACKET_TYPE as the starting point of your custom packet types. Everything before this is used internally by the library
        LAST_PACKET_TYPE
    };
    template <class CONFIGTYPE> class INetworkHandlers {
      public:
        virtual ~INetworkHandlers() {}

        virtual std::shared_ptr<CONFIGTYPE> onConnection(const std::function<void(const std::shared_ptr<SL::WS_LITE::IWSocket>)> &callback) = 0;
        virtual std::shared_ptr<CONFIGTYPE>
        onMessage(const std::function<void(const std::shared_ptr<SL::WS_LITE::IWSocket> &socket, const WS_LITE::WSMessage)> &callback) = 0;
        virtual std::shared_ptr<CONFIGTYPE> onDisconnection(
            const std::function<void(const std::shared_ptr<SL::WS_LITE::IWSocket> &socket, unsigned short code, const std::string)> &callback) = 0;
    };
    class RAT_LITE_EXTERN IConfig {
      public:
        virtual ~IConfig() {}
        virtual void ShareClipboard(bool share) = 0;
        virtual bool ShareClipboard() const = 0;
    };
    class RAT_LITE_EXTERN IServerDriver : public IConfig {
      public:
        virtual ~IServerDriver() {}

        virtual void MaxConnections(int maxconnections) = 0;
        virtual int MaxConnections() const = 0;

        static WS_LITE::WSMessage PrepareMonitorsChanged(const std::vector<Screen_Capture::Monitor> &monitors);
        // imagecompression is [0, 100]    = [WORST, BEST]   Better compression also takes more time .. 70 seems to work well
        static WS_LITE::WSMessage PrepareFrameChanged(const Screen_Capture::Image &image, const Screen_Capture::Monitor &monitor,
                                                      int imagecompression = 70, bool usegrayscale = false);
        // imagecompression is [0, 100]    = [WORST, BEST]
        static WS_LITE::WSMessage PrepareNewFrame(const Screen_Capture::Image &image, const Screen_Capture::Monitor &monitor,
                                                  int imagecompression = 70, bool usegrayscale = false);
        static WS_LITE::WSMessage PrepareMouseImageChanged(const Screen_Capture::Image &image);
        static WS_LITE::WSMessage PrepareMousePositionChanged(const SL::Screen_Capture::Point &pos);
        static WS_LITE::WSMessage PrepareClipboardChanged(const std::string &text);
    };

    class RAT_LITE_EXTERN IServerDriverConfiguration : public INetworkHandlers<IServerDriverConfiguration> {
      public:
        virtual ~IServerDriverConfiguration() {}
        // events raised from the server
        virtual std::shared_ptr<IServerDriverConfiguration>
        onKeyUp(const std::function<void(const std::shared_ptr<WS_LITE::IWSocket> &socket, Input_Lite::KeyCodes key)> &callback) = 0;
        virtual std::shared_ptr<IServerDriverConfiguration>
        onKeyDown(const std::function<void(const std::shared_ptr<WS_LITE::IWSocket> &socket, Input_Lite::KeyCodes key)> &callback) = 0;
        virtual std::shared_ptr<IServerDriverConfiguration>
        onMouseUp(const std::function<void(const std::shared_ptr<WS_LITE::IWSocket> &socket, Input_Lite::MouseButtons button)> &callback) = 0;
        virtual std::shared_ptr<IServerDriverConfiguration>
        onMouseDown(const std::function<void(const std::shared_ptr<WS_LITE::IWSocket> &socket, Input_Lite::MouseButtons button)> &callback) = 0;
        virtual std::shared_ptr<IServerDriverConfiguration>
        onMouseScroll(const std::function<void(const std::shared_ptr<WS_LITE::IWSocket> &socket, int offset)> &callback) = 0;
        virtual std::shared_ptr<IServerDriverConfiguration>
        onMousePosition(const std::function<void(const std::shared_ptr<WS_LITE::IWSocket> &socket, const Point &pos)> &callback) = 0;
        virtual std::shared_ptr<IServerDriverConfiguration> onClipboardChanged(const std::function<void(const std::string &text)> &callback) = 0;
        virtual std::shared_ptr<IServerDriver> Build(const std::shared_ptr<SL::WS_LITE::IWSListener_Configuration> &wslistenerconfig) = 0;
    };

    class RAT_LITE_EXTERN IClientDriver : public IConfig {
      public:
        virtual ~IClientDriver() {}

        virtual void SendKeyUp(SL::Input_Lite::KeyCodes key) = 0;
        virtual void SendKeyDown(SL::Input_Lite::KeyCodes key) = 0;
        virtual void SendMouseUp(const Input_Lite::MouseButtons button) = 0;
        virtual void SendMouseDown(const Input_Lite::MouseButtons button) = 0;
        virtual void SendMouseScroll(int offset) = 0;
        virtual void SendMousePosition(const Point &pos) = 0;
        virtual void SendClipboardChanged(const std::string &text) = 0;
    };

    class RAT_LITE_EXTERN IClientDriverConfiguration : public INetworkHandlers<IClientDriverConfiguration> {
      public:
        virtual ~IClientDriverConfiguration() {}
        virtual std::shared_ptr<IClientDriverConfiguration>
        onMonitorsChanged(const std::function<void(const std::vector<Screen_Capture::Monitor> &)> &callback) = 0;
        virtual std::shared_ptr<IClientDriverConfiguration>
        onFrameChanged(const std::function<void(const Image &, const SL::Screen_Capture::Monitor &)> &callback) = 0;
        virtual std::shared_ptr<IClientDriverConfiguration>
        onNewFrame(const std::function<void(const Image &, const SL::Screen_Capture::Monitor &)> &callback) = 0;
        virtual std::shared_ptr<IClientDriverConfiguration> onMouseImageChanged(const std::function<void(const Image &)> &callback) = 0;
        virtual std::shared_ptr<IClientDriverConfiguration> onMousePositionChanged(const std::function<void(const Point &)> &callback) = 0;
        virtual std::shared_ptr<IClientDriverConfiguration> onClipboardChanged(const std::function<void(const std::string &)> &callback) = 0;
        virtual std::shared_ptr<IClientDriver> Build(const std::shared_ptr<SL::WS_LITE::IWSClient_Configuration> &wsclientconfig) = 0;
    };

    std::shared_ptr<IClientDriverConfiguration> RAT_LITE_EXTERN CreateClientDriverConfiguration();
    std::shared_ptr<IServerDriverConfiguration> RAT_LITE_EXTERN CreateServerDriverConfiguration();

} // namespace RAT
} // namespace SL