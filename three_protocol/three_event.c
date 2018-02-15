
#include "three_event.h"
#include "storage_struct.h"
#include "../aes128/aes128.h"
#include "../aes128/aes128_osdcmd.h"

void set_grp_wor_time_fun(uint8_t time)
{
    if(def_sys_attr.sys_def_attribute.grp_wkup_time != time)
    {
        def_sys_attr.sys_def_attribute.grp_wkup_time = time;
        save_sys_config_info();
        Clock_stop(group_wakeup_timer_handle);
        Clock_setPeriod(group_wakeup_timer_handle, (def_sys_attr.sys_def_attribute.grp_wkup_time* 100000));
        Clock_setTimeout(group_wakeup_timer_handle, (def_sys_attr.sys_def_attribute.grp_wkup_time* 100000));
        Clock_start(group_wakeup_timer_handle);
    }
}

void select_close_timer(void)
{
    if(def_sys_attr.gwor_flag_now == RF_FSM_CMD_GRP_EXIT_LOOP)
    {
        Clock_stop(group_wakeup_timer_handle);
    }
    else if(def_sys_attr.gwor_flag_now == RF_FSM_CMD_RC_EXIT_LOOP)
    {
        Clock_stop(rc_wakeup_timer_handle);
    }else if(def_sys_attr.gwor_flag_now == RF_FSM_CMD_UPLINK_EXIT_LOOP)
    {
        Clock_stop(uplink_wakeup_timer_handle);
    }
    else
    {
        Clock_stop(set_wakeup_timer_handle);
    }
}
void rf_interrupt_into_fun(void)
{

    if(def_sys_attr.gwor_flag_now == RF_FSM_CMD_GRP_EXIT_LOOP)
    {
        Clock_stop(group_wakeup_timer_handle);
        grf_state_flag.cmd= RF_FSM_CMD_GRP_WAKEUP;
    }
    else if(def_sys_attr.gwor_flag_now == RF_FSM_CMD_RC_EXIT_LOOP)
    {
        Clock_stop(rc_wakeup_timer_handle);
        grf_state_flag.cmd= RF_FSM_CMD_RC_WAKEUP;
    }
    else if(def_sys_attr.gwor_flag_now == RF_FSM_CMD_UPLINK_EXIT_LOOP)
    {
        Clock_stop(uplink_wakeup_timer_handle);
        grf_state_flag.cmd= RF_FSM_CMD_UPLINK_WAKEUP;
    }else
    {
        Clock_stop(set_wakeup_timer_handle);
        grf_state_flag.cmd = RF_FSM_CMD_SET_WAKEUP;
    }

    rst_sid_fun();//12小时清除sid
    rf_state_machine(&grf_state_flag);

    if(rc_store_inio_flag)
    {
        rc_store_inio_flag=FALSE;
        if(FALSE == sys_page_display_store_fun( gSys_tp.default_page_id,rc_attr_info.page_num,gSys_tp.gpage_nowid ,rc_attr_info.display_time,0))//保存页显示属性 默认页、要显示的页、屏幕当前显示的页、停留时间，已经走过的时间、状态标志
        {
            gerr_info = SYS_SAVE_ERR;
        }
    }
}
void set_wor_flag_fun(RF_CMD_T tp)
{
    switch (tp)
    {
    case RF_FSM_CMD_UPLINK_EXIT_LOOP:
        def_sys_attr.gwor_flag_now = RF_FSM_CMD_UPLINK_EXIT_LOOP;
        break;
    case RF_FSM_CMD_RC_EXIT_LOOP:
        def_sys_attr.gwor_flag_now = RF_FSM_CMD_RC_EXIT_LOOP;
        break;
    case RF_FSM_CMD_GRP_EXIT_LOOP:
        def_sys_attr.gwor_flag_now = RF_FSM_CMD_GRP_EXIT_LOOP;
        def_sys_attr.gwor_flag_before = RF_FSM_CMD_GRP_EXIT_LOOP;
        break;
    default :
        def_sys_attr.gwor_flag_now = RF_FSM_CMD_SET_EXIT_LOOP;
        def_sys_attr.gwor_flag_before = RF_FSM_CMD_SET_EXIT_LOOP;
        break;
    }

}

void start_set_group_rc_timer(void)
{
    if(def_sys_attr.gwor_flag_now == RF_FSM_CMD_UPLINK_EXIT_LOOP)
    {
        Clock_start(uplink_wakeup_timer_handle);
    }
    else if(def_sys_attr.gwor_flag_now == RF_FSM_CMD_RC_EXIT_LOOP)
    {
        Clock_start(rc_wakeup_timer_handle);
    }else if(def_sys_attr.gwor_flag_now == RF_FSM_CMD_GRP_EXIT_LOOP)
    {
        Clock_start(group_wakeup_timer_handle);
    }else
    {
        Clock_start(set_wakeup_timer_handle);
    }
}

uint32_t rfid_to_u32(RFID id)
{
    uint32_t temp, value=0;
    temp =id.id0;
    value += (temp<<24);
    temp =id.id1;
    value += (temp<<16);
    temp =id.id2;
    value += (temp<<8);
    temp =id.id3;
    value += temp;
    return value;
}

void clear_queue_buf(void)
{
    uint8_t buf[128];
    currentDataEntry = RFQueue_getDataEntry();
    packetDataPointer = (uint8_t*)(&currentDataEntry->data);

    if(DATA_ENTRY_PENDING != currentDataEntry->status)
    {
        memcpy(buf, packetDataPointer, currentDataEntry->length);
        RFQueue_nextEntry();
    }
}

RF_ERROR_T rf_cmd_set_rx(RF_T *rf, uint8_t *buf)
{
    MsgObj  rf_get_msg;
    bool xStatus;
    uint32_t  id_temp;
    RFID setwkupid_temp;

    setwkupid_temp =INFO_DATA.gRFInitData.wakeup_id;
    setwkupid_temp.id2 = 0x00;
    id_temp = rfid_to_u32(setwkupid_temp);
    set_rf_parameters(DATA_RATE_500K, RF_TX_POWER_0DB , 2400+(INFO_DATA.gRFInitData.set_wkup_ch/2), INFO_DATA.gRFInitData.set_wkup_ch%2);
    RF_EventMask rx_handle = Rf_rx_package(rfHandle, &dataQueue, id_temp, RF_SET_WAKEUP_PACKET, TRUE, SET_RX_TIMEOUT);//   FALSE , 0);
    xStatus = Mailbox_pend(rf_rx_timeout_mailbox, &rf_get_msg, SET_RX_TIMEOUT);

    RF_yield(rfHandle);    //使射频进入sleep状态
    if(xStatus == TRUE)
    {
        if(rf_get_msg.val == RF_RX_DONE)
        {
            currentDataEntry = RFQueue_getDataEntry();
            packetDataPointer = (uint8_t*)(&currentDataEntry->data);
            memcpy(buf, packetDataPointer, RF_SET_WAKEUP_PACKET);
            RFQueue_nextEntry();
            return  RF_ERROR_NONE;
        }else
        {
            RF_cancelCmd(rfHandle, rx_handle,0);
            clear_queue_buf();
            return  RF_ERROR_RX_TIMEOUT;
        }
    }else
    {
        clear_queue_buf();
        return  RF_ERROR_RX_TIMEOUT;
    }
}

void set_wkup_event(RF_T *rf, UINT8 ctrl)
{
    switch(ctrl&(UINT8)SET_CTRL_INFO)
    {
    case SET_GLOBAL_CTRL:
        rf->event = RF_EVENT_GLOBLE_CMD;
        break;
    case SET_GET_FRAME1_CTRL:
        rf->event = RF_EVENT_GET_FRAME1;
        break;
    case SET_PKG_TRN_CTRL:
        rf->event = RF_EVENT_PKG_TRN;
        break;
    case SET_PKG_CH_CTRL:
        rf->event = RF_EVENT_SET_PKG_CH;
        break;
    case SET_BROADCAST_CTRL:
        rf->event = RF_EVENT_SET_BROADCAST;
        break;
    case SET_QUICK_LED_PAGE_CTRL:
        rf->event = RF_EVENT_SET_QUICK_LED_PAGE;
        break;
    case SET_UPLINK_CTRL:
        rf->event = RF_EVENT_SET_UPLINK;
        break;
    default:
        rf->event = RF_EVENT_SET_ERR;
        break;
    }
}

RF_ERROR_T rf_cmd_grp_rx(RF_T *rf, uint8_t *buf)
{
    MsgObj  rf_get_msg;
    bool xStatus;
    uint32_t  id_temp;
    RFID setwkupid_temp;

    setwkupid_temp =INFO_DATA.gRFInitData.wakeup_id;
    id_temp = rfid_to_u32(setwkupid_temp);
    set_rf_parameters(DATA_RATE_500K, RF_TX_POWER_0DB , 2400+(INFO_DATA.gRFInitData.grp_wkup_ch/2), INFO_DATA.gRFInitData.grp_wkup_ch%2);
    RF_EventMask rx_handle = Rf_rx_package(rfHandle, &dataQueue, id_temp, RF_GRP_WAKEUP_PACKET, TRUE , GRP_RX_TIMEOUT);
    xStatus =  Mailbox_pend (rf_rx_timeout_mailbox, &rf_get_msg, GRP_RX_TIMEOUT);
    RF_yield(rfHandle);    //使射频进入sleep状态
    if(xStatus == TRUE)
    {
        if(rf_get_msg.val == RF_RX_DONE)
        {
            currentDataEntry = RFQueue_getDataEntry();
            packetDataPointer = (uint8_t*)(&currentDataEntry->data);
            memcpy(buf, packetDataPointer, RF_GRP_WAKEUP_PACKET);
            RFQueue_nextEntry();
            return  RF_ERROR_NONE;
        }else
        {
            RF_cancelCmd(rfHandle, rx_handle,0);
            clear_queue_buf();
            return  RF_ERROR_RX_TIMEOUT;
        }
    }else
    {
        clear_queue_buf();
        return  RF_ERROR_RX_TIMEOUT;
    }
}

RF_CMD_T grp_wkup_event(RF_T *rf, UINT8 *buf)
{
    UINT32 wakeup1_sort = 0;

    if((buf[0] & GRP_WKUP_CTRL_INFO) != GRP_WKUP_READY)
        return RF_FSM_CMD_GRP_RX;
    wakeup1_sort = (UINT16)((UINT16)(buf[0]&0x1F)*256+(UINT16)buf[1]);
    Clock_stop(clk1Handle);
    if(wakeup1_sort>2)
    {
        wakeup1_sort -=2;
        Task_sleep(wakeup1_sort*1000);
    }
    return RF_FSM_CONFG_FRAME1_PER;
}



RF_ERROR_T rf_rc_grp_rx(RF_T *rf,UINT8 *buf)
{
    MsgObj  rf_get_msg;
    bool xStatus;
    uint32_t  id_temp;
    RFID setwkupid_temp;

    setwkupid_temp =INFO_DATA.gRFInitData.esl_id;
    id_temp = rfid_to_u32(setwkupid_temp);
    set_rf_parameters(DATA_RATE_500K, RF_TX_POWER_0DB , 2400+(INFO_DATA.gRFInitData.esl_data_ch/2), INFO_DATA.gRFInitData.esl_data_ch%2);
    RF_EventMask rx_handle = Rf_rx_package(rfHandle, &dataQueue, id_temp, RF_GRP_WAKEUP_PACKET, TRUE , RC_RX_TIMEOUT);
    xStatus =  Mailbox_pend (rf_rx_timeout_mailbox, &rf_get_msg, RC_RX_TIMEOUT);
    RF_yield(rfHandle);    //使射频进入sleep状态
    if(xStatus == TRUE)
    {
        if(rf_get_msg.val == RF_RX_DONE)
        {
            currentDataEntry = RFQueue_getDataEntry();
            packetDataPointer = (uint8_t*)(&currentDataEntry->data);
            memcpy(buf, packetDataPointer, RF_GRP_WAKEUP_PACKET);
            RFQueue_nextEntry();
            return  RF_ERROR_NONE;
        }else
        {
            RF_cancelCmd(rfHandle, rx_handle,0);
            clear_queue_buf();
            return  RF_ERROR_RX_TIMEOUT;
        }
    }else
    {
        clear_queue_buf();
        return  RF_ERROR_RX_TIMEOUT;
    }
}
RF_CMD_T rc_wor_exit_fun( void)
{
    if(def_sys_attr.gwor_flag_before == RF_FSM_CMD_GRP_EXIT_LOOP)
    {
        set_wor_flag_fun(RF_FSM_CMD_GRP_EXIT_LOOP);
        return RF_FSM_CMD_GRP_WAKEUP;
    }
    else
    {
        set_wor_flag_fun(RF_FSM_CMD_SET_EXIT_LOOP);
        return RF_FSM_CMD_SET_WAKEUP;
    }
}
RF_CMD_T rc_wkup_event(RF_T *rf, UINT8 *buf)
{
    UINT16 wakeup1_sort = 0;

    if((buf[0] & GRP_WKUP_CTRL_INFO) != GRP_WKUP_READY)
        return RF_FSM_CMD_RC_RX;
    wakeup1_sort = (UINT16)((UINT16)(buf[0]&0x1F)*256+(UINT16)buf[1]);
    Clock_stop(clk1Handle);
    Task_sleep(wakeup1_sort*1000);
    return RF_FSM_CONFG_FRAME1_PER;
}

RF_ERROR_T downlink_rx(uint8_t *buf)   //上行链路请求ACK的接收
{
    MsgObj  rf_get_msg;
    bool xStatus;
    uint32_t  id_temp;
    RFID setwkupid_temp;

    setwkupid_temp =INFO_DATA.gRFInitData.esl_id;
    id_temp = rfid_to_u32(setwkupid_temp);
    set_rf_parameters(DATA_RATE_500K, RF_TX_POWER_0DB ,  2400+(INFO_DATA.gRFInitData.set_wkup_ch/2), INFO_DATA.gRFInitData.set_wkup_ch%2);
    RF_EventMask rx_handle = Rf_rx_package(rfHandle, &dataQueue, id_temp, DOWNLINK_ACK_PACKET, TRUE, DOWNLINK_RX_TIMEOUT);
    xStatus = Mailbox_pend(rf_rx_timeout_mailbox, &rf_get_msg, DOWNLINK_RX_TIMEOUT);

    RF_yield(rfHandle);    //使射频进入sleep状态
    if(xStatus == TRUE)
    {
        if(rf_get_msg.val == RF_RX_DONE)
        {
            currentDataEntry = RFQueue_getDataEntry();
            packetDataPointer = (uint8_t*)(&currentDataEntry->data);
            memcpy(buf, packetDataPointer, DOWNLINK_ACK_PACKET);
            RFQueue_nextEntry();
            return  RF_ERROR_NONE;
        }else
        {
            RF_cancelCmd(rfHandle, rx_handle,0);
            clear_queue_buf();
            return  RF_ERROR_RX_TIMEOUT;
        }
    }else
    {
        clear_queue_buf();
        return  RF_ERROR_RX_TIMEOUT;
    }
}

RF_ERROR_T rf_uplink_set_rx(RF_T *rf,UINT8 *buf)
{
    MsgObj  rf_get_msg;
    bool xStatus;
    uint32_t  id_temp;
    RFID setwkupid_temp;

    setwkupid_temp =INFO_DATA.gRFInitData.wakeup_id;
    setwkupid_temp.id2 = 0x00;
    id_temp = rfid_to_u32(setwkupid_temp);
    set_rf_parameters(DATA_RATE_500K, RF_TX_POWER_0DB , 2400+(INFO_DATA.gRFInitData.set_wkup_ch/2), INFO_DATA.gRFInitData.set_wkup_ch%2);
    RF_EventMask rx_handle = Rf_rx_package(rfHandle, &dataQueue, id_temp, RF_SET_WAKEUP_PACKET, TRUE, SET_RX_TIMEOUT);
    xStatus = Mailbox_pend(rf_rx_timeout_mailbox, &rf_get_msg, SET_RX_TIMEOUT);
    RF_yield(rfHandle);    //使射频进入sleep状态
    if(xStatus == TRUE)
    {
        if(rf_get_msg.val == RF_RX_DONE)
        {
            currentDataEntry = RFQueue_getDataEntry();
            packetDataPointer = (uint8_t*)(&currentDataEntry->data);
            memcpy(buf, packetDataPointer, RF_SET_WAKEUP_PACKET);
            RFQueue_nextEntry();
            return  RF_ERROR_NONE;
        }else
        {
            RF_cancelCmd(rfHandle, rx_handle,0);
            clear_queue_buf();
            return  RF_ERROR_RX_TIMEOUT;
        }
    }else
    {
        clear_queue_buf();
        return  RF_ERROR_RX_TIMEOUT;
    }
}

RF_CMD_T uplink_wkup_event(RF_T *rf, UINT8 *buf)
{
    UINT16 wakeup1_sort = 0;

    if((buf[0] & GRP_WKUP_CTRL_INFO) != SET_GET_FRAME1_CTRL)
        return RF_FSM_CMD_UPLINK_RX;
    wakeup1_sort = (UINT16)((UINT16)(buf[0]&0x1F)*256+(UINT16)buf[1]);
    Task_sleep(wakeup1_sort*1000);
    return RF_FSM_CONFG_FRAME1_PER;
}

RF_ERROR_T rf_cmd_rx_fram1(RF_T *rf, uint8_t *buf)
{
    MsgObj  rf_get_msg;
    bool xStatus;
    uint32_t  id_temp;
    RFID setwkupid_temp;

    setwkupid_temp =INFO_DATA.gRFInitData.wakeup_id;
    id_temp = rfid_to_u32(setwkupid_temp);
    set_rf_parameters(DATA_RATE_500K, RF_TX_POWER_0DB , 2400+(INFO_DATA.gRFInitData.grp_wkup_ch/2), INFO_DATA.gRFInitData.grp_wkup_ch%2);
    RF_EventMask rx_handle = Rf_rx_package(rfHandle, &dataQueue, id_temp, RF_NORMAL_PACKET, TRUE , FRAME1_RX_TIMEOUT);
    xStatus =  Mailbox_pend (rf_rx_timeout_mailbox, &rf_get_msg, FRAME1_RX_TIMEOUT);
    RF_yield(rfHandle);    //使射频进入sleep状态
    if(xStatus == TRUE)
    {
        if(rf_get_msg.val == RF_RX_DONE)
        {
            currentDataEntry = RFQueue_getDataEntry();
            packetDataPointer = (uint8_t*)(&currentDataEntry->data);
            memcpy(buf, packetDataPointer, RF_NORMAL_PACKET);
            RFQueue_nextEntry();
            return  RF_ERROR_NONE;
        }else
        {
            RF_cancelCmd(rfHandle, rx_handle,0);
            clear_queue_buf();
            return  RF_ERROR_RX_TIMEOUT;
        }

    }else
    {
        clear_queue_buf();
        return  RF_ERROR_RX_TIMEOUT;
    }
}

RF_ERROR_T rf_cmd_rx_fram2(RF_T *rf, uint8_t *buf)
{
    MsgObj  rf_get_msg;
    bool xStatus;
    uint32_t  id_temp;
    RFID setwkupid_temp;

    setwkupid_temp =INFO_DATA.gRFInitData.wakeup_id;
    setwkupid_temp.id2 = 0x00;
    id_temp = rfid_to_u32(setwkupid_temp);
    set_rf_parameters(DATA_RATE_500K, RF_TX_POWER_0DB , 2400+(INFO_DATA.gRFInitData.set_wkup_ch/2), INFO_DATA.gRFInitData.set_wkup_ch%2);
    RF_EventMask rx_handle = Rf_rx_package(rfHandle, &dataQueue, id_temp, RF_NORMAL_PACKET, TRUE , FRAME2_RX_TIMEOUT);
    xStatus =  Mailbox_pend (rf_rx_timeout_mailbox, &rf_get_msg, FRAME2_RX_TIMEOUT);
    RF_yield(rfHandle);    //使射频进入sleep状态
    if(xStatus == TRUE)
    {
        if(rf_get_msg.val == RF_RX_DONE)
        {
            currentDataEntry = RFQueue_getDataEntry();
            packetDataPointer = (uint8_t*)(&currentDataEntry->data);
            memcpy(buf, packetDataPointer, RF_NORMAL_PACKET);
            RFQueue_nextEntry();
            return  RF_ERROR_NONE;
        }else
        {
            RF_cancelCmd(rfHandle, rx_handle,0);
            clear_queue_buf();
            return  RF_ERROR_RX_TIMEOUT;
        }

    }else
    {
        clear_queue_buf();
        return  RF_ERROR_RX_TIMEOUT;
    }
}

UINT16 set_rf_work_time_cont_fun(void)
{
    return 200 * def_sys_attr.sys_def_attribute.work_time;
}

#if 0
RF_CMD_T rx_frame1_mask_exit_fun( void)
{
    rc_wor_times= 0;
    if(def_sys_attr.gwor_flag_now == RF_FSM_CMD_GRP_EXIT_LOOP)
    {
        return RF_FSM_CMD_GRP_EXIT_LOOP;

    }
    else
        return RF_FSM_CMD_SET_EXIT_LOOP;
}
#else
RF_CMD_T rx_frame1_mask_exit_fun( void)
{
    rc_wor_times= 0;
    if(def_sys_attr.gwor_flag_now == RF_FSM_CMD_UPLINK_EXIT_LOOP)
    {
        return RF_FSM_CMD_UPLINK_EXIT_LOOP;

    }else if(def_sys_attr.gwor_flag_before == RF_FSM_CMD_GRP_EXIT_LOOP)
    {
        return RF_FSM_CMD_GRP_EXIT_LOOP;
    }
    else
        return RF_FSM_CMD_SET_EXIT_LOOP;
}
#endif
RF_CMD_T rx_frame1_event(RF_T *rf, UINT8 *buf)
{
    UINT8 mask_i=0,mask_m = 0;
    UINT16 time_cont = 0;

    if((buf[0] & GRP_WKUP_CTRL_INFO) != FRAME1_CTRL_INFO)
        return RF_FSM_CMD_RX_FRAME1;

    if(TRUE != rf_pgk_crc(buf,RF_NORMAL_PACKET,INFO_DATA.gRFInitData.wakeup_id))
        return RF_FSM_CMD_RX_FRAME1;

    Clock_stop(clk1Handle);
    mask_i =  INFO_DATA.gRFInitData.esl_netmask % 160 / 8;
    mask_m =  INFO_DATA.gRFInitData.esl_netmask % 160 % 8;
    if (buf[2+mask_i] & (0x01<<mask_m) )
    {
        fram1_data_channel = buf[1];
        time_cont = set_rf_work_time_cont_fun();
        set_clock_timeout(time_cont*500);    //设置接收超时时间
        return RF_FSM_CMD_RX_DATA_CONFIG;
    }

    return rx_frame1_mask_exit_fun();
}

RF_CMD_T rx_frame2_event(RF_T *rf, UINT8 *buf)
{
    UINT16 time_cont, i;
    UINT32 erase_use_slort,erase_use_slort_temp;

    memcpy((UINT8 *) &upgread_frame2,buf,sizeof(upgread_frame2));
    if(TRUE != crc_broadcast_normal_pkg_fun(buf))
        return RF_FSM_CMD_RX_FRAME2;

    Clock_stop(clk1Handle);
    if(EPD_3_VERSION == upgread_frame2.rom_version)    //rom版本号相同，不做升级
        return RF_FSM_CMD_SLEEP;
    if(upgread_frame2.updata_area >= UP_AREA_END)                //指定升级区域错误
        return RF_FSM_CMD_SLEEP;

    switch(upgread_frame2.updata_area)
    {

    case UP_APP_AREA:
        for(i=0;i<upgread_frame2.firmware_sum*4;i+=4)
        {
            //            if(esl_firmware_id == ((buf[8+i+3]<<24) + (buf[8+i+2]<<16) + (buf[8+i+1]<<8) + buf[8+i]))
            if(0 == memcmp((UINT8 *)&esl_firmware_id, (UINT8 *)&buf[8+i], sizeof(esl_firmware_id)))
            {
                erase_use_slort = Clock_getTicks();
                if((upgread_id  != upgread_frame2.upgreade_id)||(rom_version != upgread_frame2.rom_version))                          //防止第二次收到帧2时再次擦除flash
                {
                    upgrade_status_flag = TRUE;
                    fs_erase_all_file();
                    upgread_id_change_eraflag();
                    memset((UINT8 *)gdsi_info_st,0xFF,sizeof(G_DIS_PAGE_T)*MAX_DISPLAY_PGAE_SUM);
                    memset(gSys_tp.page_map,0xff,8);//清空页码
                    upgread_id = upgread_frame2.upgreade_id;
                    rom_version = upgread_frame2.rom_version;
                    upgread_pkg_all = upgread_frame2.upgread_pkg_all;
                }
                erase_use_slort_temp = Clock_getTicks();
                if(erase_use_slort_temp < erase_use_slort)
                {
                    erase_use_slort = (0xffffffff - erase_use_slort + erase_use_slort_temp)/1000;     //把时间换算成以10ms为单位，和slort值的单位统一
                }
                else
                {
                    erase_use_slort = (erase_use_slort_temp - erase_use_slort)/1000;
                }
                if(upgread_frame2.Synchronised_time > (erase_use_slort + 1))
                {
                    Task_sleep((upgread_frame2.Synchronised_time - erase_use_slort - 1)*1000);         //-2是为了提前开接收，避免时钟不准，延迟导致的丢包
                }
                time_cont = set_rf_work_time_cont_fun();
                set_clock_timeout(time_cont*500 + 3000);    //设置接收超时时间

                return RF_FSM_CMD_BROADCAST_RX_DATA_CONFIG;

            }
        }
        break;
    case UP_ALL_CODE_AREA:
        for(i=0;i<upgread_frame2.firmware_sum*4;i+=4)
        {
            if(0 == memcmp((UINT8 *)&buf[8+i], (UINT8 *)&INFO_DATA.gRFInitData.screen_id, sizeof(INFO_DATA.gRFInitData.screen_id)))
            {
                erase_use_slort = Clock_getTicks();
                if((upgread_id  != upgread_frame2.upgreade_id)||(rom_version != upgread_frame2.rom_version))                          //防止第二次收到帧2时再次擦除flash
                {
                    upgrade_status_flag = TRUE;
                    fs_erase_all_file();
                    memset((UINT8 *)gdsi_info_st,0xFF,sizeof(G_DIS_PAGE_T)*MAX_DISPLAY_PGAE_SUM);
                    memset(gSys_tp.page_map,0xff,8);//清空页码
                    upgread_id_change_eraflag();
                    upgread_id = upgread_frame2.upgreade_id;
                    upgread_pkg_all = upgread_frame2.upgread_pkg_all;
                }
                erase_use_slort_temp = Clock_getTicks();
                if(erase_use_slort_temp < erase_use_slort)
                {
                    erase_use_slort = (0xffffffff - erase_use_slort + erase_use_slort_temp)/1000;     //把时间换算成以10ms为单位，和slort值的单位统一
                }
                else
                {
                    erase_use_slort = (erase_use_slort_temp - erase_use_slort)/1000;
                }
                if(upgread_frame2.Synchronised_time > (erase_use_slort + 1))
                {
                    Task_sleep((upgread_frame2.Synchronised_time - erase_use_slort - 1)*1000);         //-2是为了提前开接收，避免时钟不准，延迟导致的丢包
                }
                time_cont = set_rf_work_time_cont_fun();
                set_clock_timeout(time_cont*500 + 3000);    //设置接收超时时间

                return RF_FSM_CMD_BROADCAST_RX_DATA_CONFIG;
            }
        }
        break;
    case UP_SCHOOL_APP_AREA:
        if(0 == memcmp((UINT8 *)(buf + 8), (UINT8 *)&school_id, sizeof(school_id)))
        {
            erase_use_slort = Clock_getTicks();
            if((upgread_id  != upgread_frame2.upgreade_id)||(rom_version != upgread_frame2.rom_version))                          //防止第二次收到帧2时再次擦除flash
            {
                upgrade_status_flag = TRUE;
                fs_erase_all_file();
                memset((UINT8 *)gdsi_info_st,0xFF,sizeof(G_DIS_PAGE_T)*MAX_DISPLAY_PGAE_SUM);
                memset(gSys_tp.page_map,0xff,8);//清空页码
                upgread_id_change_eraflag();
                upgread_id = upgread_frame2.upgreade_id;
                upgread_pkg_all = upgread_frame2.upgread_pkg_all;
            }
            erase_use_slort_temp = Clock_getTicks();
            if(erase_use_slort_temp < erase_use_slort)
            {
                erase_use_slort = (0xffffffff - erase_use_slort + erase_use_slort_temp)/1000;     //把时间换算成以10ms为单位，和slort值的单位统一
            }
            else
            {
                erase_use_slort = (erase_use_slort_temp - erase_use_slort)/1000;
            }
            if(upgread_frame2.Synchronised_time > (erase_use_slort + 1))
            {
                Task_sleep((upgread_frame2.Synchronised_time - erase_use_slort - 1)*1000);         //-2是为了提前开接收，避免时钟不准，延迟导致的丢包
            }
            time_cont = set_rf_work_time_cont_fun();
            set_clock_timeout(time_cont*500 + 3000);    //设置接收超时时间

            return RF_FSM_CMD_BROADCAST_RX_DATA_CONFIG;
        }
        break;
    default:
        break;
    }
    return RF_FSM_CMD_SLEEP;
}

RF_ERROR_T rf_cmd_rx_data(RF_T *rf, uint8_t *buf)
{
    MsgObj  rf_get_msg;
    bool xStatus;
    uint32_t  id_temp;
    RFID setwkupid_temp;

    setwkupid_temp = INFO_DATA.gRFInitData.esl_id;
    id_temp = rfid_to_u32(setwkupid_temp);
    set_rf_parameters(DATA_RATE_500K, RF_TX_POWER_0DB , 2400+(fram1_data_channel/2), fram1_data_channel%2);
    RF_EventMask rx_handle = Rf_rx_package(rfHandle, &dataQueue, id_temp, RF_NORMAL_PACKET, TRUE , (def_sys_attr.sys_def_attribute.work_time * 100000));
    xStatus =  Mailbox_pend (rf_rx_timeout_mailbox, &rf_get_msg, (def_sys_attr.sys_def_attribute.work_time * 100000));
    RF_yield(rfHandle);    //使射频进入sleep状态
    if(xStatus == TRUE)
    {
        if(rf_get_msg.val == RF_RX_DONE)
        {
            currentDataEntry = RFQueue_getDataEntry();
            packetDataPointer = (uint8_t*)(&currentDataEntry->data);
            memcpy(buf, packetDataPointer, RF_NORMAL_PACKET);
            RFQueue_nextEntry();
            return  RF_ERROR_NONE;
        }else
        {
            RF_cancelCmd(rfHandle, rx_handle,0);
            clear_queue_buf();
            return  RF_ERROR_RX_TIMEOUT;
        }

    }else
    {
        clear_queue_buf();
        return  RF_ERROR_RX_TIMEOUT;
    }
}

RF_ERROR_T rf_cmd_broadcast_rx_data(RF_T *rf, uint8_t *buf)
{
    MsgObj  rf_get_msg;
    bool xStatus;
    uint32_t  id_temp;
    RFID setwkupid_temp;

    setwkupid_temp =INFO_DATA.gRFInitData.wakeup_id;
    setwkupid_temp.id2 = 0x00;
    id_temp = rfid_to_u32(setwkupid_temp);
    set_rf_parameters(DATA_RATE_500K, RF_TX_POWER_0DB , 2400+(INFO_DATA.gRFInitData.set_wkup_ch/2), INFO_DATA.gRFInitData.set_wkup_ch%2);
    RF_EventMask rx_handle = Rf_rx_package(rfHandle, &dataQueue, id_temp, RF_NORMAL_PACKET, TRUE , (def_sys_attr.sys_def_attribute.work_time * 100000));
    xStatus =  Mailbox_pend (rf_rx_timeout_mailbox, &rf_get_msg, (def_sys_attr.sys_def_attribute.work_time * 100000));
    RF_yield(rfHandle);    //使射频进入sleep状态
    if(xStatus == TRUE)
    {
        if(rf_get_msg.val == RF_RX_DONE)
        {
            currentDataEntry = RFQueue_getDataEntry();
            packetDataPointer = (uint8_t*)(&currentDataEntry->data);
            memcpy(buf, packetDataPointer, RF_NORMAL_PACKET);
            RFQueue_nextEntry();
            return  RF_ERROR_NONE;
        }else
        {
            RF_cancelCmd(rfHandle, rx_handle,0);
            clear_queue_buf();
            return  RF_ERROR_RX_TIMEOUT;
        }

    }else
    {
        clear_queue_buf();
        return  RF_ERROR_RX_TIMEOUT;
    }
}

bool crc_normal_pkg_fun(UINT8 *buf)
{
    if((buf[0]&(UINT8)SET_CTRL_INFO) == CTRL_BROADCAST_UPDATA)     //补升级包时，计算crc时按照广播的数据包计算crc
    {
        return crc_broadcast_normal_pkg_fun(buf);
    }
    if(TRUE != rf_pgk_crc(buf,RF_NORMAL_PACKET,INFO_DATA.gRFInitData.esl_id))
        return FALSE;
    return TRUE;
}

bool crc_broadcast_normal_pkg_fun(UINT8 *buf)
{
    RFID setwkupid_temp;
    setwkupid_temp =INFO_DATA.gRFInitData.wakeup_id;
    setwkupid_temp.id2 = 0x00;
    if(TRUE != rf_pgk_crc(buf,RF_NORMAL_PACKET,setwkupid_temp))
        return FALSE;
    return TRUE;
}

void clear_gpkg_fun(void)
{
    memset((UINT8 *)&gpkg,0x00,sizeof(gpkg));
    ret_ack_flag = RF_EVENT_UNKNOWN;
}
void clear_pkg_nosd_fun(void)
{
    if(ret_ack_flag != RF_EVENT_OSD )
        clear_gpkg_fun();
}
bool all_pkg_cmp_fun(void)
{
    if(gpkg.pkg_num != gpkg.pkg_sum)
        return FALSE;
    return TRUE;
}

bool check_128_fun(UINT8 *buf,UINT8 offset)
{
    UINT8 tp[10];
    const UINT8 ml_0x80[] = {0x01,0xc0,0x00,0x80};
    memcpy(tp,buf+offset,4);
    tp[2] = 0x00;
    if(0 == memcmp(tp,ml_0x80,4))
        return TRUE;
    return FALSE;

}

bool check_0x76_fun(UINT8 *buf,UINT8 offset)
{
    UINT8 tp[10];
    const UINT8 ml_0x76[] = {0x01,0xc0,0x00,0x76};

    //查询命令
    memcpy(tp,buf+offset,4);
    tp[2] = 0x00;
    if(0 == memcmp(tp,ml_0x76,4))
        return TRUE;
    return FALSE;
}

#define OSD_PKG_START_ADDR      0x01
RF_EVENT_T osd_mode_fun(UINT8 * temp)
{
    gsid.now_sid = temp[0] & OSD_SID_FALG ;
    gsid.now_sub_sid = temp[3] & OSD_SUB_SID_FALG ;

    if(TRUE == check_128_fun(temp,OSD_PKG_START_ADDR))
    {
        return RF_EVENT_OSD_128CMD;
    }

    if(TRUE == check_0x76_fun(temp,OSD_PKG_START_ADDR) )
    {
        return RF_EVENT_OSD_76CMD;
    }
    return RF_EVENT_OSD;
}
void rx_data_event(RF_T *rf, UINT8 *buf)
{

    switch(buf[0]&(UINT8)SET_CTRL_INFO)
    {
    case CTRL_QUERY:
        rf->event = RF_EVENT_QUERY;
        break;
    case CTRL_NETLINK:
        rf->event = RF_EVENT_NETLINK;
        break;
    case CTRL_OSD:
        rf->event = osd_mode_fun(buf);
        break;
    case CTRL_LINK_QUERY:
        rf->event = RF_EVENT_LINK_QUERY;
        break;
    case CTRL_RC_PKG:
        rf->event = RF_EVENT_RC_INFO;
        break;
    case CTRL_BROADCAST_UPDATA:
        rf->event = RF_EVENT_OSD_BROADCAST_PKG;
        break;
    case CTRL_SLEEP:
        //缺少一个属性包和led灯的拣货包
    default:
        rf->event = RF_EVENT_SLEEP;
        break;
    }
}

void broadcast_rx_data_event(RF_T *rf, UINT8 *buf)
{

    switch(buf[0]&(UINT8)SET_CTRL_INFO)
    {
    case CTRL_BROADCAST_UPDATA:
        rf->event = RF_EVENT_BROADCAST_UPDATA;
        break;

    default:
        rf->event = RF_EVENT_SLEEP;
        break;
    }
}
void query_ack_buf_fun(UINT8 *tp)
{
#define ELECTRIC_QUANTITY_QUERY   0
#define UPGREAD_LOSS_PAG_QUERY    3
    UINT16 crc =0;

    memset(tp,0xFF,RF_NORMAL_PACKET);
    tp[0]= gpkg.ctrl ;
    tp[1] = gpkg.sroft + 1;
    tp[2] =0x00;    //无丢包
    tp[3] =0x00;    //无丢包
    if(g_upgread_query_data.flag == ELECTRIC_QUANTITY_QUERY)  //电量查询ACK
    {
        if(gelectric_quantity >=0x01)
            tp[4] = 0x40;
        else
            tp[4] =0x02;
        tp[5] =TR3_QUERY;    //ack-type
    }
    if(g_upgread_query_data.flag == UPGREAD_LOSS_PAG_QUERY)    //升级丢包查询ACK
    {
        tp[4] =0x40;    //ack-value
        broadcast_upgread_lose_pkg_statistics(upgread_pkg_all,read_pkg_fileid,tp+5);  //  计算丢包的bit值
        if(EPD_3_VERSION == g_upgread_query_data.upgreade_rom_version)     //升级成功后，收到查询包
        {
            memset(tp+5,0x00,RF_NORMAL_PACKET - 5);
        }
    }
    crc = ack_crc_fun(tp,RF_NORMAL_PACKET);
    memcpy(tp+24,(UINT8 *)&crc,sizeof(crc));
}
void netlink_ack_buf_fun(UINT8 *tp)
{
    UINT16 crc =0;

    tp[0]= gpkg.ctrl ;
    tp[1] = gpkg.sroft + 1;
    tp[4] = 0x40;
    tp[5] = TR3_NETLINK;
    crc = ack_crc_fun(tp,RF_NORMAL_PACKET);
    memcpy(tp+24,(UINT8 *)&crc,sizeof(crc));
}
void key_err_ack_buf_fun(UINT8 *tp)
{
    UINT16 crc =0;

    tp[0]= gpkg.ctrl ;
    tp[1] = gpkg.sroft + 1;
    tp[4] = gerr_info;
    tp[5] = TR3_KEY_ERR;
    crc = ack_crc_fun(tp,RF_NORMAL_PACKET);
    memcpy(tp+24,(UINT8 *)&crc,sizeof(crc));
}
void osd128_ack_buf_fun(UINT8 *tp)
{
    UINT16 i, crc =0;

    tp[0]= gpkg.ctrl ;
    tp[1] = gpkg.sroft + 1;
    tp[4] = 0x40;
    tp[5] = TR3_OSD_ERASER;
    for(i=0;i<8;i++)
    {
        memcpy(tp+6+2*i,(UINT8 *)&gdsi_info_st[i].page_crc,2);
    }
    crc = ack_crc_fun(tp,RF_NORMAL_PACKET);
    memcpy(tp+24,(UINT8 *)&crc,sizeof(crc));
}
void osd0x76_ack_buf_fun(UINT8 *tp)
{

    UINT16 i,crc =0;

    tp[0]= gpkg.ctrl ;
    tp[1] = gpkg.sroft + 1;

    if(gerr_info != 0)
    {
        tp[4] = gerr_info;
    }
    else
    {
        tp[4] = 0x40;
    }
    tp[5] = TR3_OSD_QUERY;
    for(i=0;i<8;i++)
    {
        memcpy(tp+6+2*i,(UINT8 *)&gdsi_info_st[i].page_crc,2);
    }

    crc = ack_crc_fun(tp,RF_NORMAL_PACKET);
    memcpy(tp+24,(UINT8 *)&crc,sizeof(crc));
}
bool osd_ack_buf_fun(UINT8 *tp)
{
    UINT16 crc =0, num=0;

    tp[0]= gpkg.ctrl ;
    tp[1] = gpkg.sroft + 1;


    if(gerr_info != 0)
    {
        tp[4] = gerr_info;
        tp[5] = TR3_OSD_DATA;
        goto loop;
    }

    num = (UINT16)lose_pkg_statistics(gpkg.pkg_sum,read_pkg_fileid,tp+4);

    if(num == 0)
    {
        tp[4] = 0x40;
        tp[5] = TR3_OSD_DATA;
    }
    else
        memcpy(tp+2,(UINT8 *)&num,sizeof(num));

    loop:

    crc = ack_crc_fun(tp,RF_NORMAL_PACKET);
    memcpy(tp+24,(UINT8 *)&crc,sizeof(crc));
    return TRUE;
}

void rc_ack_buf_fun(UINT8 *tp)
{
    UINT16 crc =0;

    tp[0]= gpkg.ctrl ;
    tp[1] = gpkg.sroft + 1;

    if( grc_err)
        tp[4] =grc_err;
    else
        tp[4] = 0x40;
    tp[5] = TR3_RC;

    crc = ack_crc_fun(tp,RF_NORMAL_PACKET);
    memcpy(tp+24,(UINT8 *)&crc,sizeof(crc));
}
RF_CMD_T tx_ack_info(UINT8 *buf)
{
    memset(buf,0x00,RF_NORMAL_PACKET);
    switch(ret_ack_flag)
    {
    case RF_EVENT_QUERY:
        query_ack_buf_fun(buf);
        break;
    case RF_EVENT_NETLINK:
        netlink_ack_buf_fun(buf);
        break;
    case RF_EVENT_OSD_128CMD:
        osd128_ack_buf_fun(buf);
        break;
    case RF_EVENT_OSD_76CMD:
        osd0x76_ack_buf_fun(buf);
        break;
    case RF_EVENT_OSD:
    case RF_EVENT_OSD_BROADCAST_PKG:
        if(FALSE == osd_ack_buf_fun(buf))
        {
            return  RF_FSM_CMD_RX_DATA;
            //  return RF_FSM_CMD_RX_DATA_CONFIG;这个目前就是接受状态，是否不用退回到rx配置状态
        }
        break;
    case  RF_EVENT_RC_INFO:
        rc_ack_buf_fun(buf);
        break;
    case RF_ENENT_KEY_ACK:
        key_err_ack_buf_fun(buf);
        ret_ack_flag = RF_EVENT_UNKNOWN;
        break;
    default:
        return RF_FSM_CMD_RX_DATA_CONFIG;
    }
    return RF_FSM_CMD_TX_DATA;
}

RF_ERROR_T ack_tx_data(UINT8 len, UINT8 *buf)
{
    uint32_t  id_temp;
    RFID setwkupid_temp;

    setwkupid_temp = INFO_DATA.gRFInitData.master_id;
    id_temp = rfid_to_u32(setwkupid_temp);

    if(g_ack_type != buf[5])
    {
        ack_power_param = def_sys_attr.power_bottom;
    }else
    {
        if(memcmp(g_ack_data,(UINT8 *)&buf[6],sizeof(g_ack_data)))
        {
            ack_power_param++;
            ack_power_param = (ack_power_param > def_sys_attr.power_ceiling)?def_sys_attr.power_ceiling:ack_power_param;
        }
    }
    g_ack_type = buf[5];                                    //ack的类型 tp[5] = TR3_NETLINK;
    memcpy(g_ack_data,(UINT8 *)&buf[6],sizeof(g_ack_data)); //ack的数据

    set_rf_parameters(DATA_RATE_100K, ack_power_param, (2400+fram1_data_channel/2), fram1_data_channel%2);
    RF_EventMask result = Rf_tx_package(rfHandle, id_temp, len, buf);
    if(!(result & RF_EventLastCmdDone))
    {
        RF_close(rfHandle);
        rf_init();
    }
    return RF_ERROR_NONE;
}

//-----------------------------------globle_cmd_fun-----------------------------

void globle_cmd_hbrqus_fun(UINT8 tp)
{

#define GBL_HBRQUS_FLAG 0x80
    if((tp &0xe0) != GBL_HBRQUS_FLAG)
        return ;

    if(hbr_mode == REQUST_HEARTBEAT)//防止重复数据包，导致心跳模式错误
    {
        return;
    }

    hbr_mode = REQUST_HEARTBEAT;
    rqst_hbr_info.cont = 0;
    rqst_hbr_info.now = 0;
    rqst_hbr_info.sum = ((tp&0x03)+1) * 3;
    rqst_hbr_info.hbt_cont= ((( (tp>>2)&0x07 )+1)*2 );
    Clock_stop(heartbeat_timer_handle);
    Clock_setPeriod(heartbeat_timer_handle, (rqst_hbr_info.hbt_cont * 100000));
    Clock_setTimeout(heartbeat_timer_handle, (rqst_hbr_info.hbt_cont * 100000));
    Clock_start(heartbeat_timer_handle);
    Event_post(protocol_eventHandle, EVENT_FLAG_QUEST_HEART);
}
void globle_cmd_hbrconf_fun(UINT8 tp)
{
#define GBL_HBRCONF_FLAG 0x20
    if((tp &0xE0) != GBL_HBRCONF_FLAG)
        return ;
    if((tp&0x1f) == 0x1f)
    {
        hbr_mode = STOP_HEARTBEAT;
        return;
    }
    hbr_mode_bef = NORMAL_HEARTBEAT;
    hbr_mode = NORMAL_HEARTBEAT;
    def_sys_attr.sys_def_attribute.heartbit_time = (UINT16)( ( (tp & 0x1f) + 1) * 30);
    Clock_stop(heartbeat_timer_handle);
    Clock_setPeriod(heartbeat_timer_handle, (def_sys_attr.sys_def_attribute.heartbit_time* 100000));
    Clock_setTimeout(heartbeat_timer_handle, (def_sys_attr.sys_def_attribute.heartbit_time* 100000));
    Clock_start(heartbeat_timer_handle);
    //保存数据到基础参数区
    save_sys_config_info();

}
void globle_cmd_setconf_fun(UINT8 tp)
{
#define GBL_SETCONF_FLAG 0x40
    if((tp &0xE0) != GBL_SETCONF_FLAG)
        return ;
    def_sys_attr.sys_def_attribute.set_wkup_time = (UINT16) (((tp&0x1f) + 1) * 2);
    if(def_sys_attr.sys_def_attribute.set_wkup_time == 2)
    {
        def_sys_attr.sys_def_attribute.set_wkup_time = 1;
    }
    save_sys_config_info();
    Clock_stop(set_wakeup_timer_handle);
    Clock_setPeriod(set_wakeup_timer_handle, (def_sys_attr.sys_def_attribute.set_wkup_time* 100000));
    Clock_setTimeout(set_wakeup_timer_handle, (def_sys_attr.sys_def_attribute.set_wkup_time* 100000));
    Clock_start(set_wakeup_timer_handle);
}
void globle_cmd_ledconf_fun(UINT8 tp)
{
#define GLO_LED_ON  0x72
#define GLO_LED_OFF 0x70

    if((tp != GLO_LED_ON) &&(tp != GLO_LED_OFF))
        return ;
    led_all_off();

    if(tp == GLO_LED_OFF)
    {
        if(led_flash_flag == TRUE)
        {
            Semaphore_post(led_off_sem);
        }
        return;
    }
    if(epd_attr_info.period == 0)
        return;

    if(epd_attr_info.led_num == 0)//属性包不支持跑马灯
        return;
    if(tp == GLO_LED_ON)
    {
        led_tp_init_fun(1);
        if(led_flash_flag == TRUE)
        {
            Semaphore_post(led_off_sem);
            Semaphore_post(led_flash_sem);
        }else
        {
            Semaphore_post(led_flash_sem);
        }
    }

}
void globle_cmd_page_change_fun(UINT8 tp)
{

#define GLO_PAGE_FLAG 0xc0
#define SET_PAGE_ID_T 0x07
#define SET_PAGE_TIME 0x18
#define SET_PAGE_30MIN ((UINT16)1800)

    UINT16 time = 0;

    time = SET_PAGE_30MIN;
    if((tp &0xe0) != GLO_PAGE_FLAG)
        return ;
    time = ( time <<(((tp & SET_PAGE_TIME)>>3)) );//修改时间30、60、120、240

    gpage.flag = FALSE;//停止之前的计数
    if(FALSE == sys_page_display_store_fun(gSys_tp.default_page_id,(tp & SET_PAGE_ID_T),gSys_tp.gpage_nowid,time ,0))//保存页显示属性
    {
        gerr_info = SYS_SAVE_ERR;
    }

    if(gchange_page_flag == TRUE)    //如果正处于切页状态，先把定时器停掉
    {
        stop_cut_page_timer();
    }
    Event_post(protocol_eventHandle, EVENT_FALG_DISPLAY_PAGE);
    gchange_page_flag = TRUE;
}


void globle_def_cmd_fun(UINT8 tp)
{
    return;
}

void globle_process_fun(uint8_t cmd,uint8_t info)
{
    switch(cmd)
    {
    case GBL_CMD_HB_REQ:
        globle_cmd_hbrqus_fun(info);
        break;
    case GBL_CMD_HB_CFG:
        globle_cmd_hbrconf_fun(info);
        break;
    case GBL_CMD_SET_CFG:
        globle_cmd_setconf_fun(info);
        break;
    case GBL_CMD_PAGE_CHG:
        globle_cmd_page_change_fun(info);
        break;
    case GBL_CMD_LED_CFG:
        globle_cmd_ledconf_fun(info);
        break;
    default:
        break;
    }
}
RF_CMD_T globle_evert_fun(UINT8 *buf)
{
#define GRP_NETMASK_START    0x02
#define GRP_PARA_START       0x05

    UINT8 mask_i=0,mask_m = 0;

    if(0xe1 == buf[1])
    {
        //解密
        if(false == glbcmd_encryption_fun(buf,true,1) )
            return RF_FSM_CMD_SLEEP;

        globle_process_fun(buf[2],buf[3]);
        return RF_FSM_CMD_SLEEP;
    }


    mask_i =  INFO_DATA.gRFInitData.wakeup_id.id2%24/8;
    mask_m =  INFO_DATA.gRFInitData.wakeup_id.id2%24%8;

    if ( 0 == (buf[GRP_NETMASK_START+mask_i] & (0x01<<mask_m) ))          //grp子网码是0的表示此组价签不需要工作，直接休眠
        return RF_FSM_CMD_SLEEP;

    globle_process_fun(buf[1],buf[5]);
    return RF_FSM_CMD_SLEEP;
}

#define SLORT_CONT_8S_TIME        (8*100)
#define SLORT_CONT_6S_TIME        (6*100)
RF_CMD_T set_get_frame1_fun(UINT8 *buf)
{

#define GRP_NETMASK_START    0x02
#define TIME_CONT_6S         (UINT16)0x258
#define TIME_CONT_8S         (UINT16)0x320
#define TIME_CONT_2S         (UINT16)0x00c8

    UINT8 mask_i=0,mask_m = 0;
    UINT16 set_slort = 0;

    mask_i =  INFO_DATA.gRFInitData.wakeup_id.id2%24/8;
    mask_m =  INFO_DATA.gRFInitData.wakeup_id.id2%24%8;

    if ( 0 == (buf[GRP_NETMASK_START+mask_i] & (0x01<<mask_m) ))
        return RF_FSM_CMD_SLEEP;
    gro_wor_times = 0; //从set层到grp唤醒前，清空grp唤醒的次数
    set_slort = (UINT16)((UINT16)(buf[0]&0x1F)*256+(UINT16)buf[1]);
    if(set_slort > SLORT_CONT_8S_TIME)
    {
        Semaphore_pend(reed_sem, 0);
        if(Semaphore_pend(reed_sem, (set_slort - SLORT_CONT_6S_TIME)*1000))
        {
            set_wor_flag_fun(RF_FSM_CMD_SET_EXIT_LOOP);
            return RF_FSM_CMD_SLEEP;
        }else
        {
            return RF_FSM_CMD_SET_WAKEUP;
        }
    }else
    {
        Semaphore_pend(reed_sem, 0);
        if(Semaphore_pend(reed_sem, set_slort*1000))
        {
            set_wor_flag_fun(RF_FSM_CMD_SET_EXIT_LOOP);
            return RF_FSM_CMD_SLEEP;
        }else
        {
            def_sys_attr.sys_def_attribute.work_time  = set_rf_work_time_fun(buf[5]);
            set_grp_wor_time_fun(set_rf_work_time_fun(buf[5]>>4));
            set_wor_flag_fun(RF_FSM_CMD_GRP_EXIT_LOOP);

            return RF_FSM_CONFG_FRAME1_PER;

        }
    }
}

RF_CMD_T set_pkg_trn_fun(UINT8 *buf)
{
#define GRP_NETMASK_START    0x02
#define TIME_CONT_6S         (UINT16)0x258
#define TIME_CONT_5S         (UINT16)0x1F4
#define TIME_CONT_8S         (UINT16)0x320
#define TIME_CONT_2S         (UINT16)0x00c8
#define TIME_CONT_1S         (UINT16)0x0064

    UINT8 mask_i=0,mask_m = 0;
    UINT16 set_slort = 0;

    mask_i =  INFO_DATA.gRFInitData.wakeup_id.id2%24/8;
    mask_m =  INFO_DATA.gRFInitData.wakeup_id.id2%24%8;

    if ( 0 == (buf[GRP_NETMASK_START+mask_i] & (0x01<<mask_m) ))
        return RF_FSM_CMD_SLEEP;

    gro_wor_times = 0; //从set层到grp唤醒前，清空grp唤醒的次数
    def_sys_attr.sys_def_attribute.work_time  = set_rf_work_time_fun(buf[5]);
    set_grp_wor_time_fun(set_rf_work_time_fun(buf[5]>>4));

    set_slort = (UINT16)((UINT16)(buf[0]&0x1F)*256+(UINT16)buf[1]);

    if(set_slort > SLORT_CONT_8S_TIME)
    {
        Semaphore_pend(reed_sem, 0);
        if(Semaphore_pend(reed_sem, (set_slort - SLORT_CONT_6S_TIME)*1000))
        {
            set_wor_flag_fun(RF_FSM_CMD_SET_EXIT_LOOP);
            return RF_FSM_CMD_SLEEP;
        }else
        {
            return RF_FSM_CMD_GRP_WAKEUP;
        }
    }else
    {
        Semaphore_pend(reed_sem, 0);
        if(Semaphore_pend(reed_sem, set_slort*1000))
        {
            set_wor_flag_fun(RF_FSM_CMD_SET_EXIT_LOOP);
            return RF_FSM_CMD_SLEEP;
        }else
        {
            return RF_FSM_CMD_GRP_WAKEUP;
        }
    }

}

RF_CMD_T set_pkg_chn_fun(UINT8 *buf)
{

    if(false == glbcmd_encryption_fun(buf,aes128.flag.aes_enable ,aes128.flag.isencryption) )
        return RF_FSM_CMD_SLEEP;

    if((aes128.flag.aes_enable == false) || (aes128.flag.isencryption == 0))//不支持加密,或者未加密
    {
        RFID tp;
        UINT16 crc =0;
        tp =INFO_DATA.gRFInitData.wakeup_id;
        tp.id2 = 0x00;
        crc= my_cal_crc16(crc,buf,4);
        crc= my_cal_crc16(crc,(UINT8 *)&tp,sizeof(RFID));
        if (0 != memcmp(buf+4,(UINT8 *)&crc,sizeof(crc)))
            return RF_FSM_CMD_SLEEP;

    }
    INFO_DATA.gRFInitData.set_wkup_ch = buf[1];
    INFO_DATA.gRFInitData.grp_wkup_ch = buf[2];
    INFO_DATA.gRFInitData.esl_data_ch = buf[3];

    save_extern_rf_info();
    return RF_FSM_CMD_SLEEP;
}

RF_CMD_T set_pkg_quick_led_page_fun(UINT8 *buf)
{
#define    SCOPE_ESL    0
#define    SCOPE_GUP    1
#define    SCOPE_SET    2
#define    CHOOSE_LED       0
#define    CHOOSE_PAGE      1
#define    CHOOSE_LED_PAGE  2
#define    T0_COUNT   1
#define    T0_TIME    1          //30ms为单位
#define    T1_TIME    1          //30ms为单位
#define    T2_TIME    40         //30ms为单位

    RFID tp;
    UINT8 temp=0;
    UINT16 crc =0;
    quick_led_page_t led_page;

    memcpy((UINT8 *)&led_page, buf,sizeof(quick_led_page_t));

    switch(led_page.scope)
    {
    case SCOPE_SET:
        tp =INFO_DATA.gRFInitData.wakeup_id;
        tp.id2 = 0x00;
        crc= my_cal_crc16(crc,buf,4);
        crc= my_cal_crc16(crc,(UINT8 *)&tp,sizeof(RFID));

        if (0 != memcmp(buf+4,(UINT8 *)&crc,sizeof(crc)))
            return RF_FSM_CMD_SLEEP;
        break;
    case SCOPE_GUP:
        tp =INFO_DATA.gRFInitData.wakeup_id;
        crc= my_cal_crc16(crc,buf,4);
        crc= my_cal_crc16(crc,(UINT8 *)&tp,sizeof(RFID));

        if (0 != memcmp(buf+4,(UINT8 *)&crc,sizeof(crc)))
            return RF_FSM_CMD_SLEEP;
        break;
    case SCOPE_ESL:
        tp =INFO_DATA.gRFInitData.esl_id;
        crc= my_cal_crc16(crc,buf,4);
        crc= my_cal_crc16(crc,(UINT8 *)&tp,sizeof(RFID));
        if (0 != memcmp(buf+4,(UINT8 *)&crc,sizeof(crc)))
            return RF_FSM_CMD_SLEEP;
        break;
    default:
        return RF_FSM_CMD_SLEEP;
    }

    switch(led_page.choose_led_page)
    {

    case CHOOSE_PAGE:
        temp = ((GLO_PAGE_FLAG)|(led_page.page_time <<3)|(led_page.page_id));
        globle_cmd_page_change_fun(temp);
        fast_refresh_flag=1;                 //这里使用快速切页
        break;
    case CHOOSE_LED_PAGE:
        temp = ((GLO_PAGE_FLAG)|(led_page.page_time <<3)|(led_page.page_id));
        globle_cmd_page_change_fun(temp);
        fast_refresh_flag=1;                 //这里使用快速切页
    case CHOOSE_LED:
        if(led_page.led_flag == 1)        //1属性包闪灯，0固定方式闪灯
        {
            temp = (led_page.led_cycle ? GLO_LED_ON:GLO_LED_OFF);
            globle_cmd_ledconf_fun(temp);
        }
        if(led_page.led_flag == 0)        //1属性包闪灯，0固定方式闪灯
        {
            if(led_page.led_cycle == 0)
            {
                globle_cmd_ledconf_fun(GLO_LED_OFF);
                return RF_FSM_CMD_SLEEP;
            }else
            {
                led_tp_init_fun(1);
                led_tp.led_num = led_page.led_color;
                led_tp.t0_cont = T0_COUNT;
                led_tp.t0_time = T0_TIME;
                led_tp.t1_time = T1_TIME;
                led_tp.t2_time = T2_TIME;
                led_tp.period  = led_page.led_cycle;
                if(led_flash_flag == TRUE)
                {
                    Semaphore_post(led_off_sem);
                    Semaphore_post(led_flash_sem);
                }else
                {
                    Semaphore_post(led_flash_sem);
                }
            }
        }
        break;
    default:
        break;
    }
    return RF_FSM_CMD_SLEEP;
}

RF_CMD_T set_pkg_uplink_ctrl_fun(UINT8 *buf)
{
#define CTRL_SUBSET_MASK            0x0f
#define POWER_CONTROL_BIT           0x01
#define CHANNEL_CONTROL_BIT         0x02
#define POWER_CHANNEL_CONTROL_BIT   0x03

    RFID tp;
    UINT16 crc =0;
    uplink_ctrl_t uplink_ctrl_framer;
    tp =INFO_DATA.gRFInitData.wakeup_id;
    tp.id2 = 0x00;
    crc= my_cal_crc16(crc,buf,4);
    crc= my_cal_crc16(crc,(UINT8 *)&tp,sizeof(RFID));

    if (0 != memcmp(buf+4,(UINT8 *)&crc,sizeof(crc)))
        return RF_FSM_CMD_SLEEP;

    memcpy((UINT8 *)&uplink_ctrl_framer, buf,sizeof(uplink_ctrl_t));

    if((uplink_ctrl_framer.ctrl_1 & CTRL_SUBSET_MASK) == POWER_CONTROL_BIT)
    {
        def_sys_attr.power_bottom = uplink_ctrl_framer.power_bottom;
        def_sys_attr.power_ceiling = uplink_ctrl_framer.power_ceiling;
    }
    if((uplink_ctrl_framer.ctrl_1 & CTRL_SUBSET_MASK) == CHANNEL_CONTROL_BIT)
    {
        def_sys_attr.uplink_defalut_channel = uplink_ctrl_framer.uplink_default_channel;
    }
    if((uplink_ctrl_framer.ctrl_1 & CTRL_SUBSET_MASK) == CHANNEL_CONTROL_BIT)
    {
        def_sys_attr.power_bottom = uplink_ctrl_framer.power_bottom;
        def_sys_attr.power_ceiling = uplink_ctrl_framer.power_ceiling;
        def_sys_attr.uplink_defalut_channel = uplink_ctrl_framer.uplink_default_channel;
    }
    if((MAX_POWER_CEILING_VALUE < def_sys_attr.power_bottom)||(def_sys_attr.power_ceiling > MAX_POWER_CEILING_VALUE))
    {
        def_sys_attr.power_bottom = MIN_POWER_BOTTOM_VALUE;
        def_sys_attr.power_ceiling = MAX_POWER_CEILING_VALUE;
    }
    save_sys_config_info();
    return RF_FSM_CMD_SLEEP;
}

RF_CMD_T set_broadcast_fun(UINT8 *buf)
{
    UINT16 set_slort;
    set_slort = (UINT16)((UINT16)(buf[0]&0x1F)*256+(UINT16)buf[1]);
    gro_wor_times = 0; //从set层到grp唤醒前，清空grp唤醒的次数
    def_sys_attr.sys_def_attribute.work_time  = set_rf_work_time_fun(buf[5]);
    if(set_slort > SLORT_CONT_8S_TIME)
    {
        Semaphore_pend(reed_sem, 0);
        if(Semaphore_pend(reed_sem, (set_slort - SLORT_CONT_6S_TIME)*1000))
        {
            set_wor_flag_fun(RF_FSM_CMD_SET_EXIT_LOOP);
            return RF_FSM_CMD_SLEEP;
        }else
        {
            return RF_FSM_CMD_SET_WAKEUP;
        }
    }else
    {
        Semaphore_pend(reed_sem, 0);
        if(Semaphore_pend(reed_sem, set_slort*1000))
        {
            set_wor_flag_fun(RF_FSM_CMD_SET_EXIT_LOOP);
            return RF_FSM_CMD_SLEEP;
        }else
        {
            return RF_FSM_CONFG_FRAME2_PER;
        }
    }
}

RF_CMD_T set_ctrl_err_fun(UINT8 *buf)
{

    return RF_FSM_CMD_SLEEP;
}

RF_CMD_T get_data_query_fun(UINT8 *buf)
{
    ret_ack_flag = RF_EVENT_QUERY;
    memcpy((UINT8 *) &g_upgread_query_data,buf,sizeof(g_upgread_query_data));
    return RF_FSM_CMD_RX_DATA;
}
RF_CMD_T get_netlink_fun(UINT8 *buf)
{
    ret_ack_flag = RF_EVENT_NETLINK;
    memset((UINT8 *)&netlink_info,0x00,sizeof(netlink_info));
    memcpy((UINT8 *)&netlink_info,buf,sizeof(netlink_info));

    Event_post(protocol_eventHandle, EVENT_FLAG_NETLINK);

    return RF_FSM_CMD_RX_DATA;
}

enum {END_PACKET = 0x03, NORMAL_PACKET = 0x00};
bool check_pkg_validity(UINT8 pkg)
{
    if(pkg != END_PACKET && pkg != NORMAL_PACKET )
    {
        return FALSE;
    }
    return TRUE;
}

UINT8 packet_bitmap(UINT16 packet_num,file_id_t file_id)
{
#ifdef PGK_BIT_FLASH_OPEN
    UINT16 packet_byte = 0,offset = 0;
    UINT8 packet_bit = 0,des = 0xff,tp=0;


    packet_num -=1;
    packet_byte = packet_num / 8;
    packet_bit  = (UINT8)(packet_num % 8);
    offset += packet_byte;

    f_read(file_id,(WORD)offset,&des,sizeof(des));
    if( (des & (0x01<<packet_bit)) == 0)
        return 0;
    tp = (0x01<<packet_bit);
    tp = ~tp;
    des &= tp;
    f_write_direct(file_id,offset,&des,sizeof(des));

    return 1;
#else
    UINT16 packet_byte = 0,offset = 0;
    UINT8 packet_bit = 0,tp=0;

    packet_num -=1;
    packet_byte = packet_num / 8;
    packet_bit  = (UINT8)(packet_num % 8);
    offset += packet_byte;
    if(offset >= G_PKG_BIT_MAP_LEN)
    {
        gerr_info = TR3_OSD_WPKG_BIT_ERR;
        return 0;
    }
    if((pkg_bit_map[offset] & (0x01<<packet_bit)) == 0)
        return 0;
    tp = (0x01<<packet_bit);
    tp = ~tp;
    pkg_bit_map[offset] &= tp;
    return 1;

#endif

}
RF_CMD_T get_osd_broadcast_pkg_fun(UINT8 *buf)
{
    get_broadcast_fun(buf);
    ret_ack_flag = RF_EVENT_OSD_BROADCAST_PKG;
    return RF_FSM_CMD_RX_DATA;
}

RF_CMD_T get_osd_updata_fun(UINT8 *buf)
{
    esl_frame_t osd_data;
    UINT8 pkg_tpye = 0;

    memcpy((UINT8 *) &osd_data,buf,RF_NORMAL_PACKET);

    if( 0 == (buf[0] & OSD_SID_FALG ))    //osd更新sid不能是0
        return RF_FSM_CMD_RX_DATA;
    gsys_reset_flag = false;
    if(( gsid.old_sid != gsid.now_sid)||(( gsid.old_sid == gsid.now_sid)&&(gsid.old_sub_sid != gsid.now_sub_sid)))
    {
        grst_sid_time = 0;
        sid_change_eraflag();
        my_ack_buf.battery = gelectric_quantity;
        my_ack_buf.receive_pkg_sum = 0;
        my_ack_buf.finish_pkg_num = 0;
        my_ack_buf.query_pkg_num = 0;
        uprom_tp.sys_update_success_flag = 0;
    }

    pkg_tpye = (osd_data.package_num & RF_PKG_TYPE)>>14;

    if(TRUE != check_pkg_validity(pkg_tpye))
        return RF_FSM_CMD_RX_DATA;

    osd_data.package_num = (osd_data.package_num &0x3fff);
#ifndef PGK_BIT_FLASH_OPEN
    if(osd_data.package_num > (G_PKG_BIT_MAP_LEN * 8))
    {
        gerr_info = TR3_MAX_PKG_ERR;
        return RF_FSM_CMD_RX_DATA;
    }
#endif
    switch (pkg_tpye)
    {
    case END_PACKET:

        gosd_pkg.finish_pkg_num = osd_data.package_num;

    case NORMAL_PACKET:

        read_pkg_fileid = write_pkg_buff_id;
        if( packet_bitmap(osd_data.package_num, write_pkg_buff_id))
        {
            gosd_pkg.current_pkg_num += 1;
        }

        my_ack_buf.receive_pkg_sum = gpkg.pkg_num;
        my_ack_buf.finish_pkg_num = gpkg.osd_pkg_all;
        ret_ack_flag = RF_EVENT_OSD;
        if(upgrade_status_flag == TRUE)
        {
            upgread_id = 0;
            upgrade_status_flag = FALSE;
            gerr_info = TR3_OSD_WPKG_BIT_ERR;
            eraser_file_flag = 0xFF;
            Event_post(protocol_eventHandle, EVENT_FLAG_ERASER_BUFF);
            break;
        }
        if(DATA_SIZE != f_write_direct(write_temp_buff_id,(WORD)((WORD)(osd_data.package_num -1)* (WORD)DATA_SIZE + cmd_start_offset),osd_data.data,DATA_SIZE))
        {
            gerr_info = TR3_OSD_WPKG_BIT_ERR;
            break;
        }

        Event_post(protocol_eventHandle, EVENT_FLAG_OSD_ANALUSIS);

        break;
    default:
        return RF_FSM_CMD_RX_DATA;
        //break;
    }
    return RF_FSM_CMD_RX_DATA;
}

RF_CMD_T get_broadcast_fun(UINT8 *buf)
{
    esl_up_data_t up_data;
    UINT8 pkg_tpye = 0;

    memcpy((UINT8 *) &up_data,buf,RF_NORMAL_PACKET);
    if( upgread_id != (buf[0] & OSD_SID_FALG ))    //upgread_id不一致，就擦除flash，准备接收新一次的升级数据包
    {
        upgrade_status_flag = TRUE;
        fs_erase_all_file();
        memset((UINT8 *)gdsi_info_st,0xFF,sizeof(G_DIS_PAGE_T)*MAX_DISPLAY_PGAE_SUM);
        memset(gSys_tp.page_map,0xff,8);//清空页码
        upgread_id_change_eraflag();
        upgread_id = (buf[0] & OSD_SID_FALG);
    }

    pkg_tpye = (up_data.package_num & RF_PKG_TYPE)>>14;
    if(TRUE != check_pkg_validity(pkg_tpye))
        return RF_FSM_CMD_BROADCAST_RX_DATA;
    up_data.package_num = (up_data.package_num &0x3fff);

#ifndef PGK_BIT_FLASH_OPEN                             //对最大包数做判断
    if(up_data.package_num > (G_PKG_BIT_MAP_LEN * 8))
    {
        gerr_info = TR3_MAX_PKG_ERR;
        return RF_FSM_CMD_BROADCAST_RX_DATA;
    }
#endif

    switch (pkg_tpye)
    {
    case END_PACKET:
        g_upgread_pkg.finish_pkg_num = up_data.package_num;      //记录升级总包数升级总包数
        receive_finish_pkg_flag = TRUE;

    case NORMAL_PACKET:
        if(packet_bitmap(up_data.package_num, write_pkg_buff_id))
        {
            g_upgread_pkg.current_pkg_num += 1;

#ifdef HAVE_EX_FLASH
        if(DATA_SIZE != f_write_direct(write_temp_buff_id,(WORD)((WORD)(up_data.package_num -1)* (WORD)DATA_SIZE + cmd_start_offset),up_data.data,DATA_SIZE))
        {
            gerr_info = TR4_UPGREAD_WPKG_ERR;
            break;
        }

#else
        rom_segment_write((WORD)((WORD)(up_data.package_num -1)* (WORD)DATA_SIZE + EF_STAA_DDR),(WORD)up_data.data,(WORD)DATA_SIZE);

#endif
        }
        ret_ack_flag = RF_EVENT_BROADCAST_UPDATA;
        break;
    default:
        return RF_FSM_CMD_BROADCAST_RX_DATA;
        //        break;
    }
    if((g_upgread_pkg.current_pkg_num == g_upgread_pkg.finish_pkg_num)&&(receive_finish_pkg_flag == TRUE))
    {
        receive_finish_pkg_flag = FALSE;
        Event_post(protocol_eventHandle, EVENT_FLAG_UPDATA_ROM);  //收到升级数据包以后的流程待定
    }

    return RF_FSM_CMD_BROADCAST_RX_DATA;
}

RF_CMD_T get_osd_128cmd_fun(UINT8 *buf)
{
    ret_ack_flag = RF_EVENT_OSD_128CMD;
    // gpkg.pkg_num  = 1;
    eraser_file_flag = buf[5]; //osd2.0  128命令后面的字节：1表示全擦，2：擦除存储区 4：擦除页数字区
    //osd4.0  128命令后面的字节：每1bit表示擦除一页，0xFF表示擦除全部的文件系统
    Event_post(protocol_eventHandle, EVENT_FLAG_ERASER_BUFF);
    return RF_FSM_CMD_RX_DATA;
}

bool check_link_query_fun(UINT8 *buf)
{
#define CHECK_CTRL_ADDR 0x04
#define CHECK_OSD_TP_ADDR 0x05
    RF_EVENT_T tp;


    switch(buf[CHECK_CTRL_ADDR]&(UINT8)SET_CTRL_INFO)
    {
    case CTRL_QUERY:
        tp = RF_EVENT_QUERY;
        break;
    case CTRL_NETLINK:
        tp = RF_EVENT_NETLINK;
        break;
    case CTRL_OSD:
        if(TRUE == check_128_fun(buf,CHECK_OSD_TP_ADDR))
            tp = RF_EVENT_OSD_128CMD;
        else if(TRUE == check_0x76_fun(buf,CHECK_OSD_TP_ADDR) )
            tp = RF_EVENT_OSD_76CMD;
        else
            tp= RF_EVENT_OSD;
        break;
    case CTRL_BROADCAST_UPDATA:
        tp = RF_EVENT_OSD_BROADCAST_PKG;
        break;
    case CTRL_LINK_QUERY:
        tp = RF_EVENT_LINK_QUERY;
        break;
    case CTRL_RC_PKG:
        tp = RF_EVENT_RC_INFO;
        break;
    case CTRL_SLEEP:
        //缺少一个属性包和led灯的拣货包
    default:
        tp =  RF_EVENT_UNKNOWN;
        return FALSE;
    }
    if(ret_ack_flag != tp)
    {
        return FALSE;
    }
    return TRUE;
}

RF_CMD_T get_link_query_fun(UINT8 *buf)
{

    if( gerr_info > (GERR_T)AES_ACK_ERR_RF_TIMEOUT_CLEAN_FLAG)
    {
        gpkg.ctrl = buf[0];
        gpkg.sroft= buf[1];
        ret_ack_flag = RF_ENENT_KEY_ACK;
        return RF_FSM_EVENT_TX_ACK_HANDLE;

    }


    if(check_link_query_fun(buf) == FALSE)
        return RF_FSM_CMD_RX_DATA;

    gpkg.ctrl = buf[0];
    gpkg.sroft= buf[1];
    gpkg.pkg_sum =(UINT16)buf[2] + (UINT16)(buf[3]*256);

    switch(ret_ack_flag)
    {
    case RF_EVENT_QUERY:
    case RF_EVENT_NETLINK:
        return RF_FSM_EVENT_TX_ACK_HANDLE;
    case RF_EVENT_OSD:
        my_ack_buf.query_pkg_num = gpkg.pkg_sum;
        return RF_FSM_EVENT_TX_ACK_HANDLE;
    case RF_EVENT_OSD_BROADCAST_PKG:
        //        my_ack_buf.query_pkg_num = gpkg.pkg_sum;
        //        return RF_FSM_EVENT_TX_ACK_HANDLE;
        return RF_FSM_EVENT_TX_ACK_HANDLE;
    case RF_EVENT_OSD_128CMD:
        //  if(TRUE != all_pkg_cmp_fun())
        //      return RF_FSM_CMD_RX_DATA;
        if(sid_cmpare(buf[0] & OSD_SID_FALG))
            return RF_FSM_EVENT_TX_ACK_HANDLE;
        else
            return RF_FSM_CMD_RX_DATA;
    case RF_EVENT_OSD_76CMD:
        // if(TRUE != all_pkg_cmp_fun())
        //     return RF_FSM_CMD_RX_DATA;
        if(sid_cmpare(buf[0] & OSD_SID_FALG))
        {
            gsid.old_sid = 0;
            return RF_FSM_EVENT_TX_ACK_HANDLE;
        }
        else
            return RF_FSM_CMD_RX_DATA;
    case RF_EVENT_RC_INFO:
        return RF_FSM_EVENT_TX_ACK_HANDLE;
    default:
        clear_gpkg_fun();
        gsid.old_sid  = 0;
        gsid.old_sub_sid = 0;
        return RF_FSM_CMD_RX_DATA;
    }
    //return RF_FSM_EVENT_TX_ACK_HANDLE;
}

RF_CMD_T get_rc_pkg_fun(UINT8 *buf)
{
    grc_err = NONEERR_1;
    //    gpkg.pkg_num = 1;
    ret_ack_flag = RF_EVENT_RC_INFO;
    memcpy((UINT8 *)&rc_attr_info,buf,sizeof(rc_attr_info));
    if((0 != memcmp(rc_attr_info.secur_code, epd_attr_info.secur_code,2)))
    {
        grc_err = RC_CODE_ERR;
        goto loop;
    }

    rc_led_init();
    rc_store_inio_flag =  TRUE;

    if(rc_attr_info.page_num != gSys_tp.page_map[rc_attr_info.page_num])
    {
        grc_err = PAGE_ID_ERR2;
        goto loop;
    }
    if(rc_attr_info.display_time)
    {
        gpage.flag = FALSE;//停止之前的计数
        //        if(gchange_page_flag == TRUE)    //如果正处于切页状态，先把定时器停掉
        //        {
        //            stop_cut_page_timer();
        //        }
        gSys_tp.present_page_id = rc_attr_info.page_num;
        gSys_tp.stay_time = rc_attr_info.display_time;
        gSys_tp.stay_time_cont = 0;
        Event_post(protocol_eventHandle, EVENT_FALG_DISPLAY_PAGE);
        fast_refresh_flag=1;
        gchange_page_flag = TRUE;
    }
    else
    {
        rc_attr_info.page_num = gSys_tp.present_page_id;
        rc_attr_info.display_time= gSys_tp.stay_time;
    }

    loop:

    return RF_FSM_CMD_RX_DATA;

}

RF_CMD_T tx_exit_fun( void)
{
    rc_wor_times= 0;
    if(def_sys_attr.gwor_flag_now == RF_FSM_CMD_UPLINK_EXIT_LOOP)
    {
        return RF_FSM_CMD_UPLINK_EXIT_LOOP;

    }else if(def_sys_attr.gwor_flag_before == RF_FSM_CMD_GRP_EXIT_LOOP)
    {
        set_wor_flag_fun(RF_FSM_CMD_GRP_EXIT_LOOP);
        return RF_FSM_CMD_GRP_EXIT_LOOP;

    }
    else
    {
        def_sys_attr.gwor_flag_now =RF_FSM_CMD_SET_EXIT_LOOP;
        return RF_FSM_CMD_SET_EXIT_LOOP;
    }
}
RF_CMD_T get_sleep_fun(UINT8 *buf)
{
    return tx_exit_fun();
}

UINT8  set_rf_work_time_fun(UINT8 buf)
{
    UINT8 ret = 4;
#define RF_WORK_1S      0x00
#define RF_WORK_2S      0x01
#define RF_WORK_4S      0x03
#define RF_WORK_6S      0x09
#define RF_WORK_8S      0x0a


    switch(buf&0x0f)
    {
    case RF_WORK_1S:
        ret = 1;
        break;
    case RF_WORK_2S:
        ret = 2;
        break;
    case RF_WORK_4S:
        ret = 4;
        break;
    case RF_WORK_6S:
        ret = 6;
        break;
    case RF_WORK_8S:
        ret = 8;
        break;
    default :
        ret = 4;

    }
    return ret;
}

bool sid_cmpare(UINT8 tp)
{
    if(tp == gsid.now_sid )
        return  TRUE;

    if(gerr_info != 0)
        return  TRUE;

    return  FALSE;
}
file_id_t swp_pkg(file_id_t src)
{
    if(src == F_BMP_PKG_1)
        return F_BMP_PKG_2;
    if(src == F_BMP_PKG_2)
        return F_BMP_PKG_1;
    return F_BMP_PKG_1;

}

UINT16 lose_pkg_statistics(UINT16 pkg_all,file_id_t readid,UINT8 *buf)
{
#ifdef PGK_BIT_FLASH_OPEN
    UINT16 j,all_pkg_byte,lose, n;
    UINT8 temp[64],i, read_byte,k, len = 0;

    all_pkg_byte = ( (pkg_all/8) +1);
    for( j = ((first_lose_pkg)/8>0)?(first_lose_pkg)/8 -1:0; j < all_pkg_byte ; j+= read_byte)
    {

        if(all_pkg_byte - j >= 64)
            read_byte = 64;
        else
            read_byte = all_pkg_byte - j;

        f_read(readid,j,temp,read_byte);

        for(i = 0 ; i < read_byte ; i++)
        {
            if(temp[i] == 0x00)
                continue;

            for(k = 0; k < 8; k++)
            {
                n = (j + i) * 8 + k;
                if(n == pkg_all)
                    goto End;
                if((temp[i] & ( 0x01<<k)) == 0)
                    continue;
                if(len == 20)
                    goto End;

                lose =  n +1;
                buf[len++] = lose & 0xff;
                buf[len++] = (lose >> 8) & 0xff;
            }
        }

    }

    End:
    if (len == 0) {
        first_lose_pkg = pkg_all;
    } else {
        first_lose_pkg = buf[0] + buf[1]*256;
    }

    return len /2;
#else
    UINT16 i,j,all_pkg_byte,lose, n;
    UINT8  k, len = 0;

    all_pkg_byte = ( (pkg_all/8) +1);
    j = ((first_lose_pkg)/8>0)?(first_lose_pkg)/8 -1:0;//第一个丢包的起始字节

    for(i = j ; i < all_pkg_byte ; i++)
    {
        if(pkg_bit_map[i] == 0x00)
            continue;

        for(k = 0; k < 8; k++)
        {
            n = i * 8 + k;
            if(n == pkg_all)
                goto End;
            if((pkg_bit_map[i] & ( 0x01<<k)) == 0)
                continue;
            if(len == 20)
                goto End;

            lose =  n +1;
            buf[len++] = lose & 0xff;
            buf[len++] = (lose >> 8) & 0xff;
        }
    }
    End:
    if (len == 0) {
        first_lose_pkg = pkg_all;
    } else {
        first_lose_pkg = buf[0] + buf[1]*256;
    }

    return len /2;
#endif
}

UINT16 broadcast_upgread_lose_pkg_statistics(UINT16 pkg_all,file_id_t readid,UINT8 *buf)
{
#ifdef PGK_BIT_FLASH_OPEN

    UINT16 all_pkg_byte,pkg_bit,pkg_byte,i=0,j=0;
    UINT8  temp,ret=0;
    UINT8  check_buf[64] = {0};     //64个字节最大支持81920个包
    UINT8  temp_buf[64];
    if((g_upgread_query_data.min_loss_subset % 8)||(g_upgread_query_data.min_loss_subset < 8))
        return 1;

    if(pkg_all < g_upgread_query_data.min_loss_subset)
        goto here;

    all_pkg_byte = pkg_all/g_upgread_query_data.min_loss_subset;
    for(i=0,j=0;i<all_pkg_byte;i++,j++)
    {
        f_read(readid,i*(g_upgread_query_data.min_loss_subset/8),temp_buf,g_upgread_query_data.min_loss_subset/8);
        if(0 == memcmp(temp_buf, check_buf,g_upgread_query_data.min_loss_subset/8))
        {
            temp = 0x00;                                                  //无丢包相应bit置0
        }
        else
        {
            ret = 1;
            temp = 0x01;                                                  //有丢包相应bit置1
        }
        buf[j/8] = (buf[j/8] | (temp<<(j%8)));
    }
    here:
    if(pkg_all % (g_upgread_query_data.min_loss_subset))     //字节未对齐的包号校验
    {
        temp = 0x00;                                         //先假设最后的1bit没有丢包
        pkg_byte = (pkg_all % (g_upgread_query_data.min_loss_subset))/8;
        pkg_bit  = (pkg_all % (g_upgread_query_data.min_loss_subset))%8;
        if(pkg_byte)
        {
            f_read(readid,i*(g_upgread_query_data.min_loss_subset/8),temp_buf,pkg_byte);
            if(0 != memcmp(temp_buf, check_buf, pkg_byte))
            {
                ret = 1;
                temp = 0x01;
                buf[j/8] = (buf[j/8] | (temp<<(j%8)));
            }
        }
        while(pkg_bit)
        {
            f_read(readid,i*(g_upgread_query_data.min_loss_subset/8)+pkg_byte,temp_buf,1);
            if(temp_buf[0] & (0x01<< pkg_bit))
            {
                ret = 1;
                temp = 0x01;
            }
            pkg_bit--;
        }
        buf[j/8] = (buf[j/8] | (temp<<(j%8)));
    }
    return ret;

#else
    UINT16 all_pkg_byte,pkg_bit,pkg_byte,i=0,j=0;
    UINT8  temp;
    //    UINT8  check_buf[g_upgread_query_data.min_loss_subset/8] = {0};
    UINT8  check_buf[128] = {0};     //128个字节最大支持163840个包

    if((0 != g_upgread_query_data.min_loss_subset%8)||(g_upgread_query_data.min_loss_subset < 8))
        return 1;

    if(pkg_all < g_upgread_query_data.min_loss_subset)
        goto here;

    all_pkg_byte = pkg_all/g_upgread_query_data.min_loss_subset;
    for(i=0,j=0;i<all_pkg_byte;i++,j++)
    {
        if(0 == memcmp(&pkg_bit_map[i*(g_upgread_query_data.min_loss_subset/8)], check_buf,g_upgread_query_data.min_loss_subset/8))
        {
            temp = 0x01;                                                  //无丢包相应bit置0
        }
        else
        {
            temp = 0x00;                                                  //有丢包相应bit置1
        }
        buf[j/8] = (buf[j/8] & (~(temp<<(j%8))));
    }
    here:
    if(pkg_all % (g_upgread_query_data.min_loss_subset))     //字节未对齐的包号校验
    {
        temp = 0x01;                                         //先假设最后的1bit没有丢包
        pkg_byte = (pkg_all % (g_upgread_query_data.min_loss_subset))/8;
        pkg_bit  = (pkg_all % (g_upgread_query_data.min_loss_subset))%8;
        if(pkg_byte)
        {
            if(0 != memcmp(&pkg_bit_map[i*(g_upgread_query_data.min_loss_subset/8)], check_buf, pkg_byte))
            {
                temp = 0x00;
                buf[j/8] = (buf[j/8] & (~(temp<<(j%8))));
            }
        }
        temp = 0x01;
        while(pkg_bit)
        {
            if(pkg_bit_map[i*(g_upgread_query_data.min_loss_subset/8)+pkg_byte] & (0x01<< pkg_bit))
            {
                temp = 0x00;
            }
            pkg_bit--;
        }
        buf[j/8] = (buf[j/8] & (~(temp<<(j%8))));
    }

    return 1;

#endif
}

void led_tp_init_fun(UINT8 ledtp_flag)
{
    if(ledtp_flag)//属性包
    {
        led_tp.t0_cont =  epd_attr_info.t0_cont;
        memcpy((UINT8 *)&led_tp.led_num,(UINT8 *)&epd_attr_info.led_num,7);
    }
    else
    {
        memcpy((UINT8 *)&led_tp.t0_cont,(UINT8 *)&rc_attr_info.t0_cont,8);
        led_tp.led_map[0] = rc_attr_info.led_map[0];
        led_tp.led_map[1] = rc_attr_info.led_map[1];
        led_tp.led_map[2] = rc_attr_info.led_map[2];
        led_tp.led_map[3] = rc_attr_info.led_map[3];
    }

    if(led_tp.t0_cont ==0)
        led_tp.t0_cont = 1;
    led_tp.t0_cont = (led_tp.t0_cont - 1)/2;     //为了和之前的平台闪灯次数吻合

    if(led_tp.t0_time ==0)
        led_tp.t0_time=1;
    if(led_tp.t1_time ==0)
        led_tp.t1_time=1;
    if(led_tp.t2_time ==0)
        led_tp.t2_time=1;
}

