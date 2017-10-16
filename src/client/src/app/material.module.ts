import {MatButtonModule, MatCheckboxModule, MatDialogModule, MatInputModule} from '@angular/material';
import { NgModule } from '@angular/core';

@NgModule({
  imports: [
    MatButtonModule, 
    MatCheckboxModule,
    MatDialogModule,
    MatInputModule
  ],
  exports: [
    MatButtonModule, 
    MatCheckboxModule,
    MatDialogModule,
    MatInputModule
  ],
})
export class MaterialModule { }