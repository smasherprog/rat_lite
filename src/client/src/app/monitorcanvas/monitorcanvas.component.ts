import {Component, ElementRef, Input, OnInit, ViewChild} from "@angular/core";

import {Monitor, Rect} from "../lib/rat_lite";

@Component({selector : 'monitor-canvas', templateUrl : './monitorcanvas.component.html'})
export class MonitorsCanvasComponent implements OnInit {
    @ViewChild('thiscanvas') ThisCanvas: ElementRef;
    @Input() Monitor: Monitor;
    
    ngOnInit(): void
    {
        if (this.ThisCanvas && this.ThisCanvas.nativeElement ) {
            this.ThisCanvas.nativeElement.width = this.getScalingFactor()*this.Monitor.Width;
            this.ThisCanvas.nativeElement.height =this.getScalingFactor()* this.Monitor.Height;
        }
    }
    public getScalingFactor(): number{
        return (1 / this.Monitor.Scaling); 
    }
    public onFrameChanged(image: HTMLImageElement, monitor: Monitor, rect: Rect): void
    {
        if (this.Monitor && this.ThisCanvas && this.ThisCanvas.nativeElement && this.Monitor.Id == monitor.Id) {
            this.ThisCanvas.nativeElement.getContext("2d").drawImage(image,this.getScalingFactor()* rect.Origin.X, this.getScalingFactor()*rect.Origin.Y, this.getScalingFactor()*image.width, this.getScalingFactor()*image.height);
        }
    }
    public onNewFrame(image: HTMLImageElement, monitor: Monitor, rect: Rect): void
    {
        if (this.Monitor && this.ThisCanvas && this.ThisCanvas.nativeElement && this.Monitor.Id == monitor.Id) {
            this.ThisCanvas.nativeElement.getContext("2d").drawImage(image, 0, 0, this.getScalingFactor()*image.width, this.getScalingFactor()*image.height);
        }
    }
}