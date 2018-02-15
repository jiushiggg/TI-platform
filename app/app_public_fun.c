
#include "app.h"

void screen_repeat_display(void)
{
    if((g_screen_repeat_dis_time * def_sys_attr.sys_def_attribute.set_wkup_time) >= SCREEN_DSIPLAY_RELOAD_CONT)  //24小时后自动刷屏
    {
        g_screen_repeat_dis_time =0;
        if(g_screen_repeat_dis_flag)
        {
            Event_post(protocol_eventHandle, EVENT_FALG_DISPLAY_PAGE);
        }
    }
}

void netlink_info_write_fun(void)
{
    INFO_DATA.gRFInitData.wakeup_id = netlink_info.set_wk_id;
    INFO_DATA.gRFInitData.set_wkup_ch = netlink_info.set_wkup_ch;
    INFO_DATA.gRFInitData.grp_wkup_ch = netlink_info.grp_wkup_ch;
    INFO_DATA.gRFInitData.esl_data_ch = netlink_info.esl_data_ch;
    INFO_DATA.gRFInitData.esl_netmask = netlink_info.esl_netmask;
    INFO_DATA.idcrc =  my_cal_crc16(0,(uint8_t *)&INFO_DATA.gRFInitData, sizeof(RFINIT));
    save_extern_rf_info();
}

void request_heartbeat_fun(void)
{
    Send_heartbeat(CTRL_HBR_REQ,1);
    rqst_hbr_info.cont++;
    if(rqst_hbr_info.cont >= rqst_hbr_info.sum)
    {
        hbr_mode = NORMAL_HEARTBEAT;
        Clock_stop(heartbeat_timer_handle);
        Clock_setPeriod(heartbeat_timer_handle, (def_sys_attr.sys_def_attribute.heartbit_time* 100000));
        Clock_setTimeout(heartbeat_timer_handle, (def_sys_attr.sys_def_attribute.heartbit_time* 100000));
        Clock_start(heartbeat_timer_handle);
    }
}

void erase_pkg_area_fun(void)
{
#ifdef PGK_BIT_FLASH_OPEN
    f_erase(F_BMP_PKG_1);
    f_erase(F_BMP_PKG_2);
#endif
}









