import {MatButtonModule, MatCheckboxModule, MatDialogModule, MatInputModule, MatSelectModule, MatSidenavModule, MatListModule} from '@angular/material';
import { NgModule } from '@angular/core';

@NgModule({
  imports: [
    MatButtonModule, 
    MatCheckboxModule,
    MatDialogModule,
    MatInputModule,
    MatSelectModule,
    MatSidenavModule,
    MatListModule
  ],
  exports: [
    MatButtonModule, 
    MatCheckboxModule,
    MatDialogModule,
    MatInputModule,
    MatSelectModule,
    MatSidenavModule,
    MatListModule
  ],
})
export class MaterialModule { }