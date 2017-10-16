import { selector } from 'rxjs/operator/multicast';
import { Component, Inject } from '@angular/core';
import { MatDialogRef } from '@angular/material';

@Component({ 
    templateUrl: './connect.dialog.html',
    styleUrls:['./connect.dialog.css']
  })
  export class ConnectDialog {
  
    constructor(
      public dialogRef: MatDialogRef<ConnectDialog>) { }
  
   
  
  }