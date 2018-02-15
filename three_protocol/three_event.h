
#ifndef THREE_EVENT_H
#define THREE_EVENT_H

#include "global_variable.h"
#include "mini_fs_conf.h"


#define DOWNLINK_RX_TIMEOUT                5000+100       //6ms     以10us为一个单位
#define SET_RX_TIMEOUT                     300+100       //3ms     以10us为一个单位
#define GRP_RX_TIMEOUT                     150+100        //1.5ms
#define FRAME1_RX_TIMEOUT                  100000     //1s
#define FRAME2_RX_TIMEOUT                  100000     //1s
#define DADA_RX_TIMEOUT                    400000     //1s
#define RC_RX_TIMEOUT                      300+100        //3ms     以10us为一个单位
#define REDUNDANT_TIME                     100

extern Clock_Handle    group_wakeup_timer_handle;
extern Clock_Handle    rc_wakeup_timer_handle;
extern Clock_Handle    uplink_wakeup_timer_handle;
extern Clock_Handle    set_wakeup_timer_handle;
extern Mailbox_Handle  rf_rx_timeout_mailbox;
extern Event_Handle    protocol_eventHandle;

extern Semaphore_Handle  led_flash_sem;
extern Semaphore_Handle  led_off_sem;
extern Semaphore_Handle  reed_sem;

#define SET_CTRL_INFO                   (0xe0)
#define SET_GLOBAL_CTRL                 (0x20)
#define SET_GET_FRAME1_CTRL             (0x40)
#define SET_PKG_TRN_CTRL                (0x60)
#define SET_PKG_CH_CTRL                 (0x80)
#define SET_BROADCAST_CTRL              (0xc0)
#define SET_QUICK_LED_PAGE_CTRL         (0xe0)
#define SET_UPLINK_CTRL                 (0x00)

#define GBL_CMD                         (0xf0)
#define GBL_CMD_COMPLEMENT              (0x0f)

#define GBL_CMD_HB_REQ                  (0x3c)

#define GBL_CMD_HB_CFG                  (0x5a)

#define GBL_CMD_SET_CFG                 (0x96)

#define GBL_CMD_PAGE_CHG                (0xa5)

#define GBL_CMD_LED_CFG                 (0xb4)

#define GBL_CMD_LCD_CFG                 (0xf0)

#define GBL_CMD_SYS_RST                 (0xc3)


#define GRP_WKUP_CTRL_INFO              (0xe0)
#define GRP_WKUP_READY                  (0xA0)

#define FRAME1_CTRL_INFO                (0xc0)

#define CTRL_QUERY                      (0x00)
#define CTRL_NETLINK                    (0xa0)
#define CTRL_OSD                        (0x40)
#define CTRL_LINK_QUERY                 (0x60)
#define CTRL_RC_PKG                     (0x80)
#define CTRL_SLEEP                      (0xe0)
#define CTRL_BROADCAST_UPDATA           (0x20)

#define OSD_SID_FALG                    (0x1f)
#define OSD_SUB_SID_FALG                (0x0f)

#define RF_PKG_TYPE                      0xc000    //osd传输时数据包的类型 00：正常数据包 11：结束数据包
#define DATA_SIZE                        20

RF_ERROR_T rf_cmd_init(RF_T *rf);
RF_ERROR_T rf_cmd_set_rx(RF_T *rf, uint8_t *buf);
RF_ERROR_T rf_into_stby(RF_T *rf);
RF_ERROR_T rf_set_wor(RF_T *rf, bool slp_flag);
RF_ERROR_T rf_cmd_sleep(RF_T *rf);
RF_CMD_T set_rx_err(RF_ERROR_T err);
void set_wkup_event(RF_T *rf, UINT8 ctrl);
RF_CMD_T globle_evert_fun(UINT8 *buf);
RF_CMD_T set_pkg_trn_fun(UINT8 *buf);
RF_CMD_T set_get_frame1_fun(UINT8 *buf);
RF_CMD_T set_pkg_chn_fun(UINT8 *buf);
RF_CMD_T set_broadcast_fun(UINT8 *buf);
RF_CMD_T set_ctrl_err_fun(UINT8 *buf);
RF_CMD_T get_sleep_fun(UINT8 *buf);

RF_ERROR_T rf_grp_wor(RF_T *rf);
RF_ERROR_T rf_cmd_grp_rx(RF_T *rf, uint8_t *buf);
RF_CMD_T grp_rx_err(RF_ERROR_T err);
RF_CMD_T grp_wkup_event(RF_T *rf, UINT8 *buf);

RF_ERROR_T rf_frame1_config(RF_T *rf);
RF_ERROR_T rf_cmd_rx_fram1(RF_T *rf, uint8_t *buf);
RF_ERROR_T rf_cmd_rx_fram2(RF_T *rf, uint8_t *buf);
RF_CMD_T frame1_rx_err(RF_ERROR_T err);
RF_CMD_T rx_frame1_event(RF_T *rf, UINT8 *buf);
RF_CMD_T rx_frame2_event(RF_T *rf, UINT8 *buf);
RF_CMD_T rc_wor_exit_fun(void);
RF_CMD_T get_rc_err(RF_ERROR_T err);
RF_CMD_T rx_frame1_tout_fun( void);
RF_CMD_T rx_frame1_mask_exit_fun( void);
RF_CMD_T rc_wkup_event(RF_T *rf, UINT8 *buf);
RF_ERROR_T rf_rc_grp_rx(RF_T *rf,UINT8 *buf);

RF_ERROR_T get_data_rf_config(RF_T *rf);
RF_ERROR_T rf_cmd_rx_data(RF_T *rf, uint8_t *buf);
RF_ERROR_T rf_cmd_broadcast_rx_data(RF_T *rf, uint8_t *buf);
RF_CMD_T rx_data_err(RF_ERROR_T err);
void rx_data_event(RF_T *rf, UINT8 *buf);
void broadcast_rx_data_event(RF_T *rf, UINT8 *buf);
RF_CMD_T get_data_query_fun(UINT8 *buf);
RF_CMD_T get_netlink_fun(UINT8 *buf);
RF_CMD_T get_osd_broadcast_pkg_fun(UINT8 *buf);
RF_CMD_T get_osd_updata_fun(UINT8 *buf);
RF_CMD_T get_broadcast_fun(UINT8 *buf);
RF_CMD_T get_osd_128cmd_fun(UINT8 *buf);
RF_CMD_T get_link_query_fun(UINT8 *buf);
RF_CMD_T get_rc_pkg_fun(UINT8 *buf);
void led_tp_init_fun(UINT8 ledtp_flag);


RF_ERROR_T ret_ack_rf_config(RF_T *rf);
RF_CMD_T tx_ack_info(UINT8 *buf);
RF_ERROR_T ack_tx_data(UINT8 len, UINT8 *buf);
RF_CMD_T tx_ack_err(RF_ERROR_T err);

void rf_interrupt_into_fun(void);
UINT8 aclk_timer_choose_cycle_fun(UINT16 tp);
void aclk_wor_ready_fun(void);
void set_wor_flag_fun(RF_CMD_T tp);
bool crc_normal_pkg_fun(UINT8 *buf);
bool crc_broadcast_normal_pkg_fun(UINT8 *buf);
RF_CMD_T tx_exit_fun( void);
void clear_gpkg_fun(void);
file_id_t swp_pkg(file_id_t src);
RF_CMD_T rx_tx_timeout_fun( void);
void clear_pkg_nosd_fun(void);
void rc_page_init_fun(void);
void lcd_reset_init(void);
UINT8  set_rf_work_time_fun(UINT8 buf);
UINT16 lose_pkg_statistics(UINT16 pkg_all,file_id_t readid,UINT8 *buf);
UINT16 broadcast_upgread_lose_pkg_statistics(UINT16 pkg_all,file_id_t readid,UINT8 *buf);
bool sid_cmpare(UINT8 tp);
void start_set_group_rc_timer(void);
uint32_t rfid_to_u32(RFID id);
RF_CMD_T set_pkg_quick_led_page_fun(UINT8 *buf);
RF_CMD_T set_pkg_uplink_ctrl_fun(UINT8 *buf);
RF_ERROR_T rf_uplink_set_rx(RF_T *rf,UINT8 *buf);
RF_ERROR_T downlink_rx(uint8_t *buf);
RF_CMD_T uplink_wkup_event(RF_T *rf, UINT8 *buf);
void select_close_timer(void);

#endif
