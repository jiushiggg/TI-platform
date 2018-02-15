#include <ti/drivers/PIN.h>
#include <ti/sysbios/knl/Task.h>
#include "typedef.h"
#include "epd_io.h"
#include "scrn_ic1675a.h"
#include "mini_fs.h"
#include "scrn_public_fun.h"



const UINT8 screen_1675A_tp[][20] ={
                                    //---------分辨率-----------cmd_01---------cmd_11-------cmd_44--------------cmd_45----------------cmd_4e--------cmd_4f-----cmd_74----size---
                                    {  0x00,0x00,0x00,0x00, 0x00,0x00,0x00,      0x00,       0x00,0x00,     0x00,0x00,0x00,0x00,         0x00,      0x00,0x00,   0x00 ,0x00,0x00},  //null
                                    {  0x00,0x98,0x00,0x98, 0x97,0x00,0x00,      0x03,       0x00,0x12,     0x00,0x00,0x97,0x00,         0x00,      0x00,0x00,   0x86 ,0x00,0x9a},  //1.54-152*152
                                    {  0x00,0xc8,0x00,0xc8, 0xc7,0x00,0x00,      0x03,       0x00,0x12,     0x00,0x00,0xc7,0x00,         0x00,      0x00,0x00,   0x86 ,0x00,0x9a},  //1.54-200*200
                                    {  0x00,0xfa,0x00,0x7a, 0x79,0x00,0x00,      0x03,       0x00,0x0c,     0x00,0x00,0x79,0x00,         0x00,      0x00,0x00,   0x86 ,0x00,0xd5},  //2.13-250*122
                                    {  0x00,0xd4,0x00,0x68, 0xd3,0x00,0x00,      0x03,       0x00,0x0c,     0x00,0x00,0xd3,0x00,         0x00,      0x00,0x00,   0x86 ,0x00,0xd5},  //2.13-212*104
                                    {  0x01,0x28,0x00,0x98, 0xd3,0x00,0x00,      0x03,       0x00,0x0c,     0x00,0x00,0xd3,0x00,         0x00,      0x00,0x00,   0x86 ,0x01,0x04},  //2.60-296*152
                                    {  0x01,0x28,0x00,0x80, 0x27,0x01,0x00,      0x03,       0x00,0x0f,     0x00,0x00,0x27,0x01,         0x00,      0x00,0x00,   0x86 ,0x01,0x28},  //2.90-296*128
};



void ic1675a_check_busy_fun(void)
{
    if( gerr_info != 0)
    {
        // gerr_info = TR3_SCREEN_ERR1;
        return;
    }
    Task_sleep(20000);
    while((isEPD_W21_BUSY) != 0) //高表示忙
    {
        Task_sleep(20000);
    }
}

static BOOL ic1675a_busy_1(void)
{

    if(isEPD_W21_BUSY)//不忙表示没刷屏幕
        return TRUE;
    else
        return FALSE;
}
//void ic1675a_deep_sleep_fun(void)
//{
//
//    EPD_W21_WriteCMD(0x10);
//    EPD_W21_WriteDATA(0x01);
//}

BOOL ic1675a_open_display_fun(SCREEN_COLOR_T color)
{
    if(color == BLACK_WHITE )
    {
        EPD_W21_WriteCMD(0x21);
        EPD_W21_WriteDATA(0x40);               //忽略红色RAM区
    }

    EPD_W21_WriteCMD(0x22);
    EPD_W21_WriteDATA(0xC7);		//0XC7   Load LUT from MCU(0x32), Display update   0XF7     FROM 内部
    EPD_W21_WriteCMD(0x20);
    Task_sleep(10000);
    if(ic1675a_busy_1()!= TRUE)
        return FALSE;
    return TRUE;
}


//设置不同厂家、不同ic、不同颜色的屏幕内存地址
void ic1675a_set_ram_fun(volatile SCREEN_DIS_T *tp,UINT8 temp)
{
    EPD_W21_WriteCMD(0x4E);		// Set RAM X address counter = 0
    EPD_W21_WriteDATA(screen_1675A_tp[tp->src_size][14]);
    EPD_W21_WriteCMD(0x4F);		// Set RAM Y address counter =295
    EPD_W21_WriteDATA(screen_1675A_tp[tp->src_size][15]);
    EPD_W21_WriteDATA(screen_1675A_tp[tp->src_size][16]);
    EPD_W21_WriteCMD(temp);

}


void display_lcd_ic1675A(volatile SCREEN_DIS_T *tp,UINT8 temp)
{
    UINT16 pcnt,k;
    UINT8 i,j;
    UINT8  data[50];

    j= ((tp->h)+7)  /8;
    k = tp->w -1;

    ic1675a_set_ram_fun(tp,temp);

    for (pcnt = 0; pcnt < tp->w; pcnt++)
    {
        f_read(tp->fd, (k-pcnt) * j + tp->offset_1 ,data, j);
        for (i = 0; i < j; i++)
        {
            EPD_W21_WriteDATA(data[i]);
            //EPD_W21_WriteDATA(0x0f);
        }
    }

}

void display_lcd_red_ic1675a(volatile SCREEN_DIS_T *tp,UINT8 temp)
{
    UINT16 pcnt,k;
    UINT8 i,j;
    UINT8  data[50];

    j= ((tp->h)+7)  /8;
    k = tp->w -1;

    ic1675a_set_ram_fun(tp,temp);

    for (pcnt = 0; pcnt < tp->w; pcnt++)
    {
        f_read(tp->fd_red, (k-pcnt) * j + tp->offset_2 ,data, j);
        for (i = 0; i < j; i++)
        {
            EPD_W21_WriteDATA(~data[i]);
            //EPD_W21_WriteDATA(0x0f);
        }
    }

}

void init_ic1675a_fun(SCREEN_SIZE_LT size)
{
    UINT8 temp1;
    EPD_W21_WriteCMD(0x74);
    EPD_W21_WriteDATA(0x54);
    EPD_W21_WriteCMD(0x7e);
    EPD_W21_WriteDATA(0x3b);

    EPD_W21_WriteCMD(0x2B); //set reduce glitch//20170804 Julian
    EPD_W21_WriteDATA(0x04);
    EPD_W21_WriteDATA(0x63);

    EPD_W21_WriteCMD(0x0C); //set soft start //20170804 Julian
    EPD_W21_WriteDATA(0x8E);
    EPD_W21_WriteDATA(0x8C);
    EPD_W21_WriteDATA(screen_1675A_tp[size][17]);
    EPD_W21_WriteDATA(0x3F);


    EPD_W21_WriteCMD(0x01);		// Set MUX as 296
    EPD_W21_WriteDATA(screen_1675A_tp[size][4]);
    EPD_W21_WriteDATA(screen_1675A_tp[size][5]);
    EPD_W21_WriteDATA(screen_1675A_tp[size][6]);

    EPD_W21_WriteCMD(0x11);		// data enter mode
    EPD_W21_WriteDATA(screen_1675A_tp[size][7]);

    EPD_W21_WriteCMD(0x18);		// internal  temperature
    EPD_W21_WriteDATA(0x80);


    EPD_W21_WriteCMD(0x44);		// set RAM x address start/end, in page 36
    EPD_W21_WriteDATA(screen_1675A_tp[size][8]);		// RAM x address start at 00h;
    EPD_W21_WriteDATA(screen_1675A_tp[size][9]);		// RAM x address end at 0fh(15+1)*8->128    2D13
    EPD_W21_WriteCMD(0x45);		// set RAM y address start/end, in page 37
    EPD_W21_WriteDATA(screen_1675A_tp[size][10]);		// RAM  y address end at 00h;		    2D13
    EPD_W21_WriteDATA(screen_1675A_tp[size][11]);		// RAM y address start at FAh;		    2D13
    EPD_W21_WriteDATA(screen_1675A_tp[size][12]);		// RAM y address end at 00h;
    EPD_W21_WriteDATA(screen_1675A_tp[size][13]);
    EPD_W21_WriteCMD(0x3C);		// board
    EPD_W21_WriteDATA(0x01);		//GS1-->GS1


    EPD_W21_WriteCMD(0x22);
    EPD_W21_WriteDATA(0xA1);
    EPD_W21_WriteCMD(0x20);
    ic1675a_check_busy_fun();

    EPD_W21_WriteCMD(0x1B);
    temp1 = EPD_W21_ReadDATA();
    disply_temperature = temp1;
    EPD_W21_ReadDATA_2();
    if((temp1&0x80)||(temp1<0x0A))
    {
        fast_refresh_flag=0;
        low_tep_flag = TRUE;
        EPD_W21_WriteCMD(0x3D);
        EPD_W21_WriteDATA(0x09);
        EPD_W21_WriteDATA(0x09);
        EPD_W21_WriteCMD(0x3E);
        EPD_W21_WriteDATA(0x01);
        EPD_W21_WriteDATA(0x11);
        EPD_W21_WriteDATA(0x0C);
        EPD_W21_WriteCMD(0x3F);
        EPD_W21_WriteDATA(0x07);
    }
    else
    {
      low_tep_flag = FALSE;
    }
#ifdef FAST_SCREEN_OPEN
    if(fast_refresh_flag)
    {
        fast_refresh_flag=0;
        EPD_W21_WriteCMD(0x1A);//写入85度驱动波形
        EPD_W21_WriteDATA(0x55);
        EPD_W21_WriteDATA(0x00);
        EPD_W21_WriteCMD(0x22);//加载内部opt，按照指定温度更新
        EPD_W21_WriteDATA(0x91);
        EPD_W21_WriteCMD(0x20);

    }
    else
#endif
    {
        EPD_W21_WriteCMD(0x22);//加载内部opt，按照指定温度更新
        EPD_W21_WriteDATA(0xB1);
        EPD_W21_WriteCMD(0x20);
    }
    ic1675a_check_busy_fun();
    EPD_W21_WriteCMD(0x21);								//
    EPD_W21_WriteDATA(0x03);
}

void ic1675a_swrest_fun(void)
{
    EPD_W21_BS_0;		// 4 wire spi mode selected
    EPD_W21_WriteCMD(0x12);			//SWRESET
    ic1675a_check_busy_fun();
}







