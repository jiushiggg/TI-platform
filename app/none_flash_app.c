
#include "app.h"
#include "../screen/lattice_epd/scrn_public_fun.h"

#ifdef NONE_EX_FLASH


void screen_display(void)
{
    if(gSys_tp.present_page_id != gSys_tp.default_page_id )
    {
        sys_page_display_store_fun(gSys_tp.default_page_id,gSys_tp.present_page_id,gSys_tp.present_page_id,gSys_tp.stay_time,0);//保存页显示属性
    }
    if(f_len(gdsi_info_st[gSys_tp.present_page_id].file_id) !=0)
    {
        if(gchange_page_flag == TRUE)    //如果正处于切页状态，先把定时器停掉
        {
            stop_cut_page_timer();
        }

        screen_dis_t.fd = gdsi_info_st[gSys_tp.present_page_id].file_id;
        screen_dis_t.offset_1 = offsetof(G_DIS_PAGE_T,page_info);
        screen_dis_t.fd_red = gdsi_info_st[gSys_tp.present_page_id].file_id;
        screen_dis_t.offset_2 = CLOLOR_RED_OFFSET + offsetof(G_DIS_PAGE_T,page_info);
        screen_dispaly_fun(&screen_dis_t);
        g_screen_repeat_dis_time =0;
    }else
    {
        gerr_info = TR2_LAYER_ID_ERR;        //无这个页码
    }
    if(gchange_page_flag == TRUE)
    {
        set_cut_page_clock_timeout(gSys_tp.stay_time );    //在这里统一开定时器，防止连续切页时，刷屏过程中，定时时间到了，把当前页修改为默认页，防止回不到默认页
    }
    change_next_pageid_fun();//切换到下一页

}

void display_page_analysis_fun(void)
{
    if(gpage_info_fun() != TRUE)//若无此页，直接退出
    {
        fast_refresh_flag=0;
        gchange_page_flag = FALSE;
        gerr_info = TR2_LAYER_ID_ERR;        //无这个页码
        goto loop;
    }
    if(change_page_makesure_fun()== FALSE)//显示的页和屏幕页相同且内容不变，不触发显示动作
    {
        if(gchange_page_flag == TRUE)
        {
            set_cut_page_clock_timeout(gSys_tp.stay_time);    //切页状态中，连续多次切相同的页，不刷屏，只是把计时器重新计时
        }
        fast_refresh_flag=0;
        goto loop;
    }

    Event_post(protocol_eventHandle, EVENT_FLAG_EPD_DISPLAY);

    loop:
    clear_event_flag_display_page=0;

}

UINT8 cmd_main_data_crc(void)
{
    UINT8 ret=1;
    UINT16 tp_crc =0;

    //命令流crc
    flash_all_data_crc(&tp_crc,write_temp_buff_id,gcmd_tp.len,0);
    if(0 != memcmp((UINT8 *)&tp_crc,(UINT8 *)&g_cmd_crc, sizeof(g_cmd_crc)))
    {
        gerr_info = TR2_CMD_CRC_ERR;
        ret = 0;
    }
    return ret;
}

void screen_init(void)
{

    screen_analysis_fun(INFO_DATA.gRFInitData.screen_id);
    esl_firmware_id = (INFO_DATA.gRFInitData.screen_id & 0xFFFFF000);
    school_id = (INFO_DATA.gRFInitData.screen_id & 0xFFF00000);
    screen_colour = (SCREEN_COLOUR_T)screen_dis_t.src_color;
    g_screen_repeat_dis_flag = screen_dis_t.refresh_screen_flag;
    high_wide.high = (screen_dis_t.h+7)/8*8;
    high_wide.wide = screen_dis_t.w;
}


#endif
