
#include "led.h"
#include "global_variable.h"


bool normal_or_horse_led_Fxn(void)
{
    bool xStatus;
    uint16_t led_flash_number= led_tp.period;
    uint8_t  normal_led_flag = led_tp.led_num;
    uint8_t move_temp;

    move_temp = led_tp.led_map[0];
    led_tp.led_map[0] = (move_temp>>4 | led_tp.led_map[0]<<4);
    move_temp = led_tp.led_map[1];
    led_tp.led_map[1] = (move_temp>>4 | led_tp.led_map[1]<<4);
    move_temp = led_tp.led_map[2];
    led_tp.led_map[2] = (move_temp>>4 | led_tp.led_map[2]<<4);
    move_temp = led_tp.led_map[3];
    led_tp.led_map[3] = (move_temp>>4 | led_tp.led_map[3]<<4);

    while(led_flash_number)
    {
        led_flash_number--;
        if(normal_led_flag)
        {
            xStatus = one_cycle_normal_led();
            if(!xStatus)
                return 0;
        }else
        {
            xStatus = one_cycle_horse_led();
            if(!xStatus)
                return 0;
        }
    }
    return 1;
}
bool one_cycle_normal_led(void)
{
    uint8_t one_cycle_T0_count = led_tp.t0_cont;
    bool xStatus;

    while(one_cycle_T0_count)
    {
        one_cycle_T0_count--;
        normal_led_color_define(led_tp.led_num);
        xStatus = Semaphore_pend(led_off_sem, led_tp.t0_time * 3000);
        if(xStatus)
        {
            return 0;
        }
        led_all_off();
        xStatus = Semaphore_pend(led_off_sem, led_tp.t1_time * 3000);
        if(xStatus)
        {
            return 0;
        }
    }
    normal_led_color_define(led_tp.led_num);
    xStatus = Semaphore_pend(led_off_sem, led_tp.t0_time * 3000);
    if(xStatus)
    {
        return 0;
    }
    led_all_off();
    xStatus = Semaphore_pend(led_off_sem, led_tp.t2_time * 3000);
    if(xStatus)
    {
        return 0;
    }
    return 1;
}
bool one_cycle_horse_led(void)
{
#define SINGKE_LED_FLAG ((uint32_t)0x0000000f)

    bool xStatus;
    uint8_t one_cycle_T0_count = led_tp.t0_cont;

    uint32_t temp = *(uint32_t*)led_tp.led_map;
    uint32_t tp= (temp & SINGKE_LED_FLAG);

    while(tp)
    {
        while(one_cycle_T0_count)
        {
            one_cycle_T0_count--;
            horse_led_color_define(tp);
            xStatus = Semaphore_pend(led_off_sem, led_tp.t0_time * 3000);
            if(xStatus)
            {
                return 0;
            }
            led_all_off();
            xStatus = Semaphore_pend(led_off_sem, led_tp.t1_time * 3000);
            if(xStatus)
            {
                return 0;
            }
        }
        horse_led_color_define(tp);
        xStatus = Semaphore_pend(led_off_sem, led_tp.t0_time * 3000);
        if(xStatus)
        {
            return 0;
        }
        led_all_off();
        xStatus = Semaphore_pend(led_off_sem, led_tp.t2_time * 3000);
        if(xStatus)
        {
            return 0;
        }
        temp = temp>>4;
        tp= (temp & SINGKE_LED_FLAG);
        one_cycle_T0_count = led_tp.t0_cont;
    }
    return 1;
}

void led_all_off(void)
{
    LED_RED_OFF;
    LED_GREEN_OFF;
    LED_BLUE_OFF;
}

void normal_led_color_define(UINT8 tp)
{

    if(led_tp_info.led_off == TRUE)
    {
        led_all_off();
        return ;
    }
    switch(tp)
    {
    case 0x01:
        LED_BLUE_ON;
        break;
    case 0x02:
        LED_RED_ON;
        break;
    case 0x03:
        LED_RED_ON;
        LED_BLUE_ON;
        break;
    case 0x04:
        LED_GREEN_ON;
        break;
    case 0x05:
        LED_GREEN_ON;
        LED_BLUE_ON;
        break;
    case 0x06:
        LED_GREEN_ON;
        LED_RED_ON;
        break;
    case 0x07:
        LED_RED_ON;
        LED_GREEN_ON;
        LED_BLUE_ON;
        break;
    default:
        LED_GREEN_ON;
        break;
    }
}

void horse_led_color_define(UINT8 tp)
{

    if(led_tp_info.led_off == TRUE)
    {
        led_all_off();
        return ;
    }
    switch(tp)
    {
    case 0x01:
        LED_BLUE_ON;
        break;
    case 0x02:
        LED_RED_ON;
        break;
    case 0x03:
        LED_RED_ON;
        LED_BLUE_ON;
        break;
    case 0x04:
        LED_GREEN_ON;
        break;
    case 0x05:
        LED_GREEN_ON;
        LED_BLUE_ON;
        break;
    case 0x06:
        LED_GREEN_ON;
        LED_RED_ON;
        break;
    case 0x07:
        LED_RED_ON;
        LED_GREEN_ON;
        LED_BLUE_ON;
        break;
    default:
        LED_GREEN_ON;
        break;
    }
}
void def_rc_attr_info_fun(void)
{
    UINT8 aa[4] = {0x12,0x34,0x56,0x70};
    memset((UINT8 *)&rc_attr_info,0x00,sizeof(rc_attr_info));
    rc_attr_info.t0_cont = 5;
    rc_attr_info.t0_time = 2;
    rc_attr_info.t1_time = 2;
    rc_attr_info.t2_time = 20;
    rc_attr_info.period = 2;
    rc_attr_info.led_num = 7;    //green
    memcpy(rc_attr_info.led_map,aa,4);
}

void def_led_attr_info_fun(void)
{
    UINT16 code = 60488;
    memset((UINT8 *)&epd_attr_info,0x00,sizeof(epd_attr_info));
    epd_attr_info.t0_cont = 3;//1\3\5
    epd_attr_info.t0_time = 1;
    epd_attr_info.t1_time = 1;
    epd_attr_info.t2_time = 30;
    epd_attr_info.led_num = 4;//green
    epd_attr_info.period = 5;
    memcpy(epd_attr_info.secur_code,(UINT8 *)&code,sizeof(code));
    // epd_attr_info.secur_code = 60488;
    epd_attr_info.magnet = 0x09;
}

void load_attribute_pkg_fun(void)
{
    UINT16 crc = 0;
    UINT8 i = 0;
    UINT8 id2[16] = {0};

    for(i=0;i<3; i++)
    {
        crc = 0;
        memset((UINT8 *)&epd_attr_info,0x00,sizeof(epd_attr_info));
        rom_segment_read(ROM_FLASH_BLOCK2_ADDR, (WORD)&epd_attr_info, sizeof(epd_attr_info));
        crc =  my_cal_crc16(crc,(UINT8 *)&epd_attr_info, sizeof(epd_attr_info)-sizeof(epd_attr_info.global_crc));
        if (epd_attr_info.global_crc == crc)
        {
            if(0 != memcmp(&epd_attr_info, id2, sizeof(id2)))
                goto loop;
        }
    }

    def_led_attr_info_fun();
    loop:
    led_tp_init_fun(TRUE);

}

void rc_led_init(void)
{

    if((rc_attr_info.t0_time ==0)&&(rc_attr_info.t1_time ==0)&&(rc_attr_info.t2_time ==0)&&(rc_attr_info.period ==0) ||(rc_attr_info.period>0))
    {
        if(led_flash_flag == TRUE)
        {
            Semaphore_post(led_off_sem);
        }
    }
    if(rc_attr_info.period>0)
    {
        led_tp_init_fun(0);
        if(led_flash_flag == TRUE)
        {
            Semaphore_post(led_off_sem);
            Semaphore_post(led_flash_sem);
        }else
        {
            Semaphore_post(led_flash_sem);
        }
    }

}

#define RED_LED_REGISTER_BIT      (uint32_t)(1 << 1)
#define GREEN_LED_REGISTER_BIT    (uint32_t)(1 << 14)
#define BLUE_LED_REGISTER_BIT     (uint32_t)(1 << 0)

void recover_led_status(uint32_t tp)
{
    if((~tp)&RED_LED_REGISTER_BIT)
    {
        LED_RED_ON;
    }
    if((~tp)&GREEN_LED_REGISTER_BIT)
    {
        LED_GREEN_ON;
    }
    if((~tp)&BLUE_LED_REGISTER_BIT)
    {
        LED_BLUE_ON;
    }
}

