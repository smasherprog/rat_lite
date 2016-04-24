/// <reference path="zstd.d.ts" />
//the file above needs to be named .ts in order for typescript to actually generate javascript code..

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
            }
            export class Rect {
                static sizeof(): number { return 8 + Point.sizeof(); }//actual bytes used
                constructor(public Origin: Point, public Height: number, public Width: number) { }
                static FromArray(data: Uint8Array): Rect {
                    var arr = new Int32Array(data.slice(0, this.sizeof()).buffer);
                    return new Rect(new Point(arr[0], arr[1]), arr[2], arr[3]);
                }
            }
        }
        export module Network {
            enum PACKET_TYPES {
                INVALID,
                HTTP_MSG,
                SCREENIMAGE,
                SCREENIMAGEDIF,
                MOUSEPOS,
                MOUSEIMAGE,
                KEYEVENT,
                //use LAST_PACKET_TYPE as the starting point of your custom packet types. Everything before this is used internally by the library
                LAST_PACKET_TYPE
            }
            export class PacketHeader {
                Packet_Type: PACKET_TYPES;
                Payload_Length: number;
                UncompressedLength: number;
                sizeof() { return 12; }//actual bytes used
                constructor(d: ArrayBuffer) {
                    var data = new DataView(d);
                    this.Packet_Type = data.getInt32(0, true);
                    this.Payload_Length = data.getInt32(4, true);
                    this.UncompressedLength = data.getInt32(8, true);
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

                constructor(private _Screen_Canvas_Id: string, private _Mouse_Canvas_Id: string) {
                    window.addEventListener("resize", this.onresize);
                }

                public ScaleView = (b: boolean): void => {
                    this._ScaleImage = b;
                }
                onresize = (ev: UIEvent): void => {

                    if (this._ScaleImage && this._OriginalImage != null) {
                        var elem = <HTMLCanvasElement>document.getElementById(this._Screen_Canvas_Id);
                        var scale = this.GetScalingFactor();
                        elem.width = this._OriginalImage.width * scale;
                        elem.height = this._OriginalImage.height * scale;
                        elem.getContext("2d").drawImage(this._OriginalImage, 0, 0, elem.width, elem.height);
                    } else if (!this._ScaleImage && this._OriginalImage != null) {
                        var elem = <HTMLCanvasElement>document.getElementById(this._Screen_Canvas_Id);
                        if (elem.height != this._OriginalImage.height || elem.width != this._OriginalImage.width) {
                            elem.width = this._OriginalImage.width;
                            elem.height = this._OriginalImage.height;
                            elem.getContext("2d").drawImage(this._OriginalImage, 0, 0);
                        }
                    }
                }
                GetScalingFactor(): number {
                    if (this._OriginalImage != null) {
                        return window.innerHeight / this._OriginalImage.height;
                    } else {
                        return 1.0;
                    }
                }
                public OnReceive_ImageDif = (socket: WebSocket, rect: Utilities.Rect, img: string): void => {
                    "use strict";
                    //console.log('coords' + coords.X + ' ' + coords.Y + ' ' + coords.Width + ' ' + coords.Height);
                    var i = new Image();
                    i.src = "data:image/jpeg;base64," + img
                    var self = this;
                    i.onload = function () {
                        var elem = <HTMLCanvasElement>document.getElementById(self._Screen_Canvas_Id);
                        if (self._ScaleImage) {
                            var scale = self.GetScalingFactor();
                            elem.getContext("2d").drawImage(i, rect.Origin.X * scale, rect.Origin.Y * scale, rect.Width * scale, rect.Height * scale);
                        } else {
                            elem.getContext("2d").drawImage(i, rect.Origin.X, rect.Origin.Y);
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
                        var elem = <HTMLCanvasElement>document.getElementById(self._Screen_Canvas_Id);

                        if (self._ScaleImage) {
                            var scale = self.GetScalingFactor();
                            elem.width = i.width * scale;
                            elem.height = i.height * scale;
                            elem.getContext("2d").drawImage(i, 0, 0, elem.width, elem.height);
                        } else {
                            elem.width = i.width;
                            elem.height = i.height;
                            elem.getContext("2d").drawImage(i, 0, 0);
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
                    var elem = <HTMLCanvasElement>document.getElementById(this._Mouse_Canvas_Id);
                    elem.width = point.X;
                    elem.height = point.Y;
                    try {
                        this._Cursor = elem.getContext("2d").createImageData(point.X, point.Y);

                        for (var i = 0; i < this._Cursor.data.length; i += 4) {
                            this._Cursor.data[i + 0] = img[i + 0];
                            this._Cursor.data[i + 1] = img[i + 1];
                            this._Cursor.data[i + 2] = img[i + 2];
                            this._Cursor.data[i + 3] = img[i + 3];
                        }
                        elem.getContext("2d").putImageData(this._Cursor, 0, 0);
                    } catch (e) {
                        console.log(e.message);
                    }

                }
                public OnReceive_MousePos = (socket: WebSocket, pos: Utilities.Point): void => {
                    var elem = <HTMLCanvasElement>document.getElementById(this._Mouse_Canvas_Id);
                    if (this._ScaleImage) {
                        var scale = this.GetScalingFactor();
                        elem.style.top = (pos.Y * scale) + "px";
                        elem.style.left = (pos.X * scale) + "px";
                    } else {
                        elem.style.top = pos.Y + "px";
                        elem.style.left = pos.X + "px";
                    }
                }
            }


            export class ClientNetworkDriver {
                _Socket: WebSocket;
                _SocketStats: SocketStats;
                _TotalMemoryUsed: number;

                constructor(private _IClientDriver: ClientDriver, private _dst_host: string, private _dst_port: string) { }
                public Start = (): void => {


                    var connectstring = "";
                    if (window.location.protocol != "https:") {
                        connectstring += "ws://";
                    } else {
                        connectstring += "wss://";
                    }
                    connectstring += this._dst_host + ":" + this._dst_port + "/rdp";
                    this._Socket = new WebSocket(connectstring);
                    this._Socket.binaryType = 'arraybuffer';
                    this._Socket.onopen = this.OnOpen;
                    this._Socket.onclose = this.OnClose;
                    this._Socket.onmessage = this.OnMessage;
                }

                public Stop = (): void => {
                    this._Socket.close(1001, "Web Browser called Stop()");
                    this._Socket = null;
                }
                OnMessage = (ev: MessageEvent): void => {
                    var t0 = performance.now();
                    var packetheader = new PacketHeader(ev.data);

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
                    console.log("took " + (t1 - t0) + " milliseconds to Decompress the receive loop")
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
                    console.log("took " + (t1 - t0) + " milliseconds to process the receive loop");
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

