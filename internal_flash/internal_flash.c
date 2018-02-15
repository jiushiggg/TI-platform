
#include "internal_flash.h"


void rom_segment_erase(WORD seg_addr)
{
    uint32_t temp;
    Task_disable();                               //失能调度器，只执行当前任务
    temp = HWREG(GPIO_BASE+GPIO_O_DOUT31_0);      //获取灯GPIO状态寄存器的值
    led_all_off();                                //关灯
    HapiSectorErase(seg_addr);
    recover_led_status(temp);                     //恢复灯的状态
    Task_enable();
}

void rom_segment_write(WORD addr, WORD data, WORD len)
{
    uint32_t temp;
    Task_disable();                               //失能调度器，只执行当前任务
    temp = HWREG(GPIO_BASE+GPIO_O_DOUT31_0);      //获取灯GPIO状态寄存器的值
    led_all_off();                                //关灯
    HapiProgramFlash((uint8_t *)data, addr,len);
    recover_led_status(temp);                     //恢复灯的状态
    Task_enable();
}
void rom_segment_read(WORD addr,WORD buf, WORD len)
{
    uint32_t i;
    char *pdst = (char *)buf;
    for(i = 0; i < len; i++)
    {
        *(pdst+ i) = HWREGB(addr + i);
    }
}
