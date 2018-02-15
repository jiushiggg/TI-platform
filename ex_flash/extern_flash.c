
#include "extern_flash.h"

NVS_Handle nvsHandle;
NVS_Attrs regionAttrs;
NVS_Params nvsParams;

void init_nvs_spi_flash(void)
{
#ifdef HAVE_EX_FLASH
    NVS_init();
    NVS_Params_init(&nvsParams);
    nvsHandle = NVS_open(Board_NVS1, &nvsParams);
    if(!nvsHandle)
    {
        while(1);
    }
#endif
}

void extern_flash_open(void)
{
#ifdef HAVE_EX_FLASH
    nvsHandle = NVS_open(Board_NVS1, &nvsParams);
#endif
}
void extern_flash_close(void)
{
#ifdef HAVE_EX_FLASH
    NVS_close(nvsHandle);           //flash����sleep״̬
#endif
}
void segment_erase(WORD seg_addr)
{
    uint32_t temp;
    Task_disable();                               //ʧ�ܵ�������ִֻ�е�ǰ����
    temp = HWREG(GPIO_BASE+GPIO_O_DOUT31_0);      //��ȡ��GPIO״̬�Ĵ�����ֵ
    led_all_off();                                //�ص�
    NVS_erase(nvsHandle, seg_addr, EF_BLOCK_SIZE);
    recover_led_status(temp);                       //�ָ��Ƶ�״̬
    Task_enable();

}

void segment_write(WORD addr, WORD data, WORD len)
{
    uint32_t temp;
    Task_disable();                               //ʧ�ܵ�������ִֻ�е�ǰ����
    temp = HWREG(GPIO_BASE+GPIO_O_DOUT31_0);      //��ȡ��GPIO״̬�Ĵ�����ֵ
    led_all_off();                                //�ص�
    NVS_write(nvsHandle, addr, (void *)data, len,  NVS_WRITE_POST_VERIFY);
    recover_led_status(temp);                     //�ָ��Ƶ�״̬
    Task_enable();
}

void segment_read(WORD addr, WORD buf, WORD len)
{
    NVS_read(nvsHandle, addr, (void *) buf,len);
}
