#include <string.h>
#include "global_variable.h"
#include "storage_struct.h"
#include "../screen/lattice_epd/scrn_ic1675a.h"
#include "../screen/lattice_epd/scrn_public_fun.h"


#define ERR_CURSOR  0xFE


ALL_PAGE_INFO_T gpage_info_st;
G_DIS_PAGE_T  gdsi_info_st[MAX_DISPLAY_PGAE_SUM]= {0};



//擦除未使用的缓存页
void erase_file_fun(void)
{
    UINT8 i=0;
    UINT32 tp=0;

    while(gpage_info_st.need_erase_page_map)
    {
        tp =(UINT32)0x01 << i;
        if(gpage_info_st.need_erase_page_map & tp )
        {
            f_erase((file_id_t)i);
            gpage_info_st.need_erase_page_map &= (0xffffffff ^ tp);//清除擦除的页标志
            gpage_info_st.used_page_map  &= (0xffffffff ^ tp);//清除使用的页标志
        }
        i++;
    }
}


//设置要擦除页的bitmap
void set_erase_fileid_map_fun(UINT8 id_map)
{
    gpage_info_st.need_erase_page_map |= ((UINT32)0x01<<id_map);
}

//设置占用页的bitmap
void set_used_fileid_map_fun(UINT8 id_map)
{
    gpage_info_st.used_page_map |= ((UINT32)0x01<<id_map);
}


//系统上电加载
void storage_struct_init(void)
{
    UINT8 ret, i = 0,dis_page_tp=0;
    G_DIS_PAGE_T  temp;

    memset((UINT8 *)&gdsi_info_st,0xFF,sizeof(G_DIS_PAGE_T)*MAX_DISPLAY_PGAE_SUM);
    memset((UINT8 *)&gpage_info_st,0x00,sizeof(ALL_PAGE_INFO_T));

    gpage_info_st.file_id_cursor = 0x00;
    for(i = 0 ; i < file_num ; i++ )
    {
        ret = f_read((file_id_t)i,0,(UINT8 *)&temp,sizeof(G_DIS_PAGE_T));
        if(ret==0)    //擦除后通过文件系统读取的话，会返回读取长度0，不允许未写入就读取
        {
            memset((UINT8 *)&temp, 0xFF, sizeof(G_DIS_PAGE_T));
        }

        switch(temp.file_featrue)
        {
        case FEATURES_DISPALY:
            if((temp.page_id > MAX_DISPLAY_PGAE_SUM)||(temp.file_id >= file_num)||(temp.color >= CLOLOR_MAX_NUM))    //确保重flash中加载到的信息都是合法的
            {
                set_erase_fileid_map_fun(i);//设置需要清除页bitmap
                break;
            }
            gpage_info_st.used_page_map |= ((UINT32)0x01<<dis_page_tp);//设置被占用的页bitmap
            gdsi_info_st[temp.page_id] = temp;//把黑白内容保存到对应的页码上

            gpage_info_st.file_id_cursor = (gpage_info_st.file_id_cursor < gdsi_info_st[temp.page_id].file_id)?gdsi_info_st[temp.page_id].file_id : gpage_info_st.file_id_cursor;
            break;

        default:

            set_erase_fileid_map_fun(i);//设置需要清除页bitmap
            break;

        }
        dis_page_tp++;
    }
    //擦除缓存页
    erase_file_fun();
}

//找空文件的偏移地址
UINT8 set_cursor_offset(void)
{
    UINT8 i=0;


    for( i = gpage_info_st.file_id_cursor; i< file_num;i ++)//从游标的位置向后找
    {
        if((gpage_info_st.used_page_map  & ((UINT32)0x01<< i)) ==0)//找到第一为0的bit
        {
            gpage_info_st.file_id_cursor = i;
            return   i;
        }
    }

    if(gpage_info_st.file_id_cursor != 0)//不是从起始位置找，并且找到文件结束时都未找到可用文件
    {
        for( i = 0; i< gpage_info_st.file_id_cursor;i ++)//从开始地址找到游标处
        {
            if((gpage_info_st.used_page_map  & ((UINT32)0x01<< i)) ==0)//找到第一为0的bit
            {
                gpage_info_st.file_id_cursor = i;
                return  i;
            }
        }
    }

    return file_num;

}

//读取flash中的页数据
BOOL read_dispage_id_data(UINT8 pageid)
{
    const UINT8 tp[]={0xff,0xff,0xff,0xff,0xff,0xff};
    UINT16 len = offsetof(G_DIS_PAGE_T,page_info);

    if((gdsi_info_st[pageid].page_id >=  MAX_DISPLAY_PGAE_SUM) && (gdsi_info_st[pageid].page_id != 0xff))//大于显示页同时不是空页时错误
        return FALSE;

    if(memcmp((void const *)&gdsi_info_st[pageid],tp,len) == 0)//是不是空白页
        return TRUE;

    f_read((file_id_t)gdsi_info_st[pageid].file_id,0,(UINT8 *)&gdsi_info_st[pageid],offsetof(G_DIS_PAGE_T,page_info));//正常页读取数据

    return TRUE;

}


BOOL check_page_correctness(UINT8 cmd_pageid ,UINT8 src_pageid)
{
    if((src_pageid >= MAX_DISPLAY_PGAE_SUM ) &&(src_pageid != 0xff) )//页码错误 ,不等于0xff，表示不是空白页
        return FALSE;
    if(cmd_pageid  >= MAX_DISPLAY_PGAE_SUM )//页码错误
        return FALSE;
    if((cmd_pageid != src_pageid) && (src_pageid != 0xff) )//页码不同不能整合
        return FALSE;
    return TRUE;
}

//将命令流的页数句和价签的原始数据整合
BOOL integration_disdata_fun(UINT8 *cmd_srcbuff,G_DIS_PAGE_T *page_tp,OSD40_LAYER_T *tp)//flag 复写还是替换
{

    UINT16 i = 0,j,len,wirte_size=LOAD_BUFF_SIZE,arrt_len, offset = CLOLOR_RED_OFFSET;
    file_id_t temp_file_id;                    //从哪个文件中获取数据
    len = (screen_dis_t.w * ((screen_dis_t.h+7)/8*8)) /8;
    arrt_len = offsetof(G_DIS_PAGE_T,page_info);
    if(tp->property.color == CLOLOR_YELLOW)
    {
        tp->property.color = CLOLOR_RED;
    }

    if(tp->property.color == CLOLOR_BW)
    {
        offset = 0;
        page_tp->page_crc =0; // 计算page_tp->page_info 的crc
        black_white_crc = 0;  //清除红色价签，黑白屏幕的crc
        temp_file_id = page_tp->file_id;
    }
    if(tp->property.color == CLOLOR_RED)
    {
        page_tp->page_crc = black_white_crc;
        temp_file_id = before_page_file;
    }
    if((screen_dis_t.src_color == BLACK_WHITE)&&(tp->property.color == CLOLOR_RED))
    {
        gerr_info = TR3_CORLOR_ERR;
        return FALSE;
    }
    for(i=0;i < len;)
    {
        if(tp->property.operation_type)
        {
            f_read(temp_file_id,i+arrt_len+offset,cache_buf_1,wirte_size);
            for(j=0;j < wirte_size; j++ )
            {
                cmd_srcbuff[i+j] ^= cache_buf_1[j];
            }
        }
        page_tp->page_crc = my_cal_crc16(page_tp->page_crc,cmd_srcbuff + i,wirte_size);
        i += wirte_size;
        if( (len - i) < LOAD_BUFF_SIZE )
            wirte_size  = len - i;
        else
            wirte_size  = LOAD_BUFF_SIZE ;
    }
    if((screen_dis_t.src_color == BLACK_WHITE_RED)&&(tp->property.color == CLOLOR_BW))  //红色价签且数据是黑色的数据
    {
        black_white_crc = page_tp->page_crc;
        goto here;
    }

    if(page_tp->page_crc != tp->page_crc)   //检验页的crc，确保数据的正确性和完整性
    {
        return FALSE;
    }

    here:
    page_tp->page_id =  tp->pageid;
    page_tp->page_info =  cmd_srcbuff ;//指针赋值
    page_tp->file_featrue = FEATURES_DISPALY;
    page_tp->color = (SCREEN_PAGE_COLOR_T)tp->property.color;
    return TRUE;
}

BOOL write_dispage_file_fun(G_DIS_PAGE_T *page_tp, OSD40_LAYER_T *tp)   //flag 复写还是替换
{
    file_id_t erase_file_id = page_tp->file_id;
    UINT16 len = offsetof(G_DIS_PAGE_T,page_info);
    if(tp->property.color == CLOLOR_YELLOW)
        tp->property.color = CLOLOR_RED;
    switch(tp->property.color)
    {
    case CLOLOR_BW:
        page_tp->file_id = (file_id_t)set_cursor_offset();//寻找空白页

        if(page_tp->file_id == file_num)  //没有空余文件
            return FALSE;

        //复原页属性
        if(page_tp->file_id != erase_file_id)//如果新写入的页的物理文件ID和之前的不一样需要把原来的id的物理区擦除
            set_erase_fileid_map_fun(erase_file_id);
        if(screen_dis_t.src_color == BLACK_WHITE)
        {
            erase_file_fun();
            f_write((file_id_t) page_tp->file_id,0,(UINT8 *)page_tp,offsetof(G_DIS_PAGE_T,page_info));//黑白价签写入页属性
        }else
        {
            f_write((file_id_t) page_tp->file_id,0,(UINT8 *)page_tp,offsetof(G_DIS_PAGE_T,page_crc));//红色价签写入部分页属性
        }
        f_write((file_id_t) page_tp->file_id,len, page_tp ->page_info,(screen_dis_t.w * ((screen_dis_t.h+7)/8*8) )/8);//写入页数据
        set_used_fileid_map_fun(page_tp->file_id);  //设置新写入的文件为占用
        break;

    case CLOLOR_RED:
        f_write((file_id_t) page_tp->file_id,offsetof(G_DIS_PAGE_T,page_crc),(UINT8 *)&page_tp->page_crc,  offsetof(G_DIS_PAGE_T,page_info) - offsetof(G_DIS_PAGE_T,page_crc));//写入此页的CRC
        f_write((file_id_t) page_tp->file_id,len + CLOLOR_RED_OFFSET, page_tp ->page_info,(screen_dis_t.w * ((screen_dis_t.h+7)/8*8) ) /8);//写入页数据
        set_used_fileid_map_fun(page_tp->file_id);  //设置新写入的文件为占用
        erase_file_fun();
        break;

    default:
        break;
    }
    return TRUE;
}


