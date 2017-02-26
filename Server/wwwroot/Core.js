/// <reference path="zstd.d.ts" />
var SL;
(function (SL) {
    var Remote_Access_Library;
    (function (Remote_Access_Library) {
        var Utilities;
        (function (Utilities) {
            var Point = (function () {
                function Point(X, Y) {
                    this.X = X;
                    this.Y = Y;
                }
                Point.sizeof = function () { return 8; }; //actual bytes used
                Point.FromArray = function (data) {
                    var arr = new Int32Array(data.slice(0, this.sizeof()).buffer);
                    return new Point(arr[0], arr[1]);
                };
                Point.prototype.Fill = function (d, offset) {
                    var dt = new DataView(d, offset);
                    dt.setInt32(0, this.X, true);
                    dt.setInt32(4, this.Y, true);
                };
                return Point;
            }());
            Utilities.Point = Point;
            var Rect = (function () {
                function Rect(Origin, Height, Width) {
                    this.Origin = Origin;
                    this.Height = Height;
                    this.Width = Width;
                }
                Rect.sizeof = function () { return 8 + Point.sizeof(); }; //actual bytes used
                Rect.FromArray = function (data) {
                    var arr = new Int32Array(data.slice(0, this.sizeof()).buffer);
                    return new Rect(new Point(arr[0], arr[1]), arr[2], arr[3]);
                };
                Rect.prototype.Fill = function (d, offset) {
                    this.Origin.Fill(d, offset);
                    var dt = new DataView(d, offset + Point.sizeof());
                    dt.setInt32(0, this.Height, true);
                    dt.setInt32(4, this.Width, true);
                };
                return Rect;
            }());
            Utilities.Rect = Rect;
        })(Utilities = Remote_Access_Library.Utilities || (Remote_Access_Library.Utilities = {}));
        var Input;
        (function (Input) {
            (function (KeyPress) {
                KeyPress[KeyPress["UP"] = 0] = "UP";
                KeyPress[KeyPress["DOWN"] = 1] = "DOWN";
            })(Input.KeyPress || (Input.KeyPress = {}));
            var KeyPress = Input.KeyPress;
            ;
            (function (SpecialKeys) {
                SpecialKeys[SpecialKeys["NO_PRESS_DATA"] = 0] = "NO_PRESS_DATA";
                SpecialKeys[SpecialKeys["ALT"] = 1] = "ALT";
                SpecialKeys[SpecialKeys["CTRL"] = 2] = "CTRL";
                SpecialKeys[SpecialKeys["SHIFT"] = 3] = "SHIFT";
            })(Input.SpecialKeys || (Input.SpecialKeys = {}));
            var SpecialKeys = Input.SpecialKeys;
            ;
            var KeyEvent = (function () {
                function KeyEvent(PressData, Key, SpecialKey) {
                    this.PressData = PressData;
                    this.Key = Key;
                    this.SpecialKey = SpecialKey;
                }
                KeyEvent.sizeof = function () { return 4 + 4 + 4; }; //actual bytes used
                KeyEvent.prototype.Fill = function (d, offset) {
                    var dt = new DataView(d, offset);
                    dt.setUint32(0, this.PressData, true);
                    dt.setInt32(4, this.Key, true);
                    dt.setUint32(4 + 4, this.SpecialKey, true);
                };
                //a mix of ASCII and FLTK enumerations
                KeyEvent.keyboardMap = [
                    0,
                    0,
                    0,
                    0xff1b,
                    0,
                    0,
                    0xff68,
                    0,
                    0xff08,
                    0xff09,
                    0,
                    0,
                    0,
                    0xff0d,
                    0,
                    0,
                    0xffe1,
                    0xffe3,
                    0xffe9,
                    0xff13,
                    0xffe5,
                    0xff2e,
                    0xff2f,
                    0,
                    0,
                    0,
                    0,
                    0xff1b,
                    0,
                    0,
                    0,
                    0,
                    0x0020,
                    0xff55,
                    0xff56,
                    0xff57,
                    0xff50,
                    0xff51,
                    0xff52,
                    0xff53,
                    0xff54,
                    0,
                    0xff61,
                    0x000d,
                    0xff61,
                    0xff63,
                    0xffff,
                    0,
                    0x30,
                    0x31,
                    0x32,
                    0x33,
                    0x34,
                    0x35,
                    0x36,
                    0x37,
                    0x38,
                    0x39,
                    0x3A,
                    0x3B,
                    0x3C,
                    0x3D,
                    0x3E,
                    0x3F,
                    0x40,
                    0x41,
                    0x42,
                    0x43,
                    0x44,
                    0x45,
                    0x46,
                    0x47,
                    0x48,
                    0x49,
                    0x4A,
                    0x4B,
                    0x4C,
                    0x4D,
                    0x4E,
                    0x4F,
                    0x50,
                    0x51,
                    0x52,
                    0x53,
                    0x54,
                    0x55,
                    0x56,
                    0x57,
                    0x58,
                    0x59,
                    0x5A,
                    0xffe7,
                    0,
                    0,
                    0,
                    0,
                    0x30,
                    0x31,
                    0x32,
                    0x33,
                    0x34,
                    0x35,
                    0x36,
                    0x37,
                    0x38,
                    0x39,
                    0xf06A,
                    0xf06B,
                    0xf06C,
                    0xf06D,
                    0xf06E,
                    0xf06F,
                    0xffbd + 1,
                    0xffbd + 2,
                    0xffbd + 3,
                    0xffbd + 4,
                    0xffbd + 5,
                    0xffbd + 6,
                    0xffbd + 7,
                    0xffbd + 8,
                    0xffbd + 9,
                    0xffbd + 10,
                    0xffbd + 11,
                    0xffbd + 12,
                    0xffbd + 13,
                    0xffbd + 14,
                    0xffbd + 15,
                    0xffbd + 16,
                    0xffbd + 17,
                    0xffbd + 18,
                    0xffbd + 19,
                    0xffbd + 20,
                    0xffbd + 21,
                    0xffbd + 22,
                    0xffbd + 23,
                    0xffbd + 24,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0xff7f,
                    0xff14,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0x60,
                    0x31,
                    0x27,
                    0x33,
                    0x34,
                    0x35,
                    0x37,
                    0x2d,
                    0x28,
                    0x29,
                    0x38,
                    0x3d,
                    0x5c,
                    0x2d,
                    0x5b,
                    0x5d,
                    0x60,
                    0,
                    0,
                    0,
                    0,
                    0xEF12,
                    0xEF11,
                    0xEF13,
                    0,
                    0,
                    0x3a,
                    0x3d,
                    0x2c,
                    0x2d,
                    0x2e,
                    0x2f,
                    0x36,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0x5b,
                    0x5c,
                    0x5d,
                    0x27,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0 // [255]
                ];
                return KeyEvent;
            }());
            Input.KeyEvent = KeyEvent;
            ;
            (function (MouseEvents) {
                MouseEvents[MouseEvents["NO_EVENTDATA"] = 0] = "NO_EVENTDATA";
                MouseEvents[MouseEvents["LEFT"] = 1] = "LEFT";
                MouseEvents[MouseEvents["MIDDLE"] = 2] = "MIDDLE";
                MouseEvents[MouseEvents["RIGHT"] = 3] = "RIGHT";
                MouseEvents[MouseEvents["SCROLL"] = 4] = "SCROLL";
            })(Input.MouseEvents || (Input.MouseEvents = {}));
            var MouseEvents = Input.MouseEvents;
            ;
            (function (MousePress) {
                MousePress[MousePress["NO_PRESS_DATA"] = 0] = "NO_PRESS_DATA";
                MousePress[MousePress["UP"] = 1] = "UP";
                MousePress[MousePress["DOWN"] = 2] = "DOWN";
                MousePress[MousePress["DBLCLICK"] = 3] = "DBLCLICK";
            })(Input.MousePress || (Input.MousePress = {}));
            var MousePress = Input.MousePress;
            ;
            var MouseEvent = (function () {
                function MouseEvent(EventData, Pos, ScrollDelta, PressData) {
                    if (EventData === void 0) { EventData = MouseEvents.NO_EVENTDATA; }
                    if (Pos === void 0) { Pos = new Utilities.Point(0, 0); }
                    if (ScrollDelta === void 0) { ScrollDelta = 0; }
                    if (PressData === void 0) { PressData = MousePress.NO_PRESS_DATA; }
                    this.EventData = EventData;
                    this.Pos = Pos;
                    this.ScrollDelta = ScrollDelta;
                    this.PressData = PressData;
                }
                MouseEvent.sizeof = function () { return 4 + Utilities.Point.sizeof() + 4 + 4; }; //actual bytes used
                MouseEvent.prototype.Fill = function (d, offset) {
                    var dt = new DataView(d, offset);
                    dt.setUint32(0, this.EventData, true);
                    this.Pos.Fill(d, offset + 4);
                    dt.setInt32(offset + 4 + Utilities.Point.sizeof(), this.ScrollDelta, true);
                    dt.setUint32(offset + 4 + Utilities.Point.sizeof() + 4, this.PressData, true);
                };
                return MouseEvent;
            }());
            Input.MouseEvent = MouseEvent;
            ;
        })(Input = Remote_Access_Library.Input || (Remote_Access_Library.Input = {}));
        var Network;
        (function (Network) {
            (function (PACKET_TYPES) {
                PACKET_TYPES[PACKET_TYPES["INVALID"] = 0] = "INVALID";
                PACKET_TYPES[PACKET_TYPES["HTTP_MSG"] = 1] = "HTTP_MSG";
                PACKET_TYPES[PACKET_TYPES["SCREENIMAGE"] = 2] = "SCREENIMAGE";
                PACKET_TYPES[PACKET_TYPES["SCREENIMAGEDIF"] = 3] = "SCREENIMAGEDIF";
                PACKET_TYPES[PACKET_TYPES["MOUSEPOS"] = 4] = "MOUSEPOS";
                PACKET_TYPES[PACKET_TYPES["MOUSEIMAGE"] = 5] = "MOUSEIMAGE";
                PACKET_TYPES[PACKET_TYPES["KEYEVENT"] = 6] = "KEYEVENT";
                PACKET_TYPES[PACKET_TYPES["MOUSEEVENT"] = 7] = "MOUSEEVENT";
                //use LAST_PACKET_TYPE as the starting point of your custom packet types. Everything before this is used internally by the library
                PACKET_TYPES[PACKET_TYPES["LAST_PACKET_TYPE"] = 8] = "LAST_PACKET_TYPE";
            })(Network.PACKET_TYPES || (Network.PACKET_TYPES = {}));
            var PACKET_TYPES = Network.PACKET_TYPES;
            var PacketHeader = (function () {
                function PacketHeader(Packet_Type, Payload_Length, UncompressedLength) {
                    if (Packet_Type === void 0) { Packet_Type = PACKET_TYPES.INVALID; }
                    if (Payload_Length === void 0) { Payload_Length = 0; }
                    if (UncompressedLength === void 0) { UncompressedLength = 0; }
                    this.Packet_Type = Packet_Type;
                    this.Payload_Length = Payload_Length;
                    this.UncompressedLength = UncompressedLength;
                    this.Payload = new ArrayBuffer(this.Payload_Length);
                }
                PacketHeader.prototype.sizeof = function () { return 12; }; //actual bytes used
                PacketHeader.prototype.Fill = function (arr) {
                    arr[0] = this.Packet_Type;
                    arr[1] = this.Payload_Length;
                    arr[2] = this.UncompressedLength;
                };
                return PacketHeader;
            }());
            Network.PacketHeader = PacketHeader;
            var SocketStats = (function () {
                function SocketStats() {
                }
                return SocketStats;
            }());
            Network.SocketStats = SocketStats;
            var ClientDriver = (function () {
                function ClientDriver(_dst_host, _dst_port) {
                    var _this = this;
                    this._dst_host = _dst_host;
                    this._dst_port = _dst_port;
                    this._ScaleImage = false;
                    this._DivRootId = 'SLRATROOTID123'; //this is an id used internally to double check if a canvas has allready been inserted 
                    this._LeftMouseStatus = Input.MousePress.UP;
                    this.Start = function () {
                        var testroot = document.getElementById(_this._DivRootId);
                        if (testroot !== null) {
                            document.removeChild(testroot);
                        }
                        _this._HTMLDivRoot = document.createElement('div');
                        _this._HTMLDivRoot.id = _this._DivRootId;
                        _this._HTMLDivRoot.style.position = 'relative';
                        _this._HTMLCanvasScreenImage = document.createElement('canvas');
                        _this._HTMLCanvasScreenImage.style.position = 'absolute';
                        _this._HTMLCanvasScreenImage.style.left = _this._HTMLCanvasScreenImage.style.top = _this._HTMLCanvasScreenImage.style.zIndex = '0';
                        _this._HTMLCanvasMouseImage = document.createElement('canvas');
                        _this._HTMLCanvasMouseImage.style.position = 'absolute';
                        _this._HTMLCanvasMouseImage.style.left = _this._HTMLCanvasMouseImage.style.top = '0';
                        _this._HTMLCanvasMouseImage.style.zIndex = '1';
                        _this._HTMLDivRoot.appendChild(_this._HTMLCanvasScreenImage);
                        _this._HTMLDivRoot.appendChild(_this._HTMLCanvasMouseImage);
                        document.body.appendChild(_this._HTMLDivRoot); //add to the dom
                        _this._ClientNetworkDriver = new ClientNetworkDriver(_this, _this._dst_host, _this._dst_port);
                        _this._ClientNetworkDriver.Start();
                    };
                    this.Stop = function () {
                        _this._ClientNetworkDriver.Stop();
                        _this._ClientNetworkDriver = null;
                        var testroot = document.getElementById(_this._DivRootId);
                        if (testroot !== null) {
                            document.removeChild(testroot);
                        }
                    };
                    this.ScaleView = function (b) {
                        _this._ScaleImage = b;
                    };
                    this.onkeydown = function (ev) {
                        _this.handle_key(ev, new Input.KeyEvent(Input.KeyPress.DOWN, ev.keyCode || ev.which, Input.SpecialKeys.NO_PRESS_DATA));
                    };
                    this.onkeyup = function (ev) {
                        _this.handle_key(ev, new Input.KeyEvent(Input.KeyPress.UP, ev.keyCode || ev.which, Input.SpecialKeys.NO_PRESS_DATA));
                    };
                    this.handle_key = function (ev, k) {
                        k.SpecialKey = Input.SpecialKeys.NO_PRESS_DATA;
                        k.Key = Input.KeyEvent.keyboardMap[k.Key];
                        console.log("Key: '" + k.Key + "'");
                        if (ev.altKey)
                            k.SpecialKey = Input.SpecialKeys.ALT;
                        else if (ev.ctrlKey)
                            k.SpecialKey = Input.SpecialKeys.CTRL;
                        else if (ev.shiftKey)
                            k.SpecialKey = Input.SpecialKeys.SHIFT;
                        ev.preventDefault(); //capture the keypress
                        if (!_this._ClientNetworkDriver.ConnectedToSelf()) {
                            _this._ClientNetworkDriver.SendKeyEvent(k); //sending input to yourself will lead to an infinite loop...
                        }
                    };
                    this.pointerEventToXY = function (e) {
                        var out = new Utilities.Point(0, 0);
                        if (e.type == 'touchstart' || e.type == 'touchmove' || e.type == 'touchend' || e.type == 'touchcancel') {
                            var touch = e.touches[0] || e.changedTouches[0];
                            out.X = touch.pageX;
                            out.Y = touch.pageY;
                        }
                        else if (e.type == 'mousedown' || e.type == 'mouseup' || e.type == 'mousemove' || e.type == 'mouseover' || e.type == 'mouseout' || e.type == 'mouseenter' || e.type == 'mouseleave') {
                            out.Y = e.pageY;
                            out.X = e.pageX;
                        }
                        return out;
                    };
                    this.ontouchend = function (ev) {
                        if (_this._LeftMouseStatus == Input.MousePress.DOWN) {
                            _this._LeftMouseStatus = Input.MousePress.UP;
                            _this.handlemouse(0, Input.MousePress.UP, _this.pointerEventToXY(ev), 0);
                        }
                    };
                    this.ontouchstart = function (ev) {
                        if (ev.touches.length == 1) {
                            _this.StartTouchTimer(ev, _this.pointerEventToXY(ev));
                        }
                    };
                    this.ontouchmove = function (ev) {
                        _this.CancelTouchClickTimer();
                        if (ev.touches.length == 1) {
                            if (_this._LeftMouseStatus == Input.MousePress.UP) {
                                _this._LeftMouseStatus = Input.MousePress.DOWN;
                                _this.handlemouse(0, Input.MousePress.DOWN, _this.pointerEventToXY(ev), 0);
                            }
                        }
                        if (_this._LeftMouseStatus == Input.MousePress.DOWN) {
                            _this.handlemouse(-1, Input.MousePress.NO_PRESS_DATA, _this.pointerEventToXY(ev), 0);
                            ev.preventDefault();
                        }
                    };
                    this.CancelTouchClickTimer = function () {
                        if (_this.TouchTimer != null) {
                            clearTimeout(_this.TouchTimer);
                        }
                        _this.TouchTimer = null;
                    };
                    this.onmousedown = function (ev) {
                        _this.handlemouse(ev.button, Input.MousePress.DOWN, _this.pointerEventToXY(ev), 0);
                    };
                    this.ondblclick = function (ev) {
                        _this.handlemouse(ev.button, Input.MousePress.DBLCLICK, _this.pointerEventToXY(ev), 0);
                    };
                    this.onmouseup = function (ev) {
                        _this.handlemouse(ev.button, Input.MousePress.UP, _this.pointerEventToXY(ev), 0);
                    };
                    this.onmove = function (ev) {
                        _this.handlemouse(-1, Input.MousePress.NO_PRESS_DATA, _this.pointerEventToXY(ev), 0);
                    };
                    this.onwheel = function (ev) {
                        _this.handlemouse(-1, Input.MousePress.NO_PRESS_DATA, _this.pointerEventToXY(ev), ev.deltaY);
                    };
                    this.handlemouse = function (button, press, pos, scroll) {
                        var ev = new Input.MouseEvent();
                        var scale = _this.GetScalingFactor();
                        ev.Pos = pos;
                        ev.Pos.X = ev.Pos.X / scale;
                        ev.Pos.Y = ev.Pos.Y / scale;
                        if (scroll != 0) {
                            ev.ScrollDelta = scroll < 0 ? -1 : 1; //force a -1 or 1 because browsers send different values for each scroll tick.
                            ev.EventData = Input.MouseEvents.SCROLL;
                        }
                        else {
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
                            }
                            ;
                        }
                        ev.PressData = press;
                        if (!_this._ClientNetworkDriver.ConnectedToSelf()) {
                            _this._ClientNetworkDriver.SendMouse(ev); //sending input to yourself will lead to an infinite loop...
                        }
                    };
                    this.onresize = function (ev) {
                        if (_this._ScaleImage && _this._OriginalImage != null) {
                            var scale = _this.GetScalingFactor();
                            _this._HTMLCanvasScreenImage.width = _this._OriginalImage.width * scale;
                            _this._HTMLCanvasScreenImage.height = _this._OriginalImage.height * scale;
                            _this._HTMLDivRoot.style.width = _this._HTMLCanvasScreenImage.width + 'px';
                            _this._HTMLDivRoot.style.height = _this._HTMLCanvasScreenImage.height + 'px';
                            _this._HTMLCanvasScreenImage.getContext("2d").drawImage(_this._OriginalImage, 0, 0, _this._HTMLCanvasScreenImage.width, _this._HTMLCanvasScreenImage.height);
                        }
                        else if (!_this._ScaleImage && _this._OriginalImage != null) {
                            if (_this._HTMLCanvasScreenImage.height != _this._OriginalImage.height || _this._HTMLCanvasScreenImage.width != _this._OriginalImage.width) {
                                _this._HTMLCanvasScreenImage.width = _this._OriginalImage.width;
                                _this._HTMLCanvasScreenImage.height = _this._OriginalImage.height;
                                _this._HTMLDivRoot.style.width = _this._HTMLCanvasScreenImage.width + 'px';
                                _this._HTMLDivRoot.style.height = _this._HTMLCanvasScreenImage.height + 'px';
                                _this._HTMLCanvasScreenImage.getContext("2d").drawImage(_this._OriginalImage, 0, 0);
                            }
                        }
                    };
                    this.OnReceive_ImageDif = function (socket, rect, img) {
                        if (_this._OriginalImage === null)
                            return;
                        "use strict";
                        //console.log('coords' + coords.X + ' ' + coords.Y + ' ' + coords.Width + ' ' + coords.Height);
                        var i = new Image();
                        i.src = "data:image/jpeg;base64," + img;
                        var self = _this;
                        i.onload = function () {
                            if (self._ScaleImage) {
                                var scale = self.GetScalingFactor();
                                self._HTMLCanvasScreenImage.getContext("2d").drawImage(i, rect.Origin.X * scale, rect.Origin.Y * scale, rect.Width * scale, rect.Height * scale);
                            }
                            else {
                                self._HTMLCanvasScreenImage.getContext("2d").drawImage(i, rect.Origin.X, rect.Origin.Y);
                            }
                            // console.log("ctx.drawImage" + coords.Y, "  " + coords.X);
                        };
                        i.onerror = function (stuff) {
                            console.log("Img Onerror:", stuff);
                        };
                    };
                    this.OnReceive_Image = function (socket, rect, img) {
                        "use strict";
                        //console.log('coords' + coords.X + ' ' + coords.Y + ' ' + coords.Width + ' ' + coords.Height);
                        var i = new Image();
                        i.src = "data:image/jpeg;base64," + img;
                        var self = _this;
                        i.onload = function () {
                            if (self._ScaleImage) {
                                var scale = self.GetScalingFactor();
                                self._HTMLCanvasScreenImage.width = i.width * scale;
                                self._HTMLCanvasScreenImage.height = i.height * scale;
                                self._HTMLDivRoot.style.width = self._HTMLCanvasScreenImage.width + 'px';
                                self._HTMLDivRoot.style.height = self._HTMLCanvasScreenImage.height + 'px';
                                self._HTMLCanvasScreenImage.getContext("2d").drawImage(i, 0, 0, self._HTMLCanvasScreenImage.width, self._HTMLCanvasScreenImage.height);
                            }
                            else {
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
                    };
                    this.OnReceive_MouseImage = function (socket, point, img) {
                        "use strict";
                        //console.log('coords' + coords.X + ' ' + coords.Y + ' ' + coords.Width + ' ' + coords.Height);
                        _this._HTMLCanvasMouseImage.width = point.X;
                        _this._HTMLCanvasMouseImage.height = point.Y;
                        try {
                            _this._Cursor = _this._HTMLCanvasMouseImage.getContext("2d").createImageData(point.X, point.Y);
                            for (var i = 0; i < _this._Cursor.data.length; i += 4) {
                                _this._Cursor.data[i + 0] = img[i + 0];
                                _this._Cursor.data[i + 1] = img[i + 1];
                                _this._Cursor.data[i + 2] = img[i + 2];
                                _this._Cursor.data[i + 3] = img[i + 3];
                            }
                            _this._HTMLCanvasMouseImage.getContext("2d").putImageData(_this._Cursor, 0, 0);
                        }
                        catch (e) {
                            console.log(e.message);
                        }
                    };
                    this.OnReceive_MousePos = function (socket, pos) {
                        if (_this._ScaleImage) {
                            var scale = _this.GetScalingFactor();
                            _this._HTMLCanvasMouseImage.style.top = (pos.Y * scale) + "px";
                            _this._HTMLCanvasMouseImage.style.left = (pos.X * scale) + "px";
                        }
                        else {
                            _this._HTMLCanvasMouseImage.style.top = pos.Y + "px";
                            _this._HTMLCanvasMouseImage.style.left = pos.X + "px";
                        }
                    };
                    window.addEventListener("resize", this.onresize);
                    window.addEventListener("mousedown", this.onmousedown); //event not needed
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
                ClientDriver.prototype.StartTouchTimer = function (ev, pos) {
                    var self = this;
                    if (self.TouchTimer == null) {
                        self.TouchTimer = setTimeout(function () {
                            self.CancelTouchClickTimer();
                            self.handlemouse(0, Input.MousePress.DOWN, pos, 0);
                            self.handlemouse(0, Input.MousePress.UP, pos, 0);
                        }, 500);
                    }
                    else {
                        ev.preventDefault();
                        self.CancelTouchClickTimer();
                        self.handlemouse(0, Input.MousePress.DBLCLICK, pos, 0);
                    }
                };
                ClientDriver.prototype.GetScalingFactor = function () {
                    if (this._ScaleImage && this._OriginalImage != null) {
                        return window.innerHeight / this._OriginalImage.height;
                    }
                    else {
                        return 1.0;
                    }
                };
                return ClientDriver;
            }());
            Network.ClientDriver = ClientDriver;
            var ClientNetworkDriver = (function () {
                function ClientNetworkDriver(_IClientDriver, _dst_host, _dst_port) {
                    var _this = this;
                    this._IClientDriver = _IClientDriver;
                    this._dst_host = _dst_host;
                    this._dst_port = _dst_port;
                    this._ConnectedToSelf = false;
                    this.Start = function () {
                        var connectstring = "";
                        if (window.location.protocol != "https:") {
                            connectstring += "ws://";
                        }
                        else {
                            connectstring += "wss://";
                        }
                        _this._ConnectedToSelf = (_this._dst_host == '127.0.0.1') || (_this._dst_host == 'localhost') || (_this._dst_host == '::1');
                        connectstring += _this._dst_host + ":" + _this._dst_port + "/rdp";
                        _this._Socket = new WebSocket(connectstring);
                        _this._Socket.binaryType = 'arraybuffer';
                        _this._Socket.onopen = _this.OnOpen;
                        _this._Socket.onclose = _this.OnClose;
                        _this._Socket.onmessage = _this.OnMessage;
                    };
                    this.ConnectedToSelf = function () {
                        return _this._ConnectedToSelf;
                    };
                    this.Stop = function () {
                        _this._Socket.close(1001, "Web Browser called Stop()");
                        _this._Socket = null;
                    };
                    this.SendMouse = function (m) {
                        var pac = new PacketHeader(PACKET_TYPES.MOUSEEVENT, Input.MouseEvent.sizeof(), Input.MouseEvent.sizeof());
                        m.Fill(pac.Payload, 0);
                        _this.Compress_and_Send(pac);
                    };
                    this.SendKeyEvent = function (ev) {
                        var pac = new PacketHeader(PACKET_TYPES.KEYEVENT, Input.KeyEvent.sizeof(), Input.KeyEvent.sizeof());
                        ev.Fill(pac.Payload, 0);
                        _this.Compress_and_Send(pac);
                    };
                    this.Compress_and_Send = function (p) {
                        var t0 = performance.now();
                        var srcPtr = Module._malloc(p.Payload_Length);
                        _this._TotalMemoryUsed += p.Payload_Length;
                        var srcbuff = new Uint8Array(Module.HEAPU8.buffer, srcPtr, p.Payload_Length); //get enough space in the heap
                        srcbuff.set(new Uint8Array(p.Payload, 0, p.Payload_Length)); //copy the data to the newly allocated memory
                        var dstsize = _ZSTD_compressBound(p.UncompressedLength + p.sizeof());
                        var dsttr = Module._malloc(dstsize); //get worst case space requirements for dst buffer
                        _this._TotalMemoryUsed += dstsize;
                        var dstbuff = new Uint8Array(Module.HEAPU8.buffer, dsttr, dstsize); // dont write to the header portion
                        p.Payload_Length = _ZSTD_compress(dstbuff.byteOffset + p.sizeof(), dstsize - p.sizeof(), srcbuff.byteOffset, p.Payload_Length, 3);
                        if (_ZSTD_isError(p.Payload_Length) > 0) {
                            console.log('zstd error' + _ZSTD_getErrorName(p.Payload_Length));
                        }
                        p.Fill(new Uint32Array(Module.HEAPU8.buffer, dsttr, p.sizeof()));
                        var t1 = performance.now();
                        //comment this line out to see performance issues... My machine takes 0 to 6 ms to complete each receive
                        // console.log("took " + (t1 - t0) + " milliseconds to Compress the packet")
                        Module._free(srcPtr);
                        _this._TotalMemoryUsed -= p.Payload_Length;
                        var test = dstbuff.buffer.slice(dsttr, dsttr + p.Payload_Length + p.sizeof());
                        var teu = new Uint8Array(test, 0, p.Payload_Length + p.sizeof());
                        _this._Socket.send(teu);
                        Module._free(dsttr);
                        _this._TotalMemoryUsed -= dstsize;
                    };
                    this.OnMessage = function (ev) {
                        var t0 = performance.now();
                        var packetheader = new PacketHeader();
                        var data = new DataView(ev.data);
                        packetheader.Packet_Type = data.getInt32(0, true);
                        packetheader.Payload_Length = data.getInt32(4, true);
                        packetheader.UncompressedLength = data.getInt32(8, true);
                        var srcPtr = Module._malloc(packetheader.Payload_Length);
                        _this._TotalMemoryUsed += packetheader.Payload_Length;
                        var srcbuff = new Uint8Array(Module.HEAPU8.buffer, srcPtr, packetheader.Payload_Length);
                        srcbuff.set(new Uint8Array(ev.data, packetheader.sizeof()));
                        var dsttr = Module._malloc(packetheader.UncompressedLength);
                        _this._TotalMemoryUsed += packetheader.UncompressedLength;
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
                                _this.Image(dstbuff);
                                break;
                            case (PACKET_TYPES.SCREENIMAGEDIF):
                                _this.ImageDif(dstbuff);
                                break;
                            case (PACKET_TYPES.MOUSEIMAGE):
                                _this.MouseImage(dstbuff);
                                break;
                            case (PACKET_TYPES.MOUSEPOS):
                                _this.MousePos(dstbuff);
                                break;
                            default:
                                console.log('received unknown packet type: ' + packetheader.Packet_Type);
                                break;
                        }
                        ;
                        Module._free(dsttr);
                        _this._TotalMemoryUsed -= packetheader.UncompressedLength;
                        Module._free(srcPtr);
                        _this._TotalMemoryUsed -= packetheader.Payload_Length;
                        t1 = performance.now();
                        //comment this line out to see performance issues... My machine takes 0 to 6 ms to complete each receive
                        //  console.log("took " + (t1 - t0) + " milliseconds to process the receive loop");
                    };
                    this.Image = function (data) {
                        _this._IClientDriver.OnReceive_Image(_this._Socket, Utilities.Rect.FromArray(data), _this._arrayBufferToBase64(data, Utilities.Rect.sizeof()));
                    };
                    this.ImageDif = function (data) {
                        _this._IClientDriver.OnReceive_ImageDif(_this._Socket, Utilities.Rect.FromArray(data), _this._arrayBufferToBase64(data, Utilities.Rect.sizeof()));
                    };
                    this.MouseImage = function (data) {
                        _this._IClientDriver.OnReceive_MouseImage(_this._Socket, Utilities.Point.FromArray(data), new Uint8Array(data.slice(Utilities.Point.sizeof()).buffer));
                    };
                    this.MousePos = function (data) {
                        _this._IClientDriver.OnReceive_MousePos(_this._Socket, Utilities.Point.FromArray(data));
                    };
                }
                ClientNetworkDriver.prototype._arrayBufferToBase64 = function (buffer, offset) {
                    var binary = '';
                    var len = buffer.byteLength;
                    for (var i = offset; i < len; i++) {
                        binary += String.fromCharCode(buffer[i]);
                    }
                    return window.btoa(binary);
                };
                ClientNetworkDriver.prototype.OnOpen = function (ev) {
                    window.document.title = "Connected: " + window.location.hostname + ":6001/rdp";
                    console.log('Socket Opened');
                };
                ClientNetworkDriver.prototype.OnClose = function (ev) {
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
                    else if (ev.code == 1010)
                        reason = "An endpoint (client) is terminating the connection because it has expected the server to negotiate one or more extension, but the server didn't return them in the response message of the WebSocket handshake. <br /> Specifically, the extensions that are needed are: " + ev.reason;
                    else if (ev.code == 1011)
                        reason = "A server is terminating the connection because it encountered an unexpected condition that prevented it from fulfilling the request.";
                    else if (ev.code == 1015)
                        reason = "The connection was closed due to a failure to perform a TLS handshake (e.g., the server certificate can't be verified).";
                    else
                        reason = "Unknown reason";
                    alert(reason);
                    console.log('Socket Closed: ' + reason);
                };
                return ClientNetworkDriver;
            }());
            Network.ClientNetworkDriver = ClientNetworkDriver;
        })(Network = Remote_Access_Library.Network || (Remote_Access_Library.Network = {}));
    })(Remote_Access_Library = SL.Remote_Access_Library || (SL.Remote_Access_Library = {}));
})(SL || (SL = {}));
