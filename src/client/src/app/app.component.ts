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
    ClientSettings_ : ClientSettings;
    Cursor_: ImageData; 
    width = '1000px';

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
    public OpenDialog(): void
    {
        this.dialog.open(ConnectDialog, {disableClose : true}).afterClosed().subscribe((a: ConnectModel) => {
            if (a) {
                this.Socket_ = new WebSocket(a.Protocol + "://" + a.Host + ":" + a.Port);
                this.Socket_.binaryType = 'arraybuffer';

                this.ClientDriver_ =
                    CreateClientDriverConfiguration()
                        .onConnection((ws: WebSocket, ev: Event) => {
                            console.log('onConnection');
                            this.ClientSettings_ = new ClientSettings(); 
                        })
                        .onMessage((ws: WebSocket, message: WSMessage) => { console.log('onMessage length:' + message.data.byteLength); })
                        .onDisconnection((ws: WebSocket, code: number, message: string) => {
                            console.log('onDisconnection ' + code + "  "+ message);
                            this.Cursor_ = null;
                            this.ScaleImage_ = false;
                            this.Monitors = new Array<Monitor>();
                            this.ClientDriver_ = null;
                            this.Socket_ = null;
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

    @HostListener('window:mousedown', [ '$event' ])
    mousedown(ev: MouseEvent)
    {
        if (this.ClientDriver_) {
            this.ClientDriver_.SendMouseDown(ev.button);
        }
    }
    @HostListener('window:onkeydown', [ '$event' ])
    onkeydown(ev: KeyboardEvent)
    {
        if (this.ClientDriver_) {
            this.ClientDriver_.SendKeyDown(ConvertToKeyCode(ev));
        }
    }
    @HostListener('window:onkeyup', [ '$event' ])
    onkeyup(ev: KeyboardEvent)
    {
        if (this.ClientDriver_) {
            this.ClientDriver_.SendKeyUp(ConvertToKeyCode(ev));
        }
    }
    @HostListener('window:onwheel', [ '$event' ])
    onwheel(ev: WheelEvent)
    {
        if (this.ClientDriver_) {
            this.ClientDriver_.SendMouseScroll(ev.deltaY < 0 ? -1 : 1);
        }
    }
    @HostListener('window:onmove', [ '$event' ])
    onmove(ev: WheelEvent)
    {
        if (this.ClientDriver_) {
            this.ClientDriver_.SendMousePosition({Y : ev.pageY, X : ev.pageX});
        }
    }
    @HostListener('window:onmouseup', [ '$event' ])
    onmouseup(ev: MouseEvent)
    {
        if (this.ClientDriver_) {
            this.ClientDriver_.SendMouseUp(ev.button);
        }
    }
    @HostListener('window:onwonmousedownheel', [ '$event' ])
    onmousedown(ev: MouseEvent)
    {
        if (this.ClientDriver_) {
            this.ClientDriver_.SendMouseDown(ev.button);
        }
    }
}
