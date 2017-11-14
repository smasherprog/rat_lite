import {Component, Inject, OnInit} from '@angular/core';
import {FormBuilder, FormGroup, Validators} from '@angular/forms';
import {MatDialogRef} from '@angular/material'; 
 
import {ValidateNumericOnly} from '../validators/numericonly';
import { ConnectModel } from '../models/connect.model';

@Component({templateUrl : './connect.dialog.html'})
export class ConnectDialog implements OnInit {
    public f: FormGroup;
    submitting=false;
    public Protocols = ['ws'
   // , 'wss'
    ];
    constructor(public dialogRef: MatDialogRef<ConnectDialog>, private fb: FormBuilder) {}
    public ngOnInit(): void
    {
        var defaltproto = 'ws';
        //if (window.location.protocol == "https:") {
        //    defaltproto= "wss"; 
       // }
        this.f = this.fb.group({ 
            'Protocol' : [
                defaltproto, [ Validators.required ]
            ],
            'Host' : [
                'localhost', [ Validators.required, Validators.minLength(2) ]
            ],
            'Port' :
                [ '6001', [ Validators.required, Validators.minLength(2), ValidateNumericOnly ] ]
        });
    }
    public onSubmit(): void {
        if(!this.submitting && this.f.valid){
            this.submitting = true;
            this.dialogRef.close(this.f.value as ConnectModel);
        }
    }
}