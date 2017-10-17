import {Component, OnInit} from '@angular/core';
import {MatDialog} from '@angular/material';
import {ConnectDialog} from './connect.dialog/connect.dialog';
import { ConnectModel } from './models/connect.model';
import { CreateClientDriverConfiguration, IClientDriver, Monitor, Point, Rect, WSMessage } from './lib/rat_lite';


@Component({selector : 'app-root', templateUrl : './app.component.html'})
export class AppComponent implements OnInit {

    ScaleImage_ = false;
    ClientDriver_: IClientDriver;
    Socket_: WebSocket;
    Monitors = new Array<Monitor>();

    constructor(public dialog: MatDialog) {}
    public ngOnInit(): void
    {
        setTimeout(() => { this.OpenDialog(); }, 100);
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

                        })
                        .onMessage((ws: WebSocket, message: WSMessage) => { console.log('onMessage length:' + message.data.byteLength); })
                        .onDisconnection((ws: WebSocket, code: number, message: string) => {
                            console.log('onDisconnection');
                            this.ScaleImage_ = false;
                            this.Monitors = new Array<Monitor>();
                            this.ClientDriver_ = null;
                            this.Socket_ = null;
                            this.OpenDialog();
                        })
                        .onClipboardChanged((clipstring: string) => { console.log('onClipboardChanged: ' + clipstring); })
                        .onFrameChanged((image: HTMLImageElement, monitor: Monitor, rect: Rect) => {

                                        })
                        .onMonitorsChanged((monitors: Monitor[]) => {
                            this.Monitors = monitors;
                            var width = 0;
                            this.Monitors.forEach((m: Monitor) => { width += m.Width; });
                            var maxheight = 0;
                            this.Monitors.forEach((m: Monitor) => {
                                if (m.Height > maxheight)
                                    maxheight = m.Height;
                            });

                        })
                        .onMouseImageChanged((image: ImageData) => {

                                             })
                        .onMousePositionChanged((pos: Point) => {

                                                })
                        .onNewFrame((image: HTMLImageElement, monitor: Monitor, rect: Rect) => {

                                    })
                        .Build(this.Socket_);

                this.Socket_.onerror = (ev: Event) => { console.log(ev); };
            }
            else {
                this.OpenDialog();
            }
        });
    }
}
