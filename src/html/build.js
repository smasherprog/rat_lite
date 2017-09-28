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
//# sourceMappingURL=data:application/json;base64,eyJ2ZXJzaW9uIjozLCJmaWxlIjoiYnVpbGQuanMiLCJzb3VyY2VSb290IjoiQzovVXNlcnMvc2NvdHQvRG9jdW1lbnRzL0dpdEh1Yi9SZW1vdGVfQWNjZXNzX0xpYnJhcnkvc3JjL2h0bWwvc3JjLyIsInNvdXJjZXMiOlsiSW5wdXRfTGl0ZS50cyIsIlJhdF9MaXRlLnRzIiwiQ2xpZW50V2luZG93LnRzIl0sIm5hbWVzIjpbXSwibWFwcGluZ3MiOiI7Ozs7Ozs7Ozs7Ozs7SUFDQSxJQUFZLFFBeUhYO0lBekhELFdBQVksUUFBUTtRQUNoQix5Q0FBUyxDQUFBO1FBQ1QseUNBQVMsQ0FBQTtRQUNULHlDQUFTLENBQUE7UUFDVCx5Q0FBUyxDQUFBO1FBQ1QseUNBQVMsQ0FBQTtRQUNULHlDQUFTLENBQUE7UUFDVCwwQ0FBVSxDQUFBO1FBQ1YsMENBQVUsQ0FBQTtRQUNWLDBDQUFVLENBQUE7UUFDViwwQ0FBVSxDQUFBO1FBQ1YsMENBQVUsQ0FBQTtRQUNWLDBDQUFVLENBQUE7UUFDViwwQ0FBVSxDQUFBO1FBQ1YsMENBQVUsQ0FBQTtRQUNWLDBDQUFVLENBQUE7UUFDViwwQ0FBVSxDQUFBO1FBQ1YsMENBQVUsQ0FBQTtRQUNWLDBDQUFVLENBQUE7UUFDViwwQ0FBVSxDQUFBO1FBQ1YsMENBQVUsQ0FBQTtRQUNWLDBDQUFVLENBQUE7UUFDViwwQ0FBVSxDQUFBO1FBQ1YsMENBQVUsQ0FBQTtRQUNWLDBDQUFVLENBQUE7UUFDViwwQ0FBVSxDQUFBO1FBQ1YsMENBQVUsQ0FBQTtRQUNWLDBDQUFVLENBQUE7UUFDViwwQ0FBVSxDQUFBO1FBQ1YsMENBQVUsQ0FBQTtRQUNWLDBDQUFVLENBQUE7UUFDViwwQ0FBVSxDQUFBO1FBQ1YsMENBQVUsQ0FBQTtRQUNWLDBDQUFVLENBQUE7UUFDViwwQ0FBVSxDQUFBO1FBQ1YsMENBQVUsQ0FBQTtRQUNWLDBDQUFVLENBQUE7UUFDVixrREFBYyxDQUFBO1FBQ2Qsb0RBQWUsQ0FBQTtRQUNmLDBEQUFrQixDQUFBO1FBQ2xCLDhDQUFZLENBQUE7UUFDWixrREFBYyxDQUFB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QztZQUNULEtBQUssR0FBRyxDQUFDO1lBQ1QsS0FBSyxHQUFHLENBQUM7WUFDVCxLQUFLLEdBQUcsQ0FBQztZQUNULEtBQUssR0FBRyxDQUFDO1lBQ1QsS0FBSyxHQUFHLENBQUM7WUFDVCxLQUFLLEdBQUcsQ0FBQztZQUNULEtBQUssR0FBRyxDQUFDO1lBQ1QsS0FBSyxHQUFHLENBQUM7WUFDVCxLQUFLLEdBQUcsQ0FBQztZQUNULEtBQUssR0FBRyxDQUFDO1lBQ1QsS0FBSyxHQUFHLENBQUM7WUFDVCxLQUFLLEdBQUcsQ0FBQztZQUNULEtBQUssR0FBRyxDQUFDO1lBQ1QsS0FBSyxHQUFHLENBQUM7WUFDVCxLQUFLLEdBQUcsQ0FBQztZQUNULEtBQUssR0FBRyxDQUFDO1lBQ1QsS0FBSyxHQUFHLENBQUM7WUFDVCxLQUFLLEdBQUcsQ0FBQztZQUNULEtBQUssR0FBRyxDQUFDO1lBQ1QsS0FBSyxHQUFHLENBQUM7WUFDVCxLQUFLLEdBQUcsQ0FBQztZQUNULEtBQUssR0FBRyxDQUFDO1lBQ1QsS0FBSyxHQUFHO2dCQUNKLE1BQU0sQ0FBQyxRQUFRLENBQUMsS0FBSyxHQUFHLENBQUMsQ0FBQyxHQUFHLENBQUMsR0FBRyxHQUFHLENBQUMsR0FBRyxDQUFDLENBQUM7WUFDOUMsS0FBSyxHQUFHLENBQUM7WUFDVCxLQUFLLEdBQUcsQ0FBQztZQUNULEtBQUssR0FBRyxDQUFDO1lBQ1QsS0FBSyxHQUFHLENBQUM7WUFDVCxLQUFLLEdBQUcsQ0FBQztZQUNULEtBQUssR0FBRyxDQUFDO1lBQ1QsS0FBSyxHQUFHLENBQUM7WUFDVCxLQUFLLEdBQUcsQ0FBQztZQUNULEtBQUssR0FBRyxDQUFDO1lBQ1QsS0FBSyxHQUFHLENBQUM7WUFDVCxLQUFLLEdBQUcsQ0FBQztZQUNULEtBQUssR0FBRyxDQUFDO1lBQ1QsS0FBSyxHQUFHLENBQUM7WUFDVCxLQUFLLEdBQUcsQ0FBQztZQUNULEtBQUssR0FBRyxDQUFDO1lBQ1QsS0FBSyxHQUFHLENBQUM7WUFDVCxLQUFLLEdBQUcsQ0FBQztZQUNULEtBQUssR0FBRyxD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