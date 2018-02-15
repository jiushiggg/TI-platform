
#include "internal_flash.h"


void rom_segment_erase(WORD seg_addr)
{
    uint32_t temp;
    Task_disable();                               //ʧ�ܵ�������ִֻ�е�ǰ����
    temp = HWREG(GPIO_BASE+GPIO_O_DOUT31_0);      //��ȡ��GPIO״̬�Ĵ�����ֵ
    led_all_off();                                //�ص�
    HapiSectorErase(seg_addr);
    recover_led_status(temp);                     //�ָ��Ƶ�״̬
    Task_enable();
}

void rom_segment_write(WORD addr, WORD data, WORD len)
{
    uint32_t temp;
    Task_disable();                               //ʧ�ܵ�������ִֻ�е�ǰ����
    temp = HWREG(GPIO_BASE+GPIO_O_DOUT31_0);      //��ȡ��GPIO״̬�Ĵ�����ֵ
    led_all_off();                                //�ص�
    HapiProgramFlash((uint8_t *)data, addr,len);
    recover_led_status(temp);                     //�ָ��Ƶ�״̬
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
