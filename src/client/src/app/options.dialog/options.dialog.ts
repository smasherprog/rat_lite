import { ClientSettings, ClipboardSharing, ImageEncoding, Monitor } from '../lib/rat_lite';
import { Component, Inject, OnInit } from '@angular/core';
import { FormBuilder, FormGroup, Validators, FormArray } from '@angular/forms';
import { MatDialogRef } from '@angular/material';
import { MAT_DIALOG_DATA } from '@angular/material';

import { OptionsModel } from '../models/connect.model';

@Component({templateUrl : './options.dialog.html'})
export class OptionsDialog implements OnInit {
    public f: FormGroup;
    submitting = false;

    public CompressionOptions = [ 30, 40, 50, 60, 70, 80, 90, 100 ];
    constructor(public dialogRef: MatDialogRef<OptionsDialog>, private fb: FormBuilder, @Inject(MAT_DIALOG_DATA) public data: OptionsModel) {}
    public ngOnInit(): void
    {   
        this.f = this.fb.group({
            ShareClip : [ this.data.ClientSettings.ShareClip, [ Validators.required ] ],
            ImageCompressionSetting : [ this.data.ClientSettings.ImageCompressionSetting, [ Validators.required, Validators.min(30), Validators.max(100) ] ],
            EncodeImagesAsGrayScale : [ this.data.ClientSettings.EncodeImagesAsGrayScale, [ Validators.required ] ],
            MonitorsToWatch: new FormArray([])
        });
    }
    public onSubmit(): void
    {
        if (!this.submitting && this.f.valid) {
            this.submitting = true;  
            var nc = new ClientSettings();
            nc.EncodeImagesAsGrayScale = this.f.value.EncodeImagesAsGrayScale ? ImageEncoding.GRAYSCALE : ImageEncoding.COLOR;
            nc.ImageCompressionSetting = this.f.value.ImageCompressionSetting;
            nc.ShareClip = this.f.value.ShareClip ? ClipboardSharing.SHARED : ClipboardSharing.NOT_SHARED;
            this.f.value.MonitorsToWatch.forEach((monitorid: number) => {
                var found = this.data.Monitors.find((m: Monitor)=>{ return m.Id == monitorid;  });
                if(found){
                    nc.MonitorsToWatch.push(found);
                }
            });  
            this.dialogRef.close(nc);
        }
    }
    public cancel(): void{
        this.dialogRef.close(null);
    }
}