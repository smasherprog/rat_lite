import { IClientDriver, Monitor, CreateClientDriverConfiguration, WSMessage, Point } from './Rat_Lite';
import { ConvertToKeyCode } from './Input_Lite';

export class ClientWindow {
    Cursor_: ImageData;
    ScaleImage_ = false;
    Monitors_: Array<Monitor>;
    ClientDriver_: IClientDriver;
    Socket_: WebSocket;

    OriginalImage_: HTMLImageElement;
    HTMLDivRoot_: HTMLDivElement;
    HTMLCanvasScreenImage_: HTMLCanvasElement;
    HTMLCanvasMouseImage_: HTMLCanvasElement;

    ConnectedToSelf_ = false;
    ScaleImage = false;

    private dst_host_: string;
    private dst_port_: string;
    private AttachedRoot_: HTMLElement;

    constructor(rootnode: HTMLElement) {
        this.AttachedRoot_ = rootnode;

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

        rootnode.appendChild(this.HTMLDivRoot_); // add to the dom
    }
    public destroy() {
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
        this.HTMLDivRoot_ = this.AttachedRoot_ =this.HTMLDivRoot_ = this.HTMLCanvasScreenImage_ = this.HTMLCanvasMouseImage_ = null;
    }
    public connect(dst_host: string, dst_port: string): void {
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
        this.ClientDriver_ = CreateClientDriverConfiguration()
            .onConnection((ws: WebSocket, ev: Event) => {

            }).onMessage((ws: WebSocket, message: WSMessage) => {

            }).onDisconnection((ws: WebSocket, code: number, message: string) => {

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
                if (this.ScaleImage) {
                    var scale = this.getScalingFactor();
                    this.HTMLCanvasScreenImage_.width = image.width * scale;
                    this.HTMLCanvasScreenImage_.height = image.height * scale;
                    this.HTMLDivRoot_.style.width = this.HTMLCanvasScreenImage_.width + 'px';
                    this.HTMLDivRoot_.style.height = this.HTMLCanvasScreenImage_.height + 'px';
                    this.HTMLCanvasScreenImage_.getContext("2d").drawImage(image, 0, 0, this.HTMLCanvasScreenImage_.width, this.HTMLCanvasScreenImage_.height);
                }
                else {
                    this.HTMLCanvasScreenImage_.width = image.width;
                    this.HTMLCanvasScreenImage_.height = image.height;
                    this.HTMLDivRoot_.style.width = this.HTMLCanvasScreenImage_.width + 'px';
                    this.HTMLDivRoot_.style.height = this.HTMLCanvasScreenImage_.height + 'px';
                    this.HTMLCanvasScreenImage_.getContext("2d").drawImage(image, 0, 0);
                }
                this.OriginalImage_ = image;
            }).Build(this.Socket_);
    }

    public getScalingFactor(): number {
        if (this.ScaleImage && this.OriginalImage_ != null) {
            return window.innerHeight / this.OriginalImage_.height;
        }
        else {
            return 1.0;
        }
    }
    private onkeydown(ev: KeyboardEvent): void {
        this.ClientDriver_.SendKeyDown(ConvertToKeyCode(ev));
    }
    private onkeyup(ev: KeyboardEvent): void {
        this.ClientDriver_.SendKeyUp(ConvertToKeyCode(ev));
    }
    private onwheel(ev: WheelEvent): void {
        this.ClientDriver_.SendMouseScroll(ev.deltaY < 0 ? -1 : 1);
    }
    private onmove(ev: MouseEvent): void {
        this.ClientDriver_.SendMousePosition({ Y:ev.pageY, X: ev.pageX });
    }
    private onmouseup(ev: MouseEvent): void {
        this.ClientDriver_.SendMouseUp(ev.button);
    }
    private onmousedown(ev: MouseEvent): void {
        this.ClientDriver_.SendMouseDown(ev.button);
    }
}
