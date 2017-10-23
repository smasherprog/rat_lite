import {Component, Inject, OnInit} from '@angular/core';
import {FormBuilder, FormGroup, Validators} from '@angular/forms';
import {MatDialogRef} from '@angular/material'; 
 
import {ValidateNumericOnly} from '../validators/numericonly'; 

@Component({templateUrl : './options.dialog.html'})
export class OptionsDialog implements OnInit {
    public f: FormGroup;
    submitting=false;

    constructor(public dialogRef: MatDialogRef<OptionsDialog>, private fb: FormBuilder) {}
    public ngOnInit(): void
    {

        this.f = this.fb.group({ });
    }
    public onSubmit(): void {
        if(!this.submitting && this.f.valid){
            this.submitting = true;
            this.dialogRef.close();
        }
    }
}