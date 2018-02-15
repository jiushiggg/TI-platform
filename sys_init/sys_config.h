#ifndef SYS_CONFIG_H
#define SYS_CONFIG_H

#include <string.h>
#include <stdint.h>
#include "internal_flash.h"
#include "crc16.h"
#include "global_variable.h"
//#include "three_protocol.h"
#include "stdbool.h"


#define ROM_FLASH_BLOCK0_ADDR  ((WORD)0x0001D000)
#define ROM_FLASH_BLOCK1_ADDR  ((WORD)0x0001D200)
#define ROM_FLASH_BLOCK2_ADDR  ((WORD)0x0001D400)//属性包
#define ROM_FLASH_BLOCK3_ADDR  ((WORD)0x0001D600)//升级标志
#define ROM_FLASH_BLOCK4_ADDR  ((WORD)0x0001D800)//页属性
//
#define ROM_FLASH_BLOCK5_ADDR  ((WORD)0x0001DA00)//密钥
#define INFO_ADDR_1            ((WORD)0X0001FF50)


#define HEART_TIME_3M           ((uint8_t)(0x17))  //8s听帧的心跳值
#define RC_WOR_CONT             ((uint8_t)0x0a)    //遥控器200模式wor的2分钟计数
#define UPLINK_WOR_CONT         ((uint16_t)0x012C)    //上行链路100ms周期超时计数(30s的超时时间，最大开300次3ms的接收)
#define GROUP_MAX_TIMES         ((uint8_t )30)    //120s
#define GROM_UPDATA_FLAG        ((uint32_t)0x66886688)
#define GROM_UPDATA_SUCCESS     ((uint32_t)0x99779977)




#pragma pack(1)

typedef enum {
  RF_A7106_SET_WKUP_MODE, 
  RF_A7106_GRP_WKUP_MODE, 
} RF_WORK_MOD_T;


typedef struct _sys_config
{
    uint8_t  set_wkup_time;
    uint8_t  grp_wkup_time;
    uint16_t heartbit_time;
    uint8_t  work_time;

}MCU_DEF_ATTR;


struct SYS_ATTR_T {
    uint16_t attrcrc;
    uint8_t  uplink_defalut_channel;
    uint8_t  power_bottom;
    uint8_t  power_ceiling;
    uint16_t hbt_cont;
    RF_WORK_MOD_T rf_wk_md;
    uint8_t exit_grp_wkup_cont;
    RF_CMD_T gwor_flag_before;
    RF_CMD_T gwor_flag_now;
    MCU_DEF_ATTR sys_def_attribute;
};

#pragma pack()


void sys_load_config_info(void);
void save_extern_rf_info(void);
bool load_extern_rf_info(void);
bool save_sys_config_info(void);
void save_state_info_fun(void);
uint8_t hb_setwor_fun(void);
bool save_sys_load_page_info(void);
void sys_config_deaf_cont(void);
bool block_data_check(uint32_t start_addr, uint32_t offset, uint8_t *buffer, uint16_t len);  //起始地址必须是4k对齐
#endif
