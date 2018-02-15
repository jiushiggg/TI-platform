#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "mini_fs_conf.h"
#include "mini_fs.h"
#include "osd4_0_cmd.h"
#include "global_variable.h"
#include "crc16.h"
#include "gunzip.h"
#include "storage_struct.h"
#include "../aes128/aes128_osdcmd.h"
//#include "reed.h"

page_crc_t     osd4_g_crc;
osd_cmd_crc_t  g_cmd_crc;
int32_t cmd_err_fun(file_id_t cmdfd, UINT32 offset )
{
    return -1;
}

void clear_err_crc_page(uint8_t page_map)
{
    uint8_t i,temp;
    for(i=0; i<8; i++)
    {
        temp = 0x01<<i;
        if((0 == (page_map & temp))&&(gdsi_info_st[i].page_id == i))
        {
            set_erase_fileid_map_fun(gdsi_info_st[i].file_id);
            erase_file_fun();
            memset((UINT8 *)&gdsi_info_st[i],0xFF,sizeof(G_DIS_PAGE_T));
            gSys_tp.page_map[i] = 0xff;
        }
    }
}
#define  DATA_BIT_MAP_OFFSET               14
static int32_t cmd3_fun(file_id_t cmdfd, UINT32 offset )
{
    OSD40_LAYER_T tp;

    f_read(cmdfd, offset, (UINT8 *)&tp, sizeof(OSD40_LAYER_T));
    clear_err_crc_page(tp.page_map);                //擦除与上层系统不匹配的页
    memset(cache_buf,0xff,SCREEN_BUFF_SIZE);

    if(tp.property.iszip)
    {
        if (tp.uzip_len != gunzip(cmdfd, offset+ sizeof(OSD40_LAYER_T), tp.len - DATA_BIT_MAP_OFFSET, cache_buf))//解压缩数据
            goto here;
        if(tp.unzip_data_crc != my_cal_crc16(0,cache_buf,tp.uzip_len))      //检验压缩数据解压后的正确性
            goto here;
    }else
    {
        f_read(cmdfd, offset+ sizeof(OSD40_LAYER_T), cache_buf, tp.uzip_len);
    }
    if(TRUE != read_dispage_id_data(tp.pageid))//读对应页数据
        goto here;
    if(tp.property.color == CLOLOR_BW)    //如果是黑白页的数据，记录之前此页是放在flash中的哪个文件中
    {
        before_page_file = gdsi_info_st[tp.pageid].file_id;
    }
    if(TRUE != check_page_correctness(tp.pageid,gdsi_info_st[tp.pageid].page_id))//比较页数据是否真确
        goto here;
    if(TRUE != integration_disdata_fun(cache_buf,&gdsi_info_st[tp.pageid],&tp))//整合数据
        goto here;
    if(TRUE != write_dispage_file_fun(&gdsi_info_st[tp.pageid], &tp))//写入数据
        goto here;

    gSys_tp.page_map[tp.pageid] = tp.pageid;//保存所有页码，页码下标对应响应的页号
    gSys_tp.change_map |= (0x01<<tp.pageid);      //记录改变的页码
    return tp.len + 5;

    here:

    gerr_info = TR5_WRITE_FLASH_ERR;
    set_erase_fileid_map_fun(gdsi_info_st[tp.pageid].file_id);
    erase_file_fun();
    memset((UINT8 *)&gdsi_info_st[tp.pageid],0xFF,sizeof(G_DIS_PAGE_T));
    gSys_tp.page_map[tp.pageid] = 0xff;
    return tp.len + 5;

}

static bool save_cmd1_cmd5(UINT8 default_page_id)
{

    if(gchange_page_flag == FALSE)
    {
        Event_post(protocol_eventHandle, EVENT_FALG_DISPLAY_PAGE);
        gpage.flag = FALSE;//停止之前的计数

        gSys_tp.stay_time = 0;
        gSys_tp.stay_time_cont = 0;
        gSys_tp.present_page_id = default_page_id;
    }
    gSys_tp.default_page_id = default_page_id;
    return TRUE;
}
static int32_t cmd5_fun(file_id_t cmdfd, UINT32 offset )
{
    dis_page_cmd_t tp;

    f_read(cmdfd, offset, (UINT8 *)&tp, sizeof(dis_page_cmd_t));
    clear_err_crc_page(tp.page_id_bit_map);                   //擦除与上层系统不匹配的页
    if(FALSE ==save_cmd1_cmd5(tp.default_page_id))
        return  -1;
    return sizeof(dis_page_cmd_t);
}


static int32_t cmd7_fun(file_id_t cmdfd, UINT32 offset )
{
#define RC_DATA_START_ADDR 4

    UINT16 len = 0;
    UINT8 temp_buf[32];

    f_read(cmdfd, offset, temp_buf, 30);
    offset += 1;
    memcpy((UINT8 *)&len,temp_buf+1,sizeof(len));
    memcpy((UINT8 *)rc_attr_info.secur_code,temp_buf+1+sizeof(len),20);

    if(rc_attr_info.display_time)
    {
        gSys_tp.present_page_id = rc_attr_info.page_num;
        gSys_tp.stay_time = rc_attr_info.display_time;
        gSys_tp.stay_time_cont = 0;
        gchange_page_flag = TRUE;
        gpage.flag = FALSE;//停止之前的计数
        Event_post(protocol_eventHandle, EVENT_FALG_DISPLAY_PAGE);
    }


    rc_led_init();

    return (1 + len + sizeof(len));

}
static int32_t cmd8_fun(file_id_t cmdfd, UINT32 offset )
{
    UINT16 len = 0,crc= 0;
    UINT8 temp_buf[32];

    f_read(cmdfd, offset, temp_buf, 30);
    offset += 1;
    memcpy((UINT8 *)&len,temp_buf+1,sizeof(len));
    memcpy((UINT8 *)&epd_attr_info,temp_buf+1+sizeof(len),sizeof(epd_attr_info));
    epd_attr_info.global_crc = my_cal_crc16(crc,(UINT8 *)&epd_attr_info,sizeof(epd_attr_info)-sizeof(epd_attr_info.global_crc));
    save_state_info_fun();
    return (1 + len + sizeof(len));
}

typedef int32_t (*cmd_cb_t) (file_id_t cmdfd, UINT32 offset);
static const cmd_cb_t cmd_cb[] = {cmd_err_fun,cmd_err_fun,cmd_err_fun,cmd3_fun,cmd_err_fun,cmd5_fun,cmd_err_fun,cmd7_fun,cmd8_fun,cmd_err_fun,cmd10_fun,cmd11_fun,cmd12_fun,cmd13_fun};

static int32_t cmd_order(file_id_t cmdfd, UINT32 offset)
{
    UINT8 cmd = 0;
    f_read(cmdfd,offset, &cmd, 1);
    if (cmd == OSD_END_CMD)
    {
        gcmd_tp.len = offset + 1; //cmd0x76
        f_read(cmdfd, offset+1, (UINT8 *)&g_cmd_crc , sizeof(osd_cmd_crc_t));
        return 0;
    }
    if((cmd <= OSD_CMD_SUN) && (cmd != 0))
        return cmd_cb[cmd](cmdfd, offset);
    else
    {
        if(gerr_info !=NONEERR )
            gerr_info = TR3_OSD_CMDERR;
    }
    return -1;
}



//osd4.0的入口地址

bool process_cmd(file_id_t cmdfd)
{
    UINT32 i = 0;
    int32_t n = 1;
    gflash_empty = 0;
    for (i = 0; n > 0; i += n)
    {
        n = cmd_order(cmdfd, i);
    }

    if(n < 0)
        return FALSE;
    return TRUE;
}

void osd_init(void)
{
    write_temp_buff_id = (file_id_t)set_cursor_offset();
    memset(pkg_bit_map,0xff,G_PKG_BIT_MAP_LEN);
    gerr_info = NONEERR;
    memset((UINT8 *)&gosd_pkg,0x00,sizeof(gosd_pkg));//删除osd数据包统计函数
    memset((UINT8 *)&osd4_g_crc,0x00,sizeof(osd4_g_crc));
    storage_struct_init();
}


void sid_change_eraflag(void)
{
    file_id_t temp;
    memset(pkg_bit_map,0xff,G_PKG_BIT_MAP_LEN);
    gsid.old_sid = gsid.now_sid;
    gsid.old_sub_sid = gsid.now_sub_sid;
    gerr_info = NONEERR;
    gFlag_bit =1;
    clear_gpkg_fun();//不能删除，原因是osd更新时，sid不同时表示一个新的周期
    memset((UINT8 *)&gosd_pkg,0x00,sizeof(gosd_pkg));//删除osd数据包统计函数
    memset((UINT8 *)&g_upgread_pkg,0x00,sizeof(g_upgread_pkg));//删除广播升级数据包统计函数
    temp = write_temp_buff_id;
    write_temp_buff_id = (file_id_t)set_cursor_offset();
    set_used_fileid_map_fun(write_temp_buff_id);     //标记此文件的位置已被使用
    if(temp != write_temp_buff_id)          //防止第一次上电更新时，占用的是第0个文件，标记的擦除也是第0个文件，导致刚接收到数据，还未解析就被擦除
    {
        set_erase_fileid_map_fun(temp);    //标记此文件的位置需要擦除
    }
    first_lose_pkg = 0;
}

void upgread_id_change_eraflag(void)
{
    memset(pkg_bit_map,0xff,G_PKG_BIT_MAP_LEN);
    gsid.old_sid = gsid.now_sid;
    gsid.old_sub_sid = gsid.now_sub_sid;
    gerr_info = NONEERR;
    clear_gpkg_fun();//不能删除，原因是osd更新时，sid不同时表示一个新的周期
    memset((UINT8 *)&gosd_pkg,0x00,sizeof(gosd_pkg));//删除osd数据包统计函数
    memset((UINT8 *)&g_upgread_pkg,0x00,sizeof(g_upgread_pkg));//删除广播升级数据包统计函数
    first_lose_pkg = 0;
}

void event_128_fun(void)
{
    UINT8 i, tp;

    switch (eraser_file_flag)
    {
    case 0xFF:
        fs_erase_all();
        f_init_check(1);
        f_init();
        memset((UINT8 *)gdsi_info_st,0xFF,sizeof(G_DIS_PAGE_T)*MAX_DISPLAY_PGAE_SUM);
        memset((UINT8 *)&gpage_info_st,0x00,sizeof(ALL_PAGE_INFO_T));
        gerr_info = NONEERR;
        gchange_page_flag = FALSE;
        memset(gSys_tp.page_map,0xff,8);
        save_sys_load_page_info();
        clear_gpkg_fun();
        memset((UINT8 *)&gosd_pkg,0x00,sizeof(gosd_pkg));//删除osd数据包统计函数
        memset((UINT8 *)&g_upgread_pkg,0x00,sizeof(g_upgread_pkg));//删除广播升级数据包统计函数
        memset((UINT8 *)&gsid, 0x00,sizeof(SID_T));
        memset(pkg_bit_map,0xff,G_PKG_BIT_MAP_LEN);
        gflash_empty = 0;
        break;
    default:
        for(i=0;i<8;i++)
        {
            tp = (0x01 << i);
            if(eraser_file_flag & tp)
            {
                if(gdsi_info_st[i].file_id >= file_num)
                    continue;
                set_erase_fileid_map_fun(gdsi_info_st[i].file_id);    //标记此文件的位置需要擦除
                erase_file_fun();
                memset((UINT8 *)&gdsi_info_st[i],0xFF,sizeof(G_DIS_PAGE_T));
                gSys_tp.page_map[i]=0xff;
            }
        }
        save_sys_load_page_info();
        clear_gpkg_fun();
        memset((UINT8 *)&gsid, 0x00,sizeof(SID_T));
        break;
    }
}

RF_CMD_T get_osd_76cmd_fun(UINT8 *buf)
{

    ret_ack_flag = RF_EVENT_OSD_76CMD;
    UINT8 tp, i;

	if(gsys_reset_flag)
    {
        gerr_info = TR3_OSD_QUERY_ERR;
        return RF_FSM_CMD_RX_DATA;

    }
    memcpy((UINT8 *)&osd4_g_crc, buf + 5, sizeof(osd4_g_crc));
    page_crc_map = 0xff;                    //1表示错误，0表示正确
    for(i=0;i< MAX_DISPLAY_PGAE_SUM;i++)
    {
        tp = 1;
        if((0 != memcmp((UINT8 *)(osd4_g_crc.crc_page0 + 2*i), (UINT8 *)&gdsi_info_st[i].page_crc , 2)))
        {
            tp = 0;
            gerr_info = TR3_STREAM_CRC_ERR;
        }
        page_crc_map &= ~(tp<<i);
    }
    if(osd4_g_crc.default_page_id != gSys_tp.default_page_id)
    {
        gerr_info = TR3_DEFAULT_ID_ERR;
    }

    return RF_FSM_CMD_RX_DATA;
}
