import {Component, Inject, Input, OnInit} from '@angular/core';
import { FormBuilder, FormControl, FormGroup, Validators } from '@angular/forms';
import {MAT_DIALOG_DATA} from '@angular/material';

import { ClientSettings, Monitor } from '../lib/rat_lite';

@Component({selector : 'monitors-towatch', templateUrl : './monitorstowatch.component.html'})
export class MonitorsToWatchComponent implements OnInit {
    @Input() f: FormGroup;
    @Input() MonitorsToWatch: Monitor[];
    @Input() Monitors: Monitor[];
    constructor(private fb: FormBuilder) {}
    public ngOnInit(): void {  

    }
}