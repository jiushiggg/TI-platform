#ifndef __TI_CC2650_GLOBAL_VARIABLE_H__
#define __TI_CC2650_GLOBAL_VARIABLE_H__

#include <stdint.h>
#include <ti/drivers/rf/RF.h>
#include "smartrf_settings/smartrf_settings.h"
#include <stdlib.h>
#include <string.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Mailbox.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/family/arm/m3/Hwi.h>
#include <ti/drivers/PIN.h>
#include <xdc/runtime/Error.h>
#include <xdc/std.h>
#include <ti\devices\cc26x0\inc\hw_rfc_dbell.h>
#include <ti\sysbios\knl\mailbox.h>
#include <ti\sysbios\family\arm\cc26xx\timer.h>
#include "CC2640R2_LAUNCHXL.h"
#include "typedef.h"
#include "internal_flash.h"
#include "sys_init.h"
#include "rf.h"
#include "extern_flash.h"
//#include "save_sys_status.h"
#include "three_protocol.h"
#include "ti_rtos_sys.h"
#include "sys_config.h"
#include "crc16.h"
#include "led.h"
#include "heartbeat.h"
#include "three_event.h"
#include "osd_cmd.h"
#include "osd_flash.h"
#include "mini_fs_conf.h"
#include "mini_fs.h"
#include "upgread.h"
#include "reed.h"
//#include "epd_io.h"
#include "bit_display.h"

#define SCREEN_BUFF_SIZE ((UINT16)(6*1024))
#define LOAD_BUFF_SIZE ((UINT16)(2*1024))


#define EPD_3_VERSION   (131)



#define MAX_DISPLAY_PGAE_SUM  8              //最大支持8页
#define G_PKG_BIT_MAP_LEN       400          //RAM中包号存储区的大小
//#define PGK_BIT_FLASH_OPEN                 //包号存储区放在外部flash中的开关
#define MAX_POWER_CEILING_VALUE      5
#define MIN_POWER_BOTTOM_VALUE       0

#define DIRECTION_0
#define DIRECTION_90
#define DIRECTION_180
#define DIRECTION_270

#define SCREEN_ALL_NUM     22  //支持的屏幕类型总数
#define SCREEN_DSIPLAY_RELOAD_CONT  ((UINT32)86400)  //24小时



#pragma pack(1)


//typedef struct _rfid
//{
//    uint8_t id0;
//    uint8_t id1;
//    uint8_t id2;
//    uint8_t id3;
//} RFID;
//
//typedef struct _rfinit
//{
//    RFID master_id;
//    RFID wakeup_id;
//    RFID extend_esl_id;
//    RFID esl_id;
//    uint8_t  set_wkup_ch;
//    uint8_t  grp_wkup_ch;
//    uint8_t  esl_data_ch;
//    uint8_t  esl_netmask;
//    uint16_t screen_id;
//
//} RFINIT;


struct OSD_PKG_NUM_T
{
  UINT16 current_pkg_num;
  UINT16 finish_pkg_num;
};
typedef struct MsgObj {
    Int     id;
    Char    val;
} MsgObj;

typedef struct MailboxMsgObj {
    Mailbox_MbxElem  elem;      /* Mailbox header        */
    MsgObj           obj;       /* Application's mailbox */
} MailboxMsgObj;

typedef enum
{
  NONEERR_2,
  TR3_QUERY,
  TR3_NETLINK,
  TR3_OSD_ERASER,
  TR3_OSD_DATA,
  TR3_OSD_QUERY,
  TR3_RC,
  TR3_KEY_ERR,
}GPKG_T;

typedef enum
{
  NONEERR,
  TR3_PKG_ERR,
  TR2_LAYER_ID_ERR,//无此页
  TR3_OSD_CMDERR,
  TR2_CMD_CRC_ERR,
  TR3_STREAM_CRC_ERR,
  TR3_OSD_WPKG_BIT_ERR,
  TR3_OSD_SB_START_ERR,
  TR3_OSD_SB_END_ADDR_ERR,
  TR3_OSD_SB_ERR,
  TR3_SCREEN_ERR1,
  TR3_OSD_QUERY_PAGE_INFO_CRC_ERR,
  TR3_OSD_QUERY_PAGE_NUM_CRC_ERR,
  TR3_OSD_CMD6_ERR,
  TR3_OSD_QUERY_ERR,
  UPROM_ERR,
  SYS_SAVE_ERR,
  PAGE_ID_ERR,
  OSD_SID_ERR,
  TR3_MAX_PKG_ERR,
  TR3_ZIP_OSD_CMDERR,
  TR3_OSD_CMD6_DATA_CRC_ERR,
  TR3_OSD_CMD6_SCREEN_ID_RANGE_ERR,
  TR4_UPGREAD_WPKG_ERR,
  TR4_UPGREAD_QUERY_DATA_ERR,
  TR3_DEFAULT_ID_ERR,
  TR3_CORLOR_ERR,
  TR5_WRITE_FLASH_ERR,
}GERR_T;

typedef enum
{
  NONEERR_1,
  RC_CODE_ERR,
  PAGE_ID_ERR2,
}RC_ERR_T;
struct PKG_NUM_T
{
  UINT16 pkg_num;
  UINT16 pkg_sum;
  UINT16 osd_pkg_all;
  UINT8 sroft;
  UINT8 ctrl;
};

typedef struct _rf_cbrt_
{
  UINT8 cbrt;
  UINT8 ifal1;
  UINT8 vcoal;
  UINT8 vcoal1;
  UINT8 r_rcoc;
}RF_CBL;

typedef struct _screen_high_wide_t
{
    UINT16 high;
    UINT16 wide;

}SCREEN_HIGH_WIDE;

#define MAX_TEMP_BUF    64
struct PUBLIC_TEMP_T {
  union
  {
    UINT32 unuserd; //必须放在首位保证4字节对齐
    osd_sttep_t td;
    UINT8 BYTE64[MAX_TEMP_BUF];     //最大空间为128字节
    UINT8 RF_RCV[64];                       //接收BUF为64字节
    UINT8 HAFF_CODE[64];                        //HAFFMAN解码时的码元
  };
};


typedef struct{
  UINT8 ctrl;
  UINT8 package_num[2];
  RFID set_wk_id;
  RFID esl_id;
  UINT8 set_wkup_ch;
  UINT8 grp_wkup_ch;
  UINT8 esl_data_ch;
  UINT8 esl_netmask;
  UINT8 reserved[9];
  UINT16 crc;
}stNetLink;
typedef struct _osdsid
{
  UINT8 old_sid;
  UINT8 now_sid;
  UINT8 old_sub_sid;
  UINT8 now_sub_sid;
}SID_T;

typedef enum
{
  NONE_AREA,
  UP_ALL_CODE_AREA,
  UP_APP_AREA,
  UP_SCREEN_AREA,
  UP_SCHOOL_APP_AREA,
  UP_AREA_END,
}UP_AREA_T;
typedef struct _upgread_frame2_t
{
    uint8_t updata_area;
    uint8_t upgreade_id;
    uint16_t Synchronised_time;
    uint16_t upgread_pkg_all;
    uint8_t rom_version;
    uint8_t firmware_sum;

}UPGREAD_FRAME2_T;

typedef struct _rom_bin_format_t
{
    uint16_t all_bin_crc;
    uint8_t  updata_area;
    uint8_t  rom_version;
    uint8_t firmware_num;

}ROM_BIN_FORMAT_T;

typedef struct _updata_addr_format_t
{
    uint32_t start_addr;
    uint32_t end_addr;
    uint32_t rom_len;
    uint16_t rom_map_crc;

}UPDATA_ADDR_FORMAT_T;

typedef struct _rom_upgread_t
{
  UINT16 crc;
  UINT32 flag;
  UINT32 src_addr;
  UINT32 dst_addr;
  UINT32 len;
  UINT8  upgread_type;
}ROM_UP_T;

typedef struct _romupqry_t
{
  UINT8 sys_update_success_flag;
  bool  sys_update_query_flag;

}ROM_UP_QRY_T;

typedef struct _screen_t
{
  bool screen_busy_flag;
  UINT8 busy_times;
}SCREEN_T;

typedef struct _sys_init_load_t
{
  file_id_t use_page_info_fileid;
  file_id_t use_page_num_fileid;
  UINT8     default_page_id;//默认页
  UINT8     present_page_id;//要显示的页
  UINT8     gpage_nowid;//当前显示的页
  UINT8     page_map[8];
  UINT16    stay_time;
  UINT16    stay_time_cont;
  UINT8    change_map;//此变量的作用是标明8页中那个页被改变了，0表示未改变，1表示数据发送变化
 // UINT8 gflag_dispage_state; //0:空闲 1：osd 命令 2：set命令 4：rc命令
}SYS_LOAD_TYPE;

typedef struct my_test_t
{
  UINT16 srn_time;
  UINT8 battery;
  UINT8 rom_num;
  UINT32 led_num;
  GPKG_T esl_pkg_flag;
  UINT16 receive_pkg_sum;
  UINT16 finish_pkg_num;
  UINT16 query_pkg_num;
}MY_TEST_T;


typedef struct osd_crc_t
{
  UINT8 crc_cmd[2];
  UINT8 crc_layer[2];
  UINT8 crc_page_info[2];
  UINT8 crc_page_num[2];
}osd_crc_t;

typedef struct osd_cmd_t
{
  UINT32 start_addr;
  UINT32 len;
}osd_cmd_t;

typedef struct dis_page_t
{
  bool  flag;
}dis_page_t;

typedef struct glo_dis_page_t
{
  UINT8 dis_pageid;
  UINT8 default_pageid;
  UINT8 before_state;//set切页的上一个状态的值
  bool  before_flag;//用来保存上一个状态标志
  bool  flag;//保存set切页的状态值
  UINT16 time_cont;
  UINT16 time_sum;
}glo_dis_page_t;

typedef struct grc_dis_page_t
{
  UINT8 dis_pageid;
  UINT8 default_pageid;
  UINT8 before_state;//set切页的上一个状态的值
  bool  before_flag;//用来保存上一个状态标志
  bool  flag;//保存set切页的状态值
  UINT16 time_cont;
  UINT16 time_sum;
}grc_dis_page_t;
typedef struct gmagnet_change_page_t
{
  UINT8 dis_pageid;
  UINT8 default_pageid;
  UINT8 before_state;//set切页的上一个状态的值
  bool  before_flag;//用来保存上一个状态标志
  bool  flag;//保存set切页的状态值
  UINT16 time_cont;
  UINT16 time_sum;
}gmagnet_change_page_t;
#pragma pack()


extern stNetLink netlink_info;
extern SID_T gsid;
extern RF_T grf_state_flag;

extern SYS_LOAD_TYPE gSys_tp;
extern struct  INFO_DATA_T    INFO_DATA;           //系统和rf工作参数信息
extern struct  SYS_ATTR_T def_sys_attr;            //系统属性包
extern volatile HRB_T hbr_mode;
extern volatile HRB_T hbr_mode_bef;
extern HRB_STP rqst_hbr_info;
extern struct PKG_NUM_T gpkg;


extern dataQueue_t dataQueue;
extern uint8_t packetLength;
extern uint8_t* packetDataPointer;

extern volatile UINT8 gro_wor_times;
extern volatile UINT8 rc_wor_times;
extern volatile UINT16 uplink_wor_times;
extern volatile UINT8 fram1_data_channel;
extern volatile RF_EVENT_T ret_ack_flag;

extern UINT8 gelectric_quantity;
extern MY_TEST_T my_ack_buf;

extern GERR_T gerr_info;
extern RC_ERR_T grc_err;

extern ROM_UP_T updata_info;
extern ROM_UP_QRY_T  uprom_tp;
extern UINT16 all_osd_crc;

#ifndef PGK_BIT_FLASH_OPEN
  extern UINT8 pkg_bit_map[G_PKG_BIT_MAP_LEN];
#endif
extern file_id_t read_pkg_fileid;
extern UINT8 gdis_id_num;
extern osd_crc_t g_crc;
extern osd_cmd_t gcmd_tp;

extern struct PUBLIC_TEMP_T TEMP;
extern file_id_t read_pkg_fileid;
extern file_id_t write_temp_buff_id;
extern UINT32 cmd_start_offset;
extern UINT32 cmd_before_offset;
extern file_id_t write_pkg_buff_id;
extern file_id_t write_zip_temp_buff_id;
extern file_id_t before_page_file;
extern volatile UINT8 gFlag_bit;
extern bool gchange_page_flag;
extern dis_page_t gpage;
extern EPD_RC_ST  rc_attr_info;
extern EPD_ATTRIBUTE_PKG_ST epd_attr_info; //属性包
extern UINT16 first_lose_pkg;
extern UINT8 eraser_file_flag;
extern bool rc_store_inio_flag;
extern LED_LOOP_ST led_tp;
extern LED_STATE led_tp_info;
extern bool  led_flash_flag;
extern PIN_Handle ledPinHandle;
extern MsgObj  msg;
extern UINT8 clear_event_flag_display_page;
extern UINT8 clear_event_flag_epd_display;
extern UINT8 screen_buf[512];
extern UINT8 fast_refresh_flag;
extern SCREEN_COLOUR_T screen_colour;
extern UINT16 screen_num;
extern UINT8 gflash_empty;
extern UINT16 screen_height;
extern UINT16 screen_width;
extern struct OSD_PKG_NUM_T  gosd_pkg;
extern UINT32 esl_firmware_id;
extern UINT32 school_id;
extern UINT8 num_extend_flag;
extern UINT32 g_screen_repeat_dis_time;
extern UINT8  g_screen_repeat_dis_flag;
extern UINT8  refresh_screen_temperature;
extern SCREEN_HIGH_WIDE high_wide;
//extern UP_AREA_T updata_area;
extern volatile UINT8 upgread_id;
extern volatile UINT16 upgread_pkg_all;
extern struct OSD_PKG_NUM_T  g_upgread_pkg;
extern struct upgread_query_data_t g_upgread_query_data;
extern UPGREAD_FRAME2_T upgread_frame2;
extern UINT8 file_num;    //文件系统实际有多少个文件
extern UINT8 disply_temperature;
extern UINT8 page_crc_map;
extern UINT16 black_white_crc;
extern uint8_t  cache_buf[SCREEN_BUFF_SIZE];
extern uint8_t  cache_buf_1[LOAD_BUFF_SIZE];
extern volatile UINT8 receive_finish_pkg_flag;
extern UINT8 exit_uplink_status_flag;
extern volatile UINT8 upgrade_status_flag;
extern UINT8 uplink_session_id;
extern volatile uint16_t grst_sid_time;//清空sid
extern bool gsys_reset_flag;
extern volatile UINT8  rom_version;
extern bool low_tep_flag;
extern volatile UINT8 ack_power_param;
extern UINT8 g_ack_type;
extern UINT8 g_ack_data[18];


#endif
