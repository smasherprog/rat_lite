import { window } from 'rxjs/operators';
import { MonitorsCanvasComponent } from './monitorcanvas/monitorcanvas.component';
import { OptionsDialog } from './options.dialog/options.dialog';
import {Component, ElementRef, HostListener, OnInit, ViewChild, QueryList, ViewChildren} from '@angular/core';
import {MatDialog} from '@angular/material';

import {ConnectDialog} from './connect.dialog/connect.dialog';
import {
    ClientSettings,
    CreateClientDriverConfiguration,
    IClientDriver,
    Monitor,
    Point,
    Rect,
    WSMessage,
} from './lib/rat_lite';
import {ConnectModel} from './models/connect.model';

@Component({selector : 'app-root', templateUrl : './app.component.html', styleUrls : [ './app.component.css' ]})
export class AppComponent implements OnInit {
    @ViewChild('HTMLCanvasMouseImage_') HTMLCanvasMouseImage_: ElementRef;
    @ViewChildren('HTMLCanvasScreenImage_') MonitorCanvas: QueryList<MonitorsCanvasComponent>;

    ScaleImage_ = false;
    ClientDriver_: IClientDriver;
    Socket_: WebSocket;
    Monitors = new Array<Monitor>();
    MouseOverMonitor: Monitor;
    ClientSettings_ : ClientSettings;
    Cursor_: ImageData; 
    width = '1000px'; 
    ConnectedTo = '';

    constructor(public dialog: MatDialog) {}
    public ngOnInit(): void
    {
        setTimeout(() => { this.OpenDialog(); }, 100);
    }
    public Disconnect(): void
    {
        if (this.Socket_) {
            this.Socket_.close(1000);
        }
    }
    public toggleoptions(): void{ 
        this.dialog.open(OptionsDialog, {
            data: 
            { 
                ClientSettings:this.ClientSettings_,
                Monitors: this.Monitors
            }
        }).afterClosed().subscribe((a: ClientSettings)=>{
            if(a){ 
                this.ClientSettings_ = a;
                this.ClientDriver_.SendClientSettingsChanged(a);
            }
        });
    }
    public overmonitor(mon: Monitor): void{
        this.MouseOverMonitor=mon;
        console.log('overmonitor');
    }
    private humanFileSize(bytes: number) : string {
        var thresh =  1024;
        if(Math.abs(bytes) < thresh) {
            return bytes + ' B';
        }
        var units =['kB','MB','GB','TB','PB','EB','ZB','YB'] ;
        var u = -1;
        do {
            bytes /= thresh;
            ++u;
        } while(Math.abs(bytes) >= thresh && u < units.length - 1);
        return bytes.toFixed(1)+' '+units[u];
    }
    public OpenDialog(): void
    {
        this.dialog.open(ConnectDialog, {disableClose : true}).afterClosed().subscribe((a: ConnectModel) => {
            if (a) {  
                this.Socket_ = new WebSocket(a.Protocol + "://" + a.Host + ":" + a.Port); 
                this.Socket_.binaryType = 'arraybuffer';
                this.ConnectedTo = a.Protocol + "://" + a.Host + ":" + a.Port;
                this.ClientDriver_ =
                    CreateClientDriverConfiguration()
                        .onConnection((ws: WebSocket, ev: Event) => {
                            console.log('onConnection');
                            this.ClientSettings_ = new ClientSettings(); 
                        })
                        .onBytesPerSecond((bytespersecond: number)=>{ 
                            document.title = 'Connected to '+ this.ConnectedTo + '  ' + this.humanFileSize(bytespersecond) + '/sec'; 
                        })
                        .onMessage((ws: WebSocket, message: WSMessage) => { console.log('onMessage length:' + message.data.byteLength); })
                        .onDisconnection((ws: WebSocket, code: number, message: string) => {
                            console.log('onDisconnection ' + code + "  "+ message);
                            this.ConnectedTo ='';
                            this.Cursor_ = null;
                            this.ScaleImage_ = false;
                            this.Monitors = new Array<Monitor>();
                            this.ClientDriver_ = null;
                            this.Socket_ = null;
                            this.MouseOverMonitor=null;
                            this.ClientSettings_ = new ClientSettings(); 
                            this.OpenDialog();
                        })
                        .onClipboardChanged((clipstring: string) => { console.log('onClipboardChanged: ' + clipstring); })
                        .onFrameChanged((image: HTMLImageElement, monitor: Monitor, rect: Rect) => {
                            this.MonitorCanvas.forEach((a: MonitorsCanvasComponent)=>{
                                a.onFrameChanged(image, monitor, rect);
                            }); 
                            var totalwidth=0;
                            this.MonitorCanvas.forEach((a: MonitorsCanvasComponent)=>{ totalwidth += (a.getScalingFactor() * a.Monitor.Width); });
                            this.width = totalwidth.toString() + 'px';
                        })
                        .onMonitorsChanged((monitors: Monitor[]) => {
                            if(this.ClientSettings_.MonitorsToWatch.length ==0){
                                this.ClientSettings_.MonitorsToWatch = monitors;
                            }
                            this.Monitors = monitors; 
                        })
                        .onMouseImageChanged((image: ImageData) => {
                            this.Cursor_ = image;
                            if (this.HTMLCanvasMouseImage_ && this.HTMLCanvasMouseImage_.nativeElement ) {
                                this.HTMLCanvasMouseImage_.nativeElement.getContext("2d").putImageData(this.Cursor_, 0, 0);
                            }
                        })
                        .onMousePositionChanged((pos: Point) => {
                            if (this.HTMLCanvasMouseImage_ && this.HTMLCanvasMouseImage_.nativeElement ) {
                                this.HTMLCanvasMouseImage_.nativeElement.style.top = pos.Y + "px";
                                this.HTMLCanvasMouseImage_.nativeElement.style.left = pos.X + "px";
                            }
                        })
                        .onNewFrame((image: HTMLImageElement, monitor: Monitor, rect: Rect) => {
                            this.MonitorCanvas.forEach((a: MonitorsCanvasComponent)=>{
                                a.onNewFrame(image, monitor, rect);
                            }); 
                        })
                        .Build(this.Socket_);

                this.Socket_.onerror = (ev: Event) => { console.log(ev); };
            }
            else {
                this.OpenDialog();
            }
        });
    }
    @HostListener('onkeydown', [ '$event' ])
    onkeydown(ev: KeyboardEvent)
    {
        if (this.ClientDriver_ && this.Socket_.readyState === this.Socket_.OPEN) {
            this.ClientDriver_.SendKeyDown(ConvertToKeyCode(ev));
        }
    }
    @HostListener('onkeyup', [ '$event' ])
    onkeyup(ev: KeyboardEvent)
    {
        if (this.ClientDriver_ && this.Socket_.readyState === this.Socket_.OPEN) {
            this.ClientDriver_.SendKeyUp(ConvertToKeyCode(ev));
        }
    }
    @HostListener('mousewheel', [ '$event' ])
    onwheel(ev: WheelEvent)
    {
        if (this.ClientDriver_ && this.Socket_.readyState === this.Socket_.OPEN) {
            this.ClientDriver_.SendMouseScroll(ev.deltaY < 0 ? -1 : 1);
        }
    }
    @HostListener('mousemove', [ '$event' ])
    onmove(ev: WheelEvent)
    { 
        if (this.ClientDriver_ && this.Socket_.readyState === this.Socket_.OPEN && this.MouseOverMonitor) {
            let scaling = this.MouseOverMonitor.Scaling; 
            let point = {Y : ev.pageY * scaling, X : ev.pageX* scaling} as Point;
            this.ClientDriver_.SendMousePosition(point);
        }
    }
    @HostListener('mouseup', [ '$event' ])
    onmouseup(ev: MouseEvent)
    {
        if (this.ClientDriver_ && this.Socket_.readyState === this.Socket_.OPEN) {
            this.ClientDriver_.SendMouseUp(ev.button);
        }
    }
    @HostListener('onmousedownheel', [ '$event' ])
    onmousedown(ev: MouseEvent)
    {
        if (this.ClientDriver_ && this.Socket_.readyState === this.Socket_.OPEN) {
            this.ClientDriver_.SendMouseDown(ev.button);
        }
    }
    @HostListener('mousedown', [ '$event' ])
    mousedown(ev: MouseEvent)
    {
        if (this.ClientDriver_ && this.Socket_.readyState === this.Socket_.OPEN) {
            this.ClientDriver_.SendMouseDown(ev.button);
        }
    }
}
