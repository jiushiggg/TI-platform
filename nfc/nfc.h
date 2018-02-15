#ifndef  NFC_H
#define NFC_H

#include <ti/drivers/PIN.h>
#include "global_variable.h"

#define NFC_INT_EN      0
#define NFC_INT_DIS      1

//nfc timer
#define NFC_TIMEOUT_BASE     5000
#define NFC_TIMEOUT_50MS       20        //100MS
#define NFC_TIMEOUT_500MS     30
#define NFC_TIMER_ONCE                              1
#define NFC_TIMER_PERIOD                          2
#define NFC_TIMER_PERIOD_VALUE      (100000/Clock_tickPeriod)

void isr_nfc_init(void);
void nfc_int_set(uint8_t tmp);

void creat_nfc_timer(void);
void set_nfc_timer(uint32_t period, uint32_t timeout);
void stop_nfc_timer(void);

#endif
