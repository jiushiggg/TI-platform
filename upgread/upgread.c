#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdint.h>
#include "mini_fs_conf.h"
#include "mini_fs.h"
#include "global_variable.h"
#include "crc16.h"
#include "three_event.h"
#include "ti/devices/cc26x0r2/driverlib/sys_ctrl.h"

#if 0
bool save_upgread_flag_fun(void)
{
    UINT8 i = 0;
    ROM_UP_T  tp;
    for(i=0;i<3; i++)
    {
        rom_segment_erase(ROM_FLASH_BLOCK3_ADDR);
        rom_segment_write(ROM_FLASH_BLOCK3_ADDR, (WORD)&updata_info, sizeof(ROM_UP_T));
        rom_segment_read(ROM_FLASH_BLOCK3_ADDR, (WORD)&tp, sizeof(ROM_UP_T));
        if((0 == memcmp(&updata_info, &tp, sizeof(ROM_UP_T))))
            return TRUE;
    }
    return FALSE;
}
#else
bool save_upgread_flag_fun(void)
{
    uint8_t ret, i = 0;
    for(i=0;i<3; i++)
    {
        ret = block_data_check(ROM_FLASH_BLOCK0_ADDR, ROM_FLASH_BLOCK3_ADDR - ROM_FLASH_BLOCK0_ADDR, (uint8_t *)&updata_info, sizeof(updata_info));
        if(ret == TRUE)
            break;
    }
    return ret;
}

#endif

#define FIRMWARE_LEN  4
#define PRODUCT_LEN   4
void write_upgread_flag_fun(void)
{
    uint8_t firmware_buf[128];
    uint16_t i, rom_offset, crc=0;
    ROM_BIN_FORMAT_T tp;
    UPDATA_ADDR_FORMAT_T updata_addr_format;

#ifdef HAVE_EX_FLASH

    f_read(F_BMP_DATA, 0, (UINT8 *)&tp, sizeof(ROM_BIN_FORMAT_T));
    f_read(F_BMP_DATA, sizeof(ROM_BIN_FORMAT_T), firmware_buf,  tp.firmware_num * PRODUCT_LEN);
#else

    rom_segment_read(EF_STAA_DDR, (WORD)&tp, sizeof(ROM_BIN_FORMAT_T));     //取出bin文件中的升级区域标志，升级bin的ROM version，以及firmware个数
    rom_segment_read(EF_STAA_DDR + sizeof(ROM_BIN_FORMAT_T), (WORD)firmware_buf, tp.firmware_num * PRODUCT_LEN);
    crc=  my_cal_crc16(0, (uint8_t *)&tp.updata_area, sizeof(ROM_BIN_FORMAT_T)-2);
    crc=  my_cal_crc16(crc, (uint8_t *)firmware_buf, tp.firmware_num * PRODUCT_LEN);
#endif

    switch(tp.updata_area)
    {

    case UP_APP_AREA:

        for(i=0;i<tp.firmware_num*4;i+=4)
        {
            //            if((esl_firmware_id == (firmware_buf[i] + (firmware_buf[i+1]<<8) + (firmware_buf[i+2]<<16) + (firmware_buf[i+3]<<24)))&&(tp.rom_version != EPD_3_VERSION))
            if((0 == memcmp((UINT8 *)&esl_firmware_id, (UINT8 *)&firmware_buf[i], sizeof(esl_firmware_id)))&&(tp.rom_version != EPD_3_VERSION))
            {
                goto here;
            }
        }
        break;

    case UP_ALL_CODE_AREA:
        for(i=0;i<tp.firmware_num*4;i+=4)
        {
            if((0 == memcmp((UINT8 *)&firmware_buf[i], (UINT8 *)&INFO_DATA.gRFInitData.screen_id, sizeof(INFO_DATA.gRFInitData.screen_id)))&&(tp.rom_version != EPD_3_VERSION))
            {
                goto here;
            }
        }
        break;
    case UP_SCHOOL_APP_AREA:
        if((0 == memcmp((UINT8 *)firmware_buf, (UINT8 *)&school_id, sizeof(school_id)))&&(tp.rom_version != EPD_3_VERSION))
        {
            goto here;
        }
        break;
    default:
        break;
    }
    upgread_id = 0;                           //收到的bin文件错误，才会走到这里。清除收到的数据和包号缓存区，升级id清零
    fs_erase_all_file();
    memset(pkg_bit_map,0xff,G_PKG_BIT_MAP_LEN);
    return;

    here:            //到了这里说明可以升级，以下是保存升级需要的相关参数

#ifdef HAVE_EX_FLASH

    f_read(F_BMP_DATA, sizeof(ROM_BIN_FORMAT_T) + tp.firmware_num * PRODUCT_LEN, (UINT8 *)&updata_addr_format,sizeof(UPDATA_ADDR_FORMAT_T));
    updata_info.src_addr = f_addr(F_BMP_DATA) + sizeof(ROM_BIN_FORMAT_T) + tp.firmware_num * PRODUCT_LEN + sizeof(UPDATA_ADDR_FORMAT_T);
#else

    rom_segment_read(EF_STAA_DDR + sizeof(ROM_BIN_FORMAT_T) + tp.firmware_num * PRODUCT_LEN, (WORD)&updata_addr_format, sizeof(UPDATA_ADDR_FORMAT_T));
    updata_info.src_addr = EF_STAA_DDR + sizeof(ROM_BIN_FORMAT_T) + tp.firmware_num * PRODUCT_LEN + sizeof(UPDATA_ADDR_FORMAT_T);
    crc=  my_cal_crc16(crc, (uint8_t *)&updata_addr_format, sizeof(UPDATA_ADDR_FORMAT_T));
#endif

    updata_info.crc = updata_addr_format.rom_map_crc;
    updata_info.len = updata_addr_format.rom_len;
    updata_info.flag = GROM_UPDATA_FLAG;
    updata_info.upgread_type = tp.updata_area;
    updata_info.dst_addr = updata_addr_format.start_addr;

    rom_offset = updata_info.src_addr - EF_STAA_DDR;      //真正的bin文件的偏移
    for(i=0;i<updata_info.len;)
    {
        if(i==0)
        {
            rom_segment_read(updata_info.src_addr, (WORD)cache_buf, EF_BLOCK_SIZE - rom_offset);
            crc = my_cal_crc16(crc, (uint8_t *)cache_buf, EF_BLOCK_SIZE - rom_offset);
            i = EF_BLOCK_SIZE - rom_offset;
        }else if((updata_info.len - i) >= EF_BLOCK_SIZE)
        {
            rom_segment_read(updata_info.src_addr + i, (WORD)cache_buf, EF_BLOCK_SIZE);
            crc = my_cal_crc16(crc, (uint8_t *)cache_buf, EF_BLOCK_SIZE);
            i += EF_BLOCK_SIZE;
        }else
        {
            rom_segment_read(updata_info.src_addr + i, (WORD)cache_buf, (updata_info.len - i));
            crc = my_cal_crc16(crc, (uint8_t *)cache_buf, (updata_info.len - i));
            i = updata_info.len;
        }
    }
    if(crc != tp.all_bin_crc)
    {
        upgread_id = 0;
        fs_erase_all_file();
        memset(pkg_bit_map,0xff,G_PKG_BIT_MAP_LEN);
        return;
    }
    save_upgread_flag_fun(); //保存升级参数和升级标志
    SysCtrlSystemReset();    //使系统复位

}

