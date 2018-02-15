
#include "reed.h"
#include "Board.h"
#include "global_variable.h"


static PIN_Handle reedPinHandle;
static PIN_State  reedPinState;
PIN_Config reedPinTable[] = {
                             Board_BUTTON0  | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_NEGEDGE,
                             PIN_TERMINATE
};

void isr_reed_init(void)
{
    reedPinHandle = PIN_open(&reedPinState, reedPinTable);
    if(!reedPinHandle)
    {
        while(1);
    }
    if (PIN_registerIntCb(reedPinHandle, &reedCallbackFxn) != 0)
    {
        while(1);
    }
}

void reedCallbackFxn(PIN_Handle handle, PIN_Id pinId)
{
    Event_post(protocol_eventHandle, EVENT_FLAG_INTER_REED);
    Semaphore_post(reed_sem);
}

void reed_fun_select( UINT16 magnet)
{
#define MAGNET_CMD_REMOTE_HB    0x0001
#define MAGNET_CMD_REMOTE_LED   0x0002
#define MAGNET_CMD_TRIGGER_LED  0x0004
#define MAGNET_CMD_TRANSFER     0x0008
#define MAGNET_CHANGE_PAGE1     0x0010
#define MAGNET_CHANGE_PAGE2     0x0020
#define MAGNET_CHANGE_PAGE3     0x0030
#define MAGNET_CHANGE_PAGE4     0x0040
#define MAGNET_CHANGE_PAGE5     0x0050
#define MAGNET_CHANGE_PAGE6     0x0060
#define MAGNET_CHANGE_PAGE7     0x0070
#define MAGNET_CHANGE_PAGE8     0x0080

    if(magnet & MAGNET_CMD_TRIGGER_LED)
    {
        LED_GREEN_ON;
        Task_sleep(3000);
        LED_GREEN_OFF;
    }
    if(magnet & MAGNET_CMD_REMOTE_HB)
    {
        Send_heartbeat(CTRL_HBR_REED,5);
    }
    if(magnet & MAGNET_CMD_TRANSFER)
    {
        select_close_timer();
        def_sys_attr.gwor_flag_now = RF_FSM_CMD_RC_EXIT_LOOP;
        Event_post(protocol_eventHandle, EVENT_FLAG_RFWORK);

    }
    else if(magnet & MAGNET_CMD_REMOTE_LED)
    {
        led_tp_init_fun(1);
        if(led_flash_flag == TRUE)
        {
            Semaphore_post(led_off_sem);
        }else
        {
            Semaphore_post(led_flash_sem);
        }

    }
    else if(magnet & 0x00f0)
    {

        UINT8 pageid=(UINT8)((magnet & 0x00f0)>>4) -1;
        gpage.flag = FALSE;//停止之前的计数
        if(FALSE == sys_page_display_store_fun(gSys_tp.default_page_id,pageid,gSys_tp.gpage_nowid ,epd_attr_info.mag_dis_time,0))//保存页显示属性 默认页、当前页、切换的页id号、停留时间，已经走过的时间、状态标志
        {
            gerr_info = SYS_SAVE_ERR;
        }
//        if(gchange_page_flag == TRUE)    //如果正处于切页状态，先把定时器停掉
//        {
//            stop_cut_page_timer();
//        }
        Event_post(protocol_eventHandle, EVENT_FALG_DISPLAY_PAGE);
        fast_refresh_flag=1;
        gchange_page_flag = TRUE;

    }
    else
        ;

}
