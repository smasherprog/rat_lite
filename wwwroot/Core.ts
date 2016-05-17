/// <reference path="zstd.d.ts" />




module SL {
    export module Remote_Access_Library {
        export module Utilities {
            export class Point {
                static sizeof(): number { return 8; }//actual bytes used
                constructor(public X: number, public Y: number) { }
                static FromArray(data: Uint8Array): Point {
                    var arr = new Int32Array(data.slice(0, this.sizeof()).buffer);
                    return new Point(arr[0], arr[1]);
                }
                Fill(d: ArrayBuffer, offset: number): void {
                    var dt = new DataView(d, offset);
                    dt.setInt32(0, this.X, true);
                    dt.setInt32(4, this.Y, true);
                }
            }
            export class Rect {
                static sizeof(): number { return 8 + Point.sizeof(); }//actual bytes used
                constructor(public Origin: Point, public Height: number, public Width: number) { }
                static FromArray(data: Uint8Array): Rect {
                    var arr = new Int32Array(data.slice(0, this.sizeof()).buffer);
                    return new Rect(new Point(arr[0], arr[1]), arr[2], arr[3]);
                }
                Fill(d: ArrayBuffer, offset: number): void {
                    this.Origin.Fill(d, offset);
                    var dt = new DataView(d, offset + Point.sizeof());
                    dt.setInt32(0, this.Height, true);
                    dt.setInt32(4, this.Width, true);
                }
            }
        }
        export module Input {

            export enum KeyPress {
                UP,
                DOWN
            };
            export enum SpecialKeys {
                ALT,
                CTRL,
                SHIFT,
                NO_PRESS_DATA
            };
            export class KeyEvent {
                constructor(public PressData: KeyPress, public Key: number, public SpecialKey: SpecialKeys) { }
                static sizeof() { return 4 + 4 + 4; }//actual bytes used
                Fill(d: ArrayBuffer, offset: number): void {
                    var dt = new DataView(d, offset);
                    dt.setUint32(0, this.PressData, true);
                    dt.setInt32(4, this.Key, true);
                    dt.setUint32(4 + 4, this.SpecialKey, true);
                }

                //a mix of ASCII and FLTK enumerations
                static keyboardMap: number[] = [
                    0, // [0]
                    0, // [1]
                    0, // [2]
                    0xff1b,//"CANCEL", // [3]
                    0, // [4]
                    0, // [5]
                    0xff68,//"HELP", // [6]
                    0, // [7]
                    8,//"BACK_SPACE", // [8]
                    9,//"TAB", // [9]
                    0, // [10]
                    0, // [11]
                    0,//"CLEAR", // [12]
                    0x000d,//"ENTER", // [13]
                    0, // [14]
                    0, // [15]
                    0xffe1,//"SHIFT", // [16]
                    0xffe3,//"CONTROL", // [17]
                    0xffe9,//"ALT", // [18]
                    0xff13,//"PAUSE", // [19]
                    0xffe5,//"CAPS_LOCK", // [20]
                    0xff2e,//"KANA", // [21]
                    0xff2f,//"EISU", // [22]
                    0,//"JUNJA", // [23]
                    0,//"FINAL", // [24]
                    0,//"HANJA", // [25]
                    0, // [26]
                    0xff1b,//"ESCAPE", // [27]
                    0,//"CONVERT", // [28]
                    0,//"NONCONVERT", // [29]
                    0,//"ACCEPT", // [30]
                    0,//"MODECHANGE", // [31]
                    0x0020,//"SPACE", // [32]
                    0xff55,//"PAGE_UP", // [33]
                    0xff56,//"PAGE_DOWN", // [34]
                    0xff57,//"END", // [35]
                    0xff50,//"HOME", // [36]
                    0xff51,//"LEFT", // [37]
                    0xff52,//"UP", // [38]
                    0xff53,//"RIGHT", // [39]
                    0xff54,//"DOWN", // [40]
                    0,//"SELECT", // [41]
                    0xff61,//"PRINT", // [42]
                    0x000d,//"EXECUTE", // [43]
                    0xff61,//"PRINTSCREEN", // [44]
                    0xff63,//"INSERT", // [45]
                    0xffff,//"DELETE", // [46]
                    0,//0, // [47]
                    0x30, //"0", // [48]
                    0x31, //"1", // [49]
                    0x32, //"2", // [50]
                    0x33, //"3", // [51]
                    0x34, //"4", // [52]
                    0x35, //"5", // [53]
                    0x36, //"6", // [54]
                    0x37, //"7", // [55]
                    0x38, //"8", // [56]
                    0x39, //"9", // [57]
                    0x3A,//"COLON", // [58]
                    0x3B, //"SEMICOLON", // [59]
                    0x3C, //"LESS_THAN", // [60]
                    0x3D,// "EQUALS", // [61]
                    0x3E,//"GREATER_THAN", // [62]
                    0x3F, //"QUESTION_MARK", // [63]
                    0x40,//"AT", // [64]
                    0x41,//"A", // [65]
                    0x42,//"B", // [66]
                    0x43,//"C", // [67]
                    0x44,//"D", // [68]
                    0x45,//"E", // [69]
                    0x46,//"F", // [70]
                    0x47,//"G", // [71]
                    0x48,//"H", // [72]
                    0x49,//"I", // [73]
                    0x4A,//"J", // [74]
                    0x4B,//"K", // [75]
                    0x4C,//"L", // [76]
                    0x4D,//"M", // [77]
                    0x4E,//"N", // [78]
                    0x4F,//"O", // [79]
                    0x50,//"P", // [80]
                    0x51,//"Q", // [81]
                    0x52,//"R", // [82]
                    0x53,//"S", // [83]
                    0x54,//"T", // [84]
                    0x55,//"U", // [85]
                    0x56,//"V", // [86]
                    0x57,//"W", // [87]
                    0x58,//"X", // [88]
                    0x59,//"Y", // [89]
                    0x5A,//"Z", // [90]
                    0xffe7,//"OS_KEY", // [91] Windows Key (Windows) or Command Key (Mac)
                    0, // [92]
                    0,//"CONTEXT_MENU", // [93]
                    0, // [94]
                    0,//"SLEEP", // [95]
                    0x30,//"NUMPAD0", // [96]
                    0x31,//"NUMPAD1", // [97]
                    0x32,//"NUMPAD2", // [98]
                    0x33,//"NUMPAD3", // [99]
                    0x34,//"NUMPAD4", // [100]
                    0x35,//"NUMPAD5", // [101]
                    0x36,//"NUMPAD6", // [102]
                    0x37,//"NUMPAD8", // [104]
                    0x38,//"NUMPAD8", // [104]
                    0x39,//"NUMPAD9", // [105]
                    0xf06A,//"MULTIPLY", // [106]
                    0xf06B,//"ADD", // [107]
                    0xf06C,//"SEPARATOR", // [108]
                    0xf06D,//"SUBTRACT", // [109]
                    0xf06E,//"DECIMAL", // [110]
                    0xf06F,//"DIVIDE", // [111]
                    0xffbd + 1,//"F1", // [112]
                    0xffbd + 2,//"F2", // [113]
                    0xffbd + 3,//"F3", // [114]
                    0xffbd + 4,//"F4", // [115]
                    0xffbd + 5,//"F5", // [116]
                    0xffbd + 6,//"F6", // [117]
                    0xffbd + 7,//"F7", // [118]
                    0xffbd + 8,//"F8", // [119]
                    0xffbd + 9,//"F9", // [120]
                    0xffbd + 10,//"F10", // [121]
                    0xffbd + 11,//"F11", // [122]
                    0xffbd + 12,//"F12", // [123]
                    0xffbd + 13,//"F13", // [124]
                    0xffbd + 14,//"F14", // [125]
                    0xffbd + 15,//"F15", // [126]
                    0xffbd + 16,//"F16", // [127]
                    0xffbd + 17,//"F17", // [128]
                    0xffbd + 18,//"F18", // [129]
                    0xffbd + 19,//"F19", // [130]
                    0xffbd + 20,//"F20", // [131]
                    0xffbd + 21,//"F21", // [132]
                    0xffbd + 22,//"F22", // [133]
                    0xffbd + 23,//F23", // [134]
                    0xffbd + 24,//"F24", // [135]
                    0, // [136]
                    0, // [137]
                    0, // [138]
                    0, // [139]
                    0, // [140]
                    0, // [141]
                    0, // [142]
                    0, // [143]
                    0xff7f,//"NUM_LOCK", // [144]
                    0xff14,//"SCROLL_LOCK", // [145]
                    0,//"WIN_OEM_FJ_JISHO", // [146]
                    0,//"WIN_OEM_FJ_MASSHOU", // [147]
                    0,//"WIN_OEM_FJ_TOUROKU", // [148]
                    0,//"WIN_OEM_FJ_LOYA", // [149]
                    0,//"WIN_OEM_FJ_ROYA", // [150]
                    0, // [151]
                    0, // [152]
                    0, // [153]
                    0, // [154]
                    0, // [155]
                    0, // [156]
                    0, // [157]
                    0, // [158]
                    0, // [159]
                    0x60, //"CIRCUMFLEX", // [160]
                    0x31,//"EXCLAMATION", // [161]
                    0x27,//"DOUBLE_QUOTE", // [162]
                    0x33,//"HASH", // [163]
                    0x34,//"DOLLAR", // [164]
                    0x35,//"PERCENT", // [165]
                    0x37,//"AMPERSAND", // [166]
                    0x2d,//"UNDERSCORE", // [167]
                    0x28,//"OPEN_PAREN", // [168]
                    0x29,//"CLOSE_PAREN", // [169]
                    0x38,//"ASTERISK", // [170]
                    0x3d,//"PLUS", // [171]
                    0x5c,//"PIPE", // [172]
                    0x2d,//"HYPHEN_MINUS", // [173]
                    0x5b,//"OPEN_CURLY_BRACKET", // [174]
                    0x5d,//"CLOSE_CURLY_BRACKET", // [175]
                    0x27,//"TILDE", // [176]
                    0, // [177]
                    0, // [178]
                    0, // [179]
                    0, // [180]
                    0xEF12,//"VOLUME_MUTE", // [181]
                    0xEF11,//"VOLUME_DOWN", // [182]
                    0xEF13,//"VOLUME_UP", // [183]
                    0, // [184]
                    0, // [185]
                    0x3a,//"SEMICOLON", // [186]
                    0x3d,//"EQUALS", // [187]
                    0x2c, //"COMMA", // [188]
                    0x2d,//"MINUS", // [189]
                    0x2e,//"PERIOD", // [190]
                    0x2f,//"SLASH", // [191]
                    0x36,//"BACK_QUOTE", // [192]
                    0, // [193]
                    0, // [194]
                    0, // [195]
                    0, // [196]
                    0, // [197]
                    0, // [198]
                    0, // [199]
                    0, // [200]
                    0, // [201]
                    0, // [202]
                    0, // [203]
                    0, // [204]
                    0, // [205]
                    0, // [206]
                    0, // [207]
                    0, // [208]
                    0, // [209]
                    0, // [210]
                    0, // [211]
                    0, // [212]
                    0, // [213]
                    0, // [214]
                    0, // [215]
                    0, // [216]
                    0, // [217]
                    0, // [218]
                    0x5b,//"OPEN_BRACKET", // [219]
                    0x5c,//"BACK_SLASH", // [220]
                    0x5d,//"CLOSE_BRACKET", // [221]
                    0x27,//"QUOTE", // [222]
                    0, // [223]
                    0,//"META", // [224]
                    0,//"ALTGR", // [225]
                    0, // [226]
                    0,//"WIN_ICO_HELP", // [227]
                    0,//"WIN_ICO_00", // [228]
                    0, // [229]
                    0,//"WIN_ICO_CLEAR", // [230]
                    0, // [231]
                    0, // [232]
                    0,//"WIN_OEM_RESET", // [233]
                    0,//"WIN_OEM_JUMP", // [234]
                    0,//"WIN_OEM_PA1", // [235]
                    0,//"WIN_OEM_PA2", // [236]
                    0,//"WIN_OEM_PA3", // [237]
                    0,//"WIN_OEM_WSCTRL", // [238]
                    0,//"WIN_OEM_CUSEL", // [239]
                    0,//"WIN_OEM_ATTN", // [240]
                    0,//"WIN_OEM_FINISH", // [241]
                    0,//"WIN_OEM_COPY", // [242]
                    0,//"WIN_OEM_AUTO", // [243]
                    0,//"WIN_OEM_ENLW", // [244]
                    0,//"WIN_OEM_BACKTAB", // [245]
                    0,//"ATTN", // [246]
                    0,//"CRSEL", // [247]
                    0,//"EXSEL", // [248]
                    0,//"EREOF", // [249]
                    0,//"PLAY", // [250]
                    0,//"ZOOM", // [251]
                    0, // [252]
                    0,//"PA1", // [253]
                    0,//"WIN_OEM_CLEAR", // [254]
                    0 // [255]
                ];
            };

            export enum MouseEvents {
                LEFT,
                MIDDLE,
                RIGHT,
                SCROLL,
                NO_EVENTDATA
            };
            export enum MousePress {
                UP,
                DOWN,
                DBLCLICK,
                NO_PRESS_DATA
            };
            export class MouseEvent {

                constructor(public EventData = MouseEvents.NO_EVENTDATA,
                    public Pos = new Utilities.Point(0, 0),
                    public ScrollDelta = 0,
                    public PressData = MousePress.NO_PRESS_DATA) { }

                static sizeof() { return 4 + Utilities.Point.sizeof() + 4 + 4; }//actual bytes used
                Fill(d: ArrayBuffer, offset: number): void {
                    var dt = new DataView(d, offset);
                    dt.setUint32(0, this.EventData, true);
                    this.Pos.Fill(d, offset + 4);
                    dt.setInt32(offset + 4 + Utilities.Point.sizeof(), this.ScrollDelta, true);
                    dt.setUint32(offset + 4 + Utilities.Point.sizeof() + 4, this.PressData, true);

                }
            };
        }
        export module Network {
            export enum PACKET_TYPES {
                INVALID,
                HTTP_MSG,
                SCREENIMAGE,
                SCREENIMAGEDIF,
                MOUSEPOS,
                MOUSEIMAGE,
                KEYEVENT,
                MOUSEEVENT,
                //use LAST_PACKET_TYPE as the starting point of your custom packet types. Everything before this is used internally by the library
                LAST_PACKET_TYPE
            }
            export class PacketHeader {
                Payload: ArrayBuffer;
                sizeof() { return 12; }//actual bytes used
                constructor(public Packet_Type = PACKET_TYPES.INVALID, public Payload_Length = 0, public UncompressedLength = 0) {
                    this.Payload = new ArrayBuffer(this.Payload_Length);
                }
                Fill(arr: Uint32Array): void {
                    arr[0] = this.Packet_Type;
                    arr[1] = this.Payload_Length;
                    arr[2] = this.UncompressedLength;
                }
            }
            export class SocketStats {
                //total bytes that the Socket layer received from the upper layer. This is not the actual amount of data send across the network due to compressoin, encryption, etc
                TotalBytesSent: number;
                //total bytes that the Socket layer seent to the network layer
                NetworkBytesSent: number;
                //total number of packets sent
                TotalPacketSent: number;
                //total bytes that the upper layer received from the socket layer after decompression, decryption, etc
                TotalBytesReceived: number;
                //total bytes that the Socket layer received from the network layer
                NetworkBytesReceived: number;
                //total number of packets received
                TotalPacketReceived: number;
            }
            export class ClientDriver {
                _Cursor: ImageData;
                _ScaleImage = false;
                _OriginalImage: HTMLImageElement;
                _ClientNetworkDriver: ClientNetworkDriver;
                _HTMLDivRoot: HTMLDivElement;
                _HTMLCanvasScreenImage: HTMLCanvasElement;
                _HTMLCanvasMouseImage: HTMLCanvasElement;
   
                _DivRootId = 'SLRATROOTID123';//this is an id used internally to double check if a canvas has allready been inserted 
                _LeftMouseStatus = Input.MousePress.UP;

                constructor(private _dst_host: string, private _dst_port: string) {

                    window.addEventListener("resize", this.onresize);
                    window.addEventListener("mousedown", this.onmousedown);//event not needed
                    window.addEventListener("mouseup", this.onmouseup);
                    window.addEventListener("mousemove", this.onmove);
                    window.addEventListener("wheel", this.onwheel);

                    window.addEventListener("keydown", this.onkeydown);
                    window.addEventListener("keyup", this.onkeyup);

                    window.addEventListener("dblclick", this.ondblclick);
                    window.addEventListener("touchend", this.ontouchend);
                    window.addEventListener("touchstart", this.ontouchstart);
                    window.addEventListener("touchmove", this.ontouchmove);
                }
                public Start = (): void => {
                    var testroot = document.getElementById(this._DivRootId);
                    if (testroot !== null) {
                        document.removeChild(testroot);
                    }
                    this._HTMLDivRoot = document.createElement('div');
                    this._HTMLDivRoot.id = this._DivRootId;
                    this._HTMLDivRoot.style.position = 'relative';
                    this._HTMLCanvasScreenImage = document.createElement('canvas');
                    this._HTMLCanvasScreenImage.style.position = 'absolute';
                    this._HTMLCanvasScreenImage.style.left = this._HTMLCanvasScreenImage.style.top = this._HTMLCanvasScreenImage.style.zIndex = '0';

                    this._HTMLCanvasMouseImage = document.createElement('canvas');
                    this._HTMLCanvasMouseImage.style.position = 'absolute';
                    this._HTMLCanvasMouseImage.style.left = this._HTMLCanvasMouseImage.style.top = '0';
                    this._HTMLCanvasMouseImage.style.zIndex = '1';

                    this._HTMLDivRoot.appendChild(this._HTMLCanvasScreenImage);
                    this._HTMLDivRoot.appendChild(this._HTMLCanvasMouseImage);

                    document.body.appendChild(this._HTMLDivRoot);//add to the dom

                    this._ClientNetworkDriver = new ClientNetworkDriver(this, this._dst_host, this._dst_port);
                    this._ClientNetworkDriver.Start();
                }
                public Stop = (): void => {
                    this._ClientNetworkDriver.Stop();
                    this._ClientNetworkDriver = null;
                    var testroot = document.getElementById(this._DivRootId);
                    if (testroot !== null) {
                        document.removeChild(testroot);
                    }
                }
                public ScaleView = (b: boolean): void => {
                    this._ScaleImage = b;
                }

                onkeydown = (ev: KeyboardEvent): void => {
                    this.handle_key(ev, new Input.KeyEvent(Input.KeyPress.DOWN, ev.keyCode || ev.which, Input.SpecialKeys.NO_PRESS_DATA));
                }
                onkeyup = (ev: KeyboardEvent): void => {
                    this.handle_key(ev, new Input.KeyEvent(Input.KeyPress.UP, ev.keyCode || ev.which, Input.SpecialKeys.NO_PRESS_DATA));
                }
                private handle_key = (ev: KeyboardEvent, k: Input.KeyEvent): void => {
                    k.SpecialKey = Input.SpecialKeys.NO_PRESS_DATA;
                    k.Key = Input.KeyEvent.keyboardMap[k.Key];
                    console.log("Key: '" + k.Key + "'");
                    if (ev.altKey) k.SpecialKey = Input.SpecialKeys.ALT;
                    else if (ev.ctrlKey) k.SpecialKey = Input.SpecialKeys.CTRL;
                    else if (ev.shiftKey) k.SpecialKey = Input.SpecialKeys.SHIFT;

                    ev.preventDefault();//capture the keypress
                    if (!this._ClientNetworkDriver.ConnectedToSelf()) {
                        this._ClientNetworkDriver.SendKeyEvent(k);//sending input to yourself will lead to an infinite loop...
                    }
                }

                private pointerEventToXY = function (e: any): Utilities.Point {
                    var out = new Utilities.Point(0, 0);
                    if (e.type == 'touchstart' || e.type == 'touchmove' || e.type == 'touchend' || e.type == 'touchcancel') {
                        var touch = e.touches[0] || e.changedTouches[0];
                        out.X = touch.pageX;
                        out.Y = touch.pageY;
                    } else if (e.type == 'mousedown' || e.type == 'mouseup' || e.type == 'mousemove' || e.type == 'mouseover' || e.type == 'mouseout' || e.type == 'mouseenter' || e.type == 'mouseleave') {
                        out.Y = e.pageY;
                        out.X = e.pageX;
                    }
                    return out;
                };

                ontouchend = (ev: TouchEvent): void => {
                    if (this._LeftMouseStatus == Input.MousePress.DOWN) {
                        this._LeftMouseStatus = Input.MousePress.UP;
                        this.handlemouse(0, Input.MousePress.UP, this.pointerEventToXY(ev), 0);
                    }
                }
                ontouchstart = (ev: TouchEvent): void => {
                    if (ev.touches.length == 1) {
                        this.StartTouchTimer(ev, this.pointerEventToXY(ev));
                    }

                }

                ontouchmove = (ev: TouchEvent): void => {
                    this.CancelTouchClickTimer();
                    if (ev.touches.length == 1) {
                        if (this._LeftMouseStatus == Input.MousePress.UP) {
                            this._LeftMouseStatus = Input.MousePress.DOWN;
                            this.handlemouse(0, Input.MousePress.DOWN, this.pointerEventToXY(ev), 0);
                        }
                    }

                    if (this._LeftMouseStatus == Input.MousePress.DOWN) {
                        this.handlemouse(-1, Input.MousePress.NO_PRESS_DATA, this.pointerEventToXY(ev), 0);
                        ev.preventDefault();
                    }
                }
                private TouchTimer: number;
                private CancelTouchClickTimer = (): void => {
                    if (this.TouchTimer != null) {
                        clearTimeout(this.TouchTimer);
                    }
                    this.TouchTimer = null;
                }

                private StartTouchTimer(ev: TouchEvent, pos: Utilities.Point) {
                    var self = this;
                    if (self.TouchTimer == null) {
                        self.TouchTimer = setTimeout(function () {//single click
                            self.CancelTouchClickTimer();
                            self.handlemouse(0, Input.MousePress.DOWN, pos, 0);
                            self.handlemouse(0, Input.MousePress.UP, pos, 0);
                        }, 500)
                    } else {
                        ev.preventDefault();
                        self.CancelTouchClickTimer();
                        self.handlemouse(0, Input.MousePress.DBLCLICK, pos, 0);
                    }
                }
                onmousedown = (ev: MouseEvent): void => {
                    this.handlemouse(ev.button, Input.MousePress.DOWN, this.pointerEventToXY(ev), 0);
                }
                ondblclick = (ev: MouseEvent): void => {
                    this.handlemouse(ev.button, Input.MousePress.DBLCLICK, this.pointerEventToXY(ev), 0);
                }
                onmouseup = (ev: MouseEvent): void => {
                    this.handlemouse(ev.button, Input.MousePress.UP, this.pointerEventToXY(ev), 0);
                }
                onmove = (ev: MouseEvent): void => {
                    this.handlemouse(-1, Input.MousePress.NO_PRESS_DATA, this.pointerEventToXY(ev), 0);
                }
                onwheel = (ev: WheelEvent): void => {
                    this.handlemouse(-1, Input.MousePress.NO_PRESS_DATA, this.pointerEventToXY(ev), ev.deltaY);
                }
                private handlemouse = (button: number, press: Input.MousePress, pos: Utilities.Point, scroll: number): void => {
                    var ev = new Input.MouseEvent();
                    var scale = this.GetScalingFactor();
                    ev.Pos = pos;
                    ev.Pos.X = ev.Pos.X / scale;
                    ev.Pos.Y = ev.Pos.Y / scale;

                    if (scroll != 0) {
                        ev.ScrollDelta = scroll < 0 ? -1 : 1;//force a -1 or 1 because browsers send different values for each scroll tick.
                        ev.EventData = Input.MouseEvents.SCROLL;
                    } else {
                        ev.ScrollDelta = 0;
                        switch (button) {
                            case 0:
                                ev.EventData = Input.MouseEvents.LEFT;
                                break;
                            case 1:
                                ev.EventData = Input.MouseEvents.MIDDLE;
                                break;
                            case 2:
                                ev.EventData = Input.MouseEvents.RIGHT;
                                break;
                            default:
                                ev.EventData = Input.MouseEvents.NO_EVENTDATA;
                                break;
                        };
                    }
                    ev.PressData = press;
                    if (!this._ClientNetworkDriver.ConnectedToSelf()) {
                        this._ClientNetworkDriver.SendMouse(ev);//sending input to yourself will lead to an infinite loop...
                    }

                }
                onresize = (ev: UIEvent): void => {

                    if (this._ScaleImage && this._OriginalImage != null) {

                        var scale = this.GetScalingFactor();
                        this._HTMLCanvasScreenImage.width = this._OriginalImage.width * scale;
                        this._HTMLCanvasScreenImage.height = this._OriginalImage.height * scale;

                        this._HTMLDivRoot.style.width = this._HTMLCanvasScreenImage.width + 'px';
                        this._HTMLDivRoot.style.height = this._HTMLCanvasScreenImage.height + 'px';
                        this._HTMLCanvasScreenImage.getContext("2d").drawImage(this._OriginalImage, 0, 0, this._HTMLCanvasScreenImage.width, this._HTMLCanvasScreenImage.height);

                    } else if (!this._ScaleImage && this._OriginalImage != null) {
                        if (this._HTMLCanvasScreenImage.height != this._OriginalImage.height || this._HTMLCanvasScreenImage.width != this._OriginalImage.width) {
                            this._HTMLCanvasScreenImage.width = this._OriginalImage.width;
                            this._HTMLCanvasScreenImage.height = this._OriginalImage.height;

                            this._HTMLDivRoot.style.width = this._HTMLCanvasScreenImage.width + 'px';
                            this._HTMLDivRoot.style.height = this._HTMLCanvasScreenImage.height + 'px';
                            this._HTMLCanvasScreenImage.getContext("2d").drawImage(this._OriginalImage, 0, 0);
                        }
                    }
                }
                GetScalingFactor(): number {
                    if (this._ScaleImage && this._OriginalImage != null) {
                        return window.innerHeight / this._OriginalImage.height;
                    } else {
                        return 1.0;
                    }
                }
                public OnReceive_ImageDif = (socket: WebSocket, rect: Utilities.Rect, img: string): void => {
                    if (this._OriginalImage === null) return;
                    "use strict";
                    //console.log('coords' + coords.X + ' ' + coords.Y + ' ' + coords.Width + ' ' + coords.Height);
                    var i = new Image();
                    i.src = "data:image/jpeg;base64," + img
                    var self = this;
                    i.onload = function () {
                        if (self._ScaleImage) {
                            var scale = self.GetScalingFactor();
                            self._HTMLCanvasScreenImage.getContext("2d").drawImage(i, rect.Origin.X * scale, rect.Origin.Y * scale, rect.Width * scale, rect.Height * scale);
                        } else {
                            self._HTMLCanvasScreenImage.getContext("2d").drawImage(i, rect.Origin.X, rect.Origin.Y);
                        }
                        // console.log("ctx.drawImage" + coords.Y, "  " + coords.X);
                    };

                    i.onerror = function (stuff) {
                        console.log("Img Onerror:", stuff);
                    };
                }
                public OnReceive_Image = (socket: WebSocket, rect: Utilities.Rect, img: string): void => {
                    "use strict";
                    //console.log('coords' + coords.X + ' ' + coords.Y + ' ' + coords.Width + ' ' + coords.Height);

                    var i = new Image();
                    i.src = "data:image/jpeg;base64," + img;

                    var self = this;
                    i.onload = function () {
                        if (self._ScaleImage) {
                            var scale = self.GetScalingFactor();
                            self._HTMLDivRoot.scrollWidth = self._HTMLCanvasScreenImage.width = i.width * scale;
                            self._HTMLDivRoot.scrollHeight = self._HTMLCanvasScreenImage.height = i.height * scale;

                            self._HTMLDivRoot.style.width = self._HTMLCanvasScreenImage.width + 'px';
                            self._HTMLDivRoot.style.height = self._HTMLCanvasScreenImage.height + 'px';
                            self._HTMLCanvasScreenImage.getContext("2d").drawImage(i, 0, 0, self._HTMLCanvasScreenImage.width, self._HTMLCanvasScreenImage.height);
                        } else {
                            self._HTMLCanvasScreenImage.width = i.width;
                            self._HTMLCanvasScreenImage.height = i.height;

                            self._HTMLDivRoot.style.width = self._HTMLCanvasScreenImage.width + 'px';
                            self._HTMLDivRoot.style.height = self._HTMLCanvasScreenImage.height + 'px';

                            self._HTMLCanvasScreenImage.getContext("2d").drawImage(i, 0, 0);
                        }
                        self._OriginalImage = i;
                        // console.log("ctx.drawImage" + coords.Y, "  " + coords.X);
                    };
                    i.onerror = function (stuff) {
                        console.log("Img Onerror:", stuff);
                    };
                }
                public OnReceive_MouseImage = (socket: WebSocket, point: Utilities.Point, img: Uint8Array): void => {
                    "use strict";
                    //console.log('coords' + coords.X + ' ' + coords.Y + ' ' + coords.Width + ' ' + coords.Height);

                    this._HTMLCanvasMouseImage.width = point.X;
                    this._HTMLCanvasMouseImage.height = point.Y;
                    try {
                        this._Cursor = this._HTMLCanvasMouseImage.getContext("2d").createImageData(point.X, point.Y);

                        for (var i = 0; i < this._Cursor.data.length; i += 4) {
                            this._Cursor.data[i + 0] = img[i + 0];
                            this._Cursor.data[i + 1] = img[i + 1];
                            this._Cursor.data[i + 2] = img[i + 2];
                            this._Cursor.data[i + 3] = img[i + 3];
                        }
                        this._HTMLCanvasMouseImage.getContext("2d").putImageData(this._Cursor, 0, 0);
                    } catch (e) {
                        console.log(e.message);
                    }

                }
                public OnReceive_MousePos = (socket: WebSocket, pos: Utilities.Point): void => {
                    if (this._ScaleImage) {
                        var scale = this.GetScalingFactor();
                        this._HTMLCanvasMouseImage.style.top = (pos.Y * scale) + "px";
                        this._HTMLCanvasMouseImage.style.left = (pos.X * scale) + "px";
                    } else {
                        this._HTMLCanvasMouseImage.style.top = pos.Y + "px";
                        this._HTMLCanvasMouseImage.style.left = pos.X + "px";
                    }
                }
            }


            export class ClientNetworkDriver {
                _Socket: WebSocket;
                _SocketStats: SocketStats;
                _TotalMemoryUsed: number;
                _ConnectedToSelf = false;
                constructor(private _IClientDriver: ClientDriver, private _dst_host: string, private _dst_port: string) { }
                public Start = (): void => {


                    var connectstring = "";
                    if (window.location.protocol != "https:") {
                        connectstring += "ws://";
                    } else {
                        connectstring += "wss://";
                    }
                    this._ConnectedToSelf = (this._dst_host == '127.0.0.1') || (this._dst_host == 'localhost') || (this._dst_host == '::1');
                    connectstring += this._dst_host + ":" + this._dst_port + "/rdp";
                    this._Socket = new WebSocket(connectstring);
                    this._Socket.binaryType = 'arraybuffer';
                    this._Socket.onopen = this.OnOpen;
                    this._Socket.onclose = this.OnClose;
                    this._Socket.onmessage = this.OnMessage;
                }
                public ConnectedToSelf = (): boolean => {
                    return this._ConnectedToSelf;
                }
                public Stop = (): void => {
                    this._Socket.close(1001, "Web Browser called Stop()");
                    this._Socket = null;
                }
                public SendMouse = (m: Input.MouseEvent): void => {
                    var pac = new PacketHeader(PACKET_TYPES.MOUSEEVENT, Input.MouseEvent.sizeof(), Input.MouseEvent.sizeof());
                    m.Fill(pac.Payload, 0);
                    this.Compress_and_Send(pac);
                }
                public SendKeyEvent = (ev: Input.KeyEvent): void => {
                    var pac = new PacketHeader(PACKET_TYPES.KEYEVENT, Input.KeyEvent.sizeof(), Input.KeyEvent.sizeof());
                    ev.Fill(pac.Payload, 0);
                    this.Compress_and_Send(pac);
                }
                private Compress_and_Send = (p: PacketHeader): void => {
                    var t0 = performance.now();

                    var srcPtr = Module._malloc(p.Payload_Length);
                    this._TotalMemoryUsed += p.Payload_Length;
                    var srcbuff = new Uint8Array(Module.HEAPU8.buffer, srcPtr, p.Payload_Length);//get enough space in the heap
                    srcbuff.set(new Uint8Array(p.Payload, 0, p.Payload_Length));//copy the data to the newly allocated memory

                    var dstsize = _ZSTD_compressBound(p.UncompressedLength + p.sizeof());
                    var dsttr = Module._malloc(dstsize);//get worst case space requirements for dst buffer
                    this._TotalMemoryUsed += dstsize;
                    var dstbuff = new Uint8Array(Module.HEAPU8.buffer, dsttr, dstsize);// dont write to the header portion

                    p.Payload_Length = _ZSTD_compress(dstbuff.byteOffset + p.sizeof(), dstsize - p.sizeof(), srcbuff.byteOffset, p.Payload_Length, 3);
                    if (_ZSTD_isError(p.Payload_Length) > 0) {
                        console.log('zstd error' + _ZSTD_getErrorName(p.Payload_Length));
                    }

                    p.Fill(new Uint32Array(Module.HEAPU8.buffer, dsttr, p.sizeof()));

                    var t1 = performance.now();
                    //comment this line out to see performance issues... My machine takes 0 to 6 ms to complete each receive
                    // console.log("took " + (t1 - t0) + " milliseconds to Compress the packet")

                    Module._free(srcPtr);
                    this._TotalMemoryUsed -= p.Payload_Length;
                    var test = dstbuff.buffer.slice(dsttr, dsttr + p.Payload_Length + p.sizeof());
                    var teu = new Uint8Array(test, 0, p.Payload_Length + p.sizeof());
                    this._Socket.send(teu);
                    Module._free(dsttr);
                    this._TotalMemoryUsed -= dstsize;

                }
                OnMessage = (ev: MessageEvent): void => {
                    var t0 = performance.now();

                    var packetheader = new PacketHeader();
                    var data = new DataView(ev.data);
                    packetheader.Packet_Type = data.getInt32(0, true);
                    packetheader.Payload_Length = data.getInt32(4, true);
                    packetheader.UncompressedLength = data.getInt32(8, true);



                    var srcPtr = Module._malloc(packetheader.Payload_Length);
                    this._TotalMemoryUsed += packetheader.Payload_Length;
                    var srcbuff = new Uint8Array(Module.HEAPU8.buffer, srcPtr, packetheader.Payload_Length);
                    srcbuff.set(new Uint8Array(ev.data, packetheader.sizeof()));

                    var dsttr = Module._malloc(packetheader.UncompressedLength);
                    this._TotalMemoryUsed += packetheader.UncompressedLength;
                    var dstbuff = new Uint8Array(Module.HEAPU8.buffer, dsttr, packetheader.UncompressedLength);

                    var decompressedsize = _ZSTD_decompress(dstbuff.byteOffset, packetheader.UncompressedLength, srcbuff.byteOffset, packetheader.Payload_Length);
                    if (_ZSTD_isError(decompressedsize) > 0) {
                        console.log('zstd error' + _ZSTD_getErrorName(decompressedsize));
                    }
                    var t1 = performance.now();
                    //comment this line out to see performance issues... My machine takes 0 to 6 ms to complete each receive
                    //   console.log("took " + (t1 - t0) + " milliseconds to Decompress the receive loop")
                    t0 = performance.now();
                    switch (packetheader.Packet_Type) {
                        case (PACKET_TYPES.SCREENIMAGE):
                            this.Image(dstbuff);
                            break;
                        case (PACKET_TYPES.SCREENIMAGEDIF):
                            this.ImageDif(dstbuff);
                            break;
                        case (PACKET_TYPES.MOUSEIMAGE):
                            this.MouseImage(dstbuff);
                            break;
                        case (PACKET_TYPES.MOUSEPOS):
                            this.MousePos(dstbuff);
                            break;
                        default:
                            console.log('received unknown packet type: ' + packetheader.Packet_Type)
                            break;
                    };
                    Module._free(dsttr);
                    this._TotalMemoryUsed -= packetheader.UncompressedLength;
                    Module._free(srcPtr);
                    this._TotalMemoryUsed -= packetheader.Payload_Length;
                    t1 = performance.now();
                    //comment this line out to see performance issues... My machine takes 0 to 6 ms to complete each receive
                    //  console.log("took " + (t1 - t0) + " milliseconds to process the receive loop");
                }
                _arrayBufferToBase64(buffer: Uint8Array, offset: number): string {
                    var binary = '';
                    var len = buffer.byteLength;
                    for (var i = offset; i < len; i++) {
                        binary += String.fromCharCode(buffer[i]);
                    }
                    return window.btoa(binary);
                }
                Image = (data: Uint8Array): void => {
                    this._IClientDriver.OnReceive_Image(this._Socket,
                        Utilities.Rect.FromArray(data),
                        this._arrayBufferToBase64(data, Utilities.Rect.sizeof()));
                }

                ImageDif = (data: Uint8Array): void => {

                    this._IClientDriver.OnReceive_ImageDif(this._Socket,
                        Utilities.Rect.FromArray(data),
                        this._arrayBufferToBase64(data, Utilities.Rect.sizeof()));
                }
                MouseImage = (data: Uint8Array): void => {
                    this._IClientDriver.OnReceive_MouseImage(this._Socket, Utilities.Point.FromArray(data), new Uint8Array(data.slice(Utilities.Point.sizeof()).buffer));
                }
                MousePos = (data: Uint8Array): void => {
                    this._IClientDriver.OnReceive_MousePos(this._Socket, Utilities.Point.FromArray(data));
                }
                OnOpen(ev: Event) {
                    window.document.title = "Connected: " + window.location.hostname + ":6001/rdp";
                    console.log('Socket Opened');
                }
                OnClose(ev: CloseEvent) {
                    var reason;
                    if (ev.code == 1000)
                        reason = "Normal closure, meaning that the purpose for which the connection was established has been fulfilled.";
                    else if (ev.code == 1001)
                        reason = "An endpoint is \"going away\", such as a server going down or a browser having navigated away from a page.";
                    else if (ev.code == 1002)
                        reason = "An endpoint is terminating the connection due to a protocol error";
                    else if (ev.code == 1003)
                        reason = "An endpoint is terminating the connection because it has received a type of data it cannot accept (e.g., an endpoint that understands only text data MAY send this if it receives a binary message).";
                    else if (ev.code == 1004)
                        reason = "Reserved. The specific meaning might be defined in the future.";
                    else if (ev.code == 1005)
                        reason = "No status code was actually present.";
                    else if (ev.code == 1006)
                        reason = "The connection was closed abnormally, e.g., without sending or receiving a Close control frame";
                    else if (ev.code == 1007)
                        reason = "An endpoint is terminating the connection because it has received data within a message that was not consistent with the type of the message (e.g., non-UTF-8 [http://tools.ietf.org/html/rfc3629] data within a text message).";
                    else if (ev.code == 1008)
                        reason = "An endpoint is terminating the connection because it has received a message that \"violates its policy\". This reason is given either if there is no other sutible reason, or if there is a need to hide specific details about the policy.";
                    else if (ev.code == 1009)
                        reason = "An endpoint is terminating the connection because it has received a message that is too big for it to process.";
                    else if (ev.code == 1010) // Note that this status code is not used by the server, because it can fail the WebSocket handshake instead.
                        reason = "An endpoint (client) is terminating the connection because it has expected the server to negotiate one or more extension, but the server didn't return them in the response message of the WebSocket handshake. <br /> Specifically, the extensions that are needed are: " + ev.reason;
                    else if (ev.code == 1011)
                        reason = "A server is terminating the connection because it encountered an unexpected condition that prevented it from fulfilling the request.";
                    else if (ev.code == 1015)
                        reason = "The connection was closed due to a failure to perform a TLS handshake (e.g., the server certificate can't be verified).";
                    else
                        reason = "Unknown reason";
                    alert(reason);
                    console.log('Socket Closed: ' + reason);
                }

            }
        }
    }
}

