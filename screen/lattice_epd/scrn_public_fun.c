#include <ti/drivers/PIN.h>
#include <ti/sysbios/knl/Task.h>
#include "typedef.h"
#include "epd_io.h"
#include "scrn_ic1675a.h"
#include "scrn_ic1673.h"
#include "mini_fs.h"
#include "scrn_public_fun.h"



volatile LATTICE_SCREEN_ATTR_T  screen_t;
volatile SCREEN_DIS_T screen_dis_t;



static void SPI_Delay(UINT8 xrate)
{
    ;
}

static void SPI_Write(UINT8 value)
{
    UINT8 i;


    SPI_Delay(1);
    for(i = 0; i < 8; i++)
    {
        EPD_W21_CLK_0;
        SPI_Delay(2);

        if(value & 0x80)
            EPD_W21_MOSI_1;
        else
            EPD_W21_MOSI_0;
        value = (value << 1);
        SPI_Delay(2);
        EPD_W21_CLK_1;
        SPI_Delay(2);
    }
}

UINT8 EPD_W21_ReadDATA(void)
{

    UINT8 i,j;

    EPD_WF_SDA_IN;
    EPD_W21_CS_0;
    EPD_W21_CLK_0;
    EPD_W21_DC_1;  // command write
    j=0;
    for(i=0; i<8; i++)
    {
        SPI_Delay(2);
        j=(j<<1);
        if(EPD_READ_SDA)
        {
            j |= 0x01;
        }
        else
            j&=0xfe;
        EPD_W21_CLK_1;
        SPI_Delay(2);
        EPD_W21_CLK_0;
    }
    return (j);
}

UINT8 EPD_W21_ReadDATA_2(void)
{

    UINT8 i,j;
    j=0;
    EPD_W21_DC_1;
    for(i=0; i<8; i++)
    {
        SPI_Delay(2);
        j=(j<<1);
        if(EPD_READ_SDA)
            j|=0x01;
        else
            j&=0xfe;
        EPD_W21_CLK_1;
        SPI_Delay(2);
        EPD_W21_CLK_0;
    }
    EPD_W21_CS_1;

    EPD_WF_SDA_OUT;
    return(j);
}

void EPD_W21_WriteCMD(UINT8 command)
{

    SPI_Delay(1);
    EPD_W21_CS_0;
    EPD_W21_DC_0;     // command write
    SPI_Write(command);
    EPD_W21_CS_1;
}

void EPD_W21_WriteDATA(UINT8 command)
{

    SPI_Delay(1);
    EPD_W21_CS_0;
    EPD_W21_DC_1;     // command write
    SPI_Write(command);
    EPD_W21_CS_1;
}


//系统重启后
void sys_rst_close_epd_fun(void)
{
    epd_spi_open();
    epd_powerIO_init();
    close_epd();
}

void screenic_deep_sleep_fun(void)
{

    EPD_W21_WriteCMD(0x10);
    EPD_W21_WriteDATA(0x01);
}
// 二选一
//屏幕进入深睡眠
void epd_power_deep_sleep_fun(void)
{
    epd_spi_open();
    epd_powerIO_init();
    open_epd();
    EPD_W21_BS_0;     // 4 wire spi mode selected
    EPD_W21_WriteCMD(0x12);           //SWRESET
    ic1675a_check_busy_fun();
    screenic_deep_sleep_fun();
    epd_deep_sleep_io();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//点阵屏幕类型解析函数

void screen_analysis_fun(UINT32 screen_id)
{
#define DEF_SCREEN_TYPE         0xC0000000
#define SCREEN_TYPE_BIT_OFFSET  30

#define DEF_SCREEN_TMP          0x00001000
#define SCREEN_TMP_BIT_OFFSET   12

#define DEF_SCREEN_SIZE         0x000F0000
#define SCREEN_SIZE_BIT_OFFSET  16

#define DEF_SCREEN_COLOR        0x0000E000
#define COLOR_BIT_OFFSET        13

#define DEF_SCREEN_FACTORY      0x000000E0
#define FACTORY_BIT_OFFSET      5

#define DEF_SCREEN_IC           0x00000F00
#define IC_BIT_OFFSET           8

#define DEF_SCREEN_DIRECTION    0x00000018
#define SCREEN_DIRECTION        3

    //厂家
    screen_t.src_company   =  (SCREEN_COMPANY_T)((screen_id & DEF_SCREEN_FACTORY) >>FACTORY_BIT_OFFSET);
    //类型
    screen_t.src_specties  =  (SCREEN_SPECIES_T)((screen_id & DEF_SCREEN_TYPE) >>SCREEN_TYPE_BIT_OFFSET);
    //温度
    screen_dis_t.src_tmp   =  (SCREEN_TEMPERATURE_T)((screen_id & DEF_SCREEN_TMP)>>SCREEN_TMP_BIT_OFFSET);
    //颜色
    screen_dis_t.src_color =  (SCREEN_COLOR_T)((screen_id & DEF_SCREEN_COLOR) >> COLOR_BIT_OFFSET);
    //屏幕尺寸
    screen_dis_t.src_size  =  (SCREEN_SIZE_LT)((screen_id & DEF_SCREEN_SIZE) >>SCREEN_SIZE_BIT_OFFSET);
    //屏幕IC
    screen_dis_t.ic_enum   =   (screen_id & DEF_SCREEN_IC) >>IC_BIT_OFFSET;
    //屏幕方向
    screen_dis_t.scr_direction  =  (SCREEN_DEF_DIRECTION_T)((screen_id & DEF_SCREEN_DIRECTION) >>SCREEN_DIRECTION);

    screen_dis_t.refresh_screen_flag  = FALSE;
    if(screen_dis_t.src_color ==BLACK_WHITE_YELLOW)//如果是黄色价签按照红色的处理
        screen_dis_t.src_color =BLACK_WHITE_RED;

    if((screen_dis_t.src_color ==BLACK_WHITE_RED) || (screen_dis_t.src_tmp ==FREEZE_TMP))
    {
        screen_dis_t.refresh_screen_flag  = TRUE;
    }

    if(screen_t.src_specties ==LATTICE_EPD)
    {

        //厂商、芯片
        screen_dis_t.src_into = (((UINT32)screen_t.src_company<<8)) | (((UINT32)screen_dis_t.ic_enum));
        switch(screen_dis_t.src_into)
        {
        case HOLITECH_IC1675A:
        case DKE_IC1675A:

            screen_dis_t.w = ((UINT16)screen_1675A_tp[screen_dis_t.src_size][0]<<8) + screen_1675A_tp[screen_dis_t.src_size][1];
            screen_dis_t.h = ((UINT16)screen_1675A_tp[screen_dis_t.src_size][2]<<8) + screen_1675A_tp[screen_dis_t.src_size][3];
            screen_dis_t.screen_size = ((UINT16)screen_1675A_tp[screen_dis_t.src_size][18]<<8) + screen_1675A_tp[screen_dis_t.src_size][19];
            break;
        case  DKE_IC_1673_BW:
        case  HOLITECH_IC_1673_BW:
            screen_dis_t.w = 250;
            screen_dis_t.h = 122;
            screen_dis_t.screen_size = 213;
            break;

        default:
            break;

        }
    }


}

void screen_dispaly_fun(volatile SCREEN_DIS_T *tp2)
{
#ifndef SCREEN_PROWR_COLES_DISABLE
    open_epd();
#endif
    epd_spi_init();
    switch(tp2->src_into)
    {
    case HOLITECH_IC1675A:
    case DKE_IC1675A:


        ic1675a_swrest_fun();
        init_ic1675a_fun(tp2->src_size);

        display_lcd_ic1675A(tp2,0x24);
        if(tp2->src_color ==BLACK_WHITE_RED)
        {
            display_lcd_red_ic1675a(tp2,0x26);
        }
        if(ic1675a_open_display_fun(tp2->src_color) == FALSE)
        {
            gerr_info = TR3_SCREEN_ERR1;
        }
        ic1675a_check_busy_fun();
        if(low_tep_flag == TRUE)
        {
          EPD_W21_WriteCMD(0x72);
          EPD_W21_WriteDATA(0x80);
        }

        screenic_deep_sleep_fun();
        break;
    case  DKE_IC_1673_BW:
    case  HOLITECH_IC_1673_BW:
        ic1673_swrest_fun();
        init_ic1673_fun();
        display_lcd_IC1673(tp2,0x24);
        if(ic1673_open_display_fun(tp2->src_color) == FALSE)
        {
            gerr_info = TR3_SCREEN_ERR1;
        }
        ic1675a_check_busy_fun();
        screenic_deep_sleep_fun();
        break;
    default:
        break;

    }

    epd_deep_sleep_io();
#ifndef SCREEN_PROWR_COLES_DISABLE
    close_epd();
#endif
}
