class ClientWindow {
    Cursor_: ImageData;
    ScaleImage_ = false;
    Monitors_: Array<Monitor>;
    ClientDriver_: IClientDriver;
    Socket_: WebSocket;

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
                window.addEventListener("mousemove",  this.onmove);
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

                this.HTMLCanvasMouseImage_ = document.createElement('canvas');
                this.HTMLCanvasMouseImage_.style.position = 'absolute';
                this.HTMLCanvasMouseImage_.style.left = this.HTMLCanvasMouseImage_.style.top = '0';
                this.HTMLCanvasMouseImage_.style.zIndex = '1';
                this.HTMLRoot_.removeChild(this.InfoDiv);
                this.HTMLRoot_.appendChild(this.HTMLCanvasScreenImage_);
                this.HTMLRoot_.appendChild(this.HTMLCanvasMouseImage_);

            }).onMessage((ws: WebSocket, message: WSMessage) => {

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
                this.Monitors_ = null;
                this.ClientDriver_ = null;
                this.Socket_ = null;
                this.OriginalImage_ = null;
                this.HTMLCanvasScreenImage_ = this.HTMLCanvasMouseImage_ = null;
                this.HTMLRoot_.appendChild(this.InfoDiv);
                this.ConnectButton.disabled = false;
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
                this.Cursor_ = image;
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
                debugger;
                if (this.ScaleImage) {
                    var scale = this.getScalingFactor();
                    this.HTMLCanvasScreenImage_.width = image.width * scale;
                    this.HTMLCanvasScreenImage_.height = image.height * scale;
                    this.HTMLRoot_.style.width = this.HTMLCanvasScreenImage_.width + 'px';
                    this.HTMLRoot_.style.height = this.HTMLCanvasScreenImage_.height + 'px';
                    this.HTMLCanvasScreenImage_.getContext("2d").drawImage(image, 0, 0, this.HTMLCanvasScreenImage_.width, this.HTMLCanvasScreenImage_.height);
                }
                else {
                   
                    this.HTMLCanvasScreenImage_.width = image.width;
                    this.HTMLCanvasScreenImage_.height = image.height;
                    this.HTMLRoot_.style.width = this.HTMLCanvasScreenImage_.width + 'px';
                    this.HTMLRoot_.style.height = this.HTMLCanvasScreenImage_.height + 'px';
                    this.HTMLCanvasScreenImage_.getContext("2d").drawImage(image, 0, 0);
                }
                this.OriginalImage_ = image;
            }).Build(this.Socket_);


        this.Socket_.onerror = (ev: Event) => {
            console.log(ev);
        };
    }

    public getScalingFactor(): number {
        if (this.ScaleImage && this.OriginalImage_ != null) {
            return window.innerHeight / this.OriginalImage_.height;
        }
        else {
            return 1.0;
        }
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
