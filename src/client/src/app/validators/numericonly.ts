import {FormControl} from '@angular/forms';
export function ValidateNumericOnly(control: FormControl)
{
    if (!+control.value) {
        return {ValidateNumericOnly : true};
    }
    return null;
}