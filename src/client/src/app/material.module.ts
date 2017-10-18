import {MatButtonModule, MatCheckboxModule, MatDialogModule, MatInputModule, MatSelectModule, MatSidenavModule} from '@angular/material';
import { NgModule } from '@angular/core';

@NgModule({
  imports: [
    MatButtonModule, 
    MatCheckboxModule,
    MatDialogModule,
    MatInputModule,
    MatSelectModule,
    MatSidenavModule
  ],
  exports: [
    MatButtonModule, 
    MatCheckboxModule,
    MatDialogModule,
    MatInputModule,
    MatSelectModule,
    MatSidenavModule
  ],
})
export class MaterialModule { }