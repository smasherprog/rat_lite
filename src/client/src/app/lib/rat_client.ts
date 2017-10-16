class ClientWindow {
    Cursor_: ImageData;
    ScaleImage_ = false;
    ClientDriver_: IClientDriver;
    Socket_: WebSocket;
    Monitors = new Array<Monitor>();

    OriginalImage_: HTMLImageElement;
    HTMLCanvasScreenImage_: HTMLCanvasElement;
    HTMLCanvasMouseImage_: HTMLCanvasElement;

    HostName: HTMLInputElement;
    ConnectButton: HTMLButtonElement;
    Protocol: HTMLSelectElement;
    Port: HTMLInputElement;

    InfoDiv: HTMLDivElement;

    ScaleImage = false;

    constructor(private HTMLRoot_: HTMLElement) {
        this.HostName = document.createElement('input');
        this.HostName.value = "localhost";
        this.ConnectButton = document.createElement('button');
        this.ConnectButton.innerText = "Connect";
        this.Protocol = document.createElement('select');
        var ws = document.createElement('option');
        ws.text = "ws";
        ws.value = "ws";
        this.Protocol.options.add(ws);
        var wss = document.createElement('option');
        wss.text = "wss";
        wss.value = "wss";
        this.Protocol.options.add(wss);

        if (window.location.protocol == "https:") {
            this.Protocol.value = "wss";
            ws.disabled = true;
        }
        this.Port = document.createElement('input');
        this.Port.value = '6001';

        this.ConnectButton.onclick = () => { this.Connect(); };
        this.InfoDiv = document.createElement('div');

        this.HTMLRoot_.appendChild(this.InfoDiv);

        this.InfoDiv.appendChild(this.Protocol);
        this.InfoDiv.appendChild(this.HostName);
        this.InfoDiv.appendChild(this.Port);
        this.InfoDiv.appendChild(this.ConnectButton);

        var scalebutton = document.createElement('button');
        scalebutton.onclick = () => {
            if (this.HTMLCanvasScreenImage_ && this.HTMLCanvasScreenImage_.style.width == "100%") {
                this.HTMLCanvasScreenImage_.style.width = "";
                scalebutton.innerText = "Scale";
            } else if (this.HTMLCanvasScreenImage_ && this.HTMLCanvasScreenImage_.style.width == "") {
                this.HTMLCanvasScreenImage_.style.width = "100%";
                scalebutton.innerText = "Scale";
            } 
        };
        scalebutton.style.position = 'absolute';
        scalebutton.style.left = scalebutton.style.top = scalebutton.style.zIndex = '400';
        scalebutton.innerText = "Scale";
        this.HTMLRoot_.appendChild(scalebutton);
    }
    private Connect(): void {
        if (this.ConnectButton.disabled) return;
        this.ConnectButton.disabled = true;

        this.Socket_ = new WebSocket(this.Protocol.value + "://" + this.HostName.value + ":" + this.Port.value);
        this.Socket_.binaryType = 'arraybuffer';
        this.ClientDriver_ = CreateClientDriverConfiguration()
            .onConnection((ws: WebSocket, ev: Event) => {
                console.log('onConnection');
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

                this.HTMLRoot_.style.position = 'relative';
                this.HTMLCanvasScreenImage_ = document.createElement('canvas');
                this.HTMLCanvasScreenImage_.style.position = 'absolute';
                this.HTMLCanvasScreenImage_.style.left = this.HTMLCanvasScreenImage_.style.top = this.HTMLCanvasScreenImage_.style.zIndex = '0';
               // this.HTMLCanvasScreenImage_.style.width = "100%";

                this.HTMLCanvasMouseImage_ = document.createElement('canvas');
                this.HTMLCanvasMouseImage_.style.position = 'absolute';
                this.HTMLCanvasMouseImage_.style.left = this.HTMLCanvasMouseImage_.style.top = '0';
                this.HTMLCanvasMouseImage_.style.zIndex = '1';
                this.HTMLRoot_.removeChild(this.InfoDiv);
                this.HTMLRoot_.appendChild(this.HTMLCanvasScreenImage_);
                this.HTMLRoot_.appendChild(this.HTMLCanvasMouseImage_);

            }).onMessage((ws: WebSocket, message: WSMessage) => {
                console.log('onMessage length:' + message.data.byteLength);
            }).onDisconnection((ws: WebSocket, code: number, message: string) => {
                console.log('onDisconnection');
                window.removeEventListener("mousedown", this.onmousedown);
                window.removeEventListener("mouseup", this.onmouseup);
                window.removeEventListener("mousemove", this.onmove);
                window.removeEventListener("wheel", this.onwheel);
                window.removeEventListener("keydown", this.onkeydown);
                window.removeEventListener("keyup", this.onkeyup);
                if (this.HTMLRoot_ && this.HTMLCanvasMouseImage_) {
                    this.HTMLRoot_.removeChild(this.HTMLCanvasMouseImage_);
                }
                if (this.HTMLRoot_ && this.HTMLCanvasScreenImage_) {
                    this.HTMLRoot_.removeChild(this.HTMLCanvasScreenImage_);
                }
                this.Cursor_ = null;
                this.ScaleImage_ = false;
                this.Monitors = new Array<Monitor>();
                this.ClientDriver_ = null;
                this.Socket_ = null;
                this.OriginalImage_ = null;
                this.HTMLCanvasScreenImage_ = this.HTMLCanvasMouseImage_ = null;
                this.HTMLRoot_.appendChild(this.InfoDiv);
                this.ConnectButton.disabled = false;
            }).onClipboardChanged((clipstring: string) => { 
                console.log('onClipboardChanged: ' + clipstring);
            }).onFrameChanged((image: HTMLImageElement, monitor: Monitor, rect: Rect) => {
                this.HTMLCanvasScreenImage_.getContext("2d").drawImage(image, monitor.OffsetX + rect.Origin.X, monitor.OffsetY + rect.Origin.Y);
            }).onMonitorsChanged((monitors: Monitor[]) => {
                this.Monitors = monitors; 
                var width = 0;
                this.Monitors.forEach((m: Monitor) => { width += m.Width; });
                var maxheight = 0;
                this.Monitors.forEach((m: Monitor) => { if (m.Height > maxheight) maxheight = m.Height; });
                this.HTMLCanvasScreenImage_.width = width;
                this.HTMLCanvasScreenImage_.height = maxheight;

            }).onMouseImageChanged((image: ImageData) => {
                this.Cursor_ = image;
                this.HTMLCanvasMouseImage_.getContext("2d").putImageData(this.Cursor_, 0, 0);
            }).onMousePositionChanged((pos: Point) => {

                this.HTMLCanvasMouseImage_.style.top = pos.Y + "px";
                this.HTMLCanvasMouseImage_.style.left = pos.X + "px";

            }).onNewFrame((image: HTMLImageElement, monitor: Monitor, rect: Rect) => {
                this.HTMLCanvasScreenImage_.getContext("2d").drawImage(image, monitor.OffsetX, monitor.OffsetY);
                this.OriginalImage_ = image;
            }).Build(this.Socket_);


        this.Socket_.onerror = (ev: Event) => {
            console.log(ev);
        };
    }
    
    private onkeydown = (ev: KeyboardEvent) => {
        this.ClientDriver_.SendKeyDown(ConvertToKeyCode(ev));
    }
    private onkeyup = (ev: KeyboardEvent) => {
        this.ClientDriver_.SendKeyUp(ConvertToKeyCode(ev));
    }
    private onwheel = (ev: WheelEvent) => {
        this.ClientDriver_.SendMouseScroll(ev.deltaY < 0 ? -1 : 1);
    }
    private onmove = (ev: MouseEvent) => {
        this.ClientDriver_.SendMousePosition({ Y: ev.pageY, X: ev.pageX });
    }
    private onmouseup = (ev: MouseEvent) => {
        this.ClientDriver_.SendMouseUp(ev.button);
    }
    private onmousedown = (ev: MouseEvent) => {
        this.ClientDriver_.SendMouseDown(ev.button);
    }
}
