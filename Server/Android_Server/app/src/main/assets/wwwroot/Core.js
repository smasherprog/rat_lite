/// <reference path="zstd.d.ts" />
//the file above needs to be named .ts in order for typescript to actually generate javascript code..
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
                return Rect;
            }());
            Utilities.Rect = Rect;
        })(Utilities = Remote_Access_Library.Utilities || (Remote_Access_Library.Utilities = {}));
        var Network;
        (function (Network) {
            var PACKET_TYPES;
            (function (PACKET_TYPES) {
                PACKET_TYPES[PACKET_TYPES["INVALID"] = 0] = "INVALID";
                PACKET_TYPES[PACKET_TYPES["HTTP_MSG"] = 1] = "HTTP_MSG";
                PACKET_TYPES[PACKET_TYPES["SCREENIMAGE"] = 2] = "SCREENIMAGE";
                PACKET_TYPES[PACKET_TYPES["SCREENIMAGEDIF"] = 3] = "SCREENIMAGEDIF";
                PACKET_TYPES[PACKET_TYPES["MOUSEPOS"] = 4] = "MOUSEPOS";
                PACKET_TYPES[PACKET_TYPES["MOUSEIMAGE"] = 5] = "MOUSEIMAGE";
                PACKET_TYPES[PACKET_TYPES["KEYEVENT"] = 6] = "KEYEVENT";
                //use LAST_PACKET_TYPE as the starting point of your custom packet types. Everything before this is used internally by the library
                PACKET_TYPES[PACKET_TYPES["LAST_PACKET_TYPE"] = 7] = "LAST_PACKET_TYPE";
            })(PACKET_TYPES || (PACKET_TYPES = {}));
            var PacketHeader = (function () {
                function PacketHeader(d) {
                    var data = new DataView(d);
                    this.Packet_Type = data.getInt32(0, true);
                    this.Payload_Length = data.getInt32(4, true);
                    this.UncompressedLength = data.getInt32(8, true);
                }
                PacketHeader.prototype.sizeof = function () { return 12; }; //actual bytes used
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
                function ClientDriver(_Screen_Canvas_Id, _Mouse_Canvas_Id) {
                    var _this = this;
                    this._Screen_Canvas_Id = _Screen_Canvas_Id;
                    this._Mouse_Canvas_Id = _Mouse_Canvas_Id;
                    this.OnReceive_ImageDif = function (socket, rect, img) {
                        "use strict";
                        //console.log('coords' + coords.X + ' ' + coords.Y + ' ' + coords.Width + ' ' + coords.Height);
                        var i = new Image();
                        i.src = "data:image/jpeg;base64," + img;
                        var canvid = _this._Screen_Canvas_Id;
                        i.onload = function () {
                            var elem = document.getElementById(canvid);
                            elem.getContext("2d").drawImage(i, rect.Origin.X, rect.Origin.Y);
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
                        var canvid = _this._Screen_Canvas_Id;
                        i.onload = function () {
                            var elem = document.getElementById(canvid);
                            elem.width = i.width;
                            elem.height = i.height;
                            elem.getContext("2d").drawImage(i, 0, 0);
                            // console.log("ctx.drawImage" + coords.Y, "  " + coords.X);
                        };
                        i.onerror = function (stuff) {
                            console.log("Img Onerror:", stuff);
                        };
                    };
                    this.OnReceive_MouseImage = function (socket, point, img) {
                        "use strict";
                        //console.log('coords' + coords.X + ' ' + coords.Y + ' ' + coords.Width + ' ' + coords.Height);
                        var elem = document.getElementById(_this._Mouse_Canvas_Id);
                        elem.width = point.X;
                        elem.height = point.Y;
                        try {
                            _this._Cursor = elem.getContext("2d").createImageData(point.X, point.Y);
                            for (var i = 0; i < _this._Cursor.data.length; i += 4) {
                                _this._Cursor.data[i + 0] = img[i + 0];
                                _this._Cursor.data[i + 1] = img[i + 1];
                                _this._Cursor.data[i + 2] = img[i + 2];
                                _this._Cursor.data[i + 3] = img[i + 3];
                            }
                            elem.getContext("2d").putImageData(_this._Cursor, 0, 0);
                        }
                        catch (e) {
                            console.log(e.message);
                        }
                    };
                    this.OnReceive_MousePos = function (socket, pos) {
                        var elem = document.getElementById(_this._Mouse_Canvas_Id);
                        elem.style.top = pos.Y + "px";
                        elem.style.left = pos.X + "px";
                    };
                }
                return ClientDriver;
            }());
            Network.ClientDriver = ClientDriver;
            var ClientNetworkDriver = (function () {
                function ClientNetworkDriver(_IClientDriver, _dst_host, _dst_port) {
                    var _this = this;
                    this._IClientDriver = _IClientDriver;
                    this._dst_host = _dst_host;
                    this._dst_port = _dst_port;
                    this.Start = function () {
                        var connectstring = "";
                        if (window.location.protocol != "https:") {
                            connectstring += "ws://";
                        }
                        else {
                            connectstring += "wss://";
                        }
                        connectstring += _this._dst_host + ":" + _this._dst_port + "/rdp";
                        _this._Socket = new WebSocket(connectstring);
                        _this._Socket.binaryType = 'arraybuffer';
                        _this._Socket.onopen = _this.OnOpen;
                        _this._Socket.onclose = _this.OnClose;
                        _this._Socket.onmessage = _this.OnMessage;
                    };
                    this.Stop = function () {
                        _this._Socket.close(1001, "Web Browser called Stop()");
                        _this._Socket = null;
                    };
                    this.OnMessage = function (ev) {
                        var t0 = performance.now();
                        var packetheader = new PacketHeader(ev.data);
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
                        console.log("took " + (t1 - t0) + " milliseconds to Decompress the receive loop");
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
                        console.log("took " + (t1 - t0) + " milliseconds to process the receive loop");
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
