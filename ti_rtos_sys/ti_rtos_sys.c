#include "ti_rtos_sys.h"



void set_wakeup_timer_Fxn(void)
{
    g_screen_repeat_dis_time ++;
    Event_post(protocol_eventHandle, EVENT_FLAG_RFWORK);
}

void group_wakeup_timer_Fxn(void)
{
    Event_post(protocol_eventHandle, EVENT_FLAG_RFWORK);
}
void rc_wakeup_timer_Fxn(void)
{
    Event_post(protocol_eventHandle, EVENT_FLAG_RFWORK);
}
void uplink_wakeup_timer_Fxn(void)
{
    Event_post(protocol_eventHandle, EVENT_FLAG_RFWORK);
}


void heartbeat_timer_Fxn(void)
{
    if(hbr_mode == REQUST_HEARTBEAT)
    {

        Event_post(protocol_eventHandle, EVENT_FLAG_QUEST_HEART);

    }else if(hbr_mode == NORMAL_HEARTBEAT)
    {

        Event_post(protocol_eventHandle, EVENT_FLAG_SYS_HEART);

    }else if(hbr_mode == STOP_HEARTBEAT)
    {
        Clock_stop(heartbeat_timer_handle);
    }
}

void clk1Fxn(void)
{
    MsgObj  rf_rx_msg;
    rf_rx_msg.id = 1;
    rf_rx_msg.val = RF_RX_TIMEOUT;
    Mailbox_post(rf_rx_timeout_mailbox, &rf_rx_msg ,BIOS_WAIT_FOREVER);
}

void clk_cut_page_Fxn(void)
{
    gSys_tp.present_page_id = gSys_tp.default_page_id;//要显示的页==默认页
    Event_post(protocol_eventHandle, EVENT_FALG_DISPLAY_PAGE);
    gchange_page_flag = FALSE;
    //加入下面两句好的目的是，放置切页任务大于刷屏任务，导致价签错误
    clear_event_flag_epd_display = 1;
}

void clk_uplink_Fxn(void)
{
    exit_uplink_status_flag = TRUE;
}

Clock_Params clk1Params;
Clock_Handle clk1Handle;
Clock_Params clk_cut_page_Params;
Clock_Handle clk_cut_page_Handle;
Clock_Params clk_uplink_Params;
Clock_Handle clk_uplink_Handle;

void creat_timer(void)   //单位是10us
{
    clk1Params.period = 0;
    clk1Params.startFlag = FALSE;
    clk1Handle = Clock_create((Clock_FuncPtr)clk1Fxn, 100, &clk1Params, 0);
//    Clock_start(clk1Handle);
}

void set_clock_timeout(uint32_t timeout)    //单位是10us
{
    Clock_stop(clk1Handle);
    Clock_setTimeout(clk1Handle,timeout);
    Clock_start(clk1Handle);
}


void creat_cut_page_timer(void)   //单位是1s
{
    clk_cut_page_Params.period = 0;
    clk_cut_page_Params.startFlag = FALSE;
    clk_cut_page_Handle = Clock_create((Clock_FuncPtr)clk_cut_page_Fxn, 100000, &clk_cut_page_Params, 0);
//    Clock_start(clk1Handle);
}

void set_cut_page_clock_timeout(uint32_t timeout)  //单位是1s
{
    Clock_stop(clk_cut_page_Handle);
    Clock_setTimeout(clk_cut_page_Handle, 100000*timeout);
    Clock_start(clk_cut_page_Handle);
}

void stop_cut_page_timer(void)
{
    Clock_stop(clk_cut_page_Handle);
}


void creat_uplink_timer(void)   //单位是1s
{
    clk_uplink_Params.period = 0;
    clk_uplink_Params.startFlag = FALSE;
    clk_uplink_Handle = Clock_create((Clock_FuncPtr)clk_uplink_Fxn, 100000, &clk_uplink_Params, 0);
//    Clock_start(clk1Handle);
}

void set_uplink_timeout(uint32_t timeout)  //单位是1s
{
    Clock_stop(clk_uplink_Handle);
    Clock_setTimeout(clk_uplink_Handle, 100000*timeout);
    Clock_start(clk_uplink_Handle);
}

void stop_uplink_timer(void)
{
    Clock_stop(clk_uplink_Handle);
}
