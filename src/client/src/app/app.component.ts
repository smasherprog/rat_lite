
import { Component, OnInit } from '@angular/core';
import { MatDialog } from '@angular/material';
import { ConnectDialog } from './connect.dialog/connect.dialog';

@Component({selector : 'app-root', templateUrl : './app.component.html'})
export class AppComponent implements OnInit {
  constructor(public dialog: MatDialog) {}
    public ngOnInit(): void {

    setTimeout(()=>{
      this.dialog.open(ConnectDialog);
    }, 100);
    }
}
