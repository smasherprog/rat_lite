import { IClientDriver, Monitor, CreateClientDriverConfiguration, WSMessage } from './Rat_Lite';
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

    DivRootId_ = 'SLRATROOTID123'; // this is an id used internally to double check if a canvas has allready been inserted
    ConnectedToSelf_ = false;

    constructor(private dst_host_: string, private dst_port_: string) {

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

        var testroot = document.getElementById(this.DivRootId_);
        if (testroot !== null) {
            document.removeChild(testroot);
        }
        this.HTMLDivRoot_ = document.createElement('div');
        this.HTMLDivRoot_.id = this.DivRootId_;
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

        document.body.appendChild(this.HTMLDivRoot_); // add to the dom
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

            }).Build(this.Socket_);
    }
    private onkeydown(ev: KeyboardEvent): void {
        this.ClientDriver_.SendKeyDown(ConvertToKeyCode(ev));
    }
    private onkeyup(ev: KeyboardEvent): void {
        this.ClientDriver_.SendKeyUp(ConvertToKeyCode(ev));
    }
    private onwheel(ev: WheelEvent): void {
        this.ClientDriver_.SendMouseScroll(ev.deltaY);
    }
    private onmove(ev: MouseEvent): void {
        this.ClientDriver_.SendMousePosition({ Y:ev.pageY, X: ev.pageX });
    }
    private onmouseup(ev: MouseEvent): void {
        this.ClientDriver_.SendMouseUp(ev.button);
    }
    private onmousedown(ev: MouseEvent): void {
        this.ClientDriver_.SendMouseUp(ev.button);
    }
}
