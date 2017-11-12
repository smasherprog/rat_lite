import {MatButtonModule, MatCheckboxModule, MatDialogModule, MatInputModule, MatSelectModule, MatSidenavModule, MatListModule, MatProgressSpinnerModule} from '@angular/material';
import { NgModule } from '@angular/core';

@NgModule({
  imports: [
    MatButtonModule, 
    MatCheckboxModule,
    MatDialogModule,
    MatInputModule,
    MatSelectModule,
    MatSidenavModule,
    MatListModule,
    MatProgressSpinnerModule
  ],
  exports: [
    MatButtonModule, 
    MatCheckboxModule,
    MatDialogModule,
    MatInputModule,
    MatSelectModule,
    MatSidenavModule,
    MatListModule,
    MatProgressSpinnerModule
  ],
})
export class MaterialModule { }