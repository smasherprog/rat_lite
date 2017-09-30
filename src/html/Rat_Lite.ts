
enum KeyCodes {
    KEY_A = 4,
    KEY_B = 5,
    KEY_C = 6,
    KEY_D = 7,
    KEY_E = 8,
    KEY_F = 9,
    KEY_G = 10,
    KEY_H = 11,
    KEY_I = 12,
    KEY_J = 13,
    KEY_K = 14,
    KEY_L = 15,
    KEY_M = 16,
    KEY_N = 17,
    KEY_O = 18,
    KEY_P = 19,
    KEY_Q = 20,
    KEY_R = 21,
    KEY_S = 22,
    KEY_T = 23,
    KEY_U = 24,
    KEY_V = 25,
    KEY_W = 26,
    KEY_X = 27,
    KEY_Y = 28,
    KEY_Z = 29,
    KEY_1 = 30,
    KEY_2 = 31,
    KEY_3 = 32,
    KEY_4 = 33,
    KEY_5 = 34,
    KEY_6 = 35,
    KEY_7 = 36,
    KEY_8 = 37,
    KEY_9 = 38,
    KEY_0 = 39,
    KEY_Enter = 40,
    KEY_Escape = 41,
    KEY_Backspace = 42,
    KEY_Tab = 43,
    KEY_Space = 44,
    KEY_Minus = 45,
    KEY_Equals = 46,
    KEY_LeftBracket = 47,
    KEY_RightBracket = 48,
    KEY_Backslash = 49,
    KEY_Semicolon = 51,
    KEY_Quote = 52,
    KEY_Grave = 53,
    KEY_Comma = 54,
    KEY_Period = 55,
    KEY_Slash = 56,
    KEY_CapsLock = 57,
    KEY_F1 = 58,
    KEY_F2 = 59,
    KEY_F3 = 60,
    KEY_F4 = 61,
    KEY_F5 = 62,
    KEY_F6 = 63,
    KEY_F7 = 64,
    KEY_F8 = 65,
    KEY_F9 = 66,
    KEY_F10 = 67,
    KEY_F11 = 68,
    KEY_F12 = 69,
    KEY_PrintScreen = 70,
    KEY_ScrollLock = 71,
    KEY_Pause = 72,
    KEY_Insert = 73,
    KEY_Home = 74,
    KEY_PageUp = 75,
    KEY_Delete = 76,
    KEY_End = 77,
    KEY_PageDown = 78,
    KEY_Right = 79,
    KEY_Left = 80,
    KEY_Down = 81,
    KEY_Up = 82,
    KP_NumLock = 83,
    KP_Divide = 84,
    KP_Multiply = 85,
    KP_Subtract = 86,
    KP_Add = 87,
    KP_Enter = 88,
    KP_1 = 89,
    KP_2 = 90,
    KP_3 = 91,
    KP_4 = 92,
    KP_5 = 93,
    KP_6 = 94,
    KP_7 = 95,
    KP_8 = 96,
    KP_9 = 97,
    KP_0 = 98,
    KP_Point = 99,
    KEY_NonUSBackslash = 100,
    KP_Equals = 103,
    KEY_F13 = 104,
    KEY_F14 = 105,
    KEY_F15 = 106,
    KEY_F16 = 107,
    KEY_F17 = 108,
    KEY_F18 = 109,
    KEY_F19 = 110,
    KEY_F20 = 111,
    KEY_F21 = 112,
    KEY_F22 = 113,
    KEY_F23 = 114,
    KEY_F24 = 115,
    KEY_Help = 117,
    KEY_Menu = 118,
    KEY_LeftControl = 224,
    KEY_LeftShift = 225,
    KEY_LeftAlt = 226,
    KEY_LeftMeta = 227,
    KEY_RightControl = 228,
    KEY_RightShift = 229,
    KEY_RightAlt = 230,
    KEY_RightMeta = 231,
    INVALID = 255
};

function ConvertToKeyCode(key: KeyboardEvent): KeyCodes {
    switch (key.key) {
        case "0":
            return KeyCodes.KEY_0;
        case "1":
        case "2":
        case "3":
        case "4":
        case "5":
        case "6":
        case "7":
        case "8":
        case "9":
            return KeyCodes.KEY_1 + (+key.key - 1);
        case "A":
        case "B":
        case "C":
        case "D":
        case "E":
        case "F":
        case "G":
        case "H":
        case "I":
        case "J":
        case "K":
        case "L":
        case "M":
        case "N":
        case "O":
        case "P":
        case "Q":
        case "R":
        case "S":
        case "T":
        case "U":
        case "V":
        case "W":
        case "X":
        case "Y":
        case "Z":
            return KeyCodes.KEY_A + (+key.key - +"A");
        case "a":
        case "b":
        case "c":
        case "d":
        case "e":
        case "f":
        case "g":
        case "h":
        case "i":
        case "j":
        case "k":
        case "l":
        case "m":
        case "n":
        case "o":
        case "p":
        case "q":
        case "r":
        case "s":
        case "t":
        case "u":
        case "v":
        case "w":
        case "x":
        case "y":
        case "z":
            return KeyCodes.KEY_A + (+key.key - +"a");
        case "Enter":
            return KeyCodes.KEY_Enter;
        case "Escape":
            return KeyCodes.KEY_Escape;
        case "Backspace":
            return KeyCodes.KEY_Backspace;
        case "Tab":
            return KeyCodes.KEY_Tab;
        case " ":
            return KeyCodes.KEY_Space;
        case "-":
            return KeyCodes.KEY_Minus;
        case "=":
            return KeyCodes.KEY_Equals; // this is correct and not a mistype
        case "[":
            return KeyCodes.KEY_LeftBracket;
        case "[":
            return KeyCodes.KEY_RightBracket;
        case "\\":
            return KeyCodes.KEY_Backslash;
        case ";":
            return KeyCodes.KEY_Semicolon;
        case "'":
            return KeyCodes.KEY_Quote;
        case "`":
            return KeyCodes.KEY_Grave;
        case ",":
            return KeyCodes.KEY_Comma;
        case ".":
            return KeyCodes.KEY_Period;
        case "/":
            return KeyCodes.KEY_Slash;
        case "CapsLock":
            return KeyCodes.KEY_CapsLock;
        case "F1":
            return KeyCodes.KEY_F1;
        case "F2":
            return KeyCodes.KEY_F2;
        case "F3":
            return KeyCodes.KEY_F3;
        case "F4":
            return KeyCodes.KEY_F4;
        case "F5":
            return KeyCodes.KEY_F5;
        case "F6":
            return KeyCodes.KEY_F6;
        case "F7":
            return KeyCodes.KEY_F7;
        case "F8":
            return KeyCodes.KEY_F8;
        case "F9":
            return KeyCodes.KEY_F9;
        case "F10":
            return KeyCodes.KEY_F10;
        case "F11":
            return KeyCodes.KEY_F11;
        case "F12":
            return KeyCodes.KEY_F12;
        case "F13":
            return KeyCodes.KEY_F13;
        case "F14":
            return KeyCodes.KEY_F14;
        case "F15":
            return KeyCodes.KEY_F15;
        case "F16":
            return KeyCodes.KEY_F1;
        case "F17":
            return KeyCodes.KEY_F1;
        case "F18":
            return KeyCodes.KEY_F1;
        case "F19":
            return KeyCodes.KEY_F1;
        case "F20":
            return KeyCodes.KEY_F1;
        case "Print":
            return KeyCodes.KEY_PrintScreen;
        case "ScrollLock":
            return KeyCodes.KEY_ScrollLock;
        case "Pause":
            return KeyCodes.KEY_Pause;
        case "Insert":
            return KeyCodes.KEY_Insert;
        case "Home":
            return KeyCodes.KEY_Home;
        case "PageUp":
            return KeyCodes.KEY_PageUp;
        case "Delete":
            return KeyCodes.KEY_Delete;
        case "End":
            return KeyCodes.KEY_End;
        case "PageDown":
            return KeyCodes.KEY_PageDown;
        case "ArrowRight":
            return KeyCodes.KEY_Right;
        case "ArrowLeft":
            return KeyCodes.KEY_Left;
        case "ArrowDown":
            return KeyCodes.KEY_Down;
        case "ArrowUp":
            return KeyCodes.KEY_Up;
        case "NumLock":
            return KeyCodes.KP_NumLock;
        case "/":
            return KeyCodes.KP_Divide;
        case "*":
            return KeyCodes.KP_Multiply;
        case "-":
            return KeyCodes.KP_Subtract;
        case "+":
            return KeyCodes.KP_Add;
        case ".":
            return KeyCodes.KP_Point;
        case "Help":
            return KeyCodes.KEY_Help;
        case "Alt":
            return KeyCodes.KEY_Menu;
        case "Control":
            return KeyCodes.KEY_LeftControl;
        case "Shift":
            return KeyCodes.KEY_LeftShift;
        case "Meta":
            return KeyCodes.KEY_LeftMeta;
        default:
            return KeyCodes.INVALID;
    }

}

enum MouseButtons { LEFT, MIDDLE, RIGHT };

class KeyEvent {
    Pressed: boolean;
    Key: KeyCodes;
};
class MouseButtonEvent {
    Pressed: boolean;
    Button: MouseButtons;
};
class MouseScrollEvent {
    Offset = 0;
};
class MousePositionOffsetEvent {
    X = 0;
    Y = 0;
};
class MousePositionAbsoluteEvent {
    X = 0;
    Y = 0;
};
/////////////////////////////////////////END INPUT_LITE



enum OpCode { CONTINUATION = 0, TEXT = 1, BINARY = 2, CLOSE = 8, PING = 9, PONG = 10, INVALID = 255 };
class WSMessage {
    data: DataView;
    code: OpCode;
};
class Point {
    X: number;
    Y: number;
};
class Rect {
    Origin: Point;
    Height: number;
    Width: number;
};
class Monitor {
    Id: number;
    Index: number;
    Height: number;
    Width: number;
    // Offsets are the number of pixels that a monitor can be from the origin. For example, users can shuffle their
    // monitors around so this affects their offset.
    OffsetX: number;
    OffsetY: number;
    Name: string;
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

class IClientDriver {
    protected ShareClip = false;
    protected Monitors: Array<Monitor>;
    protected WebSocket_: WebSocket;

    protected onConnection_: (ws: WebSocket, ev: Event) => void;
    protected onMessage_: (ws: WebSocket, message: WSMessage) => void;
    protected onDisconnection_: (ws: WebSocket, code: number, message: string) => void;
    protected onMonitorsChanged_: (monitors: Monitor[]) => void;
    protected onFrameChanged_: (image: HTMLImageElement, monitor: Monitor) => void;
    protected onNewFrame_: (image: HTMLImageElement, monitor: Monitor) => void;
    protected onMouseImageChanged_: (image: ImageData) => void;
    protected onMousePositionChanged_: (point: Point) => void;
    protected onClipboardChanged_: (clipstring: string) => void;


    setShareClipboard(share: boolean): void { this.ShareClip = share; }
    getShareClipboard(): boolean { return this.ShareClip; }
    SendKeyUp(key: KeyCodes): void {
        var data = new Uint8Array(4 + 1);
        var dataview = new DataView(data.buffer);
        dataview.setUint32(0, PACKET_TYPES.ONKEYUP);
        dataview.setUint8(4, key);
        this.WebSocket_.send(data);
    }
    SendKeyDown(key: KeyCodes): void {
        var data = new Uint8Array(4 + 1);
        var dataview = new DataView(data.buffer);
        dataview.setUint32(0, PACKET_TYPES.ONKEYDOWN);
        dataview.setUint8(4, key);
        this.WebSocket_.send(data);
    }
    SendMouseUp(button: MouseButtons): void {
        var data = new Uint8Array(4 + 1);
        var dataview = new DataView(data.buffer);
        dataview.setUint32(0, PACKET_TYPES.ONMOUSEUP);
        dataview.setUint8(4, button);
        this.WebSocket_.send(data);
    }
    SendMouseDown(button: MouseButtons): void {
        var data = new Uint8Array(4 + 1);
        var dataview = new DataView(data.buffer);
        dataview.setUint32(0, PACKET_TYPES.ONMOUSEDOWN);
        dataview.setUint8(4, button);
        this.WebSocket_.send(data);
    }
    SendMouseScroll(offset: number): void {
        var data = new Uint8Array(4 + 4);
        var dataview = new DataView(data.buffer);
        dataview.setUint32(0, PACKET_TYPES.ONMOUSESCROLL);
        dataview.setUint32(4, offset);
        this.WebSocket_.send(data);
    }
    SendMousePosition(pos: Point): void {
        var data = new Uint8Array(4 + 8);
        var dataview = new DataView(data.buffer);
        dataview.setUint32(0, PACKET_TYPES.ONMOUSEPOSITIONCHANGED);
        dataview.setInt32(4, pos.X);
        dataview.setInt32(8, pos.X);
        this.WebSocket_.send(data);
    }
    SendClipboardChanged(text: string): void {
        var data = new Uint8Array(4 + text.length);
        var dataview = new DataView(data.buffer);
        dataview.setUint32(0, PACKET_TYPES.ONMOUSESCROLL);
        for (var i = 0; i < text.length; i++) {
            data[4 + i] = text.charCodeAt(0);
        }
        this.WebSocket_.send(data);
    }

};

class IClientDriverConfiguration extends IClientDriver {
    onConnection(callback: (ws: WebSocket, ev: Event) => void): IClientDriverConfiguration {
        this.onConnection_ = callback;
        return this;
    }
    onMessage(callback: (ws: WebSocket, message: WSMessage) => void): IClientDriverConfiguration {
        this.onMessage_ = callback;
        return this;
    }
    onDisconnection(callback: (ws: WebSocket, code: number, message: string) => void): IClientDriverConfiguration {
        this.onDisconnection_ = callback;
        return this;
    }
    onMonitorsChanged(callback: (monitors: Monitor[]) => void): IClientDriverConfiguration {
        this.onMonitorsChanged_ = callback;
        return this;
    }
    onFrameChanged(callback: (image: HTMLImageElement, monitor: Monitor) => void): IClientDriverConfiguration {
        this.onFrameChanged_ = callback;
        return this;
    }
    onNewFrame(callback: (image: HTMLImageElement, monitor: Monitor) => void): IClientDriverConfiguration {
        this.onNewFrame_ = callback;
        return this;
    }
    onMouseImageChanged(callback: (image: ImageData) => void): IClientDriverConfiguration {
        this.onMouseImageChanged_ = callback;
        return this;
    }
    onMousePositionChanged(callback: (point: Point) => void): IClientDriverConfiguration {
        this.onMousePositionChanged_ = callback;
        return this;
    }
    onClipboardChanged(callback: (clipstring: string) => void): IClientDriverConfiguration {
        this.onClipboardChanged_ = callback;
        return this;
    }
    private _arrayBufferToBase64(buffer: DataView, offset: number): string {
        var binary = '';
        for (var i = offset; i < buffer.byteLength; i++) {
            binary += String.fromCharCode(buffer[i]);
        }
        return window.btoa(binary);
    }
    private MonitorsChanged(ws: WebSocket, dataview: DataView) {
        if (!this.onMonitorsChanged_)
            return;
        let sizeofmonitor = 6 * 4 + 128;
        let num = dataview.byteLength / sizeofmonitor;
        this.Monitors = new Array<Monitor>();
        if (dataview.byteLength == num * sizeofmonitor && num < 8) {
            for (var i = 0; i < num; i++) {
                var name = '';
                for (var i = 0, strLen = 128; i < strLen; i++) {
                    name += String.fromCharCode.apply(dataview.getUint8(20 + i));
                }
                this.Monitors.push({
                    Id: dataview.getInt32(0),
                    Index: dataview.getInt32(4),
                    Height: dataview.getInt32(8),
                    Width: dataview.getInt32(12),
                    OffsetX: dataview.getInt32(16),
                    OffsetY: dataview.getInt32(20),
                    Name: name
                });
            }
            return this.onMonitorsChanged_(this.Monitors);
        }
        else if (dataview.byteLength == 0) {
            // it is possible to have no monitors.. shouldnt disconnect in that case
            return this.onMonitorsChanged_(this.Monitors);
        }
        if (this.onDisconnection_) {
            this.onDisconnection_(ws, 1000, "Invalid Monitor Count");
        }
        ws.close(1000, "Invalid Monitor Count");
    }
    private Frame(ws: WebSocket, dataview: DataView, callback: (image: HTMLImageElement, monitor: Monitor) => void) {
        if (dataview.byteLength >= 4 * 4 + 4) {
            var monitorid = dataview.getInt32(0);
            var rect = {
                Origin: {
                    X: dataview.getInt32(4),
                    Y: dataview.getInt32(8)
                },
                Height: dataview.getInt32(12),
                Width: dataview.getInt32(16)
            };
            var foundmonitor = this.Monitors.filter(a => a.Id == monitorid);
            if (foundmonitor.length > 0) {
                var i = new Image();
                i.src = "data:image/jpeg;base64," + this._arrayBufferToBase64(dataview, 16);
                if (dataview.byteLength >= 4 * 4 + (rect.Width * rect.Height * 4)) {
                    return callback(i, foundmonitor[0]);
                }
            }
        }
        if (this.onDisconnection_) {
            this.onDisconnection_(ws, 1000, "Received invalid lenght on onMouseImageChanged");
        }
        ws.close(1000, "Received invalid lenght on onMouseImageChanged");
    }

    private MouseImageChanged(ws: WebSocket, dataview: DataView) {
        if (!this.onMouseImageChanged_)
            return;
        if (dataview.byteLength >= 4 * 4) {
            var rect = {
                Origin: {
                    X: dataview.getInt32(0),
                    Y: dataview.getInt32(4)
                },
                Height: dataview.getInt32(8),
                Width: dataview.getInt32(12)
            };
            var canvas = document.createElement('canvas');
            var imageData = canvas.getContext('2d').createImageData(rect.Width, rect.Height);
            for (var i = 16; i < dataview.byteLength; i++) {
                imageData.data[i] = dataview[i];
            }
            if (dataview.byteLength >= 4 * 4 + (rect.Width * rect.Height * 4)) {
                return this.onMouseImageChanged_(imageData);
            }
        }
        if (this.onDisconnection_) {
            this.onDisconnection_(ws, 1000, "Received invalid lenght on onMouseImageChanged");
        }
        ws.close(1000, "Received invalid lenght on onMouseImageChanged");
    }
    private MousePositionChanged(ws: WebSocket, dataview: DataView) {
        if (!this.onMousePositionChanged_)
            return;
        if (dataview.byteLength == 8) {
            var p = {
                X: dataview.getInt32(0),
                Y: dataview.getInt32(4)
            };
            return this.onMousePositionChanged_(p);
        }
        if (this.onDisconnection_) {
            this.onDisconnection_(ws, 1000, "Received invalid lenght on onMousePositionChanged");
        }
        ws.close(1000, "Received invalid lenght on onMousePositionChanged");
    }
    private ClipboardTextChanged(dataview: DataView) {
        if (!this.ShareClip || !this.onClipboardChanged_)
            return;
        if (dataview.byteLength < 1024 * 100) { // 100K max
            var text = '';
            for (var i = 0, strLen = 128; i < strLen; i++) {
                text += String.fromCharCode.apply(dataview.getUint8(20 + i));
            }
            this.onClipboardChanged_(text);
        }
    }
    Build(ws: WebSocket): IClientDriver {
        var self = this;
        ws.binaryType = 'arraybuffer';
        ws.onopen = (ev: Event) => {
            console.log('onopen');
            if (self.onConnection_) {
                self.onConnection_(ws, ev);
            }
        };
        ws.onclose = (ev: CloseEvent) => {
            console.log('onclose');
            if (self.onDisconnection_) {
                self.onDisconnection_(ws, ev.code, ev.reason);
            }
        };
        ws.onmessage = (ev: MessageEvent) => {
            console.log('onmessage');
            var t0 = performance.now();
            var data = new DataView(ev.data);
            var packettype = <PACKET_TYPES>data.getInt32(0, true);

            switch (packettype) {
                case PACKET_TYPES.ONMONITORSCHANGED:
                    this.MonitorsChanged(ws, new DataView(ev.data, 4));
                    break;
                case PACKET_TYPES.ONFRAMECHANGED:
                    if (this.onFrameChanged_) {
                        this.Frame(ws, new DataView(ev.data, 4), this.onFrameChanged_);
                    }
                    break;
                case PACKET_TYPES.ONNEWFRAME:
                    if (this.onNewFrame_) {
                        this.Frame(ws, new DataView(ev.data, 4), this.onNewFrame_);
                    }
                    break;
                case PACKET_TYPES.ONMOUSEIMAGECHANGED:
                    this.MouseImageChanged(ws, new DataView(ev.data, 4));
                    break;
                case PACKET_TYPES.ONMOUSEPOSITIONCHANGED:
                    this.MousePositionChanged(ws, new DataView(ev.data, 4));
                    break;
                case PACKET_TYPES.ONCLIPBOARDTEXTCHANGED:
                    this.ClipboardTextChanged(new DataView(ev.data, 4));
                    break;
                default:
                    if (this.onMessage_) {
                        var r = new WSMessage();
                        r.data = new DataView(ev.data, 4);
                        if (ev.data instanceof ArrayBuffer) {
                            r.code = OpCode.BINARY;
                        } else if (typeof ev.data === "string") {
                            r.code = OpCode.TEXT;
                        }
                        this.onMessage_(ws, r); // pass up the chain
                    }
                    break;
            }
            var t1 = performance.now();
            // comment this line out to see performance issues... My machine takes 0 to 6 ms to complete each receive
            console.log("took " + (t1 - t0) + " milliseconds to process the receive loop");
        };
        return this;
    }
};
function CreateClientDriverConfiguration(): IClientDriverConfiguration {
    return new IClientDriverConfiguration();
}



///////////////////////////////////////////////////////////////////////////END RAT_LITE



class ClientWindow {
    Cursor_: ImageData;
    ScaleImage_ = false;
    Monitors_: Array<Monitor>;
    ClientDriver_: IClientDriver;
    Socket_: WebSocket;

    OriginalImage_: HTMLImageElement;
    HTMLDivRoot_: HTMLDivElement;
    HTMLCanvasScreenImage_: HTMLCanvasElement;
    HTMLCanvasMouseImage_: HTMLCanvasElement;

    ConnectedToSelf_ = false;
    ScaleImage = false;

    private dst_host_: string;
    private dst_port_: string;
    private AttachedRoot_: HTMLElement;

    constructor(rootnode: HTMLElement) {
        this.AttachedRoot_ = rootnode;

        //window.addEventListener("resize", this.onresize);
        window.addEventListener("mousedown", this.onmousedown);
        window.addEventListener("mouseup", this.onmouseup);
        window.addEventListener("mousemove", this.onmove);
        window.addEventListener("wheel", this.onwheel);
        window.addEventListener("keydown", this.onkeydown);
        window.addEventListener("keyup", this.onkeyup);

        //window.addEventListener("touchend", this.ontouchend);
        //window.addEventListener("touchstart", this.ontouchstart);
        //window.addEventListener("touchmove", this.ontouchmove);

        this.HTMLDivRoot_ = document.createElement('div');
        this.HTMLDivRoot_.style.position = 'relative';
        this.HTMLCanvasScreenImage_ = document.createElement('canvas');
        this.HTMLCanvasScreenImage_.style.position = 'absolute';
        this.HTMLCanvasScreenImage_.style.left = this.HTMLCanvasScreenImage_.style.top = this.HTMLCanvasScreenImage_.style.zIndex = '0';

        this.HTMLCanvasMouseImage_ = document.createElement('canvas');
        this.HTMLCanvasMouseImage_.style.position = 'absolute';
        this.HTMLCanvasMouseImage_.style.left = this.HTMLCanvasMouseImage_.style.top = '0';
        this.HTMLCanvasMouseImage_.style.zIndex = '1';

        this.HTMLDivRoot_.appendChild(this.HTMLCanvasScreenImage_);
        this.HTMLDivRoot_.appendChild(this.HTMLCanvasMouseImage_);

        rootnode.appendChild(this.HTMLDivRoot_); // add to the dom
    }
    public destroy() {
        if (this.Socket_) {
            this.Socket_.close();
        }
        this.Socket_ = null;
        window.removeEventListener("mousedown", this.onmousedown);
        window.removeEventListener("mouseup", this.onmouseup);
        window.removeEventListener("mousemove", this.onmove);
        window.removeEventListener("wheel", this.onwheel);
        window.removeEventListener("keydown", this.onkeydown);
        window.removeEventListener("keyup", this.onkeyup);
        if (this.AttachedRoot_ && this.HTMLDivRoot_) {
            this.AttachedRoot_.removeChild(this.HTMLDivRoot_);
        }

        this.Cursor_ = null;
        this.ScaleImage_ = false;
        this.Monitors_ = null;
        this.ClientDriver_ = null;
        this.Socket_ = null;
        this.OriginalImage_ = null;
        this.HTMLDivRoot_ = this.AttachedRoot_ = this.HTMLDivRoot_ = this.HTMLCanvasScreenImage_ = this.HTMLCanvasMouseImage_ = null;
    }
    public connect(dst_host: string, dst_port: string): void {
        this.dst_host_ = dst_host;
        this.dst_port_ = dst_port;

        var connectstring = "";
        if (window.location.protocol != "https:") {
            connectstring += "ws://";
        }
        else {
            connectstring += "wss://";
        }

        this.ConnectedToSelf_ = (this.dst_host_ == '127.0.0.1') || (this.dst_host_ == 'localhost') || (this.dst_host_ == '::1');
        connectstring += this.dst_host_ + ":" + this.dst_port_;
        this.Socket_ = new WebSocket(connectstring);
        this.Socket_.binaryType = 'arraybuffer';
        this.ClientDriver_ = CreateClientDriverConfiguration()
            .onConnection((ws: WebSocket, ev: Event) => {

            }).onMessage((ws: WebSocket, message: WSMessage) => {

            }).onDisconnection((ws: WebSocket, code: number, message: string) => {

            }).onClipboardChanged((clipstring: string) => {

            }).onFrameChanged((image: HTMLImageElement, monitor: Monitor) => {
                if (this.ScaleImage) {
                    var scale = this.getScalingFactor();
                    this.HTMLCanvasScreenImage_.getContext("2d").drawImage(image, monitor.OffsetX * scale, monitor.OffsetY * scale, monitor.Width * scale, monitor.Height * scale);
                }
                else {
                    this.HTMLCanvasScreenImage_.getContext("2d").drawImage(image, monitor.OffsetX * scale, monitor.OffsetY);
                }
            }).onMonitorsChanged((monitors: Monitor[]) => {

            }).onMouseImageChanged((image: ImageData) => {
                this.HTMLCanvasMouseImage_.getContext("2d").putImageData(this.Cursor_, 0, 0);
            }).onMousePositionChanged((pos: Point) => {
                if (this.ScaleImage) {
                    var scale = this.getScalingFactor();
                    this.HTMLCanvasMouseImage_.style.top = (pos.Y * scale) + "px";
                    this.HTMLCanvasMouseImage_.style.left = (pos.X * scale) + "px";
                }
                else {
                    this.HTMLCanvasMouseImage_.style.top = pos.Y + "px";
                    this.HTMLCanvasMouseImage_.style.left = pos.X + "px";
                }
            }).onNewFrame((image: HTMLImageElement, monitor: Monitor) => {
                if (this.ScaleImage) {
                    var scale = this.getScalingFactor();
                    this.HTMLCanvasScreenImage_.width = image.width * scale;
                    this.HTMLCanvasScreenImage_.height = image.height * scale;
                    this.HTMLDivRoot_.style.width = this.HTMLCanvasScreenImage_.width + 'px';
                    this.HTMLDivRoot_.style.height = this.HTMLCanvasScreenImage_.height + 'px';
                    this.HTMLCanvasScreenImage_.getContext("2d").drawImage(image, 0, 0, this.HTMLCanvasScreenImage_.width, this.HTMLCanvasScreenImage_.height);
                }
                else {
                    this.HTMLCanvasScreenImage_.width = image.width;
                    this.HTMLCanvasScreenImage_.height = image.height;
                    this.HTMLDivRoot_.style.width = this.HTMLCanvasScreenImage_.width + 'px';
                    this.HTMLDivRoot_.style.height = this.HTMLCanvasScreenImage_.height + 'px';
                    this.HTMLCanvasScreenImage_.getContext("2d").drawImage(image, 0, 0);
                }
                this.OriginalImage_ = image;
            }).Build(this.Socket_);
    }

    public getScalingFactor(): number {
        if (this.ScaleImage && this.OriginalImage_ != null) {
            return window.innerHeight / this.OriginalImage_.height;
        }
        else {
            return 1.0;
        }
    }
    private onkeydown(ev: KeyboardEvent): void {
        this.ClientDriver_.SendKeyDown(ConvertToKeyCode(ev));
    }
    private onkeyup(ev: KeyboardEvent): void {
        this.ClientDriver_.SendKeyUp(ConvertToKeyCode(ev));
    }
    private onwheel(ev: WheelEvent): void {
        this.ClientDriver_.SendMouseScroll(ev.deltaY < 0 ? -1 : 1);
    }
    private onmove(ev: MouseEvent): void {
        this.ClientDriver_.SendMousePosition({ Y: ev.pageY, X: ev.pageX });
    }
    private onmouseup(ev: MouseEvent): void {
        this.ClientDriver_.SendMouseUp(ev.button);
    }
    private onmousedown(ev: MouseEvent): void {
        this.ClientDriver_.SendMouseDown(ev.button);
    }
}
