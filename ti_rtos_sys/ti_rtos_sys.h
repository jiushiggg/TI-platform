#ifndef TI_RTOS_SYS_H
#define TI_RTOS_SYS_H

#include "global_variable.h"

#define HEARTBEAT_MAIL    1
#define SET_WAKEUP_MAIL   2

extern Clock_Struct clk1Struct;
extern Clock_Handle clk1Handle;
extern Clock_Handle  set_wakeup_timer_handle;
extern Mailbox_Handle  heartbeat_set_wakeup_mailbox;
extern Mailbox_Handle  Protocol_Mailbox;
extern Clock_Params  heartbeat_timer_params;
extern Clock_Handle  heartbeat_timer_handle;
void set_wakeup_timer_Fxn(void);
void heartbeat_timer_Fxn(void);
void group_wakeup_timer_Fxn(void);
void rc_wakeup_timer_Fxn(void);
void uplink_wakeup_timer_Fxn(void);
void creat_timer(void);
void set_clock_timeout(uint32_t timeout);
void creat_cut_page_timer(void);     //单位是1s
void set_cut_page_clock_timeout(uint32_t timeout);  //单位是1s
void stop_cut_page_timer(void);
void creat_uplink_timer(void);
void set_uplink_timeout(uint32_t timeout);
void stop_uplink_timer(void);

#endif
