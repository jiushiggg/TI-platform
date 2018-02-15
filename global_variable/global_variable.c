#include "global_variable.h"

dataQueue_t dataQueue;
uint8_t packetLength;
uint8_t* packetDataPointer;

#pragma location = (0x11000000);
uint8_t  cache_buf[SCREEN_BUFF_SIZE] = {0};
#pragma location = (0x11000000 + SCREEN_BUFF_SIZE);
uint8_t  cache_buf_1[LOAD_BUFF_SIZE] = {0};
#pragma location = (0xcffc);
UINT32 gproduct_id = 13246758;

//****************************射频相关变量*****************************
SYS_LOAD_TYPE gSys_tp;
struct  INFO_DATA_T    INFO_DATA;           //系统和rf工作参数信息
struct  SYS_ATTR_T  def_sys_attr;           //系统属性包
struct  PKG_NUM_T   gpkg;
volatile UINT8 gro_wor_times = 0;
volatile UINT8 rc_wor_times = 0;
volatile UINT16 uplink_wor_times = 0;
volatile UINT8 fram1_data_channel = 0;
volatile RF_EVENT_T ret_ack_flag = RF_EVENT_QUERY;
UINT8 exit_uplink_status_flag = FALSE;
RF_T grf_state_flag;
SID_T gsid;
stNetLink netlink_info;
volatile UINT8 ack_power_param = 0;
UINT8 g_ack_type;
UINT8 g_ack_data[18];


//****************************心跳相关变量*****************************
volatile HRB_T hbr_mode;
volatile HRB_T hbr_mode_bef = NORMAL_HEARTBEAT ;
HRB_STP rqst_hbr_info;


//****************************页显示属性******************************
EPD_ATTRIBUTE_PKG_ST epd_attr_info;   //属性包
UINT8 page_crc_map;    //每页数据crc的map表，0表示正确，1表示错误

//****************************LED相关变量*****************************

bool  led_flash_flag = FALSE;
LED_LOOP_ST led_tp;
LED_STATE   led_tp_info ={0};
EPD_RC_ST  rc_attr_info;


//****************************自测相关变量*****************************
MY_TEST_T my_ack_buf;


//****************************电量相关变量*****************************
UINT8 gelectric_quantity = 0;
GERR_T gerr_info = NONEERR;
RC_ERR_T grc_err = NONEERR_1;


//****************************升级相关变量*****************************
ROM_UP_T updata_info;
//UP_AREA_T updata_area;
ROM_UP_QRY_T  uprom_tp;
UPGREAD_FRAME2_T upgread_frame2;

volatile UINT8  upgread_id = 0;
volatile UINT8  rom_version = EPD_3_VERSION;
volatile UINT16 upgread_pkg_all = 0;
struct OSD_PKG_NUM_T  g_upgread_pkg;
struct upgread_query_data_t g_upgread_query_data;
volatile UINT8 receive_finish_pkg_flag =0;
volatile UINT8 upgrade_status_flag = 0;

//****************************内存替换flash相关变量**********************

#ifndef PGK_BIT_FLASH_OPEN
UINT8 pkg_bit_map[G_PKG_BIT_MAP_LEN];
#endif
volatile UINT8 gFlag_bit = 1;
UINT8 gflash_empty  = 0;


//****************************文件系统相关变量***************************

UINT8 file_num = FILE_ID_END;    //文件系统实际有多少个文件


//****************************OSD相关变量******************************
struct OSD_PKG_NUM_T  gosd_pkg;
UINT32 cmd_start_offset = 0;
UINT32 cmd_before_offset = 0;
file_id_t write_pkg_buff_id;
file_id_t read_pkg_fileid = F_BMP_PKG_1;
file_id_t write_temp_buff_id;
file_id_t write_zip_temp_buff_id;
bool gchange_page_flag = FALSE;
UINT16 first_lose_pkg = 0;
UINT8 clear_event_flag_display_page=0;
UINT8 clear_event_flag_epd_display=0;
UINT8 gdis_id_num = 0 ;
struct PUBLIC_TEMP_T TEMP;

//****************************全局CRC相关变量****************************
osd_crc_t g_crc;
osd_cmd_t gcmd_tp;
UINT8 eraser_file_flag=0;
UINT16 all_osd_crc = 0;
UINT16 black_white_crc = 0;
file_id_t before_page_file;



//***************************页显示属性相关变量****************************
dis_page_t gpage ={0};
EPD_RC_ST  rc_attr_info;
EPD_ATTRIBUTE_PKG_ST epd_attr_info; //属性包
bool rc_store_inio_flag=FALSE;


//****************************屏幕相关变量*******************************
UINT8 disply_temperature = 0;
MsgObj  msg;
UINT8 fast_refresh_flag =0;
UINT32 g_screen_repeat_dis_time = 0;
UINT8  g_screen_repeat_dis_flag = 0;
UINT8  refresh_screen_temperature =0;
SCREEN_COLOUR_T screen_colour = SCREEN_COLOUR_NONE;
SCREEN_HIGH_WIDE high_wide;

volatile uint16_t grst_sid_time = 0;//清空sid
//****************************上行链路相关变量*******************************
UINT8 uplink_session_id = 1;

UINT16 screen_num = 1;
UINT16 screen_height;
UINT16 screen_width;
UINT32 esl_firmware_id;
UINT32 school_id;
UINT8 num_extend_flag = 0;
bool gsys_reset_flag = true;
bool low_tep_flag = FALSE;





