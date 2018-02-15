#include <string.h>
#include "global_variable.h"
#include "storage_struct.h"
#include "../screen/lattice_epd/scrn_ic1675a.h"
#include "../screen/lattice_epd/scrn_public_fun.h"


#define ERR_CURSOR  0xFE


ALL_PAGE_INFO_T gpage_info_st;
G_DIS_PAGE_T  gdsi_info_st[MAX_DISPLAY_PGAE_SUM]= {0};



//����δʹ�õĻ���ҳ
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
            gpage_info_st.need_erase_page_map &= (0xffffffff ^ tp);//���������ҳ��־
            gpage_info_st.used_page_map  &= (0xffffffff ^ tp);//���ʹ�õ�ҳ��־
        }
        i++;
    }
}


//����Ҫ����ҳ��bitmap
void set_erase_fileid_map_fun(UINT8 id_map)
{
    gpage_info_st.need_erase_page_map |= ((UINT32)0x01<<id_map);
}

//����ռ��ҳ��bitmap
void set_used_fileid_map_fun(UINT8 id_map)
{
    gpage_info_st.used_page_map |= ((UINT32)0x01<<id_map);
}


//ϵͳ�ϵ����
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
        if(ret==0)    //������ͨ���ļ�ϵͳ��ȡ�Ļ����᷵�ض�ȡ����0��������δд��Ͷ�ȡ
        {
            memset((UINT8 *)&temp, 0xFF, sizeof(G_DIS_PAGE_T));
        }

        switch(temp.file_featrue)
        {
        case FEATURES_DISPALY:
            if((temp.page_id > MAX_DISPLAY_PGAE_SUM)||(temp.file_id >= file_num)||(temp.color >= CLOLOR_MAX_NUM))    //ȷ����flash�м��ص�����Ϣ���ǺϷ���
            {
                set_erase_fileid_map_fun(i);//������Ҫ���ҳbitmap
                break;
            }
            gpage_info_st.used_page_map |= ((UINT32)0x01<<dis_page_tp);//���ñ�ռ�õ�ҳbitmap
            gdsi_info_st[temp.page_id] = temp;//�Ѻڰ����ݱ��浽��Ӧ��ҳ����

            gpage_info_st.file_id_cursor = (gpage_info_st.file_id_cursor < gdsi_info_st[temp.page_id].file_id)?gdsi_info_st[temp.page_id].file_id : gpage_info_st.file_id_cursor;
            break;

        default:

            set_erase_fileid_map_fun(i);//������Ҫ���ҳbitmap
            break;

        }
        dis_page_tp++;
    }
    //��������ҳ
    erase_file_fun();
}

//�ҿ��ļ���ƫ�Ƶ�ַ
UINT8 set_cursor_offset(void)
{
    UINT8 i=0;


    for( i = gpage_info_st.file_id_cursor; i< file_num;i ++)//���α��λ�������
    {
        if((gpage_info_st.used_page_map  & ((UINT32)0x01<< i)) ==0)//�ҵ���һΪ0��bit
        {
            gpage_info_st.file_id_cursor = i;
            return   i;
        }
    }

    if(gpage_info_st.file_id_cursor != 0)//���Ǵ���ʼλ���ң������ҵ��ļ�����ʱ��δ�ҵ������ļ�
    {
        for( i = 0; i< gpage_info_st.file_id_cursor;i ++)//�ӿ�ʼ��ַ�ҵ��α괦
        {
            if((gpage_info_st.used_page_map  & ((UINT32)0x01<< i)) ==0)//�ҵ���һΪ0��bit
            {
                gpage_info_st.file_id_cursor = i;
                return  i;
            }
        }
    }

    return file_num;

}

//��ȡflash�е�ҳ����
BOOL read_dispage_id_data(UINT8 pageid)
{
    const UINT8 tp[]={0xff,0xff,0xff,0xff,0xff,0xff};
    UINT16 len = offsetof(G_DIS_PAGE_T,page_info);

    if((gdsi_info_st[pageid].page_id >=  MAX_DISPLAY_PGAE_SUM) && (gdsi_info_st[pageid].page_id != 0xff))//������ʾҳͬʱ���ǿ�ҳʱ����
        return FALSE;

    if(memcmp((void const *)&gdsi_info_st[pageid],tp,len) == 0)//�ǲ��ǿհ�ҳ
        return TRUE;

    f_read((file_id_t)gdsi_info_st[pageid].file_id,0,(UINT8 *)&gdsi_info_st[pageid],offsetof(G_DIS_PAGE_T,page_info));//����ҳ��ȡ����

    return TRUE;

}


BOOL check_page_correctness(UINT8 cmd_pageid ,UINT8 src_pageid)
{
    if((src_pageid >= MAX_DISPLAY_PGAE_SUM ) &&(src_pageid != 0xff) )//ҳ����� ,������0xff����ʾ���ǿհ�ҳ
        return FALSE;
    if(cmd_pageid  >= MAX_DISPLAY_PGAE_SUM )//ҳ�����
        return FALSE;
    if((cmd_pageid != src_pageid) && (src_pageid != 0xff) )//ҳ�벻ͬ��������
        return FALSE;
    return TRUE;
}

//����������ҳ����ͼ�ǩ��ԭʼ��������
BOOL integration_disdata_fun(UINT8 *cmd_srcbuff,G_DIS_PAGE_T *page_tp,OSD40_LAYER_T *tp)//flag ��д�����滻
{

    UINT16 i = 0,j,len,wirte_size=LOAD_BUFF_SIZE,arrt_len, offset = CLOLOR_RED_OFFSET;
    file_id_t temp_file_id;                    //���ĸ��ļ��л�ȡ����
    len = (screen_dis_t.w * ((screen_dis_t.h+7)/8*8)) /8;
    arrt_len = offsetof(G_DIS_PAGE_T,page_info);
    if(tp->property.color == CLOLOR_YELLOW)
    {
        tp->property.color = CLOLOR_RED;
    }

    if(tp->property.color == CLOLOR_BW)
    {
        offset = 0;
        page_tp->page_crc =0; // ����page_tp->page_info ��crc
        black_white_crc = 0;  //�����ɫ��ǩ���ڰ���Ļ��crc
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
    if((screen_dis_t.src_color == BLACK_WHITE_RED)&&(tp->property.color == CLOLOR_BW))  //��ɫ��ǩ�������Ǻ�ɫ������
    {
        black_white_crc = page_tp->page_crc;
        goto here;
    }

    if(page_tp->page_crc != tp->page_crc)   //����ҳ��crc��ȷ�����ݵ���ȷ�Ժ�������
    {
        return FALSE;
    }

    here:
    page_tp->page_id =  tp->pageid;
    page_tp->page_info =  cmd_srcbuff ;//ָ�븳ֵ
    page_tp->file_featrue = FEATURES_DISPALY;
    page_tp->color = (SCREEN_PAGE_COLOR_T)tp->property.color;
    return TRUE;
}

BOOL write_dispage_file_fun(G_DIS_PAGE_T *page_tp, OSD40_LAYER_T *tp)   //flag ��д�����滻
{
    file_id_t erase_file_id = page_tp->file_id;
    UINT16 len = offsetof(G_DIS_PAGE_T,page_info);
    if(tp->property.color == CLOLOR_YELLOW)
        tp->property.color = CLOLOR_RED;
    switch(tp->property.color)
    {
    case CLOLOR_BW:
        page_tp->file_id = (file_id_t)set_cursor_offset();//Ѱ�ҿհ�ҳ

        if(page_tp->file_id == file_num)  //û�п����ļ�
            return FALSE;

        //��ԭҳ����
        if(page_tp->file_id != erase_file_id)//�����д���ҳ�������ļ�ID��֮ǰ�Ĳ�һ����Ҫ��ԭ����id������������
            set_erase_fileid_map_fun(erase_file_id);
        if(screen_dis_t.src_color == BLACK_WHITE)
        {
            erase_file_fun();
            f_write((file_id_t) page_tp->file_id,0,(UINT8 *)page_tp,offsetof(G_DIS_PAGE_T,page_info));//�ڰ׼�ǩд��ҳ����
        }else
        {
            f_write((file_id_t) page_tp->file_id,0,(UINT8 *)page_tp,offsetof(G_DIS_PAGE_T,page_crc));//��ɫ��ǩд�벿��ҳ����
        }
        f_write((file_id_t) page_tp->file_id,len, page_tp ->page_info,(screen_dis_t.w * ((screen_dis_t.h+7)/8*8) )/8);//д��ҳ����
        set_used_fileid_map_fun(page_tp->file_id);  //������д����ļ�Ϊռ��
        break;

    case CLOLOR_RED:
        f_write((file_id_t) page_tp->file_id,offsetof(G_DIS_PAGE_T,page_crc),(UINT8 *)&page_tp->page_crc,  offsetof(G_DIS_PAGE_T,page_info) - offsetof(G_DIS_PAGE_T,page_crc));//д���ҳ��CRC
        f_write((file_id_t) page_tp->file_id,len + CLOLOR_RED_OFFSET, page_tp ->page_info,(screen_dis_t.w * ((screen_dis_t.h+7)/8*8) ) /8);//д��ҳ����
        set_used_fileid_map_fun(page_tp->file_id);  //������д����ļ�Ϊռ��
        erase_file_fun();
        break;

    default:
        break;
    }
    return TRUE;
}


