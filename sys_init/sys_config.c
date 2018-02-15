
#include "sys_config.h"
#include "save_sys_status.h"



//------------------load sys configuration---------------------------

bool block_data_check(uint32_t start_addr, uint32_t offset, uint8_t *buffer, uint16_t len)  //起始地址必须是4k对齐
{
    uint8_t i = 0;
    uint16_t crc_temp = 0, crc = 0;
    for(i=0;i<3; i++)
    {
        rom_segment_read(start_addr, (WORD)cache_buf, EF_BLOCK_SIZE);
        crc=  my_cal_crc16(0, (uint8_t *)cache_buf, EF_BLOCK_SIZE-2);
        if(0 == memcmp(cache_buf+EF_BLOCK_SIZE-2,(uint8_t *)&crc,sizeof(crc)))
            break;
    }
    memcpy(cache_buf + offset, buffer, len);
    crc_temp =  my_cal_crc16(0, (uint8_t *)cache_buf, EF_BLOCK_SIZE-2);
    memcpy(cache_buf+EF_BLOCK_SIZE-2,(uint8_t *)&crc_temp,sizeof(crc_temp));
    for(i=0;i<3; i++)
    {
        rom_segment_erase(start_addr);
        rom_segment_write(start_addr, (WORD)cache_buf, EF_BLOCK_SIZE);
        rom_segment_read(start_addr, (WORD)cache_buf_1, EF_BLOCK_SIZE/2);
        crc=  my_cal_crc16(0, (uint8_t *)cache_buf_1, EF_BLOCK_SIZE/2);
        rom_segment_read(start_addr + EF_BLOCK_SIZE/2, (WORD)cache_buf_1, EF_BLOCK_SIZE/2);
        crc=  my_cal_crc16(crc, (uint8_t *)cache_buf_1, EF_BLOCK_SIZE/2 -2);
        if((0 == memcmp(&crc_temp, &crc, sizeof(crc))))
            return TRUE ;
    }
    return FALSE;
}



bool load_sys_config_info(void)
{
    uint16_t crc = 0;
    uint8_t i= 0;
    bool  ret = FALSE;
    for(i = 0 ; i < 3 ;i++)
    {
        crc=0;
        memset((uint8_t *)&def_sys_attr,0x00,sizeof(def_sys_attr));
        rom_segment_read(ROM_FLASH_BLOCK1_ADDR, (WORD)&def_sys_attr, sizeof(def_sys_attr));
        crc=  my_cal_crc16(crc,((uint8_t *)(&def_sys_attr))+sizeof(def_sys_attr.attrcrc), sizeof(def_sys_attr)-sizeof(def_sys_attr.attrcrc));
        if (def_sys_attr.attrcrc == crc)
        {
            ret =  TRUE;
            break;
        }
    }
    if((def_sys_attr.sys_def_attribute.grp_wkup_time == 0)||(def_sys_attr.sys_def_attribute.heartbit_time == 0)||
            (def_sys_attr.sys_def_attribute.set_wkup_time == 0)||(def_sys_attr.sys_def_attribute.work_time == 0))
    {
        ret =  FALSE;
    }
    return ret;

}

//----------------load rf info------------------------
#define SECTOR_LEN     512
#if 0
bool save_sys_load_page_info(void)
{
    uint8_t i = 0;
    SYS_LOAD_TYPE tp;
    for(i=0;i<3; i++)
    {
        rom_segment_erase(ROM_FLASH_BLOCK4_ADDR);
        rom_segment_write(ROM_FLASH_BLOCK4_ADDR, (WORD)&gSys_tp, sizeof(gSys_tp));
        rom_segment_read(ROM_FLASH_BLOCK4_ADDR, (WORD)&tp, sizeof(gSys_tp));
        if((0 == memcmp(&gSys_tp, &tp, sizeof(tp))))
            return TRUE ;
    }
    return FALSE;
}
#else

bool save_sys_load_page_info(void)
{
    uint8_t ret, i = 0;
    for(i=0;i<3; i++)
    {
        ret = block_data_check(ROM_FLASH_BLOCK0_ADDR, ROM_FLASH_BLOCK4_ADDR - ROM_FLASH_BLOCK0_ADDR, (uint8_t *)&gSys_tp, sizeof(gSys_tp));
        if(ret == TRUE)
            break;
    }
    return ret;
}
#endif

bool load_extern_rf_info(void)
{
    uint8_t id1[]={0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff,0xff,0xff,0xff};
    uint8_t id2[15]={0};
    uint16_t crc = 0;
    uint8_t i = 0;
    for(i=0;i<3; i++)
    {
        crc = 0;
        memset((uint8_t *)&INFO_DATA,0x00,sizeof(INFO_DATA));
        rom_segment_read(ROM_FLASH_BLOCK0_ADDR, (WORD)&INFO_DATA, sizeof(INFO_DATA));
        crc=  my_cal_crc16(crc,(uint8_t *)&INFO_DATA.gRFInitData, sizeof(RFINIT));
        if (INFO_DATA.idcrc == crc)
        {
            if(0 == memcmp(&INFO_DATA.gRFInitData, id1, sizeof(id1)))
                return FALSE;
            if(0 == memcmp(&INFO_DATA.gRFInitData, id2, sizeof(id2)))
                return FALSE;
            return TRUE;
        }
    }

    return FALSE;

}

#if 0
void save_extern_rf_info(void)
{
    uint16_t crc = 0;
    uint8_t i = 0;
    struct     INFO_DATA_T  tp;
    for(i=0;i<3; i++)
    {
        crc = 0;
        INFO_DATA.idcrc =  my_cal_crc16(crc,(uint8_t *)&INFO_DATA.gRFInitData, sizeof(RFINIT));
        rom_segment_erase(ROM_FLASH_BLOCK0_ADDR);
        rom_segment_write(ROM_FLASH_BLOCK0_ADDR, (WORD)&INFO_DATA, sizeof(INFO_DATA));
        rom_segment_read(ROM_FLASH_BLOCK0_ADDR, (WORD)&tp, sizeof(INFO_DATA));
        if((0 == memcmp(&INFO_DATA, &tp, sizeof(tp))))
            return ;
    }
}
#else
void save_extern_rf_info(void)
{
    uint8_t ret, i = 0;
    for(i=0;i<3; i++)
    {
        ret = block_data_check(ROM_FLASH_BLOCK0_ADDR, 0, (uint8_t *)&INFO_DATA, sizeof(INFO_DATA));
        if(ret == TRUE)
            break;
    }
}
#endif
//-------------------------保存属性包---------------------------------
#if 0
void save_state_info_fun(void)
{
    uint8_t i = 0;
    EPD_ATTRIBUTE_PKG_ST tp;
    for(i=0;i<3; i++)
    {
        memset((uint8_t *)&tp,0x00,sizeof(EPD_ATTRIBUTE_PKG_ST));
        rom_segment_erase(ROM_FLASH_BLOCK2_ADDR);
        rom_segment_write(ROM_FLASH_BLOCK2_ADDR, (WORD)&epd_attr_info, sizeof(EPD_ATTRIBUTE_PKG_ST));
        rom_segment_read(ROM_FLASH_BLOCK2_ADDR, (WORD)&tp, sizeof(EPD_ATTRIBUTE_PKG_ST));
        if((0 == memcmp(&epd_attr_info, &tp, sizeof(tp))))
            return ;
    }
    //属性包错误是否置全局错误标志
}
#else
void save_state_info_fun(void)
{
    uint8_t ret, i = 0;
    for(i=0;i<3; i++)
    {
        ret = block_data_check(ROM_FLASH_BLOCK0_ADDR, ROM_FLASH_BLOCK2_ADDR - ROM_FLASH_BLOCK0_ADDR, (uint8_t *)&epd_attr_info, sizeof(epd_attr_info));
        if(ret == TRUE)
            break;
    }
}
#endif
//--------------------------------------------------------------------



void sys_config_deaf_cont(void)
{
    def_sys_attr.uplink_defalut_channel = 2;
    def_sys_attr.power_bottom = MIN_POWER_BOTTOM_VALUE;
    def_sys_attr.power_ceiling = MAX_POWER_CEILING_VALUE;

    def_sys_attr.sys_def_attribute.set_wkup_time = 16;
    def_sys_attr.sys_def_attribute.grp_wkup_time = 4;
    def_sys_attr.sys_def_attribute.heartbit_time= 180;
    def_sys_attr.sys_def_attribute.work_time = 4;

    def_sys_attr.rf_wk_md = RF_A7106_SET_WKUP_MODE;
    def_sys_attr.exit_grp_wkup_cont = GROUP_MAX_TIMES;
    def_sys_attr.gwor_flag_now = RF_FSM_CMD_SET_EXIT_LOOP;
    def_sys_attr.gwor_flag_before = RF_FSM_CMD_SET_EXIT_LOOP;

}

uint8_t hb_setwor_fun(void)
{
    uint8_t ret =3;
    uint8_t BUFFER[49]={0,0,0,1,1,2,2,2,2,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5};
    if( (def_sys_attr.sys_def_attribute.set_wkup_time == 0) || ( (def_sys_attr.sys_def_attribute.set_wkup_time %2) !=0 ) ||(def_sys_attr.sys_def_attribute.set_wkup_time > 64))
    {
        ret = 7;
        goto loop;
    }
#if 0  
    //  if(def_sys_attr.sys_def_attribute.set_wkup_time <= 2 )
    //  {
    //    ret = 0;
    //  }
    //  else if(def_sys_attr.sys_def_attribute.set_wkup_time <= 4)
    //  {
    //    ret = 1;
    //  }
    //  else if( (def_sys_attr.sys_def_attribute.set_wkup_time <=8))
    //  {
    //    ret = 2;
    //  }
    //  else if( (def_sys_attr.sys_def_attribute.set_wkup_time <=16))
    //  {
    //    ret = 3;
    //  }
    //  else if((def_sys_attr.sys_def_attribute.set_wkup_time <=32))
    //  {
    //    ret = 4;
    //  }
    //  else if( (def_sys_attr.sys_def_attribute.set_wkup_time <=48))
    //  {
    //    ret = 5;
    //  }
    //  else
    //  {
    //    ret = 6;
    //  }
#else
    if(def_sys_attr.sys_def_attribute.set_wkup_time <=48)
    {
        ret = BUFFER[def_sys_attr.sys_def_attribute.set_wkup_time];
    }
    else
    {
        ret = 6;
    }
#endif  
    loop:

    ret =  ret<<5;
    ret |= 0x08;
    return ret;

}

#if 0
bool save_sys_config_info(void)
{
    uint16_t crc = 0;
    uint8_t i= 0;
    struct SYS_ATTR_T tp;
    for(i=0;i<3; i++)
    {
        crc = 0;
        crc=  my_cal_crc16(crc,((uint8_t *)(&def_sys_attr))+sizeof(def_sys_attr.attrcrc), sizeof(def_sys_attr)-sizeof(def_sys_attr.attrcrc));
        def_sys_attr.attrcrc = crc;

        rom_segment_erase(ROM_FLASH_BLOCK1_ADDR);
        rom_segment_write(ROM_FLASH_BLOCK1_ADDR, (WORD)&def_sys_attr, sizeof(def_sys_attr));
        rom_segment_read(ROM_FLASH_BLOCK1_ADDR, (WORD)&tp, sizeof(def_sys_attr));
        if((0 == memcmp(&def_sys_attr, &tp, sizeof(tp))))
            return TRUE;
    }
    return TRUE;
}
#else
bool save_sys_config_info(void)
{
    uint8_t ret, i = 0;
    uint16_t crc = 0;
    for(i=0;i<3; i++)
    {
        crc = 0;
        crc=  my_cal_crc16(crc,((uint8_t *)(&def_sys_attr))+sizeof(def_sys_attr.attrcrc), sizeof(def_sys_attr)-sizeof(def_sys_attr.attrcrc));
        def_sys_attr.attrcrc = crc;
        ret = block_data_check(ROM_FLASH_BLOCK0_ADDR, ROM_FLASH_BLOCK1_ADDR - ROM_FLASH_BLOCK0_ADDR, (uint8_t *)&def_sys_attr, sizeof(def_sys_attr));
        if(ret == TRUE)
            break;
    }
    return ret;
}
#endif
void sys_load_config_info(void)
{
    bool ret = FALSE;

    hbr_mode = NORMAL_HEARTBEAT;                          //心跳上电加载为默认心跳模式
    ret = load_sys_config_info();                         //加载rf工作参数
    if(ret == FALSE)
    {
        sys_config_deaf_cont();
        save_sys_config_info();                             //存储区的配置信息错误，是否要把默认值写回去
    }


    ret = load_extern_rf_info();                          //加载rf配置信息
    if(ret == TRUE)                                       //外部配置信息正确
        return ;

    ret = load_info_sys_parameter();                      //读取内部info信息
    if(ret == FALSE)                                      //加载RF射频测试错误
    {
        while(1);
    }

    save_extern_rf_info();

}
