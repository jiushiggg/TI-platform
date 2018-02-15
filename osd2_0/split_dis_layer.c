#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdint.h>
#include "mini_fs_conf.h"
#include "mini_fs.h"
#include "osd2_0_cmd.h"
#include "bit_display.h"
#include "global_variable.h"
#include "crc16.h"
#include "three_event.h"
//#include "reed.h"
#include "global_variable.h"


void num_block_display_fun_0(const UINT8 dire ,const  UINT8 dis_format,UINT16 all_w, UINT32 first_bit_addr,const font_arrt_t *p, property_t *pro_t,const UINT8 h ,const UINT8 w)
{
    esl_screen_block_t sb;
    UINT16 b_w,b_h;


    f_read(F_SB,(gdis_id_num - 1)* sizeof(esl_screen_block_t),(UINT8 *)&sb,sizeof(esl_screen_block_t));//读取要补充数字的sb属性

    //水平最左边的块
    b_w = first_bit_addr / screen_height;

    if(b_w >= p->sx)  //若果不是x轴的起始点才证明有最左边的块
    {

        sb.srcfd = F_BMP_DATA;//未使用参数
        sb.src_startbit = 0;//未使用参数

        sb.w = b_w - p->sx +1;
        sb.h = p->ey - p->sy +1;//水平方向的高度
        sb.dst_startbit  = p->sx * screen_height + p->sy;
        if(sb.background_color == 0)   //黑底补黑
        {
            sb.dstfd = F_BMP_BW;
            sb.attr.ispadd = 1;
            sb.attr.paddinfo =0;
        }
        if(sb.background_color == 1)   //白底补白
        {
            sb.dstfd = F_BMP_BW;
            sb.attr.ispadd = 1;
            sb.attr.paddinfo =1;
        }
        if(sb.background_color == 2)   //红底补红
        {
            sb.dstfd = F_BMP_RED;
            sb.attr.ispadd = 1;
            sb.attr.paddinfo =0;
        }
        f_write_direct(FILE_SB,(gdis_id_num)*sizeof(esl_screen_block_t),(UINT8 *)&sb,sizeof(esl_screen_block_t));
        gdis_id_num++;
    }

    //水平最右面的块
    b_w  =( b_w + all_w);//若果不是x轴的结束点才证明有最右边的块
    if(b_w <= p->ex)
    {
        sb.srcfd = F_BMP_DATA;//未使用参数
        sb.src_startbit = 0;//未使用参数

        sb.w = p->ex - b_w +1;
        sb.h = p->ey - p->sy +1;//水平方向的高度
        sb.dst_startbit  = b_w * screen_height + p->sy;
        if(sb.background_color == 0)   //黑底补黑
        {
            sb.dstfd = F_BMP_BW;
            sb.attr.ispadd = 1;
            sb.attr.paddinfo =0;
        }
        if(sb.background_color == 1)   //白底补白
        {
            sb.dstfd = F_BMP_BW;
            sb.attr.ispadd = 1;
            sb.attr.paddinfo =1;
        }
        if(sb.background_color == 2)   //红底补红
        {
            sb.dstfd = F_BMP_RED;
            sb.attr.ispadd = 1;
            sb.attr.paddinfo =0;
        }
        f_write_direct(FILE_SB,(gdis_id_num)*sizeof(esl_screen_block_t),(UINT8 *)&sb,sizeof(esl_screen_block_t));
        gdis_id_num++;
    }

    //垂直上面
    b_w = first_bit_addr / screen_height;
    b_h = first_bit_addr - (b_w * screen_height);//用除法和减法代替取余数，减小代码量，结尾不加1的目的是领结边框点不做为块返显示内容

    if(b_h >= p->sy)//若果不是Y轴的起始点才证明有最上面的块
    {
        sb.srcfd = F_BMP_DATA;//未使用参数
        sb.src_startbit = 0;//未使用参数
        sb.w = p->ex - p->sx +1;
        sb.h = b_h -p->sy +1;//水平方向的高度
        sb.dst_startbit  = b_w * screen_height + p->sy;
        if(sb.background_color == 0)   //黑底补黑
        {
            sb.dstfd = F_BMP_BW;
            sb.attr.ispadd = 1;
            sb.attr.paddinfo =0;
        }
        if(sb.background_color == 1)   //白底补白
        {
            sb.dstfd = F_BMP_BW;
            sb.attr.ispadd = 1;
            sb.attr.paddinfo =1;
        }
        if(sb.background_color == 2)   //红底补红
        {
            sb.dstfd = F_BMP_RED;
            sb.attr.ispadd = 1;
            sb.attr.paddinfo =0;
        }
        f_write_direct(FILE_SB,(gdis_id_num)*sizeof(esl_screen_block_t),(UINT8 *)&sb,sizeof(esl_screen_block_t));
        gdis_id_num++;
    }

    //垂直下面

    b_h += h;//上半部分的高度加上字体的高度

    if(b_h <= p->ey)//若果不是Y轴的起始点才证明有最上面的块
    {
        sb.srcfd = F_BMP_DATA;//未使用参数
        sb.src_startbit = 0;//未使用参数

        sb.w = p->ex - p->sx +1;
        sb.h = p->ey-b_h+1;//水平方向的高度
        sb.dst_startbit  = b_w * screen_height + b_h;
        if(sb.background_color == 0)   //黑底补黑
        {
            sb.dstfd = F_BMP_BW;
            sb.attr.ispadd = 1;
            sb.attr.paddinfo =0;
        }
        if(sb.background_color == 1)   //白底补白
        {
            sb.dstfd = F_BMP_BW;
            sb.attr.ispadd = 1;
            sb.attr.paddinfo =1;
        }
        if(sb.background_color == 2)   //红底补红
        {
            sb.dstfd = F_BMP_RED;
            sb.attr.ispadd = 1;
            sb.attr.paddinfo =0;
        }
        f_write_direct(FILE_SB,(gdis_id_num)*sizeof(esl_screen_block_t),(UINT8 *)&sb,sizeof(esl_screen_block_t));
        gdis_id_num++;
    }

}

void num_block_display_fun_180(const UINT8 dire ,const  UINT8 dis_format,UINT16 all_w, UINT32 first_bit_addr,const font_arrt_t *p, property_t *pro_t,const UINT8 h ,const UINT8 w)
{
    esl_screen_block_t sb;
    UINT16 b_w,b_h;
    UINT32 dst_addr;


    f_read(F_SB,(gdis_id_num-1) * sizeof(esl_screen_block_t),(UINT8 *)&sb,sizeof(esl_screen_block_t));//读取要补充数字的sb属性

    //水平最左边的块
    b_w = sb.dst_startbit / screen_height;
    dst_addr = sb.dst_startbit;

    if(b_w >= p->sx)  //必须在x轴起始点的右边
    {

        sb.srcfd = F_BMP_DATA;//未使用参数
        sb.src_startbit = 0;//未使用参数

        sb.w = b_w - p->sx +1;;//要显示的起始点减去x轴的起始坐标
        sb.h = p->ey - p->sy +1 ;//水平方向的高度
        sb.dst_startbit  =  p->sx * screen_height + p->sy;
        if(sb.background_color == 0)   //黑底补黑
        {
            sb.dstfd = F_BMP_BW;
            sb.attr.ispadd = 1;
            sb.attr.paddinfo =0;
        }
        if(sb.background_color == 1)   //白底补白
        {
            sb.dstfd = F_BMP_BW;
            sb.attr.ispadd = 1;
            sb.attr.paddinfo =1;
        }
        if(sb.background_color == 2)   //红底补红
        {
            sb.dstfd = F_BMP_RED;
            sb.attr.ispadd = 1;
            sb.attr.paddinfo =0;
        }
        f_write_direct(FILE_SB,(gdis_id_num)*sizeof(esl_screen_block_t),(UINT8 *)&sb,sizeof(esl_screen_block_t));
        gdis_id_num++;
    }

    //水平最右面的块
    b_w  =( b_w + all_w);//要显示的第一个字符的位置加上所有字符的宽度
    if(b_w <= p->ex)//必须在x轴结束点的左边
    {
        sb.srcfd = F_BMP_DATA;//未使用参数
        sb.src_startbit = 0;//未使用参数

        sb.w =  p->ex - b_w +1;;//结束坐标减去所有结束字符的位置
        sb.h = p->ey - p->sy +1;//水平方向的高度
        sb.dst_startbit  =  b_w * screen_height + p->sy;
        if(sb.background_color == 0)   //黑底补黑
        {
            sb.dstfd = F_BMP_BW;
            sb.attr.ispadd = 1;
            sb.attr.paddinfo =0;
        }
        if(sb.background_color == 1)   //白底补白
        {
            sb.dstfd = F_BMP_BW;
            sb.attr.ispadd = 1;
            sb.attr.paddinfo =1;
        }
        if(sb.background_color == 2)   //红底补红
        {
            sb.dstfd = F_BMP_RED;
            sb.attr.ispadd = 1;
            sb.attr.paddinfo =0;
        }
        f_write_direct(FILE_SB,(gdis_id_num)*sizeof(esl_screen_block_t),(UINT8 *)&sb,sizeof(esl_screen_block_t));
        gdis_id_num++;
    }

    //垂直上面
    b_w = dst_addr / screen_height;
    b_h = dst_addr - (b_w * screen_height);//用除法和减法代替取余数，减小代码量，结尾不加1的目的是领结边框点不做为块返显示内容

    if(b_h >= p->sy)//若果不是Y轴的起始点才证明有最上面的块
    {
        sb.srcfd = F_BMP_DATA;//未使用参数
        sb.src_startbit = 0;//未使用参数

        sb.w = p->ex - p->sx +1;;
        sb.h = b_h -p->sy+1;//第一个字符的高度减去y轴的起始点
        sb.dst_startbit  = p->sx * screen_height + p->sy;         //b_w * screen_height + p->sy;
        if(sb.background_color == 0)   //黑底补黑
        {
            sb.dstfd = F_BMP_BW;
            sb.attr.ispadd = 1;
            sb.attr.paddinfo =0;
        }
        if(sb.background_color == 1)   //白底补白
        {
            sb.dstfd = F_BMP_BW;
            sb.attr.ispadd = 1;
            sb.attr.paddinfo =1;
        }
        if(sb.background_color == 2)   //红底补红
        {
            sb.dstfd = F_BMP_RED;
            sb.attr.ispadd = 1;
            sb.attr.paddinfo =0;
        }
        f_write_direct(FILE_SB,(gdis_id_num)*sizeof(esl_screen_block_t),(UINT8 *)&sb,sizeof(esl_screen_block_t));
        gdis_id_num++;
    }

    //垂直下面

    b_h += h;//上半部分的高度加上字体的高度
    if(b_h <= p->ey)//若果不是Y轴的起始点才证明有最上面的块
    {
        sb.srcfd = F_BMP_DATA;//未使用参数
        sb.src_startbit = 0;//未使用参数

        sb.w = p->ex - p->sx +1;;
        sb.h = p->ey- b_h +1;//水平方向的高度
        sb.dst_startbit  = p->sx * screen_height + b_h;        //b_w * screen_height + b_h;
        if(sb.background_color == 0)   //黑底补黑
        {
            sb.dstfd = F_BMP_BW;
            sb.attr.ispadd = 1;
            sb.attr.paddinfo =0;
        }
        if(sb.background_color == 1)   //白底补白
        {
            sb.dstfd = F_BMP_BW;
            sb.attr.ispadd = 1;
            sb.attr.paddinfo =1;
        }
        if(sb.background_color == 2)   //红底补红
        {
            sb.dstfd = F_BMP_RED;
            sb.attr.ispadd = 1;
            sb.attr.paddinfo =0;
        }
        f_write_direct(FILE_SB,(gdis_id_num)*sizeof(esl_screen_block_t),(UINT8 *)&sb,sizeof(esl_screen_block_t));
        gdis_id_num++;
    }

}

void num_block_display_fun_90(const UINT8 dire ,const  UINT8 dis_format,UINT16 all_w, UINT32 first_bit_addr,const font_arrt_t *p, property_t *pro_t,const UINT8 h ,const UINT8 w)
{
    esl_screen_block_t sb;
    UINT16 b_w,b_h;


    f_read(F_SB,(gdis_id_num - 1)* sizeof(esl_screen_block_t),(UINT8 *)&sb,sizeof(esl_screen_block_t));//读取要补充数字的sb属性

    //水平最左边的块
    b_w = first_bit_addr / screen_height;

    if(b_w >= p->sx)  //若果不是x轴的起始点才证明有最左边的块
    {

        sb.srcfd = F_BMP_DATA;//未使用参数
        sb.src_startbit = 0;//未使用参数

        sb.w = b_w - p->sx +1;
        sb.h = p->ey - p->sy +1;//水平方向的高度
        sb.dst_startbit  = p->sx * screen_height + p->sy;
        if(sb.background_color == 0)   //黑底补黑
        {
            sb.dstfd = F_BMP_BW;
            sb.attr.ispadd = 1;
            sb.attr.paddinfo =0;
        }
        if(sb.background_color == 1)   //白底补白
        {
            sb.dstfd = F_BMP_BW;
            sb.attr.ispadd = 1;
            sb.attr.paddinfo =1;
        }
        if(sb.background_color == 2)   //红底补红
        {
            sb.dstfd = F_BMP_RED;
            sb.attr.ispadd = 1;
            sb.attr.paddinfo =0;
        }
        f_write_direct(FILE_SB,(gdis_id_num)*sizeof(esl_screen_block_t),(UINT8 *)&sb,sizeof(esl_screen_block_t));
        gdis_id_num++;
    }

    //水平最右面的块
    b_w  =( b_w + w);//若果不是x轴的结束点才证明有最右边的块
    if(b_w <= p->ex)
    {
        sb.srcfd = F_BMP_DATA;//未使用参数
        sb.src_startbit = 0;//未使用参数

        sb.w = p->ex - b_w +1;
        sb.h = p->ey - p->sy +1;//水平方向的高度
        sb.dst_startbit  = b_w * screen_height + p->sy;
        if(sb.background_color == 0)   //黑底补黑
        {
            sb.dstfd = F_BMP_BW;
            sb.attr.ispadd = 1;
            sb.attr.paddinfo =0;
        }
        if(sb.background_color == 1)   //白底补白
        {
            sb.dstfd = F_BMP_BW;
            sb.attr.ispadd = 1;
            sb.attr.paddinfo =1;
        }
        if(sb.background_color == 2)   //红底补红
        {
            sb.dstfd = F_BMP_RED;
            sb.attr.ispadd = 1;
            sb.attr.paddinfo =0;
        }
        f_write_direct(FILE_SB,(gdis_id_num)*sizeof(esl_screen_block_t),(UINT8 *)&sb,sizeof(esl_screen_block_t));
        gdis_id_num++;
    }

    //垂直上面
    b_w = first_bit_addr / screen_height;
    b_h = first_bit_addr - (b_w * screen_height);//用除法和减法代替取余数，减小代码量，结尾不加1的目的是领结边框点不做为块返显示内容

    if(b_h >= p->sy)//若果不是Y轴的起始点才证明有最上面的块
    {
        sb.srcfd = F_BMP_DATA;//未使用参数
        sb.src_startbit = 0;//未使用参数

        sb.w = p->ex - p->sx +1;
        sb.h = p->sy - b_h +1;//垂直方向的高度
        sb.dst_startbit  = b_w * screen_height + p->sy;
        if(sb.background_color == 0)   //黑底补黑
        {
            sb.dstfd = F_BMP_BW;
            sb.attr.ispadd = 1;
            sb.attr.paddinfo =0;
        }
        if(sb.background_color == 1)   //白底补白
        {
            sb.dstfd = F_BMP_BW;
            sb.attr.ispadd = 1;
            sb.attr.paddinfo =1;
        }
        if(sb.background_color == 2)   //红底补红
        {
            sb.dstfd = F_BMP_RED;
            sb.attr.ispadd = 1;
            sb.attr.paddinfo =0;
        }
        f_write_direct(FILE_SB,(gdis_id_num)*sizeof(esl_screen_block_t),(UINT8 *)&sb,sizeof(esl_screen_block_t));
        gdis_id_num++;
    }

    //垂直下面

    b_h += all_w;//上半部分的高度加上字体的总宽度

    if(b_h <= p->ey)//若果不是Y轴的起始点才证明有最上面的块
    {
        sb.srcfd = F_BMP_DATA;//未使用参数
        sb.src_startbit = 0;//未使用参数

        sb.w = p->ex - p->sx +1;
        sb.h = p->ey -b_h + 1;//垂直方向的高度
        sb.dst_startbit  = b_w * screen_height + b_h;
        if(sb.background_color == 0)   //黑底补黑
        {
            sb.dstfd = F_BMP_BW;
            sb.attr.ispadd = 1;
            sb.attr.paddinfo =0;
        }
        if(sb.background_color == 1)   //白底补白
        {
            sb.dstfd = F_BMP_BW;
            sb.attr.ispadd = 1;
            sb.attr.paddinfo =1;
        }
        if(sb.background_color == 2)   //红底补红
        {
            sb.dstfd = F_BMP_RED;
            sb.attr.ispadd = 1;
            sb.attr.paddinfo =0;
        }
        f_write_direct(FILE_SB,(gdis_id_num)*sizeof(esl_screen_block_t),(UINT8 *)&sb,sizeof(esl_screen_block_t));
        gdis_id_num++;
    }

}

void num_block_display_fun_270(const UINT8 dire ,const  UINT8 dis_format,UINT16 all_w, UINT32 first_bit_addr,const font_arrt_t *p, property_t *pro_t,const UINT8 h ,const UINT8 w)
{
    esl_screen_block_t sb;
    UINT16 b_w,b_h;

    f_read(F_SB,(gdis_id_num - 1)* sizeof(esl_screen_block_t),(UINT8 *)&sb,sizeof(esl_screen_block_t));//读取要补充数字的sb属性
    //水平最左边的块
    b_w = first_bit_addr / screen_height;

    if(b_w >= p->sx)  //若果不是x轴的起始点才证明有最左边的块
    {

        sb.srcfd = F_BMP_DATA;//未使用参数
        sb.src_startbit = 0;//未使用参数

        sb.w = b_w - p->sx +1;
        sb.h = p->ey - p->sy +1;//水平方向的高度
        sb.dst_startbit  = p->sx * screen_height + p->sy;
        if(sb.background_color == 0)   //黑底补黑
        {
            sb.dstfd = F_BMP_BW;
            sb.attr.ispadd = 1;
            sb.attr.paddinfo =0;
        }
        if(sb.background_color == 1)   //白底补白
        {
            sb.dstfd = F_BMP_BW;
            sb.attr.ispadd = 1;
            sb.attr.paddinfo =1;
        }
        if(sb.background_color == 2)   //红底补红
        {
            sb.dstfd = F_BMP_RED;
            sb.attr.ispadd = 1;
            sb.attr.paddinfo =0;
        }
        f_write_direct(FILE_SB,(gdis_id_num)*sizeof(esl_screen_block_t),(UINT8 *)&sb,sizeof(esl_screen_block_t));
        gdis_id_num++;
    }

    //水平最右面的块
    b_w  =( b_w + w);//若果不是x轴的结束点才证明有最右边的块
    if(b_w <= p->ex)
    {
        sb.srcfd = F_BMP_DATA;//未使用参数
        sb.src_startbit = 0;//未使用参数

        sb.w = p->ex - b_w +1;
        sb.h = p->ey - p->sy +1;//水平方向的高度
        sb.dst_startbit  = b_w * screen_height + p->sy;
        if(sb.background_color == 0)   //黑底补黑
        {
            sb.dstfd = F_BMP_BW;
            sb.attr.ispadd = 1;
            sb.attr.paddinfo =0;
        }
        if(sb.background_color == 1)   //白底补白
        {
            sb.dstfd = F_BMP_BW;
            sb.attr.ispadd = 1;
            sb.attr.paddinfo =1;
        }
        if(sb.background_color == 2)   //红底补红
        {
            sb.dstfd = F_BMP_RED;
            sb.attr.ispadd = 1;
            sb.attr.paddinfo =0;
        }
        f_write_direct(FILE_SB,(gdis_id_num)*sizeof(esl_screen_block_t),(UINT8 *)&sb,sizeof(esl_screen_block_t));
        gdis_id_num++;
    }

    //垂直下面
    b_w = first_bit_addr / screen_height;
    b_h = first_bit_addr % screen_height;//用除法和减法代替取余数，减小代码量，结尾不加1的目的是领结边框点不做为块返显示内容

    if(b_h <= p->ey)//若果不是Y轴的起始点才证明有最上面的块
    {
        sb.srcfd = F_BMP_DATA;//未使用参数
        sb.src_startbit = 0;//未使用参数

        sb.w = p->ex - p->sx +1;
        sb.h = p->ey -b_h +1;//垂直方向的高度
        sb.dst_startbit  = b_w * screen_height + b_h;
        if(sb.background_color == 0)   //黑底补黑
        {
            sb.dstfd = F_BMP_BW;
            sb.attr.ispadd = 1;
            sb.attr.paddinfo =0;
        }
        if(sb.background_color == 1)   //白底补白
        {
            sb.dstfd = F_BMP_BW;
            sb.attr.ispadd = 1;
            sb.attr.paddinfo =1;
        }
        if(sb.background_color == 2)   //红底补红
        {
            sb.dstfd = F_BMP_RED;
            sb.attr.ispadd = 1;
            sb.attr.paddinfo =0;
        }
        f_write_direct(FILE_SB,(gdis_id_num)*sizeof(esl_screen_block_t),(UINT8 *)&sb,sizeof(esl_screen_block_t));
        gdis_id_num++;
    }

    //垂直上面

    b_h -= all_w;//上半部分的高度加上字体的总宽度

    if(b_h >= p->sy)//若果不是Y轴的起始点才证明有最上面的块
    {
        sb.srcfd = F_BMP_DATA;//未使用参数
        sb.src_startbit = 0;//未使用参数

        sb.w = p->ex - p->sx +1;
        sb.h = b_h -p->sy + 1;//垂直方向的高度
        sb.dst_startbit  = b_w * screen_height + p->sy;
        if(sb.background_color == 0)   //黑底补黑
        {
            sb.dstfd = F_BMP_BW;
            sb.attr.ispadd = 1;
            sb.attr.paddinfo =0;
        }
        if(sb.background_color == 1)   //白底补白
        {
            sb.dstfd = F_BMP_BW;
            sb.attr.ispadd = 1;
            sb.attr.paddinfo =1;
        }
        if(sb.background_color == 2)   //红底补红
        {
            sb.dstfd = F_BMP_RED;
            sb.attr.ispadd = 1;
            sb.attr.paddinfo =0;
        }
        f_write_direct(FILE_SB,(gdis_id_num)*sizeof(esl_screen_block_t),(UINT8 *)&sb,sizeof(esl_screen_block_t));
        gdis_id_num++;
    }

}



//保存图册属性
/*
static const file_id_t FILE_ARR[] = {F_BMP_BW,F_BMP_RED,F_BMP_BW,F_BMP_BW\
,F_BMP_BW,F_BMP_BW,F_BMP_BW,F_BMP_BW\
,F_BMP_BW,F_BMP_BW,F_BMP_BW,F_BMP_BW\
,F_BMP_BW,F_BMP_BW,F_BMP_BW,F_BMP_BW};
 */
void spit_pic_layer_fun(UINT8 page_id)
{
#define LAYER_ID_SIZE  0x02
#define FINISH_ID (0xffff)
    UINT8 num_colour, temp[PAGE_LEN];
    UINT16 layer_id_tp,i;
    wrt_layer_arrt_t lay_tp;
    esl_image_t img_tp;
    esl_screen_block_t sb;

    f_read(gSys_tp.use_page_info_fileid ,page_id * PAGE_LEN ,temp,PAGE_LEN);
    for(i=0;i < PAGE_LEN;i+=2 )
    {
        memset((UINT8 *)&sb,0x00,sizeof(esl_screen_block_t));
        memcpy((UINT8 * )&layer_id_tp,temp+i,sizeof(layer_id_tp));
        if(FINISH_ID == layer_id_tp)
            break;
        f_read(F_LAY_MAP ,(layer_id_tp-1)*sizeof(wrt_layer_arrt_t),(UINT8 *)&lay_tp,sizeof(wrt_layer_arrt_t));
        f_read(lay_tp.src_file ,lay_tp.offset+ LAYER_ID_SIZE,(UINT8 *)&img_tp,sizeof(esl_image_t)); //2是layerid的长度
        sb.srcfd = lay_tp.src_file;
        sb.src_startbit = (lay_tp.offset + LAYER_ID_SIZE + sizeof(esl_image_t)) * 8;
        // sb.dstfd = FILE_ARR[img_tp.p & 0x000f];
        sb.dstfd = ((img_tp.p & 0x000f)==1?F_BMP_RED:F_BMP_BW);
        sb.dst_startbit = (screen_height * img_tp.sx ) + img_tp.sy;
        sb.h = img_tp.ey - img_tp.sy + 1;
        sb.w = img_tp.ex - img_tp.sx + 1;
        sb.attr.reverse_flag = 0;
        sb.attr.ispadd = 0;
        sb.attr.paddinfo =1;

        if(screen_colour == SCREEN_COLOUR_BLACK)
        {
            img_tp.p = ((img_tp.p & 0x2000)==1? (img_tp.p & 0xDFFF) : img_tp.p);  //如果是黑白屏幕，把红底改成黑底
            img_tp.p = ((img_tp.p & 0x0200)==1? (img_tp.p & 0xFDFF) : img_tp.p);  //如果是黑白屏幕，把红字改成黑字
        }
        num_colour = (img_tp.p & 0xFF00)>>8;
        if(num_colour == 0x12)      //白底红字
        {
            sb.dstfd = F_BMP_RED;
            sb.attr.reverse_flag = 0;
            sb.attr.ispadd = 0;
            sb.attr.paddinfo =1;

        }
        if(num_colour == 0x21)    //红底白字
        {
            sb.dstfd = F_BMP_RED;
            sb.attr.reverse_flag = 1;
            sb.attr.ispadd = 0;
            sb.attr.paddinfo =1;

        }
        if(num_colour == 0x10)    //白底黑字
        {
            sb.dstfd = F_BMP_BW;
            sb.attr.reverse_flag = 0;
            sb.attr.ispadd = 0;
            sb.attr.paddinfo =1;
        }
        if(num_colour == 0x01)    //黑底白字
        {
            sb.dstfd = F_BMP_BW;
            sb.attr.reverse_flag = 1;
            sb.attr.ispadd = 0;
            sb.attr.paddinfo =1;
        }
        if(num_colour == 0x02)   //黑底红字
        {
            sb.dstfd = F_BMP_RED;
            sb.attr.ispadd = 0;
            sb.attr.paddinfo =1;
            f_write_direct(F_SB,sizeof(esl_screen_block_t) * (gdis_id_num), (UINT8 *)&sb,sizeof(esl_screen_block_t));
            gdis_id_num++;

            sb.dstfd =F_BMP_BW;
            sb.attr.ispadd = 0;
            sb.attr.reverse_flag = 1;
        }
        if(num_colour == 0x20)  //红底黑字
        {
            sb.dstfd = F_BMP_RED;
            sb.attr.ispadd = 0;
            sb.attr.paddinfo =1;
            sb.attr.reverse_flag = 1;
            f_write_direct(F_SB,sizeof(esl_screen_block_t) * (gdis_id_num), (UINT8 *)&sb,sizeof(esl_screen_block_t));
            gdis_id_num++;

            sb.dstfd =F_BMP_BW;
            sb.attr.ispadd = 0;
            sb.attr.paddinfo =1;
            sb.attr.reverse_flag = 0;
        }

        f_write_direct(F_SB,sizeof(esl_screen_block_t) * (gdis_id_num), (UINT8 *)&sb,sizeof(esl_screen_block_t));
        gdis_id_num++;
    }
}
//---------------------------------数字显示----------------------------------------------------



UINT32 find_id_mode1(UINT8 id)
{
    wrt_layer_arrt_t tp;
    f_read(F_LAY_MAP,(id-1)*sizeof(tp),(UINT8 *)&tp,sizeof(tp));
    return tp.offset;
}
static UINT16 itos(UINT8 *str, UINT8 len, UINT32 num, UINT8 decimal_flag,UINT8 decimal_places,UINT8 *decimal_offset)
{
    int16_t i = len - 2;;

    memset(str,0xff,len);
    str[len-1] = '\0';
    *decimal_offset = i;
    if(num == 0 && decimal_places == 0)               //数字是0，且没有小数部分长度是0
    {
        str[i--] = '0';
        goto ret;
    }
    for (i = len -2; (i >= 0) && (num > 0 || ( (decimal_places>0) && i > len - (decimal_places + 4))); i--) //第二个判断条件是数字是0,且点小数
    {
        if ((decimal_places>0) && i == (len-2-decimal_places))//len-2表示最后一个要显示数的位置 ，减去小数位数，表示小数点的位置
        {
            if(decimal_flag != 0)
            {
                str[i] = '0' + 10;
                i--;//到小数前的第一个整数的位置
            }
            *decimal_offset = i;//小数前第一个整数的位置
            str[i] = '0';

        }
        str[i] = num % 10 + '0';
        num = num / 10;
    }
    ret:
    return i +1;
}



void property_format_init( dis_format_t *dst,UINT32 src, property_t *pro_dst,UINT32 property_src)
{
    memcpy((UINT8 *)dst,(UINT8 *)&src,4);
    /*
  dst->number_variety = src & BUMBER_VARIETY_F;
  dst->prefix = (src & PREFIX_F) >> 3;
  dst->pm = (src & DIGITAL_PM_F) >> 4; 
  dst->decimals_len = (src & DECIMALS_LEN_F) >> 5;


  dst->time_variety = (src & TIME_VARIETY_F)>> 8;
  dst->decimals_flag = (src & DECIMALS_FLAG_F) >> 10;
  dst->thousands_flag = (src & THOUSANDS_FLAG_F)>>11;
  dst->dis_decimals_mode = (src & DIS_MODE_F) >> 12 ;
  dst->suffix_flag = (src & SUFFIX_FLAG_F)>>14;
  dst->reverse_flag = (src & REVERSE_FLAG_F) >> 15;


  dst->horizontal_alignment = (src & HORIZONTAL_F)>> 16;
  dst->vertical_alignment = (src & VERTICAL_F)>> 18;
     */
    dst->ispadd = 0;
    dst->paddinfo = 0;
    dst->reserve = 0;

    pro_dst->color = property_src & 0x0000000f;   //颜色
    pro_dst->direction = (property_src & 0x00000060) >> 5;//方向
    pro_dst->font_color = (property_src & 0x00000F00) >> 8;//字体颜色
    pro_dst->background_color = (property_src & 0x0000F000) >> 12;//背景色
}
//---符号字库：前缀、后缀、小数点、千分位、正负号、时期分隔符、时间分隔符
static void get_number_attr(UINT8 fid, UINT8 num,  num_ft *num_tp) {
    UINT32 dstoffset = find_id_mode1(fid) + (sizeof(write_layer_t) - offsetof(write_layer_t,layerid)) + 1;
    UINT8 i, a[2];

    for(i = 0; i <= num-'0'; i++) {
        f_read(F_BMP_DATA, dstoffset, a, 2);
        dstoffset += 2 + (a[0]+7)/8 *a[1];
    }

    num_tp->font_h = a[0]; num_tp->font_w = a[1];
    num_tp->offset = dstoffset -((a[0]+7)/8 *a[1]);
}


void save_dis_num_arrt_sb_fun(UINT16 layid,UINT8 num ,num_ft *fp)
{
    get_number_attr(layid,num,fp);
}
void thousands_fag_offset_fun(UINT8 dig_num, UINT8 *temp)
{
    switch(dig_num)
    {
    case 4:
        temp[0] = 1;
        break;
    case 5:
        temp[0] = 2;
        break;
    case 6:
        temp[0] = 3;
        break;
    case 7:
        temp[0] = 1;temp[1] = 4;
        break;
    case 8:
        temp[0] = 2;temp[1] = 5;
        break;
    case 9:
        temp[0] = 3;temp[1] = 6;
        break;
    case 10:
        temp[0] = 1;temp[1] = 4;temp[2] = 7;
        break;
    case 11:
        temp[0] = 2;temp[1] = 5;temp[2] = 8;
        break;
    case 12:
        temp[0] = 3;temp[1] = 6;temp[2] = 9;
        break;
    }
}
void tmp(UINT8 *ths_offset, UINT8 type, UINT8 len, UINT8 *number_buf, font_arrt_t * ftp, num_ft *num_tp,UINT8 *cont, UINT8 start,UINT8 flag)
{
    UINT8 tpi;
    UINT16 id = flag?ftp->font_layerid_d:ftp->font_layerid_i;

    for(tpi=0;tpi< len ;tpi++)
    {
        if((tpi==ths_offset[0] && ths_offset[0]!=0) ||  (tpi==ths_offset[1] && ths_offset[1]!=0) || (tpi==ths_offset[2] && ths_offset[2]!=0))//写入日期分隔符
        {
            save_dis_num_arrt_sb_fun(ftp->font_layerid_s,type,num_tp+ (*cont));
            (*cont) += 1;
        }
        save_dis_num_arrt_sb_fun(id,number_buf[tpi+start] ,num_tp + (*cont));//写入时间数据
        (*cont) += 1;
    }
}
void number_type_fun(dis_format_t *attr_tp ,UINT8 *number_buf,UINT8 start,UINT8 end,UINT8 dig_offset,UINT8 *cont,font_arrt_t * ftp,num_ft *num_tp,UINT8 *wtp,UINT8 *htp)
{
#define DATA_SINGE_TYPR  4
#define TIME_SINGE_TYPR  5
#define THOUSANDS_FLAG_TPYE 3 
#define POINT_FLAG_TPYE 2
    UINT8 ths_offset[3]={0};
    UINT8 i,len;

    // memset(ths_offset,0,3);
    // len =  strlen((char const *)(number_buf+start));
    len = NUMBER_ALL-1 - start;
    if(attr_tp->number_variety <= 1)//数字
    {
        UINT8 d_num_cont = dig_offset - start + 1;//共有多少个整数
        UINT8 tpi=0;

        if(attr_tp->thousands_flag != 0)
            thousands_fag_offset_fun(d_num_cont,ths_offset);//获取不同长度的整数千分位的位置

        tmp(ths_offset, THOUSANDS_FLAG_TPYE+'0', d_num_cont, number_buf, ftp, num_tp, cont, start,0);
        *wtp = num_tp[*cont-1].font_w;//整数字库的宽度
        *htp = num_tp[*cont-1].font_h;//整数字库的高度

        if(attr_tp->decimals_flag != 0)//写入小数点
        {
            save_dis_num_arrt_sb_fun(ftp->font_layerid_s, 2 + '0',num_tp + (*cont)); //写入小数点
            *cont += 1;

        }
        for(tpi= 0;tpi<attr_tp->decimals_len;tpi++)//写入小数部分
        {
            UINT8 tt =  (NUMBER_ALL -1) - attr_tp->decimals_len + tpi;
            save_dis_num_arrt_sb_fun(ftp->font_layerid_d,number_buf[tt],num_tp + (*cont));
            *cont += 1;
            // save_dis_num_arrt_sb_fun(ftp->font_layerid_d,number_buf[ (NUMBER_ALL -1) - attr_tp->decimals_len + tpi],num_tp + *cont);
        }


    }
    else if(attr_tp->number_variety == 2)//日期数据存储
    {
        if(attr_tp->time_variety != 0)//欧洲日期
        {
            if(len == 7 ) //七位日期格式，最开始补0
            {
                number_buf[start-1] = '0';
                start -=1;
                len +=1;
            }
            ths_offset[0] = 2; ths_offset[1] = 4;ths_offset[2] = 0;
            tmp(ths_offset, DATA_SINGE_TYPR + '0', len, number_buf, ftp, num_tp, cont, start,1);
        }
        else //中国日期
        {
            ths_offset[0] = 4; ths_offset[1] = 6;ths_offset[2] = 0;
            tmp(ths_offset, DATA_SINGE_TYPR+'0',len, number_buf, ftp, num_tp, cont, start,1);
        }
    }
    else //时间
    {

#if 0
        if(len == 5 ) //五位时间格式，最开始补0
        {
            number_buf[start-1] = '0';
            start -=1;
        }
#else
        /*
         *begin add by zhaoyang for bug   954
         *问题描述：时间显示异常
         *解决办法：不全0
         */
        for(i=0;i<6-len;i++)
        {
            number_buf[start-1-i] = '0';
        }
        len += i;//要保存的数据长度
#endif
        ths_offset[0] = 2; ths_offset[1] = 4;ths_offset[2] = 0;
        tmp(ths_offset, TIME_SINGE_TYPR +'0',len, number_buf, ftp, num_tp, cont, start-i,1);//start-i 表示要保存的数字起始点位置

    }
}

static void save_sb(UINT32 start, const num_ft *ft, const font_arrt_t *num_attr,property_t *pro_t, dis_format_t *attr_tp)
{
    UINT8 num_colour;
    esl_screen_block_t *sb =  &TEMP.td.sb;

    sb->srcfd = F_BMP_DATA;
    sb->src_startbit = ft->offset * 8;
    sb->h = ft->font_h; sb->w = ft->font_w;
    // sb->dstfd = FILE_ARR[pro_t->color];
    sb->dstfd = (pro_t->color==1?F_BMP_RED:F_BMP_BW);
    sb->dst_startbit = start ;
    sb->attr.reverse_flag = attr_tp->reverse_flag;
    sb->attr.ispadd = 0;
    sb->attr.paddinfo = 1;
    sb->attr.area_reverse = attr_tp->area_reverse;

    num_colour = pro_t->font_color | (pro_t->background_color <<4);
    if(screen_colour == SCREEN_COLOUR_BLACK)
    {
        num_colour = ((num_colour & 0x20)==1? (num_colour & 0xDF) : num_colour);  //如果是黑白屏幕，把红底改成黑底
        num_colour = ((num_colour & 0x02)==1? (num_colour & 0xFD) : num_colour);  //如果是黑白屏幕，把红字改成黑字
    }
    sb->font_color = num_colour & 0x0F;
    sb->background_color = (num_colour & 0xF0)>>4;

    if(num_colour == 0x12)      //白底红字
    {
        sb->dstfd = F_BMP_RED;
        sb->attr.reverse_flag = 0;
        sb->attr.ispadd = 0;
        sb->attr.paddinfo =1;

    }
    if(num_colour == 0x21)    //红底白字
    {
        sb->dstfd = F_BMP_RED;
        sb->attr.reverse_flag = 1;
        sb->attr.ispadd = 0;
        sb->attr.paddinfo =1;

    }
    if(num_colour == 0x10)    //白底黑字
    {
        sb->dstfd = F_BMP_BW;
        sb->attr.reverse_flag = 0;
        sb->attr.ispadd = 0;
        sb->attr.paddinfo =1;
    }
    if(num_colour == 0x01)    //黑底白字
    {
        sb->dstfd = F_BMP_BW;
        sb->attr.reverse_flag = 1;
        sb->attr.ispadd = 0;
        sb->attr.paddinfo =1;
    }
    if(num_colour == 0x02)   //黑底红字
    {
        sb->dstfd = F_BMP_RED;
        sb->attr.reverse_flag = 0;
        sb->attr.ispadd = 0;
        sb->attr.paddinfo =1;
        f_write_direct(F_SB,sizeof(esl_screen_block_t) * (gdis_id_num), (UINT8 *)sb,sizeof(esl_screen_block_t));
        gdis_id_num++;

        sb->dstfd =F_BMP_BW;
        sb->attr.ispadd = 0;
        sb->attr.reverse_flag = 1;

        num_extend_flag=1;
    }
    if(num_colour == 0x20)  //红底黑字
    {
        sb->dstfd = F_BMP_RED;
        sb->attr.ispadd = 0;
        sb->attr.paddinfo =1;
        sb->attr.reverse_flag = 1;
        f_write_direct(F_SB,sizeof(esl_screen_block_t) * (gdis_id_num), (UINT8 *)sb,sizeof(esl_screen_block_t));
        gdis_id_num++;

        sb->dstfd =F_BMP_BW;
        sb->attr.ispadd = 0;
        sb->attr.paddinfo =1;
        sb->attr.reverse_flag = 0;

        num_extend_flag=1;
    }

    f_write_direct(FILE_SB,(gdis_id_num)*sizeof(esl_screen_block_t),(UINT8 *)sb,sizeof(esl_screen_block_t));
    gdis_id_num++;
}

void spit_number_fun(UINT8 page_id)
{
#define PREFIX_TYPE     0  //前缀
#define DIG_PM_TYPE     6  //正负号
#define PONIT_TPYE      2  //小数点
#define SUFFIX_TYPE     1
#define NIONE_DECIMALS  2


#define RET_NUMBER  (0xffff)

#define NUMBER_ATTR_NUM (20)                    //关于要显示的数字和符号结构体个数



    UINT16 offset = 0;
    UINT32 src_offset = 0;         //3号命令的图层起始位置
    UINT8 temp[PAGE_LEN];         //每个页所有数字的缓存
    number_t tp;                  //每个数字的id和数值
    font_arrt_t font_tp;          //数个数字的属性
    UINT8 number_start_addr = 0 , number_end_addr = NUMBER_ALL-2,dig_offset,k,i=0,m=0;
    UINT8 number_buf[NUMBER_ALL];
    num_ft num_tp[NUMBER_ATTR_NUM]= {0};
    dis_format_t attr_tp;
    UINT16 all_width = 0;
    property_t pro_t;
    UINT8 intg_h,intg_w;
    UINT32 tp_num=0;

    f_read(gSys_tp.use_page_num_fileid ,(WORD)(page_id * PAGE_LEN), (UINT8 *)temp, PAGE_LEN);
    for(;;)
    {
        dig_offset=0;
        k=0;
        //清0的目的是保正新的一组数据能过服用之前的结构体数组
        if(offset> PAGE_LEN)
            break;
        memcpy((UINT8 *) &tp ,temp+offset,sizeof(number_t));                       //获取这个页的数字和id属性
        if( (tp.layer_id == RET_NUMBER))                     //读取到全f时，表示此页数字全部完成，或者读到这个页的结尾
            break;
        src_offset = find_id_mode1(tp.layer_id);                                    //根据数字的id属性，找到数字属性存放的位置
        f_read(F_BMP_DATA,src_offset,(UINT8 *)&font_tp,sizeof(font_arrt_t));   //读出具体的数字显示属性
        property_format_init(&attr_tp , font_tp.format,&pro_t,font_tp.property);    //字体的format和property初始化
        offset += sizeof(number_t);                                                //指向下一个数字

        tp_num = tp.number & ((UINT32)0x7fffffff);

        if(attr_tp.dis_decimals_mode == NIONE_DECIMALS)//没有小数部分，将小数点和位数去掉
        {
            attr_tp.decimals_flag = 0;
            attr_tp.decimals_len=0;
        }

        number_start_addr = itos(number_buf,NUMBER_ALL,tp_num,attr_tp.decimals_flag,attr_tp.decimals_len,&dig_offset);

        if(attr_tp.prefix != 0)//前缀
        {
            save_dis_num_arrt_sb_fun(font_tp.font_layerid_s,PREFIX_TYPE + '0',&num_tp[k]);
            k++;
        }

        if(attr_tp.pm != 0)//正负号是否显示
        {
            save_dis_num_arrt_sb_fun(font_tp.font_layerid_s,DIG_PM_TYPE+ ((tp.number>tp_num)?1:0)+'0',&num_tp[k]);
            k++;
        }

        number_type_fun(&attr_tp,number_buf,number_start_addr,number_end_addr,dig_offset,&k,&font_tp,num_tp,&intg_w,&intg_h);//数字部分拆分

        if(attr_tp.suffix_flag != 0)//后缀是否显示
        {
            save_dis_num_arrt_sb_fun(font_tp.font_layerid_s,SUFFIX_TYPE + '0',&num_tp[k]);
            k++;
        }
        all_width = 0;//使用之前高度全部清空
        for(i=0;i<k;i++)//获取所有字符的宽度
        {
            all_width += ( (pro_t.direction == 0 )|| (pro_t.direction) == 2 )?  num_tp[i].font_w : num_tp[i].font_h;
        }
        for(i=0;i<k;i++)//保存要显示的数字到sb结构体
        {
            if (i == 0) //第一个字符的起始位置
                num_tp[i].dstoffset = get_dst_start_bit(&font_tp, &num_tp[i], all_width,pro_t.direction,intg_h,intg_w,&attr_tp);
            else
                num_tp[i].dstoffset = get_dst_bit(num_tp[i-1].dstoffset ,num_tp[i-1].font_w,num_tp[i-1].font_h , &font_tp,  &num_tp[i],&pro_t, &attr_tp);
            save_sb(num_tp[i].dstoffset,&num_tp[i], &font_tp, &pro_t,  &attr_tp);
        }
        if(attr_tp.number_variety>1)//数字类型
            continue;
        //        if(attr_tp.reverse_flag ==0)//不要反显
        //            continue;
        if(attr_tp.dis_decimals_mode >1 )//没有小数部分不用不全反显
            continue;
        m = gdis_id_num -1;//保存后一个小数字符的位置，这个地方不能用k的目的是因为防止显示数字之前有非数字原始
        if(attr_tp.suffix_flag != 0)//后缀显示,显示时，则减去后缀的位置
            m = gdis_id_num - 2;//保存后一个小数字符的位置，这个地方不能用k的目的是因为防止显示数字之前有非数字原始

        if(num_extend_flag == 1)
        {
            attr_tp.decimals_len += attr_tp.decimals_len;
            num_extend_flag = 0;
        }
        for(i=0;i<attr_tp.decimals_len;i++)//一共不全多少位
        {
            alignment_dignum(pro_t.direction,intg_h,intg_w, attr_tp.dis_decimals_mode,m-attr_tp.decimals_len+1+i);
            k++;
        }
        if(attr_tp.area_reverse)
        {
            switch(pro_t.direction)
            {
            case 1:
                num_block_display_fun_90(pro_t.direction ,attr_tp.dis_decimals_mode,all_width ,num_tp[0].dstoffset,&font_tp,&pro_t,intg_h,intg_w);
                break;
            case 2:
                num_block_display_fun_180(pro_t.direction ,attr_tp.dis_decimals_mode,all_width ,num_tp[0].dstoffset,&font_tp,&pro_t,intg_h,intg_w);
                break;
            case 3:
                num_block_display_fun_270(pro_t.direction ,attr_tp.dis_decimals_mode,all_width ,num_tp[0].dstoffset,&font_tp,&pro_t,intg_h,intg_w);
                break;
            default:
                num_block_display_fun_0(pro_t.direction ,attr_tp.dis_decimals_mode,all_width ,num_tp[0].dstoffset,&font_tp,&pro_t,intg_h,intg_w);
                break;

            }
        }
    }

}

//----------------------------------end--------------------------------------------------------
void spit_fun(UINT8 page_id)
{
    //gdis_id_num = 0;
    f_erase(F_SB);
    spit_pic_layer_fun(page_id);
    spit_number_fun(page_id);
}





