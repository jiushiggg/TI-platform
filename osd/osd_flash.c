#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include "mini_fs_conf.h"
#include "mini_fs.h"
#include "osd_cmd.h"
#include "crc16.h"
#include "three_event.h"

const uint8_t data_temp[] = {0x03,0x9F,0x04,0x00,0x00,0x00,0x01,0x10,0x00,0x01,0x00,0x80,0x12,0x00,0x00,0xBD,0x8A,0x1F,0x8B,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xD5,0xD7,0x4F,0x6F,0x1B,0x45,0x14,0x00,0xF0,0xB1,0xB3,0x6A,0x83,0x54,0x61,0x73,0x82,0x03,0x74,0x8D,0xB8,0x71,0x8A,0xC4,0x81,0x22,0x05,0x6D,0x12,0x97,0xF4,0x53,0x44,0x0A,0xE2,0x80,0x84,0x94,0xD2,0x9E,0x0A,0x32,0xB0,0x86,0x48,0x48,0x5C,0xE0,0x13,0x20,0x7A,0xE6,0x0B,0x70,0xF4,0xAE,0x1D,0xD5,0x27,0x64,0x24,0x0E,0xB4,0x4A,0xCC,0x3A,0xF8,0x60,0x11,0x0E,0xB6,0xB5,0x8A,0x77,0x9D,0x1D,0xEF,0x63,0x66,0xD6,0x3B,0x33,0x6F,0x62,0x3B,0x6D,0x38,0xB1,0x55,0xDD,0xFE,0x32,0x33,0x6F,0xDF,0x7B,0xFB,0x67,0x1C,0x80,0x4B,0xC7,0x05,0x21,0xC4,0x62,0x7F,0xED,0x11,0xFB,0x58,0xE5,0xA5,0xF3,0xDD,0xBA,0x45,0x40,0x8D,0x17,0xEA,0xFB,0xD6,0x1A,0xF1,0xA4,0x8B,0xA4,0x6C,0x59,0xA4,0x22,0xE7,0x5B,0x64,0xDD,0x5A,0x27,0x65,0xE9,0x75,0x62,0x59,0x65,0xB2,0x2E,0x5D,0x26,0x45,0xAB,0x42,0x2C,0xB9,0xDE,0x74,0xA5,0x5E,0xB0,0xB6,0xBF,0x6E,0xCA,0xF9,0xDC,0xD5,0xC3,0xA6,0x1C,0x7F,0x93,0x79,0xF7,0xC6,0x8E,0x1C,0x7F,0x0B,0x4E,0xAD,0xBB,0xB7,0x0C,0xBF,0xB6,0xA3,0xE2,0x11,0x36,0x7F,0x03,0xBB,0xDA,0x53,0xF3,0xB9,0xB7,0xBD,0x9D,0x4B,0xF9,0x2D,0xCB,0x37,0xAF,0x27,0xB7,0x59,0x6F,0xDE,0x8F,0x65,0xFD,0x32,0xFB,0xF9,0xC2,0xD7,0x03,0x3B,0x31,0xAE,0x37,0xF3,0x14,0x9B,0x26,0xD8,0x9B,0x99,0xE5,0xFA,0x3B,0xA1,0x08,0x92,0x3B,0x7D,0x05,0x3B,0xFE,0x20,0x82,0x50,0x5B,0x7F,0xFA,0x6B,0x04,0x1E,0x29,0x49,0x0F,0x7E,0x1C,0x20,0xA7,0xEF,0x0F,0x50,0xFC,0xF4,0xCB,0xA1,0x91,0xDF,0x3F,0x68,0x3C,0x82,0x64,0xA1,0xF3,0xF9,0xCB,0xC6,0x97,0xCD,0x1F,0x91,0x35,0xF6,0xCF,0x60,0xA9,0x9F,0x37,0x3E,0x1A,0x8F,0xB4,0x7E,0xCD,0xAD,0xEA,0x01,0x18,0x1A,0xF6,0x5E,0xD5,0xE7,0x03,0x5A,0x3F,0x9E,0x3B,0x9F,0x3F,0x06,0x82,0xC6,0x23,0xC3,0x93,0xD6,0x81,0x88,0xAF,0x5B,0x8F,0x3F,0x39,0x3E,0x40,0xF1,0x72,0x9B,0xE3,0xBA,0x7B,0xEF,0xAA,0xF9,0xDF,0x1E,0x17,0x20,0xF8,0x43,0x8D,0xE7,0xD6,0xE3,0xE9,0xE3,0xF9,0x7A,0x14,0x1F,0x8C,0xF3,0xEB,0x6E,0x1D,0xA0,0xFE,0x9A,0x16,0xF5,0x82,0xDE,0x1F,0x82,0xD6,0x8F,0x01,0x1D,0xF2,0xE2,0x98,0xF7,0xBF,0x74,0x60,0xDA,0xB8,0xFF,0x83,0xE4,0x5A,0xE3,0x57,0xCD,0xBF,0xCA,0xF9,0x7A,0x4F,0xDC,0x8F,0x97,0x7D,0xDD,0x78,0xD7,0xCD,0xF7,0xF9,0xC7,0x61,0xA1,0xE5,0x7C,0x58,0xED,0x21,0xB8,0xAB,0xED,0x1A,0xAE,0x19,0x7E,0x68,0xF8,0x23,0xEC,0xC3,0x7B,0x86,0x3F,0xBD,0x22,0xBE,0x69,0x42,0x56,0x5B,0x9B,0x9F,0x1A,0xA6,0xEE,0x13,0x74,0xBE,0xF8,0xE4,0x46,0x4D,0xCF,0xC7,0x3F,0xB9,0x5D,0xD3,0xF3,0xED,0x77,0x4F,0x50,0x3D,0x09,0x77,0x4D,0x79,0xDA,0x7D,0x86,0xE2,0xC7,0x7B,0xBF,0xA3,0x7E,0x5D,0xEC,0xB5,0x91,0x93,0x6E,0x07,0x5F,0x3C,0x8F,0xD4,0x60,0xA6,0x7E,0x94,0x0A,0xAB,0xFD,0x80,0xEE,0xB1,0x8B,0xAB,0x39,0xEE,0x72,0xAB,0xB7,0xBB,0xCF,0xEC,0xBD,0x11,0x52,0xBE,0xBF,0x78,0xEC,0xA3,0xDF,0xED,0x32,0x0F,0xA4,0x79,0xBE,0xDD,0xAE,0x66,0x9F,0xBB,0x23,0x3D,0xF5,0x9F,0x32,0xFF,0x44,0xF9,0xFE,0xE6,0x95,0x46,0x24,0x76,0x7D,0x66,0x87,0xF2,0xFD,0xCF,0xDB,0xB8,0x45,0x2E,0xDC,0x16,0x0B,0xE1,0x52,0xBE,0x3F,0x7A,0x5B,0x45,0x92,0x88,0x94,0x09,0xE5,0xFB,0xA7,0x57,0x2F,0x48,0xF3,0xFD,0xD5,0x27,0xA4,0x9E,0xB5,0xC8,0xA5,0x7C,0xFF,0x6D,0x12,0xB2,0x05,0xC4,0x11,0xF1,0x32,0xEF,0x6F,0x51,0x52,0x12,0xE7,0xDB,0x3E,0x2C,0xDA,0x47,0x85,0x5E,0x25,0x86,0xB6,0xC8,0xA7,0x7A,0xF8,0x8D,0x7D,0x74,0xF3,0x71,0x25,0x84,0x50,0xE4,0xBB,0x4B,0x9A,0xF6,0xD1,0xED,0x4A,0x85,0xBD,0xCB,0x45,0x3D,0x77,0xCB,0x3B,0xF6,0xD1,0x67,0xB9,0xC3,0xCC,0xEC,0x34,0xDC,0xAC,0x1F,0x74,0xF7,0x4E,0x53,0x98,0xAF,0x67,0xFD,0xA2,0xD5,0x21,0x8B,0x47,0x2A,0x22,0x3E,0xF7,0x76,0xBB,0x28,0xCC,0xCF,0xCF,0xDB,0xAA,0xE7,0x87,0x9C,0x3D,0x0F,0xA8,0x1E,0xEE,0xBC,0xDE,0xDC,0x79,0x3F,0xA2,0xB9,0xF3,0x7E,0x45,0x90,0x0A,0xE7,0xFD,0x64,0xAE,0x09,0xCF,0xFB,0x1D,0x91,0xCD,0x64,0xA5,0xDD,0xA9,0xE1,0x31,0xBA,0x5F,0x22,0xB7,0x9F,0xB9,0x91,0x7D,0x4C,0xDC,0x76,0x66,0x3B,0xF3,0xE8,0x2B,0x51,0x1F,0xA4,0x0E,0x7B,0xFE,0x37,0x1C,0x18,0xEF,0x8B,0xFA,0x20,0x26,0x05,0xB6,0xB8,0x01,0xD1,0x87,0xD9,0xF3,0xDE,0xCB,0xED,0x0E,0x85,0xF9,0x1B,0xAD,0x57,0x76,0x98,0xB3,0xFB,0x53,0x84,0x13,0xE3,0x83,0x2C,0x1E,0x5F,0xF6,0x8B,0xEE,0x31,0xA4,0x8F,0x98,0xC9,0x3B,0x73,0xF7,0xE1,0x74,0xC4,0xD6,0x93,0xF7,0xE6,0x6E,0xB3,0x8D,0x82,0x8D,0x43,0x3C,0x77,0x09,0x2E,0x84,0x69,0x4D,0x98,0xCD,0x4D,0x58,0x7E,0xF3,0x7E,0x01,0x7F,0x84,0x93,0xAC,0xA8,0xCC,0x25,0x59,0x64,0xB6,0xDE,0x93,0xEF,0xDC,0xB9,0x0B,0x30,0x6B,0x28,0xFF,0xC9,0xBE,0x43,0xCD,0x5C,0xE5,0xEF,0xB9,0x1F,0xDB,0xD2,0x4F,0x98,0x7B,0x6F,0xAB,0x78,0x7F,0x31,0x9F,0x9F,0x29,0x4F,0xC0,0x87,0xF3,0x4F,0xB4,0xF3,0x41,0x0B,0xCE,0x7F,0x56,0xE6,0xC7,0xEC,0x0C,0x3B,0xD2,0xC6,0x03,0x56,0x86,0xBE,0x3E,0x48,0x1F,0xC2,0xE4,0x4C,0x77,0x15,0xF9,0xE9,0xCB,0x1F,0xC3,0xF9,0xBE,0xAD,0xD9,0x85,0x50,0xAB,0xE7,0xF8,0x7E,0x6B,0x5E,0x7E,0x6E,0x1F,0xD5,0x7F,0x7C,0x7F,0xBA,0xD2,0xEC,0xC9,0xE6,0x37,0xA1,0xE6,0x67,0x30,0xEB,0x6A,0xF3,0x0F,0xBE,0x33,0xEC,0x1B,0xFE,0xDB,0x30,0xEB,0x1F,0x71,0x90,0x67,0xDD,0x40,0xE5,0xFF,0x92,0x8F,0x5D,0x08,0x91,0x83,0x78,0x88,0x4D,0xEF,0xB1,0xFC,0x34,0xA7,0x7D,0xFE,0xA9,0xF5,0x97,0x3D,0x59,0xA9,0xAD,0x59,0x1E,0xCA,0x8D,0xD5,0x8E,0x0C,0x4F,0xEB,0x0E,0x9E,0x9F,0xBD,0x97,0xB5,0x97,0xAF,0x11,0xCF,0xC5,0x4E,0x1E,0xE0,0x7C,0x92,0x47,0x8D,0x95,0x1E,0xDD,0xC4,0x8E,0x8C,0xF1,0x81,0xE1,0xC8,0x88,0xAF,0x9F,0x5F,0x4B,0x7A,0xA1,0x43,0xC3,0xFA,0xF7,0xFB,0x45,0xE6,0x47,0xFA,0x39,0xB6,0x39,0xFE,0xA2,0x9E,0x69,0x1E,0xB3,0x17,0x90,0xEE,0x53,0x76,0x2D,0x63,0xE2,0x4A,0x77,0xD8,0x1F,0x7D,0x7D,0x10,0x0C,0xFF,0xB3,0x29,0xD5,0xE2,0xFF,0xD0,0x61,0x3B,0x83,0x72,0x8F,0xFD,0x7A,0x97,0x44,0xCA,0xA3,0x07,0x0E,0xFF,0x62,0x80,0xF2,0x4F,0x02,0x6C,0xEA,0x28,0x07,0x46,0xBD,0x8B,0x4C,0x29,0xB6,0x1E,0x2F,0xA0,0x25,0xEC,0x9E,0xE1,0x35,0x3C,0xFF,0xC4,0xE6,0x9B,0x92,0x8A,0xF7,0x9B,0x71,0xBE,0x96,0x61,0xDE,0xCD,0xF8,0x75,0x65,0xF1,0x9F,0x86,0xBB,0xD4,0xD4,0xC1,0x8E,0x6D,0xEC,0xB0,0xC3,0x37,0x76,0x15,0x2F,0x52,0xBF,0x12,0x2E,0x74,0xD8,0x69,0xA3,0x7C,0x63,0xDB,0x46,0xF5,0xD0,0x4D,0x17,0xF7,0x77,0x6C,0xF4,0xBB,0x6D,0xD8,0xA8,0x9F,0x7E,0xE1,0xA0,0xF3,0x8D,0x79,0xC2,0x9A,0x07,0x21,0xBE,0x9F,0xB4,0x04,0x17,0xDB,0x9C,0x6F,0xC4,0x13,0x0D,0xD2,0xAD,0x8E,0xFF,0xBB,0xD1,0xF1,0x2F,0x20,0x24,0x18,0xC2,0x80,0x12,0x00,0x00,0x03,0x33,0x00,0x00,0x00,0x00,0x01,0x11,0x00,0x01,0x00,0x80,0x12,0x00,0x00,0xAD,0xB5,0x1F,0x8B,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xED,0xC1,0x31,0x01,0x00,0x00,0x00,0xC2,0xA0,0xFE,0xA9,0x67,0x0A,0x3F,0xA0,0x00,0x00,0x00,0x00,0xE0,0x67,0x58,0xFC,0x21,0xE6,0x80,0x12,0x00,0x00,0x05,0x01,0x00,0x00,0x76,0x89,0x00,0x39,0x75,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};


void rst_sid_fun(void)
{

#define    G_RESET_SID_MIN ((UINT16)43200)
    if( grf_state_flag.cmd != RF_FSM_CMD_SET_WAKEUP)
        return ;

    grst_sid_time += def_sys_attr.sys_def_attribute.set_wkup_time;
    if(grst_sid_time >=  G_RESET_SID_MIN)
    {
        grst_sid_time = 0;
        gsid.old_sid = 0;
        //注意有flash版本的要清空包号缓冲区
    }
}
UINT32 find_id_mode0(UINT8 id)
{
    return f_len((file_id_t)id);
}

bool compare_receive_data_ok(UINT16 get_success_pkg_num,UINT16 pkgnum_sum)
{ 
    if(0 == pkgnum_sum )
        return FALSE;
    if(get_success_pkg_num < pkgnum_sum )
        return FALSE;
    //当收到结束包，且数据包总数比比实际大时，表示错误
    if(get_success_pkg_num > pkgnum_sum )
    {
        gerr_info = TR3_PKG_ERR;
        return FALSE;
    }
    return TRUE;
}

UINT8 cmd_main_pkg_check()
{ 
    if(!compare_receive_data_ok(gosd_pkg.current_pkg_num,gosd_pkg.finish_pkg_num))
    {
        return 0;
    }
    clear_gpkg_fun();
    memset((UINT8 *)&gosd_pkg,0x00,sizeof(gosd_pkg));//删除osd数据包统计函数
    return 1;
}


void flash_all_data_crc(UINT16  *crc,const file_id_t id,const UINT32 len,const UINT32 offset)
{
#define TEMP_MAX_BUF  1024
    UINT8 temp[1024];
    UINT32 i = 0 ,wirte_size = 0;
    if(id == 200)//非法ID
        memset(temp, 0xff,TEMP_MAX_BUF);
    for(i=0; i < len ;)
    {
        if( (len - i) < TEMP_MAX_BUF )
            wirte_size  = len - i;
        else
            wirte_size  = TEMP_MAX_BUF ;

        if(id != 200)
            f_read(id,i + offset,temp,wirte_size);
        *crc = my_cal_crc16(*crc,temp,wirte_size);
        i += wirte_size;
    }
}





UINT16 all_ele_crc(void)
{

    UINT16  crc= 0;
    wrt_layer_arrt_t tp;
    UINT32 all_size = 0;
    UINT16 i, num = (find_id_mode0(F_LAY_MAP) / sizeof(tp));

    for(i = 0 ;i < num ;i++)
    {
        f_read(F_LAY_MAP,i * sizeof(tp),(UINT8 *)&tp,sizeof(wrt_layer_arrt_t));
        flash_all_data_crc(&crc,tp.src_file,tp.len,tp.offset);
        all_size += tp.len;
    }
    return crc;

}

UINT8 main_osd_cmd(void)
{
    if(!cmd_main_pkg_check())
        return 0;

    if(TRUE != process_cmd(write_temp_buff_id))
    {
        if(gerr_info !=NONEERR )
            gerr_info = TR3_OSD_CMDERR;
        clear_event_flag_display_page = 1;
        return 0;
    }

    if(FALSE == save_sys_load_page_info())
    {
        gerr_info = SYS_SAVE_ERR;
    }

    if(cmd_main_data_crc() ==0)
    {
        clear_event_flag_display_page = 1;   //crc 计算错误时，不刷新屏幕     需要注意的地方
    }
    return 1;

}




void eraset_cmd_buf(void)
{

    if(gFlag_bit)
    {
        volatile file_id_t eraser_id  = swp_pkg(write_pkg_buff_id);
        f_erase(eraser_id);//擦除未用的接受缓冲区
        gFlag_bit = 0;

    }
}


bool gpage_info_fun(void)
{
    if(gSys_tp.present_page_id  > ALL_PAGE_SUM)
    {
        gerr_info = TR2_LAYER_ID_ERR;//无这个页码
        return FALSE;
    }
    gSys_tp.stay_time_cont = 0;//gSys_tp.stay_time /2;当要显示新的屏数据时，保持时间计数值清0
    gdis_id_num = 0;//清空图形显示个数

    if((gSys_tp.present_page_id == gSys_tp.default_page_id ) )//如果要显示的页和默认页一样
    {
        gchange_page_flag = FALSE;
        gpage.flag = FALSE;//关掉切页标志
    }
    else
        gpage.flag = TRUE;//打开切页标志
    if(gSys_tp.present_page_id != gSys_tp.page_map[gSys_tp.present_page_id])//要显示的页码不存在
    {
        gerr_info = TR2_LAYER_ID_ERR;//无这个页码
        return FALSE;
    }
    return TRUE;
}

const UINT8 g_page_flag[8]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};

bool change_page_makesure_fun(void)
{
    if((gSys_tp.present_page_id == gSys_tp.gpage_nowid) && ((gSys_tp.change_map & g_page_flag[gSys_tp.present_page_id]) == 0)) //当前显示的页==屏幕显示的页，且这个页的内容为改变时才不触发更新操作
    {
        return FALSE;
    }
    return TRUE;
}

bool sys_page_display_store_fun(UINT8 default_page_id,UINT8 present_page_id,UINT8 now_dispaly_id,UINT16 stay_time,UINT16 stay_time_cont )
{
    gSys_tp.default_page_id = default_page_id;
    gSys_tp.present_page_id = present_page_id;
    gSys_tp.gpage_nowid = now_dispaly_id;
    gSys_tp.stay_time = stay_time;
    gSys_tp.stay_time_cont = stay_time_cont;
    if(FALSE == save_sys_load_page_info())
    {
        return FALSE;
    }
    return TRUE;
}

void change_next_pageid_fun(void)
{
    gSys_tp.gpage_nowid = gSys_tp.present_page_id;//要显示的页给屏幕显示的页保存
    sys_page_display_store_fun(gSys_tp.default_page_id,gSys_tp.present_page_id,gSys_tp.gpage_nowid,gSys_tp.stay_time,0);//保存页显示属性
    gSys_tp.change_map = (~(0x01 << gSys_tp.gpage_nowid));//清除改变的页
}
