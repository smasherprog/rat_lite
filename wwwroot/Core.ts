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

                static keyboardMap: string[]=[
                    "", // [0]
                    "", // [1]
                    "", // [2]
                    "CANCEL", // [3]
                    "", // [4]
                    "", // [5]
                    "HELP", // [6]
                    "", // [7]
                    "BACK_SPACE", // [8]
                    "TAB", // [9]
                    "", // [10]
                    "", // [11]
                    "CLEAR", // [12]
                    "ENTER", // [13]
                    "ENTER_SPECIAL", // [14]
                    "", // [15]
                    "SHIFT", // [16]
                    "CONTROL", // [17]
                    "ALT", // [18]
                    "PAUSE", // [19]
                    "CAPS_LOCK", // [20]
                    "KANA", // [21]
                    "EISU", // [22]
                    "JUNJA", // [23]
                    "FINAL", // [24]
                    "HANJA", // [25]
                    "", // [26]
                    "ESCAPE", // [27]
                    "CONVERT", // [28]
                    "NONCONVERT", // [29]
                    "ACCEPT", // [30]
                    "MODECHANGE", // [31]
                    "SPACE", // [32]
                    "PAGE_UP", // [33]
                    "PAGE_DOWN", // [34]
                    "END", // [35]
                    "HOME", // [36]
                    "LEFT", // [37]
                    "UP", // [38]
                    "RIGHT", // [39]
                    "DOWN", // [40]
                    "SELECT", // [41]
                    "PRINT", // [42]
                    "EXECUTE", // [43]
                    "PRINTSCREEN", // [44]
                    "INSERT", // [45]
                    "DELETE", // [46]
                    "", // [47]
                    "0", // [48]
                    "1", // [49]
                    "2", // [50]
                    "3", // [51]
                    "4", // [52]
                    "5", // [53]
                    "6", // [54]
                    "7", // [55]
                    "8", // [56]
                    "9", // [57]
                    "COLON", // [58]
                    "SEMICOLON", // [59]
                    "LESS_THAN", // [60]
                    "EQUALS", // [61]
                    "GREATER_THAN", // [62]
                    "QUESTION_MARK", // [63]
                    "AT", // [64]
                    "A", // [65]
                    "B", // [66]
                    "C", // [67]
                    "D", // [68]
                    "E", // [69]
                    "F", // [70]
                    "G", // [71]
                    "H", // [72]
                    "I", // [73]
                    "J", // [74]
                    "K", // [75]
                    "L", // [76]
                    "M", // [77]
                    "N", // [78]
                    "O", // [79]
                    "P", // [80]
                    "Q", // [81]
                    "R", // [82]
                    "S", // [83]
                    "T", // [84]
                    "U", // [85]
                    "V", // [86]
                    "W", // [87]
                    "X", // [88]
                    "Y", // [89]
                    "Z", // [90]
                    "OS_KEY", // [91] Windows Key (Windows) or Command Key (Mac)
                    "", // [92]
                    "CONTEXT_MENU", // [93]
                    "", // [94]
                    "SLEEP", // [95]
                    "NUMPAD0", // [96]
                    "NUMPAD1", // [97]
                    "NUMPAD2", // [98]
                    "NUMPAD3", // [99]
                    "NUMPAD4", // [100]
                    "NUMPAD5", // [101]
                    "NUMPAD6", // [102]
                    "NUMPAD7", // [103]
                    "NUMPAD8", // [104]
                    "NUMPAD9", // [105]
                    "MULTIPLY", // [106]
                    "ADD", // [107]
                    "SEPARATOR", // [108]
                    "SUBTRACT", // [109]
                    "DECIMAL", // [110]
                    "DIVIDE", // [111]
                    "F1", // [112]
                    "F2", // [113]
                    "F3", // [114]
                    "F4", // [115]
                    "F5", // [116]
                    "F6", // [117]
                    "F7", // [118]
                    "F8", // [119]
                    "F9", // [120]
                    "F10", // [121]
                    "F11", // [122]
                    "F12", // [123]
                    "F13", // [124]
                    "F14", // [125]
                    "F15", // [126]
                    "F16", // [127]
                    "F17", // [128]
                    "F18", // [129]
                    "F19", // [130]
                    "F20", // [131]
                    "F21", // [132]
                    "F22", // [133]
                    "F23", // [134]
                    "F24", // [135]
                    "", // [136]
                    "", // [137]
                    "", // [138]
                    "", // [139]
                    "", // [140]
                    "", // [141]
                    "", // [142]
                    "", // [143]
                    "NUM_LOCK", // [144]
                    "SCROLL_LOCK", // [145]
                    "WIN_OEM_FJ_JISHO", // [146]
                    "WIN_OEM_FJ_MASSHOU", // [147]
                    "WIN_OEM_FJ_TOUROKU", // [148]
                    "WIN_OEM_FJ_LOYA", // [149]
                    "WIN_OEM_FJ_ROYA", // [150]
                    "", // [151]
                    "", // [152]
                    "", // [153]
                    "", // [154]
                    "", // [155]
                    "", // [156]
                    "", // [157]
                    "", // [158]
                    "", // [159]
                    "CIRCUMFLEX", // [160]
                    "EXCLAMATION", // [161]
                    "DOUBLE_QUOTE", // [162]
                    "HASH", // [163]
                    "DOLLAR", // [164]
                    "PERCENT", // [165]
                    "AMPERSAND", // [166]
                    "UNDERSCORE", // [167]
                    "OPEN_PAREN", // [168]
                    "CLOSE_PAREN", // [169]
                    "ASTERISK", // [170]
                    "PLUS", // [171]
                    "PIPE", // [172]
                    "HYPHEN_MINUS", // [173]
                    "OPEN_CURLY_BRACKET", // [174]
                    "CLOSE_CURLY_BRACKET", // [175]
                    "TILDE", // [176]
                    "", // [177]
                    "", // [178]
                    "", // [179]
                    "", // [180]
                    "VOLUME_MUTE", // [181]
                    "VOLUME_DOWN", // [182]
                    "VOLUME_UP", // [183]
                    "", // [184]
                    "", // [185]
                    "SEMICOLON", // [186]
                    "EQUALS", // [187]
                    "COMMA", // [188]
                    "MINUS", // [189]
                    "PERIOD", // [190]
                    "SLASH", // [191]
                    "BACK_QUOTE", // [192]
                    "", // [193]
                    "", // [194]
                    "", // [195]
                    "", // [196]
                    "", // [197]
                    "", // [198]
                    "", // [199]
                    "", // [200]
                    "", // [201]
                    "", // [202]
                    "", // [203]
                    "", // [204]
                    "", // [205]
                    "", // [206]
                    "", // [207]
                    "", // [208]
                    "", // [209]
                    "", // [210]
                    "", // [211]
                    "", // [212]
                    "", // [213]
                    "", // [214]
                    "", // [215]
                    "", // [216]
                    "", // [217]
                    "", // [218]
                    "OPEN_BRACKET", // [219]
                    "BACK_SLASH", // [220]
                    "CLOSE_BRACKET", // [221]
                    "QUOTE", // [222]
                    "", // [223]
                    "META", // [224]
                    "ALTGR", // [225]
                    "", // [226]
                    "WIN_ICO_HELP", // [227]
                    "WIN_ICO_00", // [228]
                    "", // [229]
                    "WIN_ICO_CLEAR", // [230]
                    "", // [231]
                    "", // [232]
                    "WIN_OEM_RESET", // [233]
                    "WIN_OEM_JUMP", // [234]
                    "WIN_OEM_PA1", // [235]
                    "WIN_OEM_PA2", // [236]
                    "WIN_OEM_PA3", // [237]
                    "WIN_OEM_WSCTRL", // [238]
                    "WIN_OEM_CUSEL", // [239]
                    "WIN_OEM_ATTN", // [240]
                    "WIN_OEM_FINISH", // [241]
                    "WIN_OEM_COPY", // [242]
                    "WIN_OEM_AUTO", // [243]
                    "WIN_OEM_ENLW", // [244]
                    "WIN_OEM_BACKTAB", // [245]
                    "ATTN", // [246]
                    "CRSEL", // [247]
                    "EXSEL", // [248]
                    "EREOF", // [249]
                    "PLAY", // [250]
                    "ZOOM", // [251]
                    "", // [252]
                    "PA1", // [253]
                    "WIN_OEM_CLEAR", // [254]
                    "" // [255]
                ];
            };

            export enum MouseEvents {
                LEFT,
                RIGHT,
                MIDDLE,
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

                constructor(private _dst_host: string, private _dst_port: string) {
                 
                    window.addEventListener("resize", this.onresize);
                    window.addEventListener("mousedown", this.onmousedown);//event not needed
                    window.addEventListener("mouseup", this.onmouseup);
                    window.addEventListener("mousemove", this.onmove);
                    window.addEventListener("wheel", this.onwheel);
                
                    window.addEventListener("keydown", this.onkeydown);
                    window.addEventListener("keyup", this.onkeyup);

                    window.addEventListener("dblclick", this.ondblclick);
                    // window.addEventListener("touchend", this.ontouchend);
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
                    console.log("N: '" + Input.KeyEvent.keyboardMap[k.Key] + "'");
                    var spec = Input.SpecialKeys.NO_PRESS_DATA;
                    if (ev.altKey) spec = Input.SpecialKeys.ALT;
                    else if (ev.ctrlKey) spec = Input.SpecialKeys.CTRL;
                    else if (ev.shiftKey) spec = Input.SpecialKeys.SHIFT;
                    ev.preventDefault();//capture the keypress
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
                private TouchTimer: number;
                private CancelTouchClickTimer = (): void => {
                    if (this.TouchTimer != null) {
                        clearTimeout(this.TouchTimer);
                    }
                    this.TouchTimer = null;
                }
                ontouchstart = (ev: TouchEvent): void => {
                    if (ev.touches.length == 1) {
                        this.StartTouchTimer(ev, this.pointerEventToXY(ev));
                    }
                }
                ontouchmove = (ev: TouchEvent): void => {
                    this.CancelTouchClickTimer();
                }
                private StartTouchTimer(ev: TouchEvent, pos: Utilities.Point) {
                    var self = this;
                    if (self.TouchTimer == null) {
                        self.TouchTimer = setTimeout(function () {//single click
                            self.CancelTouchClickTimer();
                            self.handlemouse(0, Input.MousePress.DOWN, pos, 0);
                            // self.handlemouse(0, Input.MousePress.UP, pos, 0);
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
                        this._HTMLCanvasScreenImage.getContext("2d").drawImage(this._OriginalImage, 0, 0, this._HTMLCanvasScreenImage.width, this._HTMLCanvasScreenImage.height);
                    } else if (!this._ScaleImage && this._OriginalImage != null) {
                        if (this._HTMLCanvasScreenImage.height != this._OriginalImage.height || this._HTMLCanvasScreenImage.width != this._OriginalImage.width) {
                            this._HTMLCanvasScreenImage.width = this._OriginalImage.width;
                            this._HTMLCanvasScreenImage.height = this._OriginalImage.height;
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
                            self._HTMLCanvasScreenImage.width = i.width * scale;
                            self._HTMLCanvasScreenImage.height = i.height * scale;
                            self._HTMLCanvasScreenImage.getContext("2d").drawImage(i, 0, 0, self._HTMLCanvasScreenImage.width, self._HTMLCanvasScreenImage.height);
                        } else {
                            self._HTMLCanvasScreenImage.width = i.width;
                            self._HTMLCanvasScreenImage.height = i.height;
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

