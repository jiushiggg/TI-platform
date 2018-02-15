

#include <stdint.h>

/* POSIX Header files */
#include <pthread.h>

/* RTOS header files */
#include <ti/sysbios/BIOS.h>

/* Example/Board Header files */
#include "Board.h"

#include "app.h"
/*
 *  ======== main ========
*/



int main(void)
{
    Board_initGeneral();
    Power_setConstraint(PowerCC26XX_SB_VIMS_CACHE_RETAIN);
    Power_setConstraint(PowerCC26XX_NEED_FLASH_IN_IDLE);
    Board_initWatchdog();
    ADC_init();
//    Board_initI2C();

    rf_init();
    mailbox_semaphore_init();
    app_init();

    BIOS_start();    /* Start BIOS */
    return (0);

}
