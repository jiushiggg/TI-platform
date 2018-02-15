#ifndef APP_H
#define APP_H

#include <xdc/std.h>
#include <xdc/runtime/System.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Mailbox.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/drivers/PIN.h>
#include <ti/drivers/power/PowerCC26XX.h>
#include "Board.h"
#include "rf.h"
#include "global_variable.h"
#include "timer.h"
#include "lattice_epd/scrn_ic1675a.h"
#include "storage_struct.h"
#include "osd4_0_cmd.h"
#include "../flash_protect/flash_protect.h"
#include "uplink.h"



void init_prama(void);
void mailbox_semaphore_init(void);
void init_flash_param_fun(void);
void init_mini_file_osd_fun(void);
void screen_display(void);
void screen_repeat_display(void);
void display_page_analysis_fun(void);
void netlink_info_write_fun(void);
void request_heartbeat_fun(void);
void erase_pkg_area_fun(void);
void app_init(void);
void screen_init(void);
void sid_change_eraflag(void);


#endif
