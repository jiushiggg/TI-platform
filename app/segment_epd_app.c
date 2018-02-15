
#include "app.h"

#ifdef SEGMENT_EPD



void screen_diaplay_fun(void)
{
    if(gSys_tp.present_page_id != gSys_tp.default_page_id )
    {
        sys_page_display_store_fun(gSys_tp.default_page_id,gSys_tp.present_page_id,gSys_tp.present_page_id,gSys_tp.stay_time,0);//保存页显示属性
    }

    if(f_len(gSys_tp.use_page_num_fileid) !=0)
    {
        if(gchange_page_flag == TRUE)    //如果正处于切页状态，先把定时器停掉
        {
            stop_cut_page_timer();
        }

//        f_read(gSys_tp.use_page_num_fileid, gSys_tp.present_page_id * PAGE_LEN, screen_buf, PAGE_LEN);
//        SSD1627_open();
//        updata_screen(screen_buf, screen_buf[1]+screen_buf[2]*256 +3);
//        SSD1627_close();
        g_screen_repeat_dis_time =0;
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
    UINT16 tp_crc =0;

    //命令流crc
    flash_all_data_crc(&tp_crc,F_BMP_DATA,gcmd_tp.len,gcmd_tp.start_addr );
    if((0 != memcmp((UINT8 *)&tp_crc,(UINT8 *)g_crc.crc_cmd , 2)))
    {
        gerr_info = TR3_STREAM_CRC_ERR;
        return 0;
    }
    //所有图层crc
    tp_crc = 0;

    if(0 != memcmp((UINT8 *)&tp_crc,g_crc.crc_layer,2))
    {
        gerr_info = TR3_STREAM_CRC_ERR;
        return 0;
    }
    //页存储crc
    tp_crc =0;
    flash_all_data_crc(&tp_crc,gSys_tp.use_page_num_fileid, PAGE_LEN*8, 0);
    if(0 != memcmp((UINT8 *)&tp_crc,g_crc.crc_page_info,2))
    {
        gerr_info = TR3_OSD_QUERY_PAGE_INFO_CRC_ERR;
        return 0;
    }
    //数字存储crc
    tp_crc =0;
    if(0 != memcmp((UINT8 *)&tp_crc,g_crc.crc_page_num,2))
    {
        gerr_info = TR3_OSD_QUERY_PAGE_INFO_CRC_ERR;
        return 0;
    }

    return 1;
}

#endif
