#include <ti/drivers/PIN.h>
#include <ti/sysbios/knl/Task.h>
#include "typedef.h"
#include "epd_io.h"
#include "scrn_ic1675a.h"
#include "mini_fs.h"
#include "scrn_public_fun.h"

const unsigned char init_data[] ={
                                  0x66,  0x44,   0x55,   0xAA,   0x99,   0x99,
                                  0x88,   0x0,    0x0,    0x0,    0x0,    0x0,
                                  0x0,    0x0,    0x0,    0x0,    0x34,   0x5,
                                  0x7E,   0x1E,   0x14,   0x14,   0xA,    0x0,
                                  0x0,    0x0,    0x0,    0x0,    0x0,    0x6,
                                  0x1B,   0x0B
};


void write_look_up_table(void)
{
    uint8_t i;

    EPD_W21_WriteCMD(0x32);           // write LUT register
    //  if(fast_refresh_flag==1)
    //  {
    //    fast_refresh_flag=0;
    //    for(i=0;i<30;i++)
    //      EPD_W21_WriteDATA(init_data_fast[i]);
    //  }
    //  else
    {
        for(i=0;i<30;i++)
            EPD_W21_WriteDATA(init_data[i]);
    }
}

//void ic1673_deep_sleep_fun(void)
//{
//
//    EPD_W21_WriteCMD(0x10);
//    EPD_W21_WriteDATA(0x01);
//}
void ic1673_swrest_fun(void)
{
    EPD_W21_BS_0;       // 4 wire spi mode selected
    EPD_W21_WriteCMD(0x12);         //SWRESET
    ic1675a_check_busy_fun();
}

void init_ic1673_fun(void)
{
    EPD_W21_WriteCMD(0x01);       // Set MUX as 250
    EPD_W21_WriteDATA(0xF9);
    EPD_W21_WriteDATA(0x01);
    //****** delete PLL setting  Ê¯ºÆ******//
    /*
      EPD_W21_WriteCMD(0x3A);       // Set 50Hz
      EPD_W21_WriteDATA(0x06);
      EPD_W21_WriteCMD(0x3B);       // Set 50Hz
      EPD_W21_WriteDATA(0x0B);
     */

    EPD_W21_WriteCMD(0x11);       // data enter mode
    EPD_W21_WriteDATA(0x03);
    EPD_W21_WriteCMD(0x44);       // set RAM x address start/end, in page 36
    EPD_W21_WriteDATA(0x00);      // RAM x address start at 00h;
    EPD_W21_WriteDATA(0x0f);      // RAM x address end at 0fh(15+1)*8->128    2D13
    EPD_W21_WriteCMD(0x45);       // set RAM y address start/end, in page 37
    EPD_W21_WriteDATA(0x00);      // RAM  y address end at 00h;           2D13
    EPD_W21_WriteDATA(0xf9);      // RAM y address start at FAh;          2D13



    EPD_W21_WriteCMD(0x3C);       // board
    EPD_W21_WriteDATA(0x01);      //GS1-->GS1


    write_look_up_table();
}
void ic1673_set_ram_fun(void)
{

    EPD_W21_WriteCMD(0x4E);       // set RAM x address count to 0;
    EPD_W21_WriteDATA(0x00);
    EPD_W21_WriteCMD(0x4F);       // set RAM y address count to 250;  2D13
    EPD_W21_WriteDATA(0x00);
    EPD_W21_WriteCMD(0x24);
}
static BOOL ic1673_busy_1(void)
{

    if(isEPD_W21_BUSY)
        return TRUE;
    else
        return FALSE;
}
BOOL ic1673_open_display_fun(SCREEN_COLOR_T color)
{
    EPD_W21_WriteCMD(0x22);
    EPD_W21_WriteDATA(0xC7);        //0XC7   Load LUT from MCU(0x32), Display update   0XF7     FROM ÄÚ²¿
    EPD_W21_WriteCMD(0x20);
    Task_sleep(10000);
    if(ic1673_busy_1()!= TRUE)
        return FALSE;
    return TRUE;
}
void display_lcd_IC1673(volatile SCREEN_DIS_T *tp,UINT8 temp)
{

    UINT16 pcnt;
    UINT8 i,j;
    UINT8  data[50];

    j= ((tp->h) + 7) /8;

    ic1673_set_ram_fun();

    for (pcnt = 0; pcnt < tp->w; pcnt++)
    {
        f_read(tp->fd, pcnt * j + tp->offset_1 ,data, j);
        for (i = 0; i < j; i++)
        {
            EPD_W21_WriteDATA(data[i]);
            //EPD_W21_WriteDATA(0x0f);
        }
    }
}
