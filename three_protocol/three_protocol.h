#ifndef _THREE_PROTOCOL_H_
#define _THREE_PROTOCOL_H_

#include <stdint.h>

#define RF_SET_WAKEUP_PACKET       0x06     //SET唤醒数据包大小
#define RF_GRP_WAKEUP_PACKET       0x02     //GRP唤醒数据包大小
#define RF_NORMAL_PACKET           0x1a         //正常数据包大小
#define DOWNLINK_ACK_PACKET        0x09     //上行链路请求后ACK数据包的长度

#pragma pack(1)

typedef struct _hrb
{
    uint8_t sum;        //心跳總的次數
    uint8_t now;
    uint8_t cont;
    uint8_t hbt_cont;    //心跳間隔
}HRB_STP;
/** RF工作设置参数 */

typedef struct _rfid
{
    uint8_t id0;
    uint8_t id1;
    uint8_t id2;
    uint8_t id3;
} RFID;

typedef struct _rfinit
{
    RFID master_id;
    RFID wakeup_id;
    RFID extend_esl_id;
    RFID esl_id;
    uint8_t  set_wkup_ch;
    uint8_t  grp_wkup_ch;
    uint8_t  esl_data_ch;
    uint8_t  esl_netmask;
    uint32_t screen_id;
    uint8_t data_buffer[20];
    uint8_t key_buffer[16];
} RFINIT;

typedef struct esl_frame_t {

    uint8_t ctrl;
    uint16_t package_num;
    uint8_t addr;
    uint8_t data[20];
    uint8_t crc[2];
}esl_frame_t;

typedef struct esl_up_data_t {

    uint8_t ctrl;
    uint16_t package_num;
    uint8_t flag;
    uint8_t data[20];
    uint8_t crc[2];
}esl_up_data_t;

typedef struct upgread_query_data_t {

    uint8_t ctrl;
    uint16_t total_package_num;
    uint8_t flag;
    uint16_t package_num;
    uint8_t upgreade_rom_version;    //（查询包中数据）升级后的rom_version
    uint8_t upgreade_id;
    uint8_t min_loss_subset;
    uint16_t upgreade_firmware;      //（查询包中数据）升级后的firmware
}upgread_query_data_t;

typedef struct quick_led_page_t
{
    uint8_t scope:2;
    uint8_t led_color:3;
    uint8_t  ctrl:3;


    uint8_t led_flag:1;
    uint8_t page_id:3;
    uint8_t page_time:2;
    uint8_t choose_led_page:2;

    uint16_t led_cycle;
    uint16_t scope_crc;
}quick_led_page_t;

typedef struct uplink_ctrl_t
{
    uint8_t ctrl_1;
    uint8_t uplink_default_channel;
    uint8_t power_bottom;
    uint8_t power_ceiling;
}uplink_ctrl_t;

#pragma pack()


typedef enum{
    NORMAL_HEARTBEAT,
    REQUST_HEARTBEAT,
    STOP_HEARTBEAT
}HRB_T;

typedef enum {
    // RF_EVENT_NONE,
    RF_EVENT_GLOBLE_CMD,
    RF_EVENT_GET_FRAME1,
    RF_EVENT_PKG_TRN,
    RF_EVENT_SET_PKG_CH,
    RF_EVENT_SET_BROADCAST,
    RF_EVENT_SET_QUICK_LED_PAGE,
    RF_EVENT_SET_UPLINK,
    RF_EVENT_SET_ERR,


    RF_EVENT_QUERY,
    RF_EVENT_NETLINK,
    RF_EVENT_OSD,
    RF_EVENT_OSD_BROADCAST_PKG,
    RF_EVENT_OSD_128CMD,
    RF_EVENT_OSD_76CMD,
    RF_EVENT_LINK_QUERY,
    RF_EVENT_BROADCAST_UPDATA,
    RF_EVENT_RC_INFO,
    RF_EVENT_SLEEP,
    RF_ENENT_ACK,
    RF_ENENT_KEY_ACK,

    RF_EVENT_UNKNOWN,
} RF_EVENT_T;

typedef enum {
    RF_ERROR_NONE,
    RF_ERROR_RF_CRC,
    RF_ERROR_ENTER_TXRX,
    RF_ERROR_TX_TIMEOUT,
    RF_ERROR_RX_TIMEOUT,
    RF_ERROR_CAL,
    RF_ERROR_UNKNOWN,
} RF_ERROR_T;

typedef enum {
    RF_FSM_CMD_NONE,

    RF_FSM_CMD_RF_POR_RST,
    RF_FSM_CMD_INIT,

    RF_FSM_CMD_SET_EXIT_LOOP,
    RF_FSM_CMD_SET_WAKEUP,
    RF_FSM_CMD_SET_RX,
    RF_FSM_EVENT_SET_HANDLE,

    RF_FSM_CMD_GRP_EXIT_LOOP,
    RF_FSM_CMD_GRP_WAKEUP,
    RF_FSM_CMD_GRP_RX,

    RF_FSM_CMD_GRP_ADD_SET_RX,
    RF_FSM_EVENT_GRP_HANDLE,

    RF_FSM_CONFG_FRAME1_PER,
    RF_FSM_CMD_RX_FRAME1,

    RF_FSM_CONFG_FRAME2_PER,
    RF_FSM_CMD_RX_FRAME2,

    RF_FSM_CMD_RX_DATA_CONFIG,
    RF_FSM_CMD_RX_DATA,
    RF_FSM_EVENT_RX_DATA_HANDLE,

    RF_FSM_CMD_BROADCAST_RX_DATA_CONFIG,
    RF_FSM_CMD_BROADCAST_RX_DATA,
    RF_FSM_EVENT_BROADCAST_RX_DATA_HANDLE,

    RF_FSM_EVENT_TX_ACK_HANDLE,
    RF_FSM_CMD_TX_DATA_CONFIG,
    RF_FSM_CMD_TX_DATA,

    RF_FSM_CMD_RC_EXIT_LOOP,
    RF_FSM_CMD_RC_WAKEUP,
    RF_FSM_CMD_RC_RX,

    RF_FSM_CMD_UPLINK_EXIT_LOOP,
    RF_FSM_CMD_UPLINK_WAKEUP,
    RF_FSM_CMD_UPLINK_RX,

    RF_FSM_CMD_SLEEP,


    RF_FSM_ERROR_HANDLE,

    RF_FSM_CMD_UNKNOWN,
} RF_CMD_T;

typedef struct RF_T {
    RF_EVENT_T event;         //记录当前事件
    RF_EVENT_T prev_event;    //记录上一次事件

    RF_CMD_T prev_cmd;        //记录上一次CMD
    RF_CMD_T cmd;             //记录当前CMD
    RF_CMD_T next_cmd;        //记录下一次CMD

    RF_ERROR_T error;         //记录RF错误标识
    uint8_t ack;              //RF ACK值
} RF_T;
typedef RF_CMD_T (*rf_cmd_t) (uint8_t *buf);
int rf_state_machine(RF_T *rf);
#endif

