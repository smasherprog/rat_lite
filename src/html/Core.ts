
export class ClientDriver {
    _Cursor: ImageData;
    _ScaleImage = false;
    _OriginalImage: HTMLImageElement;
    _ClientNetworkDriver: ClientNetworkDriver;
    _HTMLDivRoot: HTMLDivElement;
    _HTMLCanvasScreenImage: HTMLCanvasElement;
    _HTMLCanvasMouseImage: HTMLCanvasElement;

    _DivRootId = 'SLRATROOTID123'; // this is an id used internally to double check if a canvas has allready been inserted
    _LeftMouseStatus = Input.MousePress.UP;

    constructor(private _dst_host: string, private _dst_port: string) {

        window.addEventListener("resize", this.onresize);
        window.addEventListener("mousedown", this.onmousedown); // event not needed
        window.addEventListener("mouseup", this.onmouseup);
        window.addEventListener("mousemove", this.onmove);
        window.addEventListener("wheel", this.onwheel);

        window.addEventListener("keydown", this.onkeydown);
        window.addEventListener("keyup", this.onkeyup);

        window.addEventListener("dblclick", this.ondblclick);
        window.addEventListener("touchend", this.ontouchend);
        window.addEventListener("touchstart", this.ontouchstart);
        window.addEventListener("touchmove", this.ontouchmove);
    }
    public
    Start = (): void => {
        var testroot = document.getElementById(this._DivRootId);
        if (testroot !== null) {
            document.removeChild(testroot);
        }
        this._HTMLDivRoot = document.createElement('div');
        this._HTMLDivRoot.id = this._DivRootId;
        this._HTMLDivRoot.style.position = 'relative';
        this._HTMLCanvasScreenImage = document.createElement('canvas');
        this._HTMLCanvasScreenImage.style.position = 'absolute';
        this._HTMLCanvasScreenImage.style.left = this._HTMLCanvasScreenImage.style.top = this._HTMLCanvasScreenImage.style.zIndex = '0';

        this._HTMLCanvasMouseImage = document.createElement('canvas');
        this._HTMLCanvasMouseImage.style.position = 'absolute';
        this._HTMLCanvasMouseImage.style.left = this._HTMLCanvasMouseImage.style.top = '0';
        this._HTMLCanvasMouseImage.style.zIndex = '1';

        this._HTMLDivRoot.appendChild(this._HTMLCanvasScreenImage);
        this._HTMLDivRoot.appendChild(this._HTMLCanvasMouseImage);

        document.body.appendChild(this._HTMLDivRoot); // add to the dom

        this._ClientNetworkDriver = new ClientNetworkDriver(this, this._dst_host, this._dst_port);
        this._ClientNetworkDriver.Start();
    }
    public
    Stop = (): void => {
        this._ClientNetworkDriver.Stop();
        this._ClientNetworkDriver = null;
        var testroot = document.getElementById(this._DivRootId);
        if (testroot !== null) {
            document.removeChild(testroot);
        }
    }
    public
    ScaleView = (b: boolean): void => { this._ScaleImage = b; }

    onkeydown = (ev: KeyboardEvent): void => {
        this.handle_key(ev, new Input.KeyEvent(Input.KeyPress.DOWN, ev.keyCode || ev.which, Input.SpecialKeys.NO_PRESS_DATA));
    }
    onkeyup = (ev: KeyboardEvent): void => {
        this.handle_key(ev, new Input.KeyEvent(Input.KeyPress.UP, ev.keyCode || ev.which, Input.SpecialKeys.NO_PRESS_DATA));
    }
    private
    handle_key = (ev: KeyboardEvent, k: Input.KeyEvent): void => {
        k.SpecialKey = Input.SpecialKeys.NO_PRESS_DATA;
        k.Key = Input.KeyEvent.keyboardMap[k.Key];
        console.log("Key: '" + k.Key + "'");
        if (ev.altKey)
            k.SpecialKey = Input.SpecialKeys.ALT;
        else if (ev.ctrlKey)
            k.SpecialKey = Input.SpecialKeys.CTRL;
        else if (ev.shiftKey)
            k.SpecialKey = Input.SpecialKeys.SHIFT;

        ev.preventDefault(); // capture the keypress
        if (!this._ClientNetworkDriver.ConnectedToSelf()) {
            this._ClientNetworkDriver.SendKeyEvent(k); // sending input to yourself will lead to an infinite loop...
        }
    }

    private
    pointerEventToXY = function (e: any): Utilities.Point {
        var out = new Utilities.Point(0, 0);
        if (e.type == 'touchstart' || e.type == 'touchmove' || e.type == 'touchend' || e.type == 'touchcancel') {
            var touch = e.touches[0] || e.changedTouches[0];
            out.X = touch.pageX;
            out.Y = touch.pageY;
        }
        else if (e.type == 'mousedown' || e.type == 'mouseup' || e.type == 'mousemove' || e.type == 'mouseover' || e.type == 'mouseout' ||
            e.type == 'mouseenter' || e.type == 'mouseleave') {
            out.Y = e.pageY;
            out.X = e.pageX;
        }
        return out;
    };

    ontouchend = (ev: TouchEvent): void => {
        if (this._LeftMouseStatus == Input.MousePress.DOWN) {
            this._LeftMouseStatus = Input.MousePress.UP;
            this.handlemouse(0, Input.MousePress.UP, this.pointerEventToXY(ev), 0);
        }
    }
    ontouchstart = (ev: TouchEvent): void => {
        if (ev.touches.length == 1) {
            this.StartTouchTimer(ev, this.pointerEventToXY(ev));
        }
    }

    ontouchmove = (ev: TouchEvent): void => {
        this.CancelTouchClickTimer();
        if (ev.touches.length == 1) {
            if (this._LeftMouseStatus == Input.MousePress.UP) {
                this._LeftMouseStatus = Input.MousePress.DOWN;
                this.handlemouse(0, Input.MousePress.DOWN, this.pointerEventToXY(ev), 0);
            }
        }

        if (this._LeftMouseStatus == Input.MousePress.DOWN) {
            this.handlemouse(-1, Input.MousePress.NO_PRESS_DATA, this.pointerEventToXY(ev), 0);
            ev.preventDefault();
        }
    }
    private
    TouchTimer: number;
    private
    CancelTouchClickTimer = (): void => {
        if (this.TouchTimer != null) {
            clearTimeout(this.TouchTimer);
        }
        this.TouchTimer = null;
    }

    private
    StartTouchTimer(ev: TouchEvent, pos: Utilities.Point) {
        var self = this;
        if (self.TouchTimer == null) {
            self.TouchTimer = setTimeout(
                function () { // single click
                    self.CancelTouchClickTimer();
                    self.handlemouse(0, Input.MousePress.DOWN, pos, 0);
                    self.handlemouse(0, Input.MousePress.UP, pos, 0);
                },
                500)
        }
        else {
            ev.preventDefault();
            self.CancelTouchClickTimer();
            self.handlemouse(0, Input.MousePress.DBLCLICK, pos, 0);
        }
    }
    onmousedown = (ev: MouseEvent): void => { this.handlemouse(ev.button, Input.MousePress.DOWN, this.pointerEventToXY(ev), 0); }
    ondblclick = (ev: MouseEvent): void => { this.handlemouse(ev.button, Input.MousePress.DBLCLICK, this.pointerEventToXY(ev), 0); }
    onmouseup = (ev: MouseEvent): void => { this.handlemouse(ev.button, Input.MousePress.UP, this.pointerEventToXY(ev), 0); }
    onmove = (ev: MouseEvent): void => { this.handlemouse(-1, Input.MousePress.NO_PRESS_DATA, this.pointerEventToXY(ev), 0); }
    onwheel = (ev: WheelEvent): void => { this.handlemouse(-1, Input.MousePress.NO_PRESS_DATA, this.pointerEventToXY(ev), ev.deltaY); }
    private
    handlemouse = (button: number, press: Input.MousePress, pos: Utilities.Point, scroll: number): void => {
        var ev = new Input.MouseEvent();
        var scale = this.GetScalingFactor();
        ev.Pos = pos;
        ev.Pos.X = ev.Pos.X / scale;
        ev.Pos.Y = ev.Pos.Y / scale;

        if (scroll != 0) {
            ev.ScrollDelta = scroll < 0 ? -1 : 1; // force a -1 or 1 because browsers send different values for each scroll tick.
            ev.EventData = Input.MouseEvents.SCROLL;
        }
        else {
            ev.ScrollDelta = 0;
            switch (button) {
                case 0:
                    ev.EventData = Input.MouseEvents.LEFT;
                    break;
                case 1:
                    ev.EventData = Input.MouseEvents.MIDDLE;
                    break;
                case 2:
                    ev.EventData = Input.MouseEvents.RIGHT;
                    break;
                default:
                    ev.EventData = Input.MouseEvents.NO_EVENTDATA;
                    break;
            };
        }
        ev.PressData = press;
        if (!this._ClientNetworkDriver.ConnectedToSelf()) {
            this._ClientNetworkDriver.SendMouse(ev); // sending input to yourself will lead to an infinite loop...
        }
    }
    onresize = (ev: UIEvent): void => {

        if (this._ScaleImage && this._OriginalImage != null) {

            var scale = this.GetScalingFactor();
            this._HTMLCanvasScreenImage.width = this._OriginalImage.width * scale;
            this._HTMLCanvasScreenImage.height = this._OriginalImage.height * scale;

            this._HTMLDivRoot.style.width = this._HTMLCanvasScreenImage.width + 'px';
            this._HTMLDivRoot.style.height = this._HTMLCanvasScreenImage.height + 'px';
            this._HTMLCanvasScreenImage.getContext("2d").drawImage(this._OriginalImage, 0, 0, this._HTMLCanvasScreenImage.width,
                this._HTMLCanvasScreenImage.height);
        }
        else if (!this._ScaleImage && this._OriginalImage != null) {
            if (this._HTMLCanvasScreenImage.height != this._OriginalImage.height ||
                this._HTMLCanvasScreenImage.width != this._OriginalImage.width) {
                this._HTMLCanvasScreenImage.width = this._OriginalImage.width;
                this._HTMLCanvasScreenImage.height = this._OriginalImage.height;

                this._HTMLDivRoot.style.width = this._HTMLCanvasScreenImage.width + 'px';
                this._HTMLDivRoot.style.height = this._HTMLCanvasScreenImage.height + 'px';
                this._HTMLCanvasScreenImage.getContext("2d").drawImage(this._OriginalImage, 0, 0);
            }
        }
    }
    GetScalingFactor(): number {
        if (this._ScaleImage && this._OriginalImage != null) {
            return window.innerHeight / this._OriginalImage.height;
        }
        else {
            return 1.0;
        }
    }
    public
    OnReceive_ImageDif = (socket: WebSocket, rect: Utilities.Rect, img: string): void => {
        if (this._OriginalImage === null)
        return;
        "use strict";
        // console.log('coords' + coords.X + ' ' + coords.Y + ' ' + coords.Width + ' ' + coords.Height);
        var i = new Image();
        i.src = "data:image/jpeg;base64," + img var self = this;
        i.onload = function () {
            if (self._ScaleImage) {
                var scale = self.GetScalingFactor();
                self._HTMLCanvasScreenImage.getContext("2d").drawImage(i, rect.Origin.X * scale, rect.Origin.Y * scale,
                    rect.Width * scale, rect.Height * scale);
            }
            else {
                self._HTMLCanvasScreenImage.getContext("2d").drawImage(i, rect.Origin.X, rect.Origin.Y);
            }
            // console.log("ctx.drawImage" + coords.Y, "  " + coords.X);
        };

        i.onerror = function (stuff) { console.log("Img Onerror:", stuff); };
    }
    public
    OnReceive_Image = (socket: WebSocket, rect: Utilities.Rect, img: string): void => {
        "use strict";
        // console.log('coords' + coords.X + ' ' + coords.Y + ' ' + coords.Width + ' ' + coords.Height);

        var i = new Image();
        i.src = "data:image/jpeg;base64," + img;

        var self = this;
        i.onload = function () {
            if (self._ScaleImage) {
                var scale = self.GetScalingFactor();
                self._HTMLCanvasScreenImage.width = i.width * scale;
                self._HTMLCanvasScreenImage.height = i.height * scale;

                self._HTMLDivRoot.style.width = self._HTMLCanvasScreenImage.width + 'px';
                self._HTMLDivRoot.style.height = self._HTMLCanvasScreenImage.height + 'px';
                self._HTMLCanvasScreenImage.getContext("2d").drawImage(i, 0, 0, self._HTMLCanvasScreenImage.width,
                    self._HTMLCanvasScreenImage.height);
            }
            else {
                self._HTMLCanvasScreenImage.width = i.width;
                self._HTMLCanvasScreenImage.height = i.height;

                self._HTMLDivRoot.style.width = self._HTMLCanvasScreenImage.width + 'px';
                self._HTMLDivRoot.style.height = self._HTMLCanvasScreenImage.height + 'px';

                self._HTMLCanvasScreenImage.getContext("2d").drawImage(i, 0, 0);
            }
            self._OriginalImage = i;
            // console.log("ctx.drawImage" + coords.Y, "  " + coords.X);
        };
        i.onerror = function (stuff) { console.log("Img Onerror:", stuff); };
    }
    public
    OnReceive_MouseImage = (socket: WebSocket, point: Utilities.Point, img: Uint8Array): void => {
        "use strict";
        // console.log('coords' + coords.X + ' ' + coords.Y + ' ' + coords.Width + ' ' + coords.Height);

        this._HTMLCanvasMouseImage.width = point.X;
        this._HTMLCanvasMouseImage.height = point.Y;
        try {
            this._Cursor = this._HTMLCanvasMouseImage.getContext("2d").createImageData(point.X, point.Y);

            for (var i = 0; i < this._Cursor.data.length; i += 4) {
                this._Cursor.data[i + 0] = img[i + 0];
                this._Cursor.data[i + 1] = img[i + 1];
                this._Cursor.data[i + 2] = img[i + 2];
                this._Cursor.data[i + 3] = img[i + 3];
            }
            this._HTMLCanvasMouseImage.getContext("2d").putImageData(this._Cursor, 0, 0);
        }
        catch (e) {
            console.log(e.message);
        }
    }
    public
    OnReceive_MousePos = (socket: WebSocket, pos: Utilities.Point): void => {
        if (this._ScaleImage) {
            var scale = this.GetScalingFactor();
            this._HTMLCanvasMouseImage.style.top = (pos.Y * scale) + "px";
            this._HTMLCanvasMouseImage.style.left = (pos.X * scale) + "px";
        }
        else {
            this._HTMLCanvasMouseImage.style.top = pos.Y + "px";
            this._HTMLCanvasMouseImage.style.left = pos.X + "px";
        }
    }
}

