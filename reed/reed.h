#ifndef REED_H
#define REED_H

#include <ti/drivers/PIN.h>
#include "global_variable.h"

void reedCallbackFxn(PIN_Handle handle, PIN_Id pinId);
void isr_reed_init(void);
void reed_fun_select( UINT16 magnet);








#endif
