#ifndef __HOLTEK_MSP430_OSD_FLASH_H__
#define __HOLTEK_MSP430_OSD_FLASH_H__

#include "three_event.h"
#include "global_variable.h"


void eraset_cmd_buf(void);
UINT32 find_id_mode0(UINT8 id);
bool compare_receive_data_ok(UINT16 get_success_pkg_num,UINT16 pkgnum_sum);
UINT8 cmd_main_pkg_check();
void flash_all_data_crc(UINT16  *crc,const file_id_t id,const UINT32 len,const UINT32 offset);
UINT16 all_ele_crc(void);
UINT8 main_osd_cmd(void);
UINT8 cmd_main_data_crc(void);
bool sys_page_display_store_fun(UINT8 default_page_id,UINT8 present_page_id,UINT8 page_id,UINT16 stay_time,UINT16 stay_time_cont);// 默认页、当前页、切换的页id号、停留时间，已经走过的时间、状态标志
void change_next_pageid_fun(void);
bool change_page_makesure_fun(void);
bool  gpage_info_fun(void);
void rst_sid_fun(void);
#endif
