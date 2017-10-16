import { selector } from 'rxjs/operator/multicast';
import { Component, Inject, OnInit } from '@angular/core';
import { MatDialogRef } from '@angular/material';
import { FormBuilder, FormGroup, Validators } from '@angular/forms';

@Component({ 
    templateUrl: './connect.dialog.html',
    styleUrls:['./connect.dialog.css']
  })
  export class ConnectDialog implements OnInit {
  public f: FormGroup;
    constructor(
      public dialogRef: MatDialogRef<ConnectDialog>,
      private fb: FormBuilder
    ) { }
    public ngOnInit(): void { 
        this.f = this.fb.group({
            'Host': ['localhost',[Validators.required,Validators.minLength(2)]],
            'Port': ['6001',[Validators.required,Validators.minLength(2)]]
        });
    }
    public tryconnect(): void{
        
    }
  }