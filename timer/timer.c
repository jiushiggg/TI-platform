
#include "timer.h"

#define USE_WD

Watchdog_Params params;
Watchdog_Handle watchdogHandle;

GPTimerCC26XX_Handle aTimer;

void watchdogCallback(uintptr_t unused)     //看门狗定时器计时到一半时，会进入此回调函数中
{
//   Watchdog_clear(watchdogHandle);
}

void timerCallback(GPTimerCC26XX_Handle handle, GPTimerCC26XX_IntMask interruptMask)    //定时器到时会进入此回调函数
{

}
void open_watchdog(void)
{
#ifdef USE_WD
    uint32_t tickValue = 0;
    Watchdog_Params_init(&params);
    params.callbackFxn = (Watchdog_Callback)watchdogCallback;
    //    params.resetMode = Watchdog_RESET_OFF;
    //    params.debugStallMode = Watchdog_DEBUG_STALL_ON;
    watchdogHandle = Watchdog_open(Board_WATCHDOG0, &params);
    if (watchdogHandle == NULL)
    {
        while (1);
    }
    tickValue = Watchdog_convertMsToTicks(watchdogHandle, 200000);
    Watchdog_setReload(watchdogHandle, tickValue);
#endif
}
void SetWatchDog(void)
{
#ifdef USE_WD
    Watchdog_clear(watchdogHandle);
#endif
}





void init_hw_timer(void)
{
     GPTimerCC26XX_Params params;
     GPTimerCC26XX_Params_init(&params);
     params.width          = GPT_CONFIG_32BIT;
     params.mode           = GPT_MODE_PERIODIC_UP;
     params.debugStallMode = GPTimerCC26XX_DEBUG_STALL_OFF;
     aTimer = GPTimerCC26XX_open(Board_GPTIMER0A, &params);
     if(aTimer == NULL)
     {
         while (1);
     }
}

void set_hw_A_timer(uint32_t loadVal)
{
//    xdc_runtime_Types_FreqHz  freq;
//    BIOS_getCpuFreq(&freq);
//    GPTimerCC26XX_Value loadVal = 48000000-1;
    GPTimerCC26XX_setLoadValue(aTimer, loadVal);
    GPTimerCC26XX_registerInterrupt(aTimer, timerCallback, GPT_INT_TIMEOUT);
}

void start_hw_A_timer(void)
{
    GPTimerCC26XX_start(aTimer);
}

void stop_hw_A_timer(void)
{
    GPTimerCC26XX_stop(aTimer);
}




