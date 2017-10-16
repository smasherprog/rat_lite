import {Component, Inject, OnInit} from '@angular/core';
import {FormBuilder, FormGroup, Validators} from '@angular/forms';
import {MatDialogRef} from '@angular/material';
import {selector} from 'rxjs/operator/multicast';
 
import {ValidateNumericOnly} from '../validators/numericonly';

@Component({templateUrl : './connect.dialog.html', styleUrls : [ './connect.dialog.css' ]})
export class ConnectDialog implements OnInit {
    public f: FormGroup;
    constructor(public dialogRef: MatDialogRef<ConnectDialog>, private fb: FormBuilder) {}
    public ngOnInit(): void
    {
        this.f = this.fb.group({
            'Host' : [
                'localhost', [ Validators.required, Validators.minLength(2) ]
            ],
            'Port' :
                [ '6001', [ Validators.required, Validators.minLength(2), ValidateNumericOnly ] ]
        });
    }
    public tryconnect(): void {}
}