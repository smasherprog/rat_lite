export enum OpCode { CONTINUATION = 0, TEXT = 1, BINARY = 2, CLOSE = 8, PING = 9, PONG = 10, INVALID = 255 };
export class WSMessage {
    data: DataView;
    code: OpCode;
};
export class Point {
    X: number;
    Y: number;
};
export class Rect {
    Origin: Point;
    Height: number;
    Width: number;
};
export class Monitor {
    Id: number;
    Index: number;
    Height: number;
    Width: number;
    // Offsets are the number of pixels that a monitor can be from the origin. For example, users can shuffle their
    // monitors around so this affects their offset.
    OffsetX: number;
    OffsetY: number;
    Name: string;
    Scaling: number;
};
export class ClientSettings {
    ShareClip = false;
    ImageCompressionSetting = 70;
    EncodeImagesAsGrayScale = false;
   MonitorsToWatch = new Array<Monitor>();
};
export enum PACKET_TYPES {
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
    ONCLIENTSETTINGSCHANGED,
    // use LAST_PACKET_TYPE as the starting point of your custom packet types. Everything before this is used internally by the library
    LAST_PACKET_TYPE
};

export class IClientDriver {
    protected ShareClip = false;
    protected Monitors = new Array<Monitor>();
    protected WebSocket_: WebSocket;

    protected onConnection_: (ws: WebSocket, ev: Event) => void;
    protected onMessage_: (ws: WebSocket, message: WSMessage) => void;
    protected onDisconnection_: (ws: WebSocket, code: number, message: string) => void;
    protected onMonitorsChanged_: (monitors: Monitor[]) => void;
    protected onFrameChanged_: (image: HTMLImageElement, monitor: Monitor, rect: Rect) => void;
    protected onNewFrame_: (image: HTMLImageElement, monitor: Monitor, rect: Rect) => void;
    protected onMouseImageChanged_: (image: ImageData) => void;
    protected onMousePositionChanged_: (point: Point) => void;
    protected onClipboardChanged_: (clipstring: string) => void;

    public ConnectedToSelf_ = false;
    setShareClipboard(share: boolean): void { this.ShareClip = share; }
    getShareClipboard(): boolean { return this.ShareClip; }
    SendKeyUp(key: KeyCodes): void {
        if (this.ConnectedToSelf_) return;
        var data = new Uint8Array(4 + 1);
        var dataview = new DataView(data.buffer);
        dataview.setUint32(0, PACKET_TYPES.ONKEYUP);
        dataview.setUint8(4, key);
        this.WebSocket_.send(data);
    }
    SendKeyDown(key: KeyCodes): void {
        if (this.ConnectedToSelf_) return;
        var data = new Uint8Array(4 + 1);
        var dataview = new DataView(data.buffer);
        dataview.setUint32(0, PACKET_TYPES.ONKEYDOWN);
        dataview.setUint8(4, key);
        this.WebSocket_.send(data);
    }
    SendMouseUp(button: MouseButtons): void {
        if (this.ConnectedToSelf_) return;
        var data = new Uint8Array(4 + 1);
        var dataview = new DataView(data.buffer);
        dataview.setUint32(0, PACKET_TYPES.ONMOUSEUP);
        dataview.setUint8(4, button);
        this.WebSocket_.send(data);
    }
    SendMouseDown(button: MouseButtons): void {
        if (this.ConnectedToSelf_) return;
        var data = new Uint8Array(4 + 1);
        var dataview = new DataView(data.buffer);
        dataview.setUint32(0, PACKET_TYPES.ONMOUSEDOWN);
        dataview.setUint8(4, button);
        this.WebSocket_.send(data);
    }
    SendMouseScroll(offset: number): void {
        if (this.ConnectedToSelf_) return;
        var data = new Uint8Array(4 + 4);
        var dataview = new DataView(data.buffer);
        dataview.setUint32(0, PACKET_TYPES.ONMOUSESCROLL);
        dataview.setUint32(4, offset);
        this.WebSocket_.send(data);
    }
    SendMousePosition(pos: Point): void {
        if (this.ConnectedToSelf_) return;
        var data = new Uint8Array(4 + 8);
        var dataview = new DataView(data.buffer);
        dataview.setUint32(0, PACKET_TYPES.ONMOUSEPOSITIONCHANGED);
        dataview.setInt32(4, pos.X);
        dataview.setInt32(8, pos.X);

        this.WebSocket_.send(data);
    }
    SendClipboardChanged(text: string): void {
        if (this.ConnectedToSelf_) return;
        var data = new Uint8Array(4 + text.length);
        var dataview = new DataView(data.buffer);
        dataview.setUint32(0, PACKET_TYPES.ONMOUSESCROLL);
        for (var i = 0; i < text.length; i++) {
            data[4 + i] = text.charCodeAt(0);
        }
        this.WebSocket_.send(data);
    }
    SendClientSettingsChanged(clientsettings:ClientSettings): void {
        if (!clientsettings || !clientsettings.MonitorsToWatch || clientsettings.MonitorsToWatch.length<=0) return;
        var beginsize = 1 + 4 + 1;
        var data = new Uint8Array(4 +beginsize + (4*clientsettings.MonitorsToWatch.length));
        var dataview = new DataView(data.buffer);
        var offset =0;
        dataview.setUint32(offset, PACKET_TYPES.ONCLIENTSETTINGSCHANGED);
        offset +=4;
        dataview.setInt8(offset, clientsettings.ShareClip ? 1: 0);
        offset +=1;
        dataview.setInt32(offset, clientsettings.ImageCompressionSetting);
        offset +=4;
        dataview.setInt8(offset, clientsettings.EncodeImagesAsGrayScale ? 1: 0);
        offset +=1; 
        for (var i = 0; i < clientsettings.MonitorsToWatch.length; i++) {
            dataview.setInt32(offset, clientsettings.MonitorsToWatch[i].Id);
            offset +=4;
        }
        this.WebSocket_.send(data);
    }
};

export class IClientDriverConfiguration extends IClientDriver {
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
    onFrameChanged(callback: (image: HTMLImageElement, monitor: Monitor, rect: Rect) => void): IClientDriverConfiguration {
        this.onFrameChanged_ = callback;
        return this;
    }
    onNewFrame(callback: (image: HTMLImageElement, monitor: Monitor, rect: Rect) => void): IClientDriverConfiguration {
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
    private _arrayBufferToBase64(buffer: Uint8Array): string {
        var binary = '';
        for (var i = 0; i < buffer.byteLength; i++) {
            binary += String.fromCharCode(buffer[i]);
        }
        return window.btoa(binary);
    }
    private MonitorsChanged(ws: WebSocket, dataview: DataView) {

        if (!this.onMonitorsChanged_)
            return;
        let sizeofmonitor = 7 * 4 + 128;
        let num = dataview.byteLength / sizeofmonitor;
        
        if (dataview.byteLength == num * sizeofmonitor && num < 8) {
            this.Monitors = new Array<Monitor>();
            let currentoffset = 0;
            for (var i = 0; i < num; i++) {
                currentoffset = i * sizeofmonitor;
                var name = '';
                for (var j = 0, strLen = 128; j < strLen; j++) { 
                    var char =String.fromCharCode(dataview.getUint8((24 +j) + currentoffset));
                    if(char == '\0') {
                        break;
                    }
                    name += char; 
                }
                this.Monitors.push({
                    Id: dataview.getInt32(0 + currentoffset, true),
                    Index: dataview.getInt32(4 + currentoffset, true),
                    Height: dataview.getInt32(8 + currentoffset, true),
                    Width: dataview.getInt32(12 + currentoffset, true),
                    OffsetX: dataview.getInt32(16 + currentoffset, true),
                    OffsetY: dataview.getInt32(20 + currentoffset, true), 
                    Name: name,
                    Scaling: dataview.getFloat32(24 + 128 + currentoffset, true)
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

    private Frame(ws: WebSocket, dataview: DataView, callback: (image: HTMLImageElement, monitor: Monitor, rect: Rect) => void) {

        if (dataview.byteLength >= 4 * 4 + 4) {
            var monitorid = dataview.getInt32(0, true);
            var rect = {
                Origin: {
                    X: dataview.getInt32(4, true),
                    Y: dataview.getInt32(8, true)
                },
                Height: dataview.getInt32(12, true),
                Width: dataview.getInt32(16, true)
            };

            var foundmonitor = this.Monitors.filter(a => a.Id == monitorid);
  
            if (foundmonitor.length > 0) {
                var i = new Image();
                i.src = "data:image/jpeg;base64," + this._arrayBufferToBase64(new Uint8Array(dataview.buffer, 20 + dataview.byteOffset));
                i.onload = (ev: Event) => {
                    callback(i, foundmonitor[0], rect);
                };
                i.onerror = (ev: Event) => {
                    console.log(ev);
                };
                i.oninvalid = (ev: Event) => { 
                    console.log(ev);
                };
            }
            return;
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
                    X: dataview.getInt32(0, true),
                    Y: dataview.getInt32(4, true)
                },
                Height: dataview.getInt32(8, true),
                Width: dataview.getInt32(12, true)
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
                X: dataview.getInt32(0, true),
                Y: dataview.getInt32(4, true)
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
        this.ConnectedToSelf_ = ws.url.toLowerCase().indexOf('127.0.0.1') != -1 || ws.url.toLowerCase().indexOf('localhost') != -1 || ws.url.toLowerCase().indexOf('::1') != -1;

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

            var t0 = performance.now();
            var data = new DataView(ev.data);
            var packettype = <PACKET_TYPES>data.getInt32(0, true);
            var self = this;
            //  console.log('received: ' + packettype);
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
            // console.log("took " + (t1 - t0) + " milliseconds to process the receive loop");
        };
        return this;
    }
};
export function CreateClientDriverConfiguration(): IClientDriverConfiguration {
    return new IClientDriverConfiguration();
}
