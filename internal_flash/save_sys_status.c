
#include "save_sys_status.h"

static void _save_sys_status(uint32_t addr) {
    rom_segment_erase(addr);
    rom_segment_write(addr, (WORD)&INFO_DATA, sizeof(INFO_DATA));

}

void save_info_sys_parameter(void) 
{

    INFO_DATA.idcrc = my_cal_crc16(0,(uint8_t *)&INFO_DATA.gRFInitData,sizeof(RFINIT));
    _save_sys_status(INFO_ADDR_1);

} 

bool load_id(uint32_t addr)
{
    uint16_t crc;
    uint8_t i;

    //加载系统配置信息到 INFO_DATA中
    for(i = 0 ; i < 3 ;i++)
    {
        crc = 0;
        //memset((uint8_t *)&INFO_DATA,0,sizeof(INFO_DATA));
        rom_segment_read(addr, (WORD)&INFO_DATA, sizeof(INFO_DATA));
        crc=  my_cal_crc16(crc,(uint8_t *)&INFO_DATA.gRFInitData, sizeof(RFINIT));
        if (INFO_DATA.idcrc == crc)
            return TRUE;

    }
    return FALSE;
}

bool load_info_sys_parameter(void)
{ 
    if(load_id(INFO_ADDR_1))
    {
        return TRUE;
    }
    return FALSE;

}
void test_save_id(void)
{
    uint8_t id1[]={0x5e,0x11,0x22,0x66};
    uint8_t id2[]={0x56,0xB4,0x85,0x13};
    uint8_t id3[]={0x52,0x56,0x78,0x53};
    //    uint8_t id1[]={0x5E,0x11,0x22,0x66};
    //    uint8_t id2[]={0x57,0x31,0x14,0x0C};
    //    uint8_t id3[]={0x52,0x56,0x78,0x53};
    memset(&INFO_DATA.gRFInitData,0x00,sizeof(RFINIT));
    memcpy((void *)&INFO_DATA.gRFInitData.esl_id,id2,4);
    memcpy(&INFO_DATA.gRFInitData.wakeup_id,id1,4);
    memcpy(&INFO_DATA.gRFInitData.master_id,id3,4);
    INFO_DATA.gRFInitData.grp_wkup_ch = 99;
    INFO_DATA.gRFInitData.set_wkup_ch = 99;
    INFO_DATA.gRFInitData.esl_data_ch = 99;
    INFO_DATA.gRFInitData.esl_netmask = 8;
    INFO_DATA.gRFInitData.screen_id = 1387471424;   // 13246758;2.13黑白1675A   //13247014;2.13红色1675A    //13244708; 2.13黑白 1673   //0x00CA3126;  2.9黑白 1608
    rom_segment_erase(INFO_ADDR_1 - 0x0F50);
    save_info_sys_parameter();
}
