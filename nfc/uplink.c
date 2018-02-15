
#include "uplink.h"
#include "rf.h"

UINT8 g_uplink_status = 0;
UINT8 uplink_buf[26];
UINT8 test_buf[16]={0x32,0x33,0x34,0x35,0x36,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};


void creat_uplink_request_pkg(UINT8 request_type, UINT8 *buf)
{
#define UPLINK_CTRL0_INFO  0xf0
#define UPLINK_CTRL0       0x70
#pragma pack(1)
    struct uplink_data_buf {
        UINT8 ctrl_0;
        UINT8 esl_id[4];
        UINT8 session_id;
        UINT8 default_chn;
        UINT8 REQ_type;
        UINT8 REQ_content[16];
        UINT16 crc;
    }uplink_data_buf;
#pragma pack()

    uplink_data_buf.ctrl_0 = UPLINK_CTRL0;
    memcpy(uplink_data_buf.esl_id,(UINT8 *)&INFO_DATA.gRFInitData.esl_id,4);
    uplink_session_id = ((uplink_session_id++)>=255)?1:uplink_session_id;
    uplink_data_buf.session_id = uplink_session_id;
    uplink_data_buf.default_chn = INFO_DATA.gRFInitData.set_wkup_ch;

    uplink_data_buf.REQ_type = request_type;
    memcpy(uplink_data_buf.REQ_content, buf, 16);

    uplink_data_buf.crc = my_cal_crc16(0,(UINT8 *)&uplink_data_buf,24);
    uplink_data_buf.crc  = my_cal_crc16(uplink_data_buf.crc ,(UINT8 *)&INFO_DATA.gRFInitData.esl_id,4);
    memcpy(uplink_buf,(UINT8 *)&uplink_data_buf, sizeof(uplink_data_buf));

}

void tx_uplink_request_pak(void)
{
    UINT32 id_temp;

    id_temp = rfid_to_u32(INFO_DATA.gRFInitData.master_id);
    set_rf_parameters(DATA_RATE_100K, ack_power_param, 2400+def_sys_attr.uplink_defalut_channel/2, def_sys_attr.uplink_defalut_channel%2);
    RF_EventMask result = Rf_tx_package(rfHandle, id_temp, UPLINK_DATA_LEN, (UINT8 *)uplink_buf);
    if(!(result & RF_EventLastCmdDone))
    {
        while(1);
    }
}

UINT8 uplink_request_ack_analysis(UINT8 *buf)
{
    uint16_t crc;
    if((buf[0] & UPLINK_CTRL0_INFO) != UPLINK_CTRL0)
        return CTRL_0_ERR;

    crc = ack_crc_fun(buf,DOWNLINK_ACK_PACKET);
    if (0 != memcmp(buf+7,(uint8_t *)&crc,sizeof(crc)))
        return CRC_ERR;

    if(buf[5] != uplink_session_id)
        return SESSION_ID_ERR;

    return UPLINK_ACK_OK;
}

void uplink_request_ack_event(uint8_t tp)
{
    g_uplink_status = tp;
    switch(tp)
    {
    case UPLINK_OK:

        select_close_timer();
        def_sys_attr.gwor_flag_now = RF_FSM_CMD_UPLINK_EXIT_LOOP;
        Event_post(protocol_eventHandle, EVENT_FLAG_RFWORK);
        set_uplink_timeout(180);   //上行链路超时最长3分钟
        uplink_wor_times= 0;
        break;

    case AP_OFFLINE:
        LED_BLUE_ON;
        Task_sleep(3000);
        LED_BLUE_OFF;
        break;
    default:
        break;
    }
}

void uplink_request_and_rx_ack(void)
{
#define RETRY_TIME                       10
#define UPLINK_ACK_STATUS_OFFSET         6
    uint8_t i, err, uplink_ack_buf[26];

    ack_power_param = def_sys_attr.power_bottom;  //第一次请求时，设置发射功率为最低档
    for(i=0;i<RETRY_TIME;i++)
    {
        tx_uplink_request_pak();
        ack_power_param++;
        ack_power_param = (ack_power_param > def_sys_attr.power_ceiling)?def_sys_attr.power_ceiling:ack_power_param;
//        tx_uplink_request_pak();
        err = downlink_rx(uplink_ack_buf);
        if (err == RF_ERROR_NONE)
        {
            err = uplink_request_ack_analysis(uplink_ack_buf);
            if(err == UPLINK_ACK_OK)
            {
                uplink_request_ack_event(uplink_ack_buf[UPLINK_ACK_STATUS_OFFSET]);
                break;
            }
        }
    }
    if(i >= RETRY_TIME)    //未正确收到上行请求的ACK
    {
        LED_RED_ON;
        Task_sleep(3000);
        LED_RED_OFF;
    }
}


