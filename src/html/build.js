var __extends = (this && this.__extends) || (function () {
    var extendStatics = Object.setPrototypeOf ||
        ({ __proto__: [] } instanceof Array && function (d, b) { d.__proto__ = b; }) ||
        function (d, b) { for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p]; };
    return function (d, b) {
        extendStatics(d, b);
        function __() { this.constructor = d; }
        d.prototype = b === null ? Object.create(b) : (__.prototype = b.prototype, new __());
    };
})();
define("Input_Lite", ["require", "exports"], function (require, exports) {
    "use strict";
    Object.defineProperty(exports, "__esModule", { value: true });
    var KeyCodes;
    (function (KeyCodes) {
        KeyCodes[KeyCodes["KEY_A"] = 4] = "KEY_A";
        KeyCodes[KeyCodes["KEY_B"] = 5] = "KEY_B";
        KeyCodes[KeyCodes["KEY_C"] = 6] = "KEY_C";
        KeyCodes[KeyCodes["KEY_D"] = 7] = "KEY_D";
        KeyCodes[KeyCodes["KEY_E"] = 8] = "KEY_E";
        KeyCodes[KeyCodes["KEY_F"] = 9] = "KEY_F";
        KeyCodes[KeyCodes["KEY_G"] = 10] = "KEY_G";
        KeyCodes[KeyCodes["KEY_H"] = 11] = "KEY_H";
        KeyCodes[KeyCodes["KEY_I"] = 12] = "KEY_I";
        KeyCodes[KeyCodes["KEY_J"] = 13] = "KEY_J";
        KeyCodes[KeyCodes["KEY_K"] = 14] = "KEY_K";
        KeyCodes[KeyCodes["KEY_L"] = 15] = "KEY_L";
        KeyCodes[KeyCodes["KEY_M"] = 16] = "KEY_M";
        KeyCodes[KeyCodes["KEY_N"] = 17] = "KEY_N";
        KeyCodes[KeyCodes["KEY_O"] = 18] = "KEY_O";
        KeyCodes[KeyCodes["KEY_P"] = 19] = "KEY_P";
        KeyCodes[KeyCodes["KEY_Q"] = 20] = "KEY_Q";
        KeyCodes[KeyCodes["KEY_R"] = 21] = "KEY_R";
        KeyCodes[KeyCodes["KEY_S"] = 22] = "KEY_S";
        KeyCodes[KeyCodes["KEY_T"] = 23] = "KEY_T";
        KeyCodes[KeyCodes["KEY_U"] = 24] = "KEY_U";
        KeyCodes[KeyCodes["KEY_V"] = 25] = "KEY_V";
        KeyCodes[KeyCodes["KEY_W"] = 26] = "KEY_W";
        KeyCodes[KeyCodes["KEY_X"] = 27] = "KEY_X";
        KeyCodes[KeyCodes["KEY_Y"] = 28] = "KEY_Y";
        KeyCodes[KeyCodes["KEY_Z"] = 29] = "KEY_Z";
        KeyCodes[KeyCodes["KEY_1"] = 30] = "KEY_1";
        KeyCodes[KeyCodes["KEY_2"] = 31] = "KEY_2";
        KeyCodes[KeyCodes["KEY_3"] = 32] = "KEY_3";
        KeyCodes[KeyCodes["KEY_4"] = 33] = "KEY_4";
        KeyCodes[KeyCodes["KEY_5"] = 34] = "KEY_5";
        KeyCodes[KeyCodes["KEY_6"] = 35] = "KEY_6";
        KeyCodes[KeyCodes["KEY_7"] = 36] = "KEY_7";
        KeyCodes[KeyCodes["KEY_8"] = 37] = "KEY_8";
        KeyCodes[KeyCodes["KEY_9"] = 38] = "KEY_9";
        KeyCodes[KeyCodes["KEY_0"] = 39] = "KEY_0";
        KeyCodes[KeyCodes["KEY_Enter"] = 40] = "KEY_Enter";
        KeyCodes[KeyCodes["KEY_Escape"] = 41] = "KEY_Escape";
        KeyCodes[KeyCodes["KEY_Backspace"] = 42] = "KEY_Backspace";
        KeyCodes[KeyCodes["KEY_Tab"] = 43] = "KEY_Tab";
        KeyCodes[KeyCodes["KEY_Space"] = 44] = "KEY_Space";
        KeyCodes[KeyCodes["KEY_Minus"] = 45] = "KEY_Minus";
        KeyCodes[KeyCodes["KEY_Equals"] = 46] = "KEY_Equals";
        KeyCodes[KeyCodes["KEY_LeftBracket"] = 47] = "KEY_LeftBracket";
        KeyCodes[KeyCodes["KEY_RightBracket"] = 48] = "KEY_RightBracket";
        KeyCodes[KeyCodes["KEY_Backslash"] = 49] = "KEY_Backslash";
        KeyCodes[KeyCodes["KEY_Semicolon"] = 51] = "KEY_Semicolon";
        KeyCodes[KeyCodes["KEY_Quote"] = 52] = "KEY_Quote";
        KeyCodes[KeyCodes["KEY_Grave"] = 53] = "KEY_Grave";
        KeyCodes[KeyCodes["KEY_Comma"] = 54] = "KEY_Comma";
        KeyCodes[KeyCodes["KEY_Period"] = 55] = "KEY_Period";
        KeyCodes[KeyCodes["KEY_Slash"] = 56] = "KEY_Slash";
        KeyCodes[KeyCodes["KEY_CapsLock"] = 57] = "KEY_CapsLock";
        KeyCodes[KeyCodes["KEY_F1"] = 58] = "KEY_F1";
        KeyCodes[KeyCodes["KEY_F2"] = 59] = "KEY_F2";
        KeyCodes[KeyCodes["KEY_F3"] = 60] = "KEY_F3";
        KeyCodes[KeyCodes["KEY_F4"] = 61] = "KEY_F4";
        KeyCodes[KeyCodes["KEY_F5"] = 62] = "KEY_F5";
        KeyCodes[KeyCodes["KEY_F6"] = 63] = "KEY_F6";
        KeyCodes[KeyCodes["KEY_F7"] = 64] = "KEY_F7";
        KeyCodes[KeyCodes["KEY_F8"] = 65] = "KEY_F8";
        KeyCodes[KeyCodes["KEY_F9"] = 66] = "KEY_F9";
        KeyCodes[KeyCodes["KEY_F10"] = 67] = "KEY_F10";
        KeyCodes[KeyCodes["KEY_F11"] = 68] = "KEY_F11";
        KeyCodes[KeyCodes["KEY_F12"] = 69] = "KEY_F12";
        KeyCodes[KeyCodes["KEY_PrintScreen"] = 70] = "KEY_PrintScreen";
        KeyCodes[KeyCodes["KEY_ScrollLock"] = 71] = "KEY_ScrollLock";
        KeyCodes[KeyCodes["KEY_Pause"] = 72] = "KEY_Pause";
        KeyCodes[KeyCodes["KEY_Insert"] = 73] = "KEY_Insert";
        KeyCodes[KeyCodes["KEY_Home"] = 74] = "KEY_Home";
        KeyCodes[KeyCodes["KEY_PageUp"] = 75] = "KEY_PageUp";
        KeyCodes[KeyCodes["KEY_Delete"] = 76] = "KEY_Delete";
        KeyCodes[KeyCodes["KEY_End"] = 77] = "KEY_End";
        KeyCodes[KeyCodes["KEY_PageDown"] = 78] = "KEY_PageDown";
        KeyCodes[KeyCodes["KEY_Right"] = 79] = "KEY_Right";
        KeyCodes[KeyCodes["KEY_Left"] = 80] = "KEY_Left";
        KeyCodes[KeyCodes["KEY_Down"] = 81] = "KEY_Down";
        KeyCodes[KeyCodes["KEY_Up"] = 82] = "KEY_Up";
        KeyCodes[KeyCodes["KP_NumLock"] = 83] = "KP_NumLock";
        KeyCodes[KeyCodes["KP_Divide"] = 84] = "KP_Divide";
        KeyCodes[KeyCodes["KP_Multiply"] = 85] = "KP_Multiply";
        KeyCodes[KeyCodes["KP_Subtract"] = 86] = "KP_Subtract";
        KeyCodes[KeyCodes["KP_Add"] = 87] = "KP_Add";
        KeyCodes[KeyCodes["KP_Enter"] = 88] = "KP_Enter";
        KeyCodes[KeyCodes["KP_1"] = 89] = "KP_1";
        KeyCodes[KeyCodes["KP_2"] = 90] = "KP_2";
        KeyCodes[KeyCodes["KP_3"] = 91] = "KP_3";
        KeyCodes[KeyCodes["KP_4"] = 92] = "KP_4";
        KeyCodes[KeyCodes["KP_5"] = 93] = "KP_5";
        KeyCodes[KeyCodes["KP_6"] = 94] = "KP_6";
        KeyCodes[KeyCodes["KP_7"] = 95] = "KP_7";
        KeyCodes[KeyCodes["KP_8"] = 96] = "KP_8";
        KeyCodes[KeyCodes["KP_9"] = 97] = "KP_9";
        KeyCodes[KeyCodes["KP_0"] = 98] = "KP_0";
        KeyCodes[KeyCodes["KP_Point"] = 99] = "KP_Point";
        KeyCodes[KeyCodes["KEY_NonUSBackslash"] = 100] = "KEY_NonUSBackslash";
        KeyCodes[KeyCodes["KP_Equals"] = 103] = "KP_Equals";
        KeyCodes[KeyCodes["KEY_F13"] = 104] = "KEY_F13";
        KeyCodes[KeyCodes["KEY_F14"] = 105] = "KEY_F14";
        KeyCodes[KeyCodes["KEY_F15"] = 106] = "KEY_F15";
        KeyCodes[KeyCodes["KEY_F16"] = 107] = "KEY_F16";
        KeyCodes[KeyCodes["KEY_F17"] = 108] = "KEY_F17";
        KeyCodes[KeyCodes["KEY_F18"] = 109] = "KEY_F18";
        KeyCodes[KeyCodes["KEY_F19"] = 110] = "KEY_F19";
        KeyCodes[KeyCodes["KEY_F20"] = 111] = "KEY_F20";
        KeyCodes[KeyCodes["KEY_F21"] = 112] = "KEY_F21";
        KeyCodes[KeyCodes["KEY_F22"] = 113] = "KEY_F22";
        KeyCodes[KeyCodes["KEY_F23"] = 114] = "KEY_F23";
        KeyCodes[KeyCodes["KEY_F24"] = 115] = "KEY_F24";
        KeyCodes[KeyCodes["KEY_Help"] = 117] = "KEY_Help";
        KeyCodes[KeyCodes["KEY_Menu"] = 118] = "KEY_Menu";
        KeyCodes[KeyCodes["KEY_LeftControl"] = 224] = "KEY_LeftControl";
        KeyCodes[KeyCodes["KEY_LeftShift"] = 225] = "KEY_LeftShift";
        KeyCodes[KeyCodes["KEY_LeftAlt"] = 226] = "KEY_LeftAlt";
        KeyCodes[KeyCodes["KEY_LeftMeta"] = 227] = "KEY_LeftMeta";
        KeyCodes[KeyCodes["KEY_RightControl"] = 228] = "KEY_RightControl";
        KeyCodes[KeyCodes["KEY_RightShift"] = 229] = "KEY_RightShift";
        KeyCodes[KeyCodes["KEY_RightAlt"] = 230] = "KEY_RightAlt";
        KeyCodes[KeyCodes["KEY_RightMeta"] = 231] = "KEY_RightMeta";
        KeyCodes[KeyCodes["INVALID"] = 255] = "INVALID";
    })(KeyCodes = exports.KeyCodes || (exports.KeyCodes = {}));
    ;
    function ConvertToKeyCode(key) {
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
                return KeyCodes.KEY_Equals;
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
    exports.ConvertToKeyCode = ConvertToKeyCode;
    var MouseButtons;
    (function (MouseButtons) {
        MouseButtons[MouseButtons["LEFT"] = 0] = "LEFT";
        MouseButtons[MouseButtons["MIDDLE"] = 1] = "MIDDLE";
        MouseButtons[MouseButtons["RIGHT"] = 2] = "RIGHT";
    })(MouseButtons = exports.MouseButtons || (exports.MouseButtons = {}));
    ;
    var KeyEvent = (function () {
        function KeyEvent() {
        }
        return KeyEvent;
    }());
    exports.KeyEvent = KeyEvent;
    ;
    var MouseButtonEvent = (function () {
        function MouseButtonEvent() {
        }
        return MouseButtonEvent;
    }());
    exports.MouseButtonEvent = MouseButtonEvent;
    ;
    var MouseScrollEvent = (function () {
        function MouseScrollEvent() {
            this.Offset = 0;
        }
        return MouseScrollEvent;
    }());
    exports.MouseScrollEvent = MouseScrollEvent;
    ;
    var MousePositionOffsetEvent = (function () {
        function MousePositionOffsetEvent() {
            this.X = 0;
            this.Y = 0;
        }
        return MousePositionOffsetEvent;
    }());
    exports.MousePositionOffsetEvent = MousePositionOffsetEvent;
    ;
    var MousePositionAbsoluteEvent = (function () {
        function MousePositionAbsoluteEvent() {
            this.X = 0;
            this.Y = 0;
        }
        return MousePositionAbsoluteEvent;
    }());
    exports.MousePositionAbsoluteEvent = MousePositionAbsoluteEvent;
    ;
});
define("Rat_Lite", ["require", "exports"], function (require, exports) {
    "use strict";
    Object.defineProperty(exports, "__esModule", { value: true });
    var OpCode;
    (function (OpCode) {
        OpCode[OpCode["CONTINUATION"] = 0] = "CONTINUATION";
        OpCode[OpCode["TEXT"] = 1] = "TEXT";
        OpCode[OpCode["BINARY"] = 2] = "BINARY";
        OpCode[OpCode["CLOSE"] = 8] = "CLOSE";
        OpCode[OpCode["PING"] = 9] = "PING";
        OpCode[OpCode["PONG"] = 10] = "PONG";
        OpCode[OpCode["INVALID"] = 255] = "INVALID";
    })(OpCode = exports.OpCode || (exports.OpCode = {}));
    ;
    var WSMessage = (function () {
        function WSMessage() {
        }
        return WSMessage;
    }());
    exports.WSMessage = WSMessage;
    ;
    var Point = (function () {
        function Point() {
        }
        return Point;
    }());
    exports.Point = Point;
    ;
    var Rect = (function () {
        function Rect() {
        }
        return Rect;
    }());
    exports.Rect = Rect;
    ;
    var Monitor = (function () {
        function Monitor() {
        }
        return Monitor;
    }());
    exports.Monitor = Monitor;
    ;
    var PACKET_TYPES;
    (function (PACKET_TYPES) {
        PACKET_TYPES[PACKET_TYPES["INVALID"] = 0] = "INVALID";
        PACKET_TYPES[PACKET_TYPES["HTTP_MSG"] = 1] = "HTTP_MSG";
        PACKET_TYPES[PACKET_TYPES["ONMONITORSCHANGED"] = 2] = "ONMONITORSCHANGED";
        PACKET_TYPES[PACKET_TYPES["ONFRAMECHANGED"] = 3] = "ONFRAMECHANGED";
        PACKET_TYPES[PACKET_TYPES["ONNEWFRAME"] = 4] = "ONNEWFRAME";
        PACKET_TYPES[PACKET_TYPES["ONMOUSEIMAGECHANGED"] = 5] = "ONMOUSEIMAGECHANGED";
        PACKET_TYPES[PACKET_TYPES["ONMOUSEPOSITIONCHANGED"] = 6] = "ONMOUSEPOSITIONCHANGED";
        PACKET_TYPES[PACKET_TYPES["ONKEYUP"] = 7] = "ONKEYUP";
        PACKET_TYPES[PACKET_TYPES["ONKEYDOWN"] = 8] = "ONKEYDOWN";
        PACKET_TYPES[PACKET_TYPES["ONMOUSEUP"] = 9] = "ONMOUSEUP";
        PACKET_TYPES[PACKET_TYPES["ONMOUSEDOWN"] = 10] = "ONMOUSEDOWN";
        PACKET_TYPES[PACKET_TYPES["ONMOUSESCROLL"] = 11] = "ONMOUSESCROLL";
        PACKET_TYPES[PACKET_TYPES["ONCLIPBOARDTEXTCHANGED"] = 12] = "ONCLIPBOARDTEXTCHANGED";
        PACKET_TYPES[PACKET_TYPES["LAST_PACKET_TYPE"] = 13] = "LAST_PACKET_TYPE";
    })(PACKET_TYPES || (PACKET_TYPES = {}));
    ;
    var IClientDriver = (function () {
        function IClientDriver() {
            this.ShareClip = false;
        }
        IClientDriver.prototype.setShareClipboard = function (share) { this.ShareClip = share; };
        IClientDriver.prototype.getShareClipboard = function () { return this.ShareClip; };
        IClientDriver.prototype.SendKeyUp = function (key) {
            var data = new Uint8Array(4 + 1);
            var dataview = new DataView(data);
            dataview.setUint32(0, PACKET_TYPES.ONKEYUP);
            dataview.setUint8(4, key);
            this.WebSocket_.send(data);
        };
        IClientDriver.prototype.SendKeyDown = function (key) {
            var data = new Uint8Array(4 + 1);
            var dataview = new DataView(data);
            dataview.setUint32(0, PACKET_TYPES.ONKEYDOWN);
            dataview.setUint8(4, key);
            this.WebSocket_.send(data);
        };
        IClientDriver.prototype.SendMouseUp = function (button) {
            var data = new Uint8Array(4 + 1);
            var dataview = new DataView(data);
            dataview.setUint32(0, PACKET_TYPES.ONMOUSEUP);
            dataview.setUint8(4, button);
            this.WebSocket_.send(data);
        };
        IClientDriver.prototype.SendMouseDown = function (button) {
            var data = new Uint8Array(4 + 1);
            var dataview = new DataView(data);
            dataview.setUint32(0, PACKET_TYPES.ONMOUSEDOWN);
            dataview.setUint8(4, button);
            this.WebSocket_.send(data);
        };
        IClientDriver.prototype.SendMouseScroll = function (offset) {
            var data = new Uint8Array(4 + 4);
            var dataview = new DataView(data);
            dataview.setUint32(0, PACKET_TYPES.ONMOUSESCROLL);
            dataview.setUint32(4, offset);
            this.WebSocket_.send(data);
        };
        IClientDriver.prototype.SendMousePosition = function (pos) {
            var data = new Uint8Array(4 + 8);
            var dataview = new DataView(data);
            dataview.setUint32(0, PACKET_TYPES.ONMOUSEPOSITIONCHANGED);
            dataview.setInt32(4, pos.X);
            dataview.setInt32(8, pos.X);
            this.WebSocket_.send(data);
        };
        IClientDriver.prototype.SendClipboardChanged = function (text) {
            var data = new Uint8Array(4 + text.length);
            var dataview = new DataView(data);
            dataview.setUint32(0, PACKET_TYPES.ONMOUSESCROLL);
            for (var i = 0; i < text.length; i++) {
                data[4 + i] = text.charCodeAt(0);
            }
            this.WebSocket_.send(data);
        };
        return IClientDriver;
    }());
    exports.IClientDriver = IClientDriver;
    ;
    var IClientDriverConfiguration = (function (_super) {
        __extends(IClientDriverConfiguration, _super);
        function IClientDriverConfiguration() {
            return _super !== null && _super.apply(this, arguments) || this;
        }
        IClientDriverConfiguration.prototype.onConnection = function (callback) {
            this.onConnection_ = callback;
            return this;
        };
        IClientDriverConfiguration.prototype.onMessage = function (callback) {
            this.onMessage_ = callback;
            return this;
        };
        IClientDriverConfiguration.prototype.onDisconnection = function (callback) {
            this.onDisconnection_ = callback;
            return this;
        };
        IClientDriverConfiguration.prototype.onMonitorsChanged = function (callback) {
            this.onMonitorsChanged_ = callback;
            return this;
        };
        IClientDriverConfiguration.prototype.onFrameChanged = function (callback) {
            this.onFrameChanged_ = callback;
            return this;
        };
        IClientDriverConfiguration.prototype.onNewFrame = function (callback) {
            this.onNewFrame_ = callback;
            return this;
        };
        IClientDriverConfiguration.prototype.onMouseImageChanged = function (callback) {
            this.onMouseImageChanged_ = callback;
            return this;
        };
        IClientDriverConfiguration.prototype.onMousePositionChanged = function (callback) {
            this.onMousePositionChanged_ = callback;
            return this;
        };
        IClientDriverConfiguration.prototype.onClipboardChanged = function (callback) {
            this.onClipboardChanged_ = callback;
            return this;
        };
        IClientDriverConfiguration.prototype._arrayBufferToBase64 = function (buffer, offset) {
            var binary = '';
            for (var i = offset; i < buffer.byteLength; i++) {
                binary += String.fromCharCode(buffer[i]);
            }
            return window.btoa(binary);
        };
        IClientDriverConfiguration.prototype.MonitorsChanged = function (ws, dataview) {
            if (!this.onMonitorsChanged_)
                return;
            var sizeofmonitor = 6 * 4 + 128;
            var num = dataview.byteLength / sizeofmonitor;
            this.Monitors = new Array();
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
                return this.onMonitorsChanged_(this.Monitors);
            }
            if (this.onDisconnection_) {
                this.onDisconnection_(ws, 1000, "Invalid Monitor Count");
            }
            ws.close(1000, "Invalid Monitor Count");
        };
        IClientDriverConfiguration.prototype.Frame = function (ws, dataview, callback) {
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
                var foundmonitor = this.Monitors.filter(function (a) { return a.Id == monitorid; });
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
        };
        IClientDriverConfiguration.prototype.MouseImageChanged = function (ws, dataview) {
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
        };
        IClientDriverConfiguration.prototype.MousePositionChanged = function (ws, dataview) {
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
        };
        IClientDriverConfiguration.prototype.ClipboardTextChanged = function (dataview) {
            if (!this.ShareClip || !this.onClipboardChanged_)
                return;
            if (dataview.byteLength < 1024 * 100) {
                var text = '';
                for (var i = 0, strLen = 128; i < strLen; i++) {
                    text += String.fromCharCode.apply(dataview.getUint8(20 + i));
                }
                this.onClipboardChanged_(text);
            }
        };
        IClientDriverConfiguration.prototype.Build = function (ws) {
            var _this = this;
            var self = this;
            ws.binaryType = 'arraybuffer';
            ws.onopen = function (ev) {
                console.log('onopen');
                if (self.onConnection_) {
                    self.onConnection_(ws, ev);
                }
            };
            ws.onclose = function (ev) {
                console.log('onclose');
                if (self.onDisconnection_) {
                    self.onDisconnection_(ws, ev.code, ev.reason);
                }
            };
            ws.onmessage = function (ev) {
                console.log('onmessage');
                var t0 = performance.now();
                var data = new DataView(ev.data);
                var packettype = data.getInt32(0, true);
                switch (packettype) {
                    case PACKET_TYPES.ONMONITORSCHANGED:
                        _this.MonitorsChanged(ws, new DataView(ev.data, 4));
                        break;
                    case PACKET_TYPES.ONFRAMECHANGED:
                        if (_this.onFrameChanged_) {
                            _this.Frame(ws, new DataView(ev.data, 4), _this.onFrameChanged_);
                        }
                        break;
                    case PACKET_TYPES.ONNEWFRAME:
                        if (_this.onNewFrame_) {
                            _this.Frame(ws, new DataView(ev.data, 4), _this.onNewFrame_);
                        }
                        break;
                    case PACKET_TYPES.ONMOUSEIMAGECHANGED:
                        _this.MouseImageChanged(ws, new DataView(ev.data, 4));
                        break;
                    case PACKET_TYPES.ONMOUSEPOSITIONCHANGED:
                        _this.MousePositionChanged(ws, new DataView(ev.data, 4));
                        break;
                    case PACKET_TYPES.ONCLIPBOARDTEXTCHANGED:
                        _this.ClipboardTextChanged(new DataView(ev.data, 4));
                        break;
                    default:
                        if (_this.onMessage_) {
                            var r = new WSMessage();
                            r.data = new DataView(ev.data, 4);
                            if (ev.data instanceof ArrayBuffer) {
                                r.code = OpCode.BINARY;
                            }
                            else if (typeof ev.data === "string") {
                                r.code = OpCode.TEXT;
                            }
                            _this.onMessage_(ws, r);
                        }
                        break;
                }
                var t1 = performance.now();
                console.log("took " + (t1 - t0) + " milliseconds to process the receive loop");
            };
            return this;
        };
        return IClientDriverConfiguration;
    }(IClientDriver));
    exports.IClientDriverConfiguration = IClientDriverConfiguration;
    ;
    function CreateClientDriverConfiguration() {
        return new IClientDriverConfiguration();
    }
    exports.CreateClientDriverConfiguration = CreateClientDriverConfiguration;
});
define("ClientWindow", ["require", "exports", "Rat_Lite", "Input_Lite"], function (require, exports, Rat_Lite_1, Input_Lite_1) {
    "use strict";
    Object.defineProperty(exports, "__esModule", { value: true });
    var ClientWindow = (function () {
        function ClientWindow(rootnode) {
            this.ScaleImage_ = false;
            this.ConnectedToSelf_ = false;
            this.ScaleImage = false;
            this.AttachedRoot_ = rootnode;
            window.addEventListener("mousedown", this.onmousedown);
            window.addEventListener("mouseup", this.onmouseup);
            window.addEventListener("mousemove", this.onmove);
            window.addEventListener("wheel", this.onwheel);
            window.addEventListener("keydown", this.onkeydown);
            window.addEventListener("keyup", this.onkeyup);
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
            rootnode.appendChild(this.HTMLDivRoot_);
        }
        ClientWindow.prototype.destroy = function () {
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
        };
        ClientWindow.prototype.connect = function (dst_host, dst_port) {
            var _this = this;
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
            this.ClientDriver_ = Rat_Lite_1.CreateClientDriverConfiguration()
                .onConnection(function (ws, ev) {
            }).onMessage(function (ws, message) {
            }).onDisconnection(function (ws, code, message) {
            }).onClipboardChanged(function (clipstring) {
            }).onFrameChanged(function (image, monitor) {
                if (_this.ScaleImage) {
                    var scale = _this.getScalingFactor();
                    _this.HTMLCanvasScreenImage_.getContext("2d").drawImage(image, monitor.OffsetX * scale, monitor.OffsetY * scale, monitor.Width * scale, monitor.Height * scale);
                }
                else {
                    _this.HTMLCanvasScreenImage_.getContext("2d").drawImage(image, monitor.OffsetX * scale, monitor.OffsetY);
                }
            }).onMonitorsChanged(function (monitors) {
            }).onMouseImageChanged(function (image) {
                _this.HTMLCanvasMouseImage_.getContext("2d").putImageData(_this.Cursor_, 0, 0);
            }).onMousePositionChanged(function (pos) {
                if (_this.ScaleImage) {
                    var scale = _this.getScalingFactor();
                    _this.HTMLCanvasMouseImage_.style.top = (pos.Y * scale) + "px";
                    _this.HTMLCanvasMouseImage_.style.left = (pos.X * scale) + "px";
                }
                else {
                    _this.HTMLCanvasMouseImage_.style.top = pos.Y + "px";
                    _this.HTMLCanvasMouseImage_.style.left = pos.X + "px";
                }
            }).onNewFrame(function (image, monitor) {
                if (_this.ScaleImage) {
                    var scale = _this.getScalingFactor();
                    _this.HTMLCanvasScreenImage_.width = image.width * scale;
                    _this.HTMLCanvasScreenImage_.height = image.height * scale;
                    _this.HTMLDivRoot_.style.width = _this.HTMLCanvasScreenImage_.width + 'px';
                    _this.HTMLDivRoot_.style.height = _this.HTMLCanvasScreenImage_.height + 'px';
                    _this.HTMLCanvasScreenImage_.getContext("2d").drawImage(image, 0, 0, _this.HTMLCanvasScreenImage_.width, _this.HTMLCanvasScreenImage_.height);
                }
                else {
                    _this.HTMLCanvasScreenImage_.width = image.width;
                    _this.HTMLCanvasScreenImage_.height = image.height;
                    _this.HTMLDivRoot_.style.width = _this.HTMLCanvasScreenImage_.width + 'px';
                    _this.HTMLDivRoot_.style.height = _this.HTMLCanvasScreenImage_.height + 'px';
                    _this.HTMLCanvasScreenImage_.getContext("2d").drawImage(image, 0, 0);
                }
                _this.OriginalImage_ = image;
            }).Build(this.Socket_);
        };
        ClientWindow.prototype.getScalingFactor = function () {
            if (this.ScaleImage && this.OriginalImage_ != null) {
                return window.innerHeight / this.OriginalImage_.height;
            }
            else {
                return 1.0;
            }
        };
        ClientWindow.prototype.onkeydown = function (ev) {
            this.ClientDriver_.SendKeyDown(Input_Lite_1.ConvertToKeyCode(ev));
        };
        ClientWindow.prototype.onkeyup = function (ev) {
            this.ClientDriver_.SendKeyUp(Input_Lite_1.ConvertToKeyCode(ev));
        };
        ClientWindow.prototype.onwheel = function (ev) {
            this.ClientDriver_.SendMouseScroll(ev.deltaY < 0 ? -1 : 1);
        };
        ClientWindow.prototype.onmove = function (ev) {
            this.ClientDriver_.SendMousePosition({ Y: ev.pageY, X: ev.pageX });
        };
        ClientWindow.prototype.onmouseup = function (ev) {
            this.ClientDriver_.SendMouseUp(ev.button);
        };
        ClientWindow.prototype.onmousedown = function (ev) {
            this.ClientDriver_.SendMouseDown(ev.button);
        };
        return ClientWindow;
    }());
    exports.ClientWindow = ClientWindow;
});
//# sourceMappingURL=data:application/json;base64,eyJ2ZXJzaW9uIjozLCJmaWxlIjoiYnVpbGQuanMiLCJzb3VyY2VSb290IjoiQzovVXNlcnMvc2NvdHQvRG9jdW1lbnRzL0dpdEh1Yi9yYXRfbGl0ZS9zcmMvaHRtbC9zcmMvIiwic291cmNlcyI6WyJJbnB1dF9MaXRlLnRzIiwiUmF0X0xpdGUudHMiLCJDbGllbnRXaW5kb3cudHMiXSwibmFtZXMiOltdLCJtYXBwaW5ncyI6Ijs7Ozs7Ozs7Ozs7OztJQUNBLElBQVksUUF5SFg7SUF6SEQsV0FBWSxRQUFRO1FBQ2hCLHlDQUFTLENBQUE7UUFDVCx5Q0FBUyxDQUFBO1FBQ1QseUNBQVMsQ0FBQTtRQUNULHlDQUFTLENBQUE7UUFDVCx5Q0FBUyxDQUFBO1FBQ1QseUNBQVMsQ0FBQTtRQUNULDBDQUFVLENBQUE7UUFDViwwQ0FBVSxDQUFBO1FBQ1YsMENBQVUsQ0FBQTtRQUNWLDBDQUFVLENBQUE7UUFDViwwQ0FBVSxDQUFBO1FBQ1YsMENBQVUsQ0FBQTtRQUNWLDBDQUFVLENBQUE7UUFDViwwQ0FBVSxDQUFBO1FBQ1YsMENBQVUsQ0FBQTtRQUNWLDBDQUFVLENBQUE7UUFDViwwQ0FBVSxDQUFBO1FBQ1YsMENBQVUsQ0FBQTtRQUNWLDBDQUFVLENBQUE7UUFDViwwQ0FBVSxDQUFBO1FBQ1YsMENBQVUsQ0FBQTtRQUNWLDBDQUFVLENBQUE7UUFDViwwQ0FBVSxDQUFBO1FBQ1YsMENBQVUsQ0FBQTtRQUNWLDBDQUFVLENBQUE7UUFDViwwQ0FBVSxDQUFBO1FBQ1YsMENBQVUsQ0FBQTtRQUNWLDBDQUFVLENBQUE7UUFDViwwQ0FBVSxDQUFBO1FBQ1YsMENBQVUsQ0FBQTtRQUNWLDBDQUFVLENBQUE7UUFDViwwQ0FBVSxDQUFBO1FBQ1YsMENBQVUsQ0FBQTtRQUNWLDBDQUFVLENBQUE7UUFDViwwQ0FBVSxDQUFBO1FBQ1YsMENBQVUsQ0FBQTtRQUNWLGtEQUFjLENBQUE7UUFDZCxvREFBZSxDQUFBO1FBQ2YsMERBQWtCLENBQUE7UUFDbEIsOENBQVksQ0FBQTtRQUNaLGtEQUFjLENBQUE7UUFDZCxrREFBYyxD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QUFHLENBQUM7WUFDVCxLQUFLLEdBQUcsQ0FBQztZQUNULEtBQUssR0FBRyxDQUFDO1lBQ1QsS0FBSyxHQUFHLENBQUM7WUFDVCxLQUFLLEdBQUcsQ0FBQztZQUNULEtBQUssR0FBRyxDQUFDO1lBQ1QsS0FBSyxHQUFHLENBQUM7WUFDVCxLQUFLLEdBQUcsQ0FBQztZQUNULEtBQUssR0FBRyxDQUFDO1lBQ1QsS0FBSyxHQUFHLENBQUM7WUFDVCxLQUFLLEdBQUcsQ0FBQztZQUNULEtBQUssR0FBRyxDQUFDO1lBQ1QsS0FBSyxHQUFHLENBQUM7WUFDVCxLQUFLLEdBQUcsQ0FBQztZQUNULEtBQUssR0FBRyxDQUFDO1lBQ1QsS0FBSyxHQUFHLENBQUM7WUFDVCxLQUFLLEdBQUcsQ0FBQztZQUNULEtBQUssR0FBRyxDQUFDO1lBQ1QsS0FBSyxHQUFHLENBQUM7WUFDVCxLQUFLLEdBQUcsQ0FBQztZQUNULEtBQUssR0FBRyxDQUFDO1lBQ1QsS0FBSyxHQUFHLENBQUM7WUFDVCxLQUFLLEdBQUc7Z0JBQ0osTUFBTSxDQUFDLFFBQVEsQ0FBQyxLQUFLLEdBQUcsQ0FBQyxDQUFDLEdBQUcsQ0FBQyxHQUFHLEdBQUcsQ0FBQyxHQUFHLENBQUMsQ0FBQztZQUM5QyxLQUFLLEdBQUcsQ0FBQztZQUNULEtBQUssR0FBRyxDQUFDO1lBQ1QsS0FBSyxHQUFHLENBQUM7WUFDVCxLQUFLLEdBQUcsQ0FBQztZQUNULEtBQUssR0FBRyxDQUFDO1lBQ1QsS0FBSyxHQUFHLENBQUM7WUFDVCxLQUFLLEdBQUcsQ0FBQztZQUNULEtBQUssR0FBRyxDQUFDO1lBQ1QsS0FBSyxHQUFHLENBQUM7WUFDVCxLQUFLLEdBQUcsQ0FBQztZQUNULEtBQUssR0FBRyxDQUFDO1lBQ1QsS0FBSyxHQUFHLENBQUM7WUFDVCxLQUFLLEdBQUcsQ0FBQztZQUNULEtBQUssR0FBRyxDQUFDO1lBQ1QsS0FBSyxHQUFHLENBQUM7WUFDVCxLQUFLLEdBQUcsQ0FBQztZQUNULEtBQUssR0FBRyxDQUFDO1lBQ1QsS0FBSyxHQUFHLENBQUM7WUFDVCxLQUFLLEdBQUcsQ0FBQztZQUNULEtBQUssR0FBRyxDQUFDO1lBQ1QsS0FBSyxHQUFHLENBQUM7WUFDVCxLQUFLLEdBQUcsQ0FBQztZQUNULEtBQUssR0FBRyxDQUFDO1lBQ1QsS0FBSyxHQUFHLENBQUM7WUFDVCxLQUFLLEdBQUcsQ0FBQztZQUNULEtBQUssR0FBRztnQkFDSixNQUFNLENBQUMsUUFBUSxDQUFDLEtBQUssR0FBRyxDQUFDLENBQUMsR0FBRyxDQUFDLEdBQUcsR0FBRyxDQUFDLEdBQUcsQ0FBQyxDQUFDO1lBQzlDLEtBQUssT0FBTztnQkFDUixNQUFNLENBQUMsUUFBUSxDQUFDLFNBQVMsQ0FBQztZQUM5QixLQUFLLFFBQVE7Z0JBQ1QsTUFBTSxDQUFDLFFBQVEsQ0FBQyxVQUFVLENBQUM7WUFDL0IsS0FBSyxXQUFXO2dCQUNaLE1BQU0sQ0FBQyxRQUFRLENBQUMsYUFBYSxDQUFDO1lBQ2xDLEtBQUssS0FBSztnQkFDTixNQUFNLENBQUMsUUFBUSxDQUFDLE9BQU8sQ0FBQztZQUM1QixLQUFLLEdBQUc7Z0JBQ0osTUFBTSxDQUFDLFFBQVEsQ0FBQyxTQUFTLENBQUM7WUFDOUIsS0FBSyxHQUFHO2dCQUNKLE1BQU0sQ0FBQyxRQUFRLENBQUMsU0FBUyxDQUFDO1lBQzlCLEtBQUssR0FBRztnQkFDSixNQUFNLENBQUMsUUFBUSxDQUFDLFVBQVUsQ0FBQztZQUMvQixLQUFLLEdBQUc7Z0JBQ0osTUFBTSxDQUFDLFFBQVEsQ0FBQyxlQUFlLENBQUM7WUFDcEMsS0FBSyxHQUFHO2dCQUNKLE1BQU0sQ0FBQyxRQUFRLENBQUMsZ0JBQWdCLENBQUM7WUFDckMsS0FBSyxJQUFJO2dCQUNMLE1BQU0sQ0FBQyxRQUFRLENBQUMsYUFBYSxDQUFDO1lBQ2xDLEtBQUssR0FBRztnQkFDSixNQUFNLENBQUMsUUFBUSxDQUFDLGFBQWEsQ0FBQztZQUNsQyxLQUFLLEdBQUc7Z0JBQ0osTUFBTSxDQUFDLFFBQVEsQ0FBQyxTQUFTLENBQUM7WUFDOUIsS0FBSyxHQUFHO2dCQUNKLE1BQU0sQ0FBQyxRQUFRLENBQUMsU0FBUyxDQUFDO1lBQzlCLEtBQUssR0FBRztnQkFDSixNQUFNLENBQUMsUUFBUSxDQUFDLFNBQVMsQ0FBQztZQUM5QixLQUFLLEdBQUc7Z0JBQ0osTUFBTSxDQUFDLFFBQVEsQ0FBQyxVQUFVLENBQUM7WUFDL0IsS0FBSyxHQUFHO2dCQUNKLE1BQU0sQ0FBQyxRQUFRLENBQUMsU0FBUyxDQUFDO1lBQzlCLEtBQUssVUFBVTtnQkFDWCxNQUFNLENBQUMsUUFBUSxDQUFDLFlBQVksQ0FBQztZQUNqQyxLQUFLLElBQUk7Z0JBQ0wsTUFBTSxDQUFDLFFBQVEsQ0FBQyxNQUFNLENBQUM7WUFDM0IsS0FBSyxJQUFJO2dCQUNMLE1BQU0sQ0FBQyxRQUFRLENBQUMsTUFBTSxDQUFDO1lBQzNCLEtBQUssSUFBSTtnQkFDTCxNQUFNLENBQUMsUUFBUSxDQUFDLE1BQU0sQ0FBQztZQUMzQixLQUFLLElBQUk7Z0JBQ0wsTUFBTSxDQUFDLFFBQVEsQ0FBQyxNQUFNLENBQUM7WUFDM0IsS0FBSyxJQUFJO2dCQUNMLE1BQU0sQ0FBQyxRQUFRLENBQUMsTUFBTSxDQUFDO1lBQzNCLEtBQUssSUFBSTtnQkFDTCxNQUFNLENBQUMsUUFBUSxDQUFDLE1BQU0sQ0FBQztZQUMzQixLQUFLLElBQUk7Z0JBQ0wsTUFBTSxDQUFDLFFBQVEsQ0FBQyxNQUFNLENBQUM7WUFDM0IsS0FBSyxJQUFJO2dCQUNMLE1BQU0sQ0FBQyxRQUFRLENBQUMsTUFBTSxDQUFDO1lBQzNCLEtBQUssSUFBSTtnQkFDTCxNQUFNLENBQUMsUUFBUSxDQUFDLE1BQU0sQ0FBQztZQUMzQixLQUFLLEtBQUs7Z0JBQ04sTUFBTSxDQUFDLFFBQVEsQ0FBQyxPQUFPLENBQUM7WUFDNUIsS0FBSyxLQUFLO2dCQUNOLE1BQU0sQ0FBQyxRQUFRLENBQUMsT0FBTyxDQUFDO1lBQzVCLEtBQUssS0FBSztnQkFDTixNQUFNLENBQUMsUUFBUSxDQUFDLE9BQU8sQ0FBQztZQUM1QixLQUFLLEtBQUs7Z0JBQ04sTUFBTSxDQUFDLFFBQVEsQ0FBQyxPQUFPLENBQUM7WUFDNUIsS0FBSyxLQUFLO2dCQUNOLE1BQU0sQ0FBQyxRQUFRLENBQUMsT0FBTyxDQUFDO1lBQzVCLEtBQUssS0FBSztnQkFDTixNQUFNLENBQUMsUUFBUSxDQUFDLE9BQU8sQ0FBQztZQUM1QixLQUFLLEtBQUs7Z0JBQ04sTUFBTSxDQUFDLFFBQVEsQ0FBQyxNQUFNLENBQUM7WUFDM0IsS0FBSyxLQUFLO2dCQUNOLE1BQU0sQ0FBQyxRQUFRLENBQUMsTUFBTSxDQUFDO1lBQzNCLEtBQUssS0FBSztnQkFDTixNQUFNLENBQUMsUUFBUSxDQUFDLE1BQU0sQ0FBQztZQUMzQixLQUFLLEtBQUs7Z0JBQ04sTUFBTSxDQUFDLFFBQVEsQ0FBQyxNQUFNLENBQUM7WUFDM0IsS0FBSyxLQUFLO2dCQUNOLE1BQU0sQ0FBQyxRQUFRLENBQUMsTUFBTSxDQUFDO1lBQzNCLEtBQUssT0FBTztnQkFDUixNQUFNLENBQUMsUUFBUSxDQUFDLGVBQWUsQ0FBQztZQUNwQyxLQUFLLFlBQVk7Z0JBQ2IsTUFBTSxDQUFDLFFBQVEsQ0FBQyxjQUFjLENBQUM7WUFDbkMsS0FBSyxPQUFPO2dCQUNSLE1BQU0sQ0FBQyxRQUFRLENBQUMsU0FBUyxDQUFDO1lBQzlCLEtBQUssUUFBUTtnQkFDVCxNQUFNLENBQUMsUUFBUSxDQUFDLFVBQVUsQ0FBQztZQUMvQixLQUFLLE1BQU07Z0JBQ1AsTUFBTSxDQUFDLFFBQVEsQ0FBQyxRQUFRLENBQUM7WUFDN0IsS0FBSyxRQUFRO2dCQUNULE1BQU0sQ0FBQyxRQUFRLENBQUMsVUFBVSxDQUFDO1lBQy9CLEtBQUssUUFBUTtnQkFDVCxNQUFNLENBQUMsUUFBUSxDQUFDLFVBQVUsQ0FBQztZQUMvQixLQUFLLEtBQUs7Z0JBQ04sTUFBTSxDQUFDLFFBQVEsQ0FBQyxPQUFPLENBQUM7WUFDNUIsS0FBSyxVQUFVO2dCQUNYLE1BQU0sQ0FBQyxRQUFRLENBQUMsWUFBWSxDQUFDO1lBQ2pDLEtBQUssWUFBWTtnQkFDYixNQUFNLENBQUMsUUFBUSxDQUFDLFNBQVMsQ0FBQztZQUM5QixLQUFLLFdBQVc7Z0JBQ1osTUFBTSxDQUFDLFFBQVEsQ0FBQyxRQUFRLENBQUM7WUFDN0IsS0FBSyxXQUFXO2dCQUNaLE1BQU0sQ0FBQyxRQUFRLENBQUMsUUFBUSxDQUFDO1lBQzdCLEtBQUssU0FBUztnQkFDVixNQUFNLENBQUMsUUFBUSxDQUFDLE1BQU0sQ0FBQztZQUMzQixLQUFLLFNBQVM7Z0JBQ1YsTUFBTSxDQUFDLFFBQVEsQ0FBQyxVQUFVLENBQUM7WUFDL0IsS0FBSyxHQUFHO2dCQUNKLE1BQU0sQ0FBQyxRQUFRLENBQUMsU0FBUyxDQUFDO1lBQzlCLEtBQUssR0FBRztnQkFDSixNQUFNLENBQUMsUUFBUSxDQUFDLFdBQVcsQ0FBQztZQUNoQyxLQUFLLEdBQUc7Z0JBQ0osTUFBTSxDQUFDLFFBQVEsQ0FBQyxXQUFXLENBQUM7WUFDaEMsS0FBSyxHQUFHO2dCQUNKLE1BQU0sQ0FBQyxRQUFRLENBQUMsTUFBTSxDQUFDO1lBQzNCLEtBQUssR0FBRztnQkFDSixNQUFNLENBQUMsUUFBUSxDQUFDLFFBQVEsQ0FBQztZQUM3QixLQUFLLE1BQU07Z0JBQ1AsTUFBTSxDQUFDLFFBQVEsQ0FBQyxRQUFRLENBQUM7WUFDN0IsS0FBSyxLQUFLO2dCQUNOLE1BQU0sQ0FBQyxRQUFRLENBQUMsUUFBUSxDQUFDO1lBQzdCLEtBQUssU0FBUztnQkFDVixNQUFNLENBQUMsUUFBUSxDQUFDLGVBQWUsQ0FBQztZQUNwQyxLQUFLLE9BQU87Z0JBQ1IsTUFBTSxDQUFDLFFBQVEsQ0FBQyxhQUFhLENBQUM7WUFDbEMsS0FBSyxNQUFNO2dCQUNQLE1BQU0sQ0FBQyxRQUFRLENBQUMsWUFBWSxDQUFDO1lBQ2pDO2dCQUNJLE1BQU0sQ0FBQyxRQUFRLENBQUMsT0FBTyxDQUFDO1FBQ2hDLENBQUM7SUFFTCxDQUFDO0lBbE1ELDRDQWtNQztJQUVELElBQVksWUFBb0M7SUFBaEQsV0FBWSxZQUFZO1FBQUcsK0NBQUksQ0FBQTtRQUFFLG1EQUFNLENBQUE7UUFBRSxpREFBSyxDQUFBO0lBQUMsQ0FBQyxFQUFwQyxZQUFZLEdBQVosb0JBQVksS0FBWixvQkFBWSxRQUF3QjtJQUFBLENBQUM7SUFFakQ7UUFBQTtRQUdBLENBQUM7UUFBRCxlQUFDO0lBQUQsQ0FBQyxBQUhELElBR0M7SUFIWSw0QkFBUTtJQUdwQixDQUFDO0lBQ0Y7UUFBQTtRQUdBLENBQUM7UUFBRCx1QkFBQztJQUFELENBQUMsQUFIRCxJQUdDO0lBSFksNENBQWdCO0lBRzVCLENBQUM7SUFDRjtRQUFBO1lBQ0ksV0FBTSxHQUFHLENBQUMsQ0FBQztRQUNmLENBQUM7UUFBRCx1QkFBQztJQUFELENBQUMsQUFGRCxJQUVDO0lBRlksNENBQWdCO0lBRTVCLENBQUM7SUFDRjtRQUFBO1lBQ0ksTUFBQyxHQUFHLENBQUMsQ0FBQztZQUNOLE1BQUMsR0FBRyxDQUFDLENBQUM7UUFDVixDQUFDO1FBQUQsK0JBQUM7SUFBRCxDQUFDLEFBSEQsSUFHQztJQUhZLDREQUF3QjtJQUdwQyxDQUFDO0lBQ0Y7UUFBQTtZQUNJLE1BQUMsR0FBRyxDQUFDLENBQUM7WUFDTixNQUFDLEdBQUcsQ0FBQyxDQUFDO1FBQ1YsQ0FBQztRQUFELGlDQUFDO0lBQUQsQ0FBQyxBQUhELElBR0M7SUFIWSxnRUFBMEI7SUFHdEMsQ0FBQzs7Ozs7SUNoVkYsSUFBWSxNQUFnRztJQUE1RyxXQUFZLE1BQU07UUFBRyxtREFBZ0IsQ0FBQTtRQUFFLG1DQUFRLENBQUE7UUFBRSx1Q0FBVSxDQUFBO1FBQUUscUNBQVMsQ0FBQTtRQUFFLG1DQUFRLENBQUE7UUFBRSxvQ0FBUyxDQUFBO1FBQUUsMkNBQWEsQ0FBQTtJQUFDLENBQUMsRUFBaEcsTUFBTSxHQUFOLGNBQU0sS0FBTixjQUFNLFFBQTBGO0lBQUEsQ0FBQztJQUM3RztRQUFBO1FBR0EsQ0FBQztRQUFELGdCQUFDO0lBQUQsQ0FBQyxBQUhELElBR0M7SUFIWSw4QkFBUztJQUdyQixDQUFDO0lBQ0Y7UUFBQTtRQUdBLENBQUM7UUFBRCxZQUFDO0lBQUQsQ0FBQyxBQUhELElBR0M7SUFIWSxzQkFBSztJQUdqQixDQUFDO0lBQ0Y7UUFBQTtRQUlBLENBQUM7UUFBRCxXQUFDO0lBQUQsQ0FBQyxBQUpELElBSUM7SUFKWSxvQkFBSTtJQUloQixDQUFDO0lBQ0Y7UUFBQTtRQVVBLENBQUM7UUFBRCxjQUFDO0lBQUQsQ0FBQyxBQVZELElBVUM7SUFWWSwwQkFBTztJQVVuQixDQUFDO0lBQ0YsSUFBSyxZQWdCSjtJQWhCRCxXQUFLLFlBQVk7UUFDYixxREFBTyxDQUFBO1FBQ1AsdURBQVEsQ0FBQTtRQUNSLHlFQUFpQixDQUFBO1FBQ2pCLG1FQUFjLENBQUE7UUFDZCwyREFBVSxDQUFBO1FBQ1YsNkVBQW1CLENBQUE7UUFDbkIsbUZBQXNCLENBQUE7UUFDdEIscURBQU8sQ0FBQTtRQUNQLHlEQUFTLENBQUE7UUFDVCx5REFBUyxDQUFBO1FBQ1QsOERBQVcsQ0FBQTtRQUNYLGtFQUFhLENBQUE7UUFDYixvRkFBc0IsQ0FBQTtRQUV0Qix3RUFBZ0IsQ0FBQTtJQUNwQixDQUFDLEVBaEJJLFlBQVksS0FBWixZQUFZLFFBZ0JoQjtJQUFBLENBQUM7SUFFRjtRQUFBO1lBQ2MsY0FBUyxHQUFHLEtBQUssQ0FBQztRQXNFaEMsQ0FBQztRQXZERyx5Q0FBaUIsR0FBakIsVUFBa0IsS0FBYyxJQUFVLElBQUksQ0FBQyxTQUFTLEdBQUcsS0FBSyxDQUFDLENBQUMsQ0FBQztRQUNuRSx5Q0FBaUIsR0FBakIsY0FBK0IsTUFBTSxDQUFDLElBQUksQ0FBQyxTQUFTLENBQUMsQ0FBQyxDQUFDO1FBQ3ZELGlDQUFTLEdBQVQsVUFBVSxHQUFhO1lBQ25CLElBQUksSUFBSSxHQUFHLElBQUksVUFBVSxDQUFDLENBQUMsR0FBRyxDQUFDLENBQUMsQ0FBQztZQUNqQyxJQUFJLFFBQVEsR0FBRyxJQUFJLFFBQVEsQ0FBQyxJQUFJLENBQUMsQ0FBQztZQUNsQyxRQUFRLENBQUMsU0FBUyxDQUFDLENBQUMsRUFBRSxZQUFZLENBQUMsT0FBTyxDQUFDLENBQUM7WUFDNUMsUUFBUSxDQUFDLFFBQVEsQ0FBQyxDQUFDLEVBQUUsR0FBRyxDQUFDLENBQUM7WUFDMUIsSUFBSSxDQUFDLFVBQVUsQ0FBQyxJQUFJLENBQUMsSUFBSSxDQUFDLENBQUM7UUFDL0IsQ0FBQztRQUNELG1DQUFXLEdBQVgsVUFBWSxHQUFhO1lBQ3JCLElBQUksSUFBSSxHQUFHLElBQUksVUFBVSxDQUFDLENBQUMsR0FBRyxDQUFDLENBQUMsQ0FBQztZQUNqQyxJQUFJLFFBQVEsR0FBRyxJQUFJLFFBQVEsQ0FBQyxJQUFJLENBQUMsQ0FBQztZQUNsQyxRQUFRLENBQUMsU0FBUyxDQUFDLENBQUMsRUFBRSxZQUFZLENBQUMsU0FBUyxDQUFDLENBQUM7WUFDOUMsUUFBUSxDQUFDLFFBQVEsQ0FBQyxDQUFDLEVBQUUsR0FBRyxDQUFDLENBQUM7WUFDMUIsSUFBSSxDQUFDLFVBQVUsQ0FBQyxJQUFJLENBQUMsSUFBSSxDQUFDLENBQUM7UUFDL0IsQ0FBQztRQUNELG1DQUFXLEdBQVgsVUFBWSxNQUFvQjtZQUM1QixJQUFJLElBQUksR0FBRyxJQUFJLFVBQVUsQ0FBQyxDQUFDLEdBQUcsQ0FBQyxDQUFDLENBQUM7WUFDakMsSUFBSSxRQUFRLEdBQUcsSUFBSSxRQUFRLENBQUMsSUFBSSxDQUFDLENBQUM7WUFDbEMsUUFBUSxDQUFDLFNBQVMsQ0FBQyxDQUFDLEVBQUUsWUFBWSxDQUFDLFNBQVMsQ0FBQyxDQUFDO1lBQzlDLFFBQVEsQ0FBQyxRQUFRLENBQUMsQ0FBQyxFQUFFLE1BQU0sQ0FBQyxDQUFDO1lBQzdCLElBQUksQ0FBQyxVQUFVLENBQUMsSUFBSSxDQUFDLElBQUksQ0FBQyxDQUFDO1FBQy9CLENBQUM7UUFDRCxxQ0FBYSxHQUFiLFVBQWMsTUFBb0I7WUFDOUIsSUFBSSxJQUFJLEdBQUcsSUFBSSxVQUFVLENBQUMsQ0FBQyxHQUFHLENBQUMsQ0FBQyxDQUFDO1lBQ2pDLElBQUksUUFBUSxHQUFHLElBQUksUUFBUSxDQUFDLElBQUksQ0FBQyxDQUFDO1lBQ2xDLFFBQVEsQ0FBQyxTQUFTLENBQUMsQ0FBQyxFQUFFLFlBQVksQ0FBQyxXQUFXLENBQUMsQ0FBQztZQUNoRCxRQUFRLENBQUMsUUFBUSxDQUFDLENBQUMsRUFBRSxNQUFNLENBQUMsQ0FBQztZQUM3QixJQUFJLENBQUMsVUFBVSxDQUFDLElBQUksQ0FBQyxJQUFJLENBQUMsQ0FBQztRQUMvQixDQUFDO1FBQ0QsdUNBQWUsR0FBZixVQUFnQixNQUFjO1lBQzFCLElBQUksSUFBSSxHQUFHLElBQUksVUFBVSxDQUFDLENBQUMsR0FBRyxDQUFDLENBQUMsQ0FBQztZQUNqQyxJQUFJLFFBQVEsR0FBRyxJQUFJLFFBQVEsQ0FBQyxJQUFJLENBQUMsQ0FBQztZQUNsQyxRQUFRLENBQUMsU0FBUyxDQUFDLENBQUMsRUFBRSxZQUFZLENBQUMsYUFBYSxDQUFDLENBQUM7WUFDbEQsUUFBUSxDQUFDLFNBQVMsQ0FBQyxDQUFDLEVBQUUsTUFBTSxDQUFDLENBQUM7WUFDOUIsSUFBSSxDQUFDLFVBQVUsQ0FBQyxJQUFJLENBQUMsSUFBSSxDQUFDLENBQUM7UUFDL0IsQ0FBQztRQUNELHlDQUFpQixHQUFqQixVQUFrQixHQUFVO1lBQ3hCLElBQUksSUFBSSxHQUFHLElBQUksVUFBVSxDQUFDLENBQUMsR0FBRyxDQUFDLENBQUMsQ0FBQztZQUNqQyxJQUFJLFFBQVEsR0FBRyxJQUFJLFFBQVEsQ0FBQyxJQUFJLENBQUMsQ0FBQztZQUNsQyxRQUFRLENBQUMsU0FBUyxDQUFDLENBQUMsRUFBRSxZQUFZLENBQUMsc0JBQXNCLENBQUMsQ0FBQztZQUMzRCxRQUFRLENBQUMsUUFBUSxDQUFDLENBQUMsRUFBRSxHQUFHLENBQUMsQ0FBQyxDQUFDLENBQUM7WUFDNUIsUUFBUSxDQUFDLFFBQVEsQ0FBQyxDQUFDLEVBQUUsR0FBRyxDQUFDLENBQUMsQ0FBQyxDQUFDO1lBQzVCLElBQUksQ0FBQyxVQUFVLENBQUMsSUFBSSxDQUFDLElBQUksQ0FBQyxDQUFDO1FBQy9CLENBQUM7UUFDRCw0Q0FBb0IsR0FBcEIsVUFBcUIsSUFBWTtZQUM3QixJQUFJLElBQUksR0FBRyxJQUFJLFVBQVUsQ0FBQyxDQUFDLEdBQUcsSUFBSSxDQUFDLE1BQU0sQ0FBQyxDQUFDO1lBQzNDLElBQUksUUFBUSxHQUFHLElBQUksUUFBUSxDQUFDLElBQUksQ0FBQyxDQUFDO1lBQ2xDLFFBQVEsQ0FBQyxTQUFTLENBQUMsQ0FBQyxFQUFFLFlBQVksQ0FBQyxhQUFhLENBQUMsQ0FBQztZQUNsRCxHQUFHLENBQUMsQ0FBQyxJQUFJLENBQUMsR0FBRyxDQUFDLEVBQUUsQ0FBQyxHQUFHLElBQUksQ0FBQyxNQUFNLEVBQUUsQ0FBQyxFQUFFLEVBQUUsQ0FBQztnQkFDbkMsSUFBSSxDQUFDLENBQUMsR0FBRyxDQUFDLENBQUMsR0FBRyxJQUFJLENBQUMsVUFBVSxDQUFDLENBQUMsQ0FBQyxDQUFDO1lBQ3JDLENBQUM7WUFDRCxJQUFJLENBQUMsVUFBVSxDQUFDLElBQUksQ0FBQyxJQUFJLENBQUMsQ0FBQztRQUMvQixDQUFDO1FBRUwsb0JBQUM7SUFBRCxDQUFDLEFBdkVELElBdUVDO0lBdkVZLHNDQUFhO0lBdUV6QixDQUFDO0lBRUY7UUFBZ0QsOENBQWE7UUFBN0Q7O1FBME5BLENBQUM7UUF6TkcsaURBQVksR0FBWixVQUFhLFFBQTRDO1lBQ3JELElBQUksQ0FBQyxhQUFhLEdBQUcsUUFBUSxDQUFDO1lBQzlCLE1BQU0sQ0FBQyxJQUFJLENBQUM7UUFDaEIsQ0FBQztRQUNELDhDQUFTLEdBQVQsVUFBVSxRQUFxRDtZQUMzRCxJQUFJLENBQUMsVUFBVSxHQUFHLFFBQVEsQ0FBQztZQUMzQixNQUFNLENBQUMsSUFBSSxDQUFDO1FBQ2hCLENBQUM7UUFDRCxvREFBZSxHQUFmLFVBQWdCLFFBQWdFO1lBQzVFLElBQUksQ0FBQyxnQkFBZ0IsR0FBRyxRQUFRLENBQUM7WUFDakMsTUFBTSxDQUFDLElBQUksQ0FBQztRQUNoQixDQUFDO1FBQ0Qsc0RBQWlCLEdBQWpCLFVBQWtCLFFBQXVDO1lBQ3JELElBQUksQ0FBQyxrQkFBa0IsR0FBRyxRQUFRLENBQUM7WUFDbkMsTUFBTSxDQUFDLElBQUksQ0FBQztRQUNoQixDQUFDO1FBQ0QsbURBQWMsR0FBZCxVQUFlLFFBQTZEO1lBQ3hFLElBQUksQ0FBQyxlQUFlLEdBQUcsUUFBUSxDQUFDO1lBQ2hDLE1BQU0sQ0FBQyxJQUFJLENBQUM7UUFDaEIsQ0FBQztRQUNELCtDQUFVLEdBQVYsVUFBVyxRQUE2RDtZQUNwRSxJQUFJLENBQUMsV0FBVyxHQUFHLFFBQVEsQ0FBQztZQUM1QixNQUFNLENBQUMsSUFBSSxDQUFDO1FBQ2hCLENBQUM7UUFDRCx3REFBbUIsR0FBbkIsVUFBb0IsUUFBb0M7WUFDcEQsSUFBSSxDQUFDLG9CQUFvQixHQUFHLFFBQVEsQ0FBQztZQUNyQyxNQUFNLENBQUMsSUFBSSxDQUFDO1FBQ2hCLENBQUM7UUFDRCwyREFBc0IsR0FBdEIsVUFBdUIsUUFBZ0M7WUFDbkQsSUFBSSxDQUFDLHVCQUF1QixHQUFHLFFBQVEsQ0FBQztZQUN4QyxNQUFNLENBQUMsSUFBSSxDQUFDO1FBQ2hCLENBQUM7UUFDRCx1REFBa0IsR0FBbEIsVUFBbUIsUUFBc0M7WUFDckQsSUFBSSxDQUFDLG1CQUFtQixHQUFHLFFBQVEsQ0FBQztZQUNwQyxNQUFNLENBQUMsSUFBSSxDQUFDO1FBQ2hCLENBQUM7UUFDTyx5REFBb0IsR0FBNUIsVUFBNkIsTUFBZ0IsRUFBRSxNQUFjO1lBQ3pELElBQUksTUFBTSxHQUFHLEVBQUUsQ0FBQztZQUNoQixHQUFHLENBQUMsQ0FBQyxJQUFJLENBQUMsR0FBRyxNQUFNLEVBQUUsQ0FBQyxHQUFHLE1BQU0sQ0FBQyxVQUFVLEVBQUUsQ0FBQyxFQUFFLEVBQUUsQ0FBQztnQkFDOUMsTUFBTSxJQUFJLE1BQU0sQ0FBQyxZQUFZLENBQUMsTUFBTSxDQUFDLENBQUMsQ0FBQyxDQUFDLENBQUM7WUFDN0MsQ0FBQztZQUNELE1BQU0sQ0FBQyxNQUFNLENBQUMsSUFBSSxDQUFDLE1BQU0sQ0FBQyxDQUFDO1FBQy9CLENBQUM7UUFDTyxvREFBZSxHQUF2QixVQUF3QixFQUFhLEVBQUUsUUFBa0I7WUFDckQsRUFBRSxDQUFDLENBQUMsQ0FBQyxJQUFJLENBQUMsa0JBQWtCLENBQUM7Z0JBQ3pCLE1BQU0sQ0FBQztZQUNYLElBQUksYUFBYSxHQUFHLENBQUMsR0FBRyxDQUFDLEdBQUcsR0FBRyxDQUFDO1lBQ2hDLElBQUksR0FBRyxHQUFHLFFBQVEsQ0FBQyxVQUFVLEdBQUcsYUFBYSxDQUFDO1lBQzlDLElBQUksQ0FBQyxRQUFRLEdBQUcsSUFBSSxLQUFLLEVBQVcsQ0FBQztZQUNyQyxFQUFFLENBQUMsQ0FBQyxRQUFRLENBQUMsVUFBVSxJQUFJLEdBQUcsR0FBRyxhQUFhLElBQUksR0FBRyxHQUFHLENBQUMsQ0FBQyxDQUFDLENBQUM7Z0JBQ3hELEdBQUcsQ0FBQyxDQUFDLElBQUksQ0FBQyxHQUFHLENBQUMsRUFBRSxDQUFDLEdBQUcsR0FBRyxFQUFFLENBQUMsRUFBRSxFQUFFLENBQUM7b0JBQzNCLElBQUksSUFBSSxHQUFHLEVBQUUsQ0FBQztvQkFDZCxHQUFHLENBQUMsQ0FBQyxJQUFJLENBQUMsR0FBRyxDQUFDLEVBQUUsTUFBTSxHQUFHLEdBQUcsRUFBRSxDQUFDLEdBQUcsTUFBTSxFQUFFLENBQUMsRUFBRSxFQUFFLENBQUM7d0JBQzVDLElBQUksSUFBSSxNQUFNLENBQUMsWUFBWSxDQUFDLEtBQUssQ0FBQyxRQUFRLENBQUMsUUFBUSxDQUFDLEVBQUUsR0FBRyxDQUFDLENBQUMsQ0FBQyxDQUFDO29CQUNqRSxDQUFDO29CQUNELElBQUksQ0FBQyxRQUFRLENBQUMsSUFBSSxDQUFDO3dCQUNmLEVBQUUsRUFBRSxRQUFRLENBQUMsUUFBUSxDQUFDLENBQUMsQ0FBQzt3QkFDeEIsS0FBSyxFQUFFLFFBQVEsQ0FBQyxRQUFRLENBQUMsQ0FBQyxDQUFDO3dCQUMzQixNQUFNLEVBQUUsUUFBUSxDQUFDLFFBQVEsQ0FBQyxDQUFDLENBQUM7d0JBQzVCLEtBQUssRUFBRSxRQUFRLENBQUMsUUFBUSxDQUFDLEVBQUUsQ0FBQzt3QkFDNUIsT0FBTyxFQUFFLFFBQVEsQ0FBQyxRQUFRLENBQUMsRUFBRSxDQUFDO3dCQUM5QixPQUFPLEVBQUUsUUFBUSxDQUFDLFFBQVEsQ0FBQyxFQUFFLENBQUM7d0JBQzlCLElBQUksRUFBRSxJQUFJO3FCQUNiLENBQUMsQ0FBQztnQkFDUCxDQUFDO2dCQUNELE1BQU0sQ0FBQyxJQUFJLENBQUMsa0JBQWtCLENBQUMsSUFBSSxDQUFDLFFBQVEsQ0FBQyxDQUFDO1lBQ2xELENBQUM7WUFDRCxJQUFJLENBQUMsRUFBRSxDQUFDLENBQUMsUUFBUSxDQUFDLFVBQVUsSUFBSSxDQUFDLENBQUMsQ0FBQyxDQUFDO2dCQUVoQyxNQUFNLENBQUMsSUFBSSxDQUFDLGtCQUFrQixDQUFDLElBQUksQ0FBQyxRQUFRLENBQUMsQ0FBQztZQUNsRCxDQUFDO1lBQ0QsRUFBRSxDQUFDLENBQUMsSUFBSSxDQUFDLGdCQUFnQixDQUFDLENBQUMsQ0FBQztnQkFDeEIsSUFBSSxDQUFDLGdCQUFnQixDQUFDLEVBQUUsRUFBRSxJQUFJLEVBQUUsdUJBQXVCLENBQUMsQ0FBQztZQUM3RCxDQUFDO1lBQ0QsRUFBRSxDQUFDLEtBQUssQ0FBQyxJQUFJLEVBQUUsdUJBQXVCLENBQUMsQ0FBQztRQUM1QyxDQUFDO1FBQ08sMENBQUssR0FBYixVQUFjLEVBQWEsRUFBRSxRQUFrQixFQUFFLFFBQTZEO1lBQzFHLEVBQUUsQ0FBQyxDQUFDLFFBQVEsQ0FBQyxVQUFVLElBQUksQ0FBQyxHQUFHLENBQUMsR0FBRyxDQUFDLENBQUMsQ0FBQyxDQUFDO2dCQUNuQyxJQUFJLFNBQVMsR0FBRyxRQUFRLENBQUMsUUFBUSxDQUFDLENBQUMsQ0FBQyxDQUFDO2dCQUNyQyxJQUFJLElBQUksR0FBRztvQkFDUCxNQUFNLEVBQUU7d0JBQ0osQ0FBQyxFQUFFLFFBQVEsQ0FBQyxRQUFRLENBQUMsQ0FBQyxDQUFDO3dCQUN2QixDQUFDLEVBQUUsUUFBUSxDQUFDLFFBQVEsQ0FBQyxDQUFDLENBQUM7cUJBQzFCO29CQUNELE1BQU0sRUFBRSxRQUFRLENBQUMsUUFBUSxDQUFDLEVBQUUsQ0FBQztvQkFDN0IsS0FBSyxFQUFFLFFBQVEsQ0FBQyxRQUFRLENBQUMsRUFBRSxDQUFDO2lCQUMvQixDQUFDO2dCQUNGLElBQUksWUFBWSxHQUFHLElBQUksQ0FBQyxRQUFRLENBQUMsTUFBTSxDQUFDLFVBQUEsQ0FBQyxJQUFJLE9BQUEsQ0FBQyxDQUFDLEVBQUUsSUFBSSxTQUFTLEVBQWpCLENBQWlCLENBQUMsQ0FBQztnQkFDaEUsRUFBRSxDQUFDLENBQUMsWUFBWSxDQUFDLE1BQU0sR0FBRyxDQUFDLENBQUMsQ0FBQyxDQUFDO29CQUMxQixJQUFJLENBQUMsR0FBRyxJQUFJLEtBQUssRUFBRSxDQUFDO29CQUNwQixDQUFDLENBQUMsR0FBRyxHQUFHLHlCQUF5QixHQUFHLElBQUksQ0FBQyxvQkFBb0IsQ0FBQyxRQUFRLEVBQUUsRUFBRSxDQUFDLENBQUM7b0JBQzVFLEVBQUUsQ0FBQyxDQUFDLFFBQVEsQ0FBQyxVQUFVLElBQUksQ0FBQyxHQUFHLENBQUMsR0FBRyxDQUFDLElBQUksQ0FBQyxLQUFLLEdBQUcsSUFBSSxDQUFDLE1BQU0sR0FBRyxDQUFDLENBQUMsQ0FBQyxDQUFDLENBQUM7d0JBQ2hFLE1BQU0sQ0FBQyxRQUFRLENBQUMsQ0FBQyxFQUFFLFlBQVksQ0FBQyxDQUFDLENBQUMsQ0FBQyxDQUFDO29CQUN4QyxDQUFDO2dCQUNMLENBQUM7WUFDTCxDQUFDO1lBQ0QsRUFBRSxDQUFDLENBQUMsSUFBSSxDQUFDLGdCQUFnQixDQUFDLENBQUMsQ0FBQztnQkFDeEIsSUFBSSxDQUFDLGdCQUFnQixDQUFDLEVBQUUsRUFBRSxJQUFJLEVBQUUsZ0RBQWdELENBQUMsQ0FBQztZQUN0RixDQUFDO1lBQ0QsRUFBRSxDQUFDLEtBQUssQ0FBQyxJQUFJLEVBQUUsZ0RBQWdELENBQUMsQ0FBQztRQUNyRSxDQUFDO1FBRU8sc0RBQWlCLEdBQXpCLFVBQTBCLEVBQWEsRUFBRSxRQUFrQjtZQUN2RCxFQUFFLENBQUMsQ0FBQyxDQUFDLElBQUksQ0FBQyxvQkFBb0IsQ0FBQztnQkFDM0IsTUFBTSxDQUFDO1lBQ1gsRUFBRSxDQUFDLENBQUMsUUFBUSxDQUFDLFVBQVUsSUFBSSxDQUFDLEdBQUcsQ0FBQyxDQUFDLENBQUMsQ0FBQztnQkFDL0IsSUFBSSxJQUFJLEdBQUc7b0JBQ1AsTUFBTSxFQUFFO3dCQUNKLENBQUMsRUFBRSxRQUFRLENBQUMsUUFBUSxDQUFDLENBQUMsQ0FBQzt3QkFDdkIsQ0FBQyxFQUFFLFFBQVEsQ0FBQyxRQUFRLENBQUMsQ0FBQyxDQUFDO3FCQUMxQjtvQkFDRCxNQUFNLEVBQUUsUUFBUSxDQUFDLFFBQVEsQ0FBQyxDQUFDLENBQUM7b0JBQzVCLEtBQUssRUFBRSxRQUFRLENBQUMsUUFBUSxDQUFDLEVBQUUsQ0FBQztpQkFDL0IsQ0FBQztnQkFDRixJQUFJLE1BQU0sR0FBRyxRQUFRLENBQUMsYUFBYSxDQUFDLFFBQVEsQ0FBQyxDQUFDO2dCQUM5QyxJQUFJLFNBQVMsR0FBRyxNQUFNLENBQUMsVUFBVSxDQUFDLElBQUksQ0FBQyxDQUFDLGVBQWUsQ0FBQyxJQUFJLENBQUMsS0FBSyxFQUFFLElBQUksQ0FBQyxNQUFNLENBQUMsQ0FBQztnQkFDakYsR0FBRyxDQUFDLENBQUMsSUFBSSxDQUFDLEdBQUcsRUFBRSxFQUFFLENBQUMsR0FBRyxRQUFRLENBQUMsVUFBVSxFQUFFLENBQUMsRUFBRSxFQUFFLENBQUM7b0JBQzVDLFNBQVMsQ0FBQyxJQUFJLENBQUMsQ0FBQyxDQUFDLEdBQUcsUUFBUSxDQUFDLENBQUMsQ0FBQyxDQUFDO2dCQUNwQyxDQUFDO2dCQUNELEVBQUUsQ0FBQyxDQUFDLFFBQVEsQ0FBQyxVQUFVLElBQUksQ0FBQyxHQUFHLENBQUMsR0FBRyxDQUFDLElBQUksQ0FBQyxLQUFLLEdBQUcsSUFBSSxDQUFDLE1BQU0sR0FBRyxDQUFDLENBQUMsQ0FBQyxDQUFDLENBQUM7b0JBQ2hFLE1BQU0sQ0FBQyxJQUFJLENBQUMsb0JBQW9CLENBQUMsU0FBUyxDQUFDLENBQUM7Z0JBQ2hELENBQUM7WUFDTCxDQUFDO1lBQ0QsRUFBRSxDQUFDLENBQUMsSUFBSSxDQUFDLGdCQUFnQixDQUFDLENBQUMsQ0FBQztnQkFDeEIsSUFBSSxDQUFDLGdCQUFnQixDQUFDLEVBQUUsRUFBRSxJQUFJLEVBQUUsZ0RBQWdELENBQUMsQ0FBQztZQUN0RixDQUFDO1lBQ0QsRUFBRSxDQUFDLEtBQUssQ0FBQyxJQUFJLEVBQUUsZ0RBQWdELENBQUMsQ0FBQztRQUNyRSxDQUFDO1FBQ08seURBQW9CLEdBQTVCLFVBQTZCLEVBQWEsRUFBRSxRQUFrQjtZQUMxRCxFQUFFLENBQUMsQ0FBQyxDQUFDLElBQUksQ0FBQyx1QkFBdUIsQ0FBQztnQkFDOUIsTUFBTSxDQUFDO1lBQ1gsRUFBRSxDQUFDLENBQUMsUUFBUSxDQUFDLFVBQVUsSUFBSSxDQUFDLENBQUMsQ0FBQyxDQUFDO2dCQUMzQixJQUFJLENBQUMsR0FBRztvQkFDSixDQUFDLEVBQUUsUUFBUSxDQUFDLFFBQVEsQ0FBQyxDQUFDLENBQUM7b0JBQ3ZCLENBQUMsRUFBRSxRQUFRLENBQUMsUUFBUSxDQUFDLENBQUMsQ0FBQztpQkFDMUIsQ0FBQztnQkFDRixNQUFNLENBQUMsSUFBSSxDQUFDLHVCQUF1QixDQUFDLENBQUMsQ0FBQyxDQUFDO1lBQzNDLENBQUM7WUFDRCxFQUFFLENBQUMsQ0FBQyxJQUFJLENBQUMsZ0JBQWdCLENBQUMsQ0FBQyxDQUFDO2dCQUN4QixJQUFJLENBQUMsZ0JBQWdCLENBQUMsRUFBRSxFQUFFLElBQUksRUFBRSxtREFBbUQsQ0FBQyxDQUFDO1lBQ3pGLENBQUM7WUFDRCxFQUFFLENBQUMsS0FBSyxDQUFDLElBQUksRUFBRSxtREFBbUQsQ0FBQyxDQUFDO1FBQ3hFLENBQUM7UUFDTyx5REFBb0IsR0FBNUIsVUFBNkIsUUFBa0I7WUFDM0MsRUFBRSxDQUFDLENBQUMsQ0FBQyxJQUFJLENBQUMsU0FBUyxJQUFJLENBQUMsSUFBSSxDQUFDLG1CQUFtQixDQUFDO2dCQUM3QyxNQUFNLENBQUM7WUFDWCxFQUFFLENBQUMsQ0FBQyxRQUFRLENBQUMsVUFBVSxHQUFHLElBQUksR0FBRyxHQUFHLENBQUMsQ0FBQyxDQUFDO2dCQUNuQyxJQUFJLElBQUksR0FBRyxFQUFFLENBQUM7Z0JBQ2QsR0FBRyxDQUFDLENBQUMsSUFBSSxDQUFDLEdBQUcsQ0FBQyxFQUFFLE1BQU0sR0FBRyxHQUFHLEVBQUUsQ0FBQyxHQUFHLE1BQU0sRUFBRSxDQUFDLEVBQUUsRUFBRSxDQUFDO29CQUM1QyxJQUFJLElBQUksTUFBTSxDQUFDLFlBQVksQ0FBQyxLQUFLLENBQUMsUUFBUSxDQUFDLFFBQVEsQ0FBQyxFQUFFLEdBQUcsQ0FBQyxDQUFDLENBQUMsQ0FBQztnQkFDakUsQ0FBQztnQkFDRCxJQUFJLENBQUMsbUJBQW1CLENBQUMsSUFBSSxDQUFDLENBQUM7WUFDbkMsQ0FBQztRQUNMLENBQUM7UUFDRCwwQ0FBSyxHQUFMLFVBQU0sRUFBYTtZQUFuQixpQkE4REM7WUE3REcsSUFBSSxJQUFJLEdBQUcsSUFBSSxDQUFDO1lBQ2hCLEVBQUUsQ0FBQyxVQUFVLEdBQUcsYUFBYSxDQUFDO1lBQzlCLEVBQUUsQ0FBQyxNQUFNLEdBQUcsVUFBQyxFQUFTO2dCQUNsQixPQUFPLENBQUMsR0FBRyxDQUFDLFFBQVEsQ0FBQyxDQUFDO2dCQUN0QixFQUFFLENBQUMsQ0FBQyxJQUFJLENBQUMsYUFBYSxDQUFDLENBQUMsQ0FBQztvQkFDckIsSUFBSSxDQUFDLGFBQWEsQ0FBQyxFQUFFLEVBQUUsRUFBRSxDQUFDLENBQUM7Z0JBQy9CLENBQUM7WUFDTCxDQUFDLENBQUM7WUFDRixFQUFFLENBQUMsT0FBTyxHQUFHLFVBQUMsRUFBYztnQkFDeEIsT0FBTyxDQUFDLEdBQUcsQ0FBQyxTQUFTLENBQUMsQ0FBQztnQkFDdkIsRUFBRSxDQUFDLENBQUMsSUFBSSxDQUFDLGdCQUFnQixDQUFDLENBQUMsQ0FBQztvQkFDeEIsSUFBSSxDQUFDLGdCQUFnQixDQUFDLEVBQUUsRUFBRSxFQUFFLENBQUMsSUFBSSxFQUFFLEVBQUUsQ0FBQyxNQUFNLENBQUMsQ0FBQztnQkFDbEQsQ0FBQztZQUNMLENBQUMsQ0FBQztZQUNGLEVBQUUsQ0FBQyxTQUFTLEdBQUcsVUFBQyxFQUFnQjtnQkFDNUIsT0FBTyxDQUFDLEdBQUcsQ0FBQyxXQUFXLENBQUMsQ0FBQztnQkFDekIsSUFBSSxFQUFFLEdBQUcsV0FBVyxDQUFDLEdBQUcsRUFBRSxDQUFDO2dCQUMzQixJQUFJLElBQUksR0FBRyxJQUFJLFFBQVEsQ0FBQyxFQUFFLENBQUMsSUFBSSxDQUFDLENBQUM7Z0JBQ2pDLElBQUksVUFBVSxHQUFpQixJQUFJLENBQUMsUUFBUSxDQUFDLENBQUMsRUFBRSxJQUFJLENBQUMsQ0FBQztnQkFFdEQsTUFBTSxDQUFDLENBQUMsVUFBVSxDQUFDLENBQUMsQ0FBQztvQkFDakIsS0FBSyxZQUFZLENBQUMsaUJBQWlCO3dCQUMvQixLQUFJLENBQUMsZUFBZSxDQUFDLEVBQUUsRUFBRSxJQUFJLFFBQVEsQ0FBQyxFQUFFLENBQUMsSUFBSSxFQUFFLENBQUMsQ0FBQyxDQUFDLENBQUM7d0JBQ25ELEtBQUssQ0FBQztvQkFDVixLQUFLLFlBQVksQ0FBQyxjQUFjO3dCQUM1QixFQUFFLENBQUMsQ0FBQyxLQUFJLENBQUMsZUFBZSxDQUFDLENBQUMsQ0FBQzs0QkFDdkIsS0FBSSxDQUFDLEtBQUssQ0FBQyxFQUFFLEVBQUUsSUFBSSxRQUFRLENBQUMsRUFBRSxDQUFDLElBQUksRUFBRSxDQUFDLENBQUMsRUFBRSxLQUFJLENBQUMsZUFBZSxDQUFDLENBQUM7d0JBQ25FLENBQUM7d0JBQ0QsS0FBSyxDQUFDO29CQUNWLEtBQUssWUFBWSxDQUFDLFVBQVU7d0JBQ3hCLEVBQUUsQ0FBQyxDQUFDLEtBQUksQ0FBQyxXQUFXLENBQUMsQ0FBQyxDQUFDOzRCQUNuQixLQUFJLENBQUMsS0FBSyxDQUFDLEVBQUUsRUFBRSxJQUFJLFFBQVEsQ0FBQyxFQUFFLENBQUMsSUFBSSxFQUFFLENBQUMsQ0FBQyxFQUFFLEtBQUksQ0FBQyxXQUFXLENBQUMsQ0FBQzt3QkFDL0QsQ0FBQzt3QkFDRCxLQUFLLENBQUM7b0JBQ1YsS0FBSyxZQUFZLENBQUMsbUJBQW1CO3dCQUNqQyxLQUFJLENBQUMsaUJBQWlCLENBQUMsRUFBRSxFQUFFLElBQUksUUFBUSxDQUFDLEVBQUUsQ0FBQyxJQUFJLEVBQUUsQ0FBQyxDQUFDLENBQUMsQ0FBQzt3QkFDckQsS0FBSyxDQUFDO29CQUNWLEtBQUssWUFBWSxDQUFDLHNCQUFzQjt3QkFDcEMsS0FBSSxDQUFDLG9CQUFvQixDQUFDLEVBQUUsRUFBRSxJQUFJLFFBQVEsQ0FBQyxFQUFFLENBQUMsSUFBSSxFQUFFLENBQUMsQ0FBQyxDQUFDLENBQUM7d0JBQ3hELEtBQUssQ0FBQztvQkFDVixLQUFLLFlBQVksQ0FBQyxzQkFBc0I7d0JBQ3BDLEtBQUksQ0FBQyxvQkFBb0IsQ0FBQyxJQUFJLFFBQVEsQ0FBQyxFQUFFLENBQUMsSUFBSSxFQUFFLENBQUMsQ0FBQyxDQUFDLENBQUM7d0JBQ3BELEtBQUssQ0FBQztvQkFDVjt3QkFDSSxFQUFFLENBQUMsQ0FBQyxLQUFJLENBQUMsVUFBVSxDQUFDLENBQUMsQ0FBQzs0QkFDbEIsSUFBSSxDQUFDLEdBQUcsSUFBSSxTQUFTLEVBQUUsQ0FBQzs0QkFDeEIsQ0FBQyxDQUFDLElBQUksR0FBRyxJQUFJLFFBQVEsQ0FBQyxFQUFFLENBQUMsSUFBSSxFQUFFLENBQUMsQ0FBQyxDQUFDOzRCQUNsQyxFQUFFLENBQUMsQ0FBQyxFQUFFLENBQUMsSUFBSSxZQUFZLFdBQVcsQ0FBQyxDQUFDLENBQUM7Z0NBQ2pDLENBQUMsQ0FBQyxJQUFJLEdBQUcsTUFBTSxDQUFDLE1BQU0sQ0FBQzs0QkFDM0IsQ0FBQzs0QkFBQyxJQUFJLENBQUMsRUFBRSxDQUFDLENBQUMsT0FBTyxFQUFFLENBQUMsSUFBSSxLQUFLLFFBQVEsQ0FBQyxDQUFDLENBQUM7Z0NBQ3JDLENBQUMsQ0FBQyxJQUFJLEdBQUcsTUFBTSxDQUFDLElBQUksQ0FBQzs0QkFDekIsQ0FBQzs0QkFDRCxLQUFJLENBQUMsVUFBVSxDQUFDLEVBQUUsRUFBRSxDQUFDLENBQUMsQ0FBQzt3QkFDM0IsQ0FBQzt3QkFDRCxLQUFLLENBQUM7Z0JBQ2QsQ0FBQztnQkFDRCxJQUFJLEVBQUUsR0FBRyxXQUFXLENBQUMsR0FBRyxFQUFFLENBQUM7Z0JBRTNCLE9BQU8sQ0FBQyxHQUFHLENBQUMsT0FBTyxHQUFHLENBQUMsRUFBRSxHQUFHLEVBQUUsQ0FBQyxHQUFHLDJDQUEyQyxDQUFDLENBQUM7WUFDbkYsQ0FBQyxDQUFDO1lBQ0YsTUFBTSxDQUFDLElBQUksQ0FBQztRQUNoQixDQUFDO1FBQ0wsaUNBQUM7SUFBRCxDQUFDLEFBMU5ELENBQWdELGFBQWEsR0EwTjVEO0lBMU5ZLGdFQUEwQjtJQTBOdEMsQ0FBQztJQUNGO1FBQ0ksTUFBTSxDQUFDLElBQUksMEJBQTBCLEVBQUUsQ0FBQztJQUM1QyxDQUFDO0lBRkQsMEVBRUM7Ozs7O0lDbFZEO1FBbUJJLHNCQUFZLFFBQXFCO1lBakJqQyxnQkFBVyxHQUFHLEtBQUssQ0FBQztZQVVwQixxQkFBZ0IsR0FBRyxLQUFLLENBQUM7WUFDekIsZUFBVSxHQUFHLEtBQUssQ0FBQztZQU9mLElBQUksQ0FBQyxhQUFhLEdBQUcsUUFBUSxDQUFDO1lBRzlCLE1BQU0sQ0FBQyxnQkFBZ0IsQ0FBQyxXQUFXLEVBQUUsSUFBSSxDQUFDLFdBQVcsQ0FBQyxDQUFDO1lBQ3ZELE1BQU0sQ0FBQyxnQkFBZ0IsQ0FBQyxTQUFTLEVBQUUsSUFBSSxDQUFDLFNBQVMsQ0FBQyxDQUFDO1lBQ25ELE1BQU0sQ0FBQyxnQkFBZ0IsQ0FBQyxXQUFXLEVBQUUsSUFBSSxDQUFDLE1BQU0sQ0FBQyxDQUFDO1lBQ2xELE1BQU0sQ0FBQyxnQkFBZ0IsQ0FBQyxPQUFPLEVBQUUsSUFBSSxDQUFDLE9BQU8sQ0FBQyxDQUFDO1lBQy9DLE1BQU0sQ0FBQyxnQkFBZ0IsQ0FBQyxTQUFTLEVBQUUsSUFBSSxDQUFDLFNBQVMsQ0FBQyxDQUFDO1lBQ25ELE1BQU0sQ0FBQyxnQkFBZ0IsQ0FBQyxPQUFPLEVBQUUsSUFBSSxDQUFDLE9BQU8sQ0FBQyxDQUFDO1lBTS9DLElBQUksQ0FBQyxZQUFZLEdBQUcsUUFBUSxDQUFDLGFBQWEsQ0FBQyxLQUFLLENBQUMsQ0FBQztZQUNsRCxJQUFJLENBQUMsWUFBWSxDQUFDLEtBQUssQ0FBQyxRQUFRLEdBQUcsVUFBVSxDQUFDO1lBQzlDLElBQUksQ0FBQyxzQkFBc0IsR0FBRyxRQUFRLENBQUMsYUFBYSxDQUFDLFFBQVEsQ0FBQyxDQUFDO1lBQy9ELElBQUksQ0FBQyxzQkFBc0IsQ0FBQyxLQUFLLENBQUMsUUFBUSxHQUFHLFVBQVUsQ0FBQztZQUN4RCxJQUFJLENBQUMsc0JBQXNCLENBQUMsS0FBSyxDQUFDLElBQUksR0FBRyxJQUFJLENBQUMsc0JBQXNCLENBQUMsS0FBSyxDQUFDLEdBQUcsR0FBRyxJQUFJLENBQUMsc0JBQXNCLENBQUMsS0FBSyxDQUFDLE1BQU0sR0FBRyxHQUFHLENBQUM7WUFFaEksSUFBSSxDQUFDLHFCQUFxQixHQUFHLFFBQVEsQ0FBQyxhQUFhLENBQUMsUUFBUSxDQUFDLENBQUM7WUFDOUQsSUFBSSxDQUFDLHFCQUFxQixDQUFDLEtBQUssQ0FBQyxRQUFRLEdBQUcsVUFBVSxDQUFDO1lBQ3ZELElBQUksQ0FBQyxxQkFBcUIsQ0FBQyxLQUFLLENBQUMsSUFBSSxHQUFHLElBQUksQ0FBQyxxQkFBcUIsQ0FBQyxLQUFLLENBQUMsR0FBRyxHQUFHLEdBQUcsQ0FBQztZQUNuRixJQUFJLENBQUMscUJBQXFCLENBQUMsS0FBSyxDQUFDLE1BQU0sR0FBRyxHQUFHLENBQUM7WUFFOUMsSUFBSSxDQUFDLFlBQVksQ0FBQyxXQUFXLENBQUMsSUFBSSxDQUFDLHNCQUFzQixDQUFDLENBQUM7WUFDM0QsSUFBSSxDQUFDLFlBQVksQ0FBQyxXQUFXLENBQUMsSUFBSSxDQUFDLHFCQUFxQixDQUFDLENBQUM7WUFFMUQsUUFBUSxDQUFDLFdBQVcsQ0FBQyxJQUFJLENBQUMsWUFBWSxDQUFDLENBQUM7UUFDNUMsQ0FBQztRQUNNLDhCQUFPLEdBQWQ7WUFDSSxFQUFFLENBQUMsQ0FBQyxJQUFJLENBQUMsT0FBTyxDQUFDLENBQUMsQ0FBQztnQkFDZixJQUFJLENBQUMsT0FBTyxDQUFDLEtBQUssRUFBRSxDQUFDO1lBQ3pCLENBQUM7WUFDRCxJQUFJLENBQUMsT0FBTyxHQUFHLElBQUksQ0FBQztZQUNwQixNQUFNLENBQUMsbUJBQW1CLENBQUMsV0FBVyxFQUFFLElBQUksQ0FBQyxXQUFXLENBQUMsQ0FBQztZQUMxRCxNQUFNLENBQUMsbUJBQW1CLENBQUMsU0FBUyxFQUFFLElBQUksQ0FBQyxTQUFTLENBQUMsQ0FBQztZQUN0RCxNQUFNLENBQUMsbUJBQW1CLENBQUMsV0FBVyxFQUFFLElBQUksQ0FBQyxNQUFNLENBQUMsQ0FBQztZQUNyRCxNQUFNLENBQUMsbUJBQW1CLENBQUMsT0FBTyxFQUFFLElBQUksQ0FBQyxPQUFPLENBQUMsQ0FBQztZQUNsRCxNQUFNLENBQUMsbUJBQW1CLENBQUMsU0FBUyxFQUFFLElBQUksQ0FBQyxTQUFTLENBQUMsQ0FBQztZQUN0RCxNQUFNLENBQUMsbUJBQW1CLENBQUMsT0FBTyxFQUFFLElBQUksQ0FBQyxPQUFPLENBQUMsQ0FBQztZQUNsRCxFQUFFLENBQUMsQ0FBQyxJQUFJLENBQUMsYUFBYSxJQUFJLElBQUksQ0FBQyxZQUFZLENBQUMsQ0FBQyxDQUFDO2dCQUMxQyxJQUFJLENBQUMsYUFBYSxDQUFDLFdBQVcsQ0FBQyxJQUFJLENBQUMsWUFBWSxDQUFDLENBQUM7WUFDdEQsQ0FBQztZQUVELElBQUksQ0FBQyxPQUFPLEdBQUcsSUFBSSxDQUFDO1lBQ3BCLElBQUksQ0FBQyxXQUFXLEdBQUcsS0FBSyxDQUFDO1lBQ3pCLElBQUksQ0FBQyxTQUFTLEdBQUcsSUFBSSxDQUFDO1lBQ3RCLElBQUksQ0FBQyxhQUFhLEdBQUcsSUFBSSxDQUFDO1lBQzFCLElBQUksQ0FBQyxPQUFPLEdBQUcsSUFBSSxDQUFDO1lBQ3BCLElBQUksQ0FBQyxjQUFjLEdBQUcsSUFBSSxDQUFDO1lBQzNCLElBQUksQ0FBQyxZQUFZLEdBQUcsSUFBSSxDQUFDLGFBQWEsR0FBRSxJQUFJLENBQUMsWUFBWSxHQUFHLElBQUksQ0FBQyxzQkFBc0IsR0FBRyxJQUFJLENBQUMscUJBQXFCLEdBQUcsSUFBSSxDQUFDO1FBQ2hJLENBQUM7UUFDTSw4QkFBTyxHQUFkLFVBQWUsUUFBZ0IsRUFBRSxRQUFnQjtZQUFqRCxpQkFpRUM7WUFoRUcsSUFBSSxDQUFDLFNBQVMsR0FBRyxRQUFRLENBQUM7WUFDMUIsSUFBSSxDQUFDLFNBQVMsR0FBRyxRQUFRLENBQUM7WUFFMUIsSUFBSSxhQUFhLEdBQUcsRUFBRSxDQUFDO1lBQ3ZCLEVBQUUsQ0FBQyxDQUFDLE1BQU0sQ0FBQyxRQUFRLENBQUMsUUFBUSxJQUFJLFFBQVEsQ0FBQyxDQUFDLENBQUM7Z0JBQ3ZDLGFBQWEsSUFBSSxPQUFPLENBQUM7WUFDN0IsQ0FBQztZQUNELElBQUksQ0FBQyxDQUFDO2dCQUNGLGFBQWEsSUFBSSxRQUFRLENBQUM7WUFDOUIsQ0FBQztZQUVELElBQUksQ0FBQyxnQkFBZ0IsR0FBRyxDQUFDLElBQUksQ0FBQyxTQUFTLElBQUksV0FBVyxDQUFDLElBQUksQ0FBQyxJQUFJLENBQUMsU0FBUyxJQUFJLFdBQVcsQ0FBQyxJQUFJLENBQUMsSUFBSSxDQUFDLFNBQVMsSUFBSSxLQUFLLENBQUMsQ0FBQztZQUN4SCxhQUFhLElBQUksSUFBSSxDQUFDLFNBQVMsR0FBRyxHQUFHLEdBQUcsSUFBSSxDQUFDLFNBQVMsQ0FBQztZQUN2RCxJQUFJLENBQUMsT0FBTyxHQUFHLElBQUksU0FBUyxDQUFDLGFBQWEsQ0FBQyxDQUFDO1lBQzVDLElBQUksQ0FBQyxPQUFPLENBQUMsVUFBVSxHQUFHLGFBQWEsQ0FBQztZQUN4QyxJQUFJLENBQUMsYUFBYSxHQUFHLDBDQUErQixFQUFFO2lCQUNqRCxZQUFZLENBQUMsVUFBQyxFQUFhLEVBQUUsRUFBUztZQUV2QyxDQUFDLENBQUMsQ0FBQyxTQUFTLENBQUMsVUFBQyxFQUFhLEVBQUUsT0FBa0I7WUFFL0MsQ0FBQyxDQUFDLENBQUMsZUFBZSxDQUFDLFVBQUMsRUFBYSxFQUFFLElBQVksRUFBRSxPQUFlO1lBRWhFLENBQUMsQ0FBQyxDQUFDLGtCQUFrQixDQUFDLFVBQUMsVUFBa0I7WUFFekMsQ0FBQyxDQUFDLENBQUMsY0FBYyxDQUFDLFVBQUMsS0FBdUIsRUFBRSxPQUFnQjtnQkFDeEQsRUFBRSxDQUFDLENBQUMsS0FBSSxDQUFDLFVBQVUsQ0FBQyxDQUFDLENBQUM7b0JBQ2xCLElBQUksS0FBSyxHQUFHLEtBQUksQ0FBQyxnQkFBZ0IsRUFBRSxDQUFDO29CQUNwQyxLQUFJLENBQUMsc0JBQXNCLENBQUMsVUFBVSxDQUFDLElBQUksQ0FBQyxDQUFDLFNBQVMsQ0FBQyxLQUFLLEVBQUUsT0FBTyxDQUFDLE9BQU8sR0FBRyxLQUFLLEVBQUUsT0FBTyxDQUFDLE9BQU8sR0FBRyxLQUFLLEVBQUUsT0FBTyxDQUFDLEtBQUssR0FBRyxLQUFLLEVBQUUsT0FBTyxDQUFDLE1BQU0sR0FBRyxLQUFLLENBQUMsQ0FBQztnQkFDbkssQ0FBQztnQkFDRCxJQUFJLENBQUMsQ0FBQztvQkFDRixLQUFJLENBQUMsc0JBQXNCLENBQUMsVUFBVSxDQUFDLElBQUksQ0FBQyxDQUFDLFNBQVMsQ0FBQyxLQUFLLEVBQUUsT0FBTyxDQUFDLE9BQU8sR0FBRyxLQUFLLEVBQUUsT0FBTyxDQUFDLE9BQU8sQ0FBQyxDQUFDO2dCQUM1RyxDQUFDO1lBQ0wsQ0FBQyxDQUFDLENBQUMsaUJBQWlCLENBQUMsVUFBQyxRQUFtQjtZQUV6QyxDQUFDLENBQUMsQ0FBQyxtQkFBbUIsQ0FBQyxVQUFDLEtBQWdCO2dCQUNwQyxLQUFJLENBQUMscUJBQXFCLENBQUMsVUFBVSxDQUFDLElBQUksQ0FBQyxDQUFDLFlBQVksQ0FBQyxLQUFJLENBQUMsT0FBTyxFQUFFLENBQUMsRUFBRSxDQUFDLENBQUMsQ0FBQztZQUNqRixDQUFDLENBQUMsQ0FBQyxzQkFBc0IsQ0FBQyxVQUFDLEdBQVU7Z0JBQ2pDLEVBQUUsQ0FBQyxDQUFDLEtBQUksQ0FBQyxVQUFVLENBQUMsQ0FBQyxDQUFDO29CQUNsQixJQUFJLEtBQUssR0FBRyxLQUFJLENBQUMsZ0JBQWdCLEVBQUUsQ0FBQztvQkFDcEMsS0FBSSxDQUFDLHFCQUFxQixDQUFDLEtBQUssQ0FBQyxHQUFHLEdBQUcsQ0FBQyxHQUFHLENBQUMsQ0FBQyxHQUFHLEtBQUssQ0FBQyxHQUFHLElBQUksQ0FBQztvQkFDOUQsS0FBSSxDQUFDLHFCQUFxQixDQUFDLEtBQUssQ0FBQyxJQUFJLEdBQUcsQ0FBQyxHQUFHLENBQUMsQ0FBQyxHQUFHLEtBQUssQ0FBQyxHQUFHLElBQUksQ0FBQztnQkFDbkUsQ0FBQztnQkFDRCxJQUFJLENBQUMsQ0FBQztvQkFDRixLQUFJLENBQUMscUJBQXFCLENBQUMsS0FBSyxDQUFDLEdBQUcsR0FBRyxHQUFHLENBQUMsQ0FBQyxHQUFHLElBQUksQ0FBQztvQkFDcEQsS0FBSSxDQUFDLHFCQUFxQixDQUFDLEtBQUssQ0FBQyxJQUFJLEdBQUcsR0FBRyxDQUFDLENBQUMsR0FBRyxJQUFJLENBQUM7Z0JBQ3pELENBQUM7WUFDTCxDQUFDLENBQUMsQ0FBQyxVQUFVLENBQUMsVUFBQyxLQUF1QixFQUFFLE9BQWdCO2dCQUNwRCxFQUFFLENBQUMsQ0FBQyxLQUFJLENBQUMsVUFBVSxDQUFDLENBQUMsQ0FBQztvQkFDbEIsSUFBSSxLQUFLLEdBQUcsS0FBSSxDQUFDLGdCQUFnQixFQUFFLENBQUM7b0JBQ3BDLEtBQUksQ0FBQyxzQkFBc0IsQ0FBQyxLQUFLLEdBQUcsS0FBSyxDQUFDLEtBQUssR0FBRyxLQUFLLENBQUM7b0JBQ3hELEtBQUksQ0FBQyxzQkFBc0IsQ0FBQyxNQUFNLEdBQUcsS0FBSyxDQUFDLE1BQU0sR0FBRyxLQUFLLENBQUM7b0JBQzFELEtBQUksQ0FBQyxZQUFZLENBQUMsS0FBSyxDQUFDLEtBQUssR0FBRyxLQUFJLENBQUMsc0JBQXNCLENBQUMsS0FBSyxHQUFHLElBQUksQ0FBQztvQkFDekUsS0FBSSxDQUFDLFlBQVksQ0FBQyxLQUFLLENBQUMsTUFBTSxHQUFHLEtBQUksQ0FBQyxzQkFBc0IsQ0FBQyxNQUFNLEdBQUcsSUFBSSxDQUFDO29CQUMzRSxLQUFJLENBQUMsc0JBQXNCLENBQUMsVUFBVSxDQUFDLElBQUksQ0FBQyxDQUFDLFNBQVMsQ0FBQyxLQUFLLEVBQUUsQ0FBQyxFQUFFLENBQUMsRUFBRSxLQUFJLENBQUMsc0JBQXNCLENBQUMsS0FBSyxFQUFFLEtBQUksQ0FBQyxzQkFBc0IsQ0FBQyxNQUFNLENBQUMsQ0FBQztnQkFDL0ksQ0FBQztnQkFDRCxJQUFJLENBQUMsQ0FBQztvQkFDRixLQUFJLENBQUMsc0JBQXNCLENBQUMsS0FBSyxHQUFHLEtBQUssQ0FBQyxLQUFLLENBQUM7b0JBQ2hELEtBQUksQ0FBQyxzQkFBc0IsQ0FBQyxNQUFNLEdBQUcsS0FBSyxDQUFDLE1BQU0sQ0FBQztvQkFDbEQsS0FBSSxDQUFDLFlBQVksQ0FBQyxLQUFLLENBQUMsS0FBSyxHQUFHLEtBQUksQ0FBQyxzQkFBc0IsQ0FBQyxLQUFLLEdBQUcsSUFBSSxDQUFDO29CQUN6RSxLQUFJLENBQUMsWUFBWSxDQUFDLEtBQUssQ0FBQyxNQUFNLEdBQUcsS0FBSSxDQUFDLHNCQUFzQixDQUFDLE1BQU0sR0FBRyxJQUFJLENBQUM7b0JBQzNFLEtBQUksQ0FBQyxzQkFBc0IsQ0FBQyxVQUFVLENBQUMsSUFBSSxDQUFDLENBQUMsU0FBUyxDQUFDLEtBQUssRUFBRSxDQUFDLEVBQUUsQ0FBQyxDQUFDLENBQUM7Z0JBQ3hFLENBQUM7Z0JBQ0QsS0FBSSxDQUFDLGNBQWMsR0FBRyxLQUFLLENBQUM7WUFDaEMsQ0FBQyxDQUFDLENBQUMsS0FBSyxDQUFDLElBQUksQ0FBQyxPQUFPLENBQUMsQ0FBQztRQUMvQixDQUFDO1FBRU0sdUNBQWdCLEdBQXZCO1lBQ0ksRUFBRSxDQUFDLENBQUMsSUFBSSxDQUFDLFVBQVUsSUFBSSxJQUFJLENBQUMsY0FBYyxJQUFJLElBQUksQ0FBQyxDQUFDLENBQUM7Z0JBQ2pELE1BQU0sQ0FBQyxNQUFNLENBQUMsV0FBVyxHQUFHLElBQUksQ0FBQyxjQUFjLENBQUMsTUFBTSxDQUFDO1lBQzNELENBQUM7WUFDRCxJQUFJLENBQUMsQ0FBQztnQkFDRixNQUFNLENBQUMsR0FBRyxDQUFDO1lBQ2YsQ0FBQztRQUNMLENBQUM7UUFDTyxnQ0FBUyxHQUFqQixVQUFrQixFQUFpQjtZQUMvQixJQUFJLENBQUMsYUFBYSxDQUFDLFdBQVcsQ0FBQyw2QkFBZ0IsQ0FBQyxFQUFFLENBQUMsQ0FBQyxDQUFDO1FBQ3pELENBQUM7UUFDTyw4QkFBTyxHQUFmLFVBQWdCLEVBQWlCO1lBQzdCLElBQUksQ0FBQyxhQUFhLENBQUMsU0FBUyxDQUFDLDZCQUFnQixDQUFDLEVBQUUsQ0FBQyxDQUFDLENBQUM7UUFDdkQsQ0FBQztRQUNPLDhCQUFPLEdBQWYsVUFBZ0IsRUFBYztZQUMxQixJQUFJLENBQUMsYUFBYSxDQUFDLGVBQWUsQ0FBQyxFQUFFLENBQUMsTUFBTSxHQUFHLENBQUMsQ0FBQyxDQUFDLENBQUMsQ0FBQyxDQUFDLENBQUMsQ0FBQyxDQUFDLENBQUMsQ0FBQyxDQUFDO1FBQy9ELENBQUM7UUFDTyw2QkFBTSxHQUFkLFVBQWUsRUFBYztZQUN6QixJQUFJLENBQUMsYUFBYSxDQUFDLGlCQUFpQixDQUFDLEVBQUUsQ0FBQyxFQUFDLEVBQUUsQ0FBQyxLQUFLLEVBQUUsQ0FBQyxFQUFFLEVBQUUsQ0FBQyxLQUFLLEVBQUUsQ0FBQyxDQUFDO1FBQ3RFLENBQUM7UUFDTyxnQ0FBUyxHQUFqQixVQUFrQixFQUFjO1lBQzVCLElBQUksQ0FBQyxhQUFhLENBQUMsV0FBVyxDQUFDLEVBQUUsQ0FBQyxNQUFNLENBQUMsQ0FBQztRQUM5QyxDQUFDO1FBQ08sa0NBQVcsR0FBbkIsVUFBb0IsRUFBYztZQUM5QixJQUFJLENBQUMsYUFBYSxDQUFDLGFBQWEsQ0FBQyxFQUFFLENBQUMsTUFBTSxDQUFDLENBQUM7UUFDaEQsQ0FBQztRQUNMLG1CQUFDO0lBQUQsQ0FBQyxBQXRLRCxJQXNLQztJQXRLWSxvQ0FBWSJ9