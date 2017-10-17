import {MatButtonModule, MatCheckboxModule, MatDialogModule, MatInputModule, MatSelectModule} from '@angular/material';
import { NgModule } from '@angular/core';

@NgModule({
  imports: [
    MatButtonModule, 
    MatCheckboxModule,
    MatDialogModule,
    MatInputModule,
    MatSelectModule
  ],
  exports: [
    MatButtonModule, 
    MatCheckboxModule,
    MatDialogModule,
    MatInputModule,
    MatSelectModule
  ],
})
export class MaterialModule { }