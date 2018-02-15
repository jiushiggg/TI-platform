
#include "app.h"

#ifdef HAVE_EX_FLASH



void screen_diaplay_fun(void)
{
    if(gSys_tp.present_page_id != gSys_tp.default_page_id )
    {
        sys_page_display_store_fun(gSys_tp.default_page_id,gSys_tp.present_page_id,gSys_tp.present_page_id,gSys_tp.stay_time,0);//����ҳ��ʾ����
    }
    if( (f_len(F_BMP_RED) !=0) || (f_len(F_BMP_BW) != 0 ))
    {
        if(gchange_page_flag == TRUE)    //�����������ҳ״̬���ȰѶ�ʱ��ͣ��
        {
            stop_cut_page_timer();
        }

        screen_dis_t.fd = F_BMP_BW;
        screen_dis_t.offset_1 = 0;
        screen_dis_t.fd_red = F_BMP_RED;
        screen_dis_t.offset_2 = 0;
        screen_dispaly_fun(&screen_dis_t);
        g_screen_repeat_dis_time =0;
    }
    if(gchange_page_flag == TRUE)
    {
        set_cut_page_clock_timeout(gSys_tp.stay_time );    //������ͳһ����ʱ������ֹ������ҳʱ��ˢ�������У���ʱʱ�䵽�ˣ��ѵ�ǰҳ�޸�ΪĬ��ҳ����ֹ�ز���Ĭ��ҳ
    }
    change_next_pageid_fun();//�л�����һҳ

}

void display_page_analysis_fun(void)
{
    if(gpage_info_fun() != TRUE)//���޴�ҳ��ֱ���˳�
    {
        fast_refresh_flag=0;
        gchange_page_flag = FALSE;
        goto loop;
    }
    if(change_page_makesure_fun()== FALSE)//��ʾ��ҳ����Ļҳ��ͬ�����ݲ��䣬��������ʾ����
    {
        if(gchange_page_flag == TRUE)
        {
            set_cut_page_clock_timeout(gSys_tp.stay_time);    //��ҳ״̬�У������������ͬ��ҳ����ˢ����ֻ�ǰѼ�ʱ�����¼�ʱ
        }
        fast_refresh_flag=0;
        goto loop;
    }

    if(clear_event_flag_display_page == 0)
    {
        f_erase(F_BMP_BW);
        f_erase(F_BMP_RED);
        spit_fun(gSys_tp.present_page_id);
        if(dis2screen(FILE_SB,gdis_id_num) != 0)
        {
            Event_post(protocol_eventHandle, EVENT_FLAG_EPD_DISPLAY);
        }
    }

    loop:
    clear_event_flag_display_page=0;

}

UINT8 cmd_main_data_crc(void)
{
    UINT16 tp_crc =0;

    //������crc
    flash_all_data_crc(&tp_crc,F_BMP_DATA,gcmd_tp.len,gcmd_tp.start_addr );
    if((0 != memcmp((UINT8 *)&tp_crc,(UINT8 *)g_crc.crc_cmd , 2)))
    {
        gerr_info = TR3_STREAM_CRC_ERR;
        return 0;
    }
    //����ͼ��crc
    tp_crc =all_ele_crc();

    if(0 != memcmp((UINT8 *)&tp_crc,g_crc.crc_layer,2))
    {
        gerr_info = TR3_STREAM_CRC_ERR;
        return 0;
    }
    //ҳ�洢crc
    tp_crc =0;
    flash_all_data_crc(&tp_crc,gSys_tp.use_page_info_fileid,f_size(gSys_tp.use_page_info_fileid),0);
    if(0 != memcmp((UINT8 *)&tp_crc,g_crc.crc_page_info,2))
    {
        gerr_info = TR3_OSD_QUERY_PAGE_INFO_CRC_ERR;
        return 0;
    }
    //���ִ洢crc
    tp_crc =0;
    flash_all_data_crc(&tp_crc,gSys_tp.use_page_num_fileid,f_size(gSys_tp.use_page_num_fileid),0);
    if(0 != memcmp((UINT8 *)&tp_crc,g_crc.crc_page_num,2))
    {
        gerr_info = TR3_OSD_QUERY_PAGE_INFO_CRC_ERR;
        return 0;
    }

    return 1;
}

void screen_init(void)
{
    sys_rst_close_epd_fun();     //��ʼ����Ļ
    screen_analysis_fun(INFO_DATA.gRFInitData.screen_id);
    version_op5 = (UINT16)((INFO_DATA.gRFInitData.screen_id & 0xFFFF0000)>> 16);
    screen_colour = (SCREEN_COLOUR_T)screen_dis_t.src_color;
    g_screen_repeat_dis_flag = screen_dis_t.refresh_screen_flag;
    high_wide.high = screen_dis_t.h;
    high_wide.wide = screen_dis_t.w;
}


#endif







