
import *from './Input_Lite';
module SL
{
    export module RAT_Lite
    {
        export class Point {
            static sizeof() : number { return 8; } // actual bytes used
            constructor(public X : number, public Y : number) {}
            static FromArray(data : Uint8Array) : Point
            {
                var arr = new Int32Array(data.slice(0, this.sizeof()).buffer);
                return new Point(arr[0], arr[1]);
            }
            Fill(d : ArrayBuffer, offset : number) : void
            {
                var dt = new DataView(d, offset);
                dt.setInt32(0, this.X, true);
                dt.setInt32(4, this.Y, true);
            }
        };
        export class Rect {
            static sizeof() : number { return 8 + Point.sizeof(); } // actual bytes used
            constructor(public Origin : Point, public Height : number, public Width : number) {}
            static FromArray(data : Uint8Array) : Rect
            {
                var arr = new Int32Array(data.slice(0, this.sizeof()).buffer);
                return new Rect(new Point(arr[0], arr[1]), arr[2], arr[3]);
            }
            Fill(d : ArrayBuffer, offset : number) : void
            {
                this.Origin.Fill(d, offset);
                var dt = new DataView(d, offset + Point.sizeof());
                dt.setInt32(0, this.Height, true);
                dt.setInt32(4, this.Width, true);
            }
        };
        enum PACKET_TYPES {
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

        export class IClientDriver {

          public
            ShareClipboard(share : boolean) : void {}

          public
            ShareClipboard() : boolean {}
          public
            SendKeyUp(SL::Input_Lite::KeyCodes key) : void {}
          public
            SendKeyDown(SL::Input_Lite::KeyCodes key) : void {}
          public
            SendMouseUp(const Input_Lite::MouseButtons button) : void {}
          public
            SendMouseDown(const Input_Lite::MouseButtons button) : void {}
          public
            SendMouseScroll(int offset) : void {}
          public
            SendMousePosition(const Point &pos) : void {}
          public
            SendClipboardChanged(const std::string &text) : void {}
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
    }
}
