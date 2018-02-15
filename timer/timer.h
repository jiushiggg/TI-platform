
#ifndef _TIMER_H_
#define _TIMER_H_

#include <ti/drivers/Watchdog.h>
#include "Board.h"
#include <ti/drivers/timer/GPTimerCC26XX.h>


void open_watchdog(void);
void SetWatchDog(void);

void init_hw_timer(void);
void set_hw_A_timer(uint32_t loadVal);
void start_hw_A_timer(void);
void stop_hw_A_timer(void);





#endif
