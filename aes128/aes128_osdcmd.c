#include "aes128.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "mini_fs_conf.h"
#include "mini_fs.h"
#include "osd4_0_cmd.h"
#include "global_variable.h"
#include "crc16.h"
#include "gunzip.h"
#include "storage_struct.h"
#include "aes128_osdcmd.h"





int32_t cmd10_fun(file_id_t cmdfd, UINT32 offset )
{
#define CMD10_INFO_LEN 37
    uint8_t temp[CMD10_INFO_LEN];
    uint16_t crc = 0;



    f_read(cmdfd, offset, temp, CMD10_INFO_LEN);

    //检查密钥索引合法性
    if((temp[1] != CRYPTOCC26XX_KEY_2) ||(temp[18] != CRYPTOCC26XX_KEY_3))
        goto cmd10_ret;

    //计算crc是否正确
    crc = my_cal_crc16(crc,temp,CMD10_INFO_LEN - 2);
    if(memcmp((uint8_t *)&crc,temp+35,2) !=0 )
        goto cmd10_ret;

    //替换密钥
    memcpy(&aes128.aes128_key_info[1],temp+1,34);
    aes128.flag.isencryption = 1;//设置为加密状态
    //保存密钥
    if(save_main_communication_key()==false)
    {
        gerr_info = (GERR_T)AES_SAVE_KEY_ERR;
    }

    cmd10_ret:

    return CMD10_INFO_LEN;
}

int32_t cmd11_fun(file_id_t cmdfd, UINT32 offset )
{
#define CMD11_INFO_LEN 20
    uint8_t temp[CMD11_INFO_LEN];
    uint16_t crc = 0;

    f_read(cmdfd, offset, temp, CMD11_INFO_LEN);

    //检查密钥索引合法性
    if(temp[1] != CRYPTOCC26XX_KEY_3)//通讯密钥所以
        goto cmd11_ret;

    //计算crc是否正确
    crc = my_cal_crc16(crc,temp,CMD11_INFO_LEN - 2);
    if(memcmp((uint8_t *)&crc,temp+18,2) !=0 )
        goto cmd11_ret;

    //替换密钥
    memcpy(&aes128.aes128_key_info[2],temp+1,17);
    //保存密钥
    if(save_main_communication_key()==false)
    {
        gerr_info = (GERR_T)AES_SAVE_KEY_ERR;
    }

    cmd11_ret:

    return CMD11_INFO_LEN;
}

int32_t cmd12_fun(file_id_t cmdfd, UINT32 offset )
{
#define CMD12_INFO_LEN 5
    uint8_t temp[CMD12_INFO_LEN];
    uint16_t crc = 0;


    f_read(cmdfd, offset, temp, CMD12_INFO_LEN);

    //计算主密钥crc是否正确
    crc = my_cal_crc16(crc,(uint8_t *)&aes128.aes128_key_info[1].key,16);
    if(memcmp((uint8_t *)&crc,temp+1,2) !=0 )
        goto cmd12_ret;

    //计算命令流crc是否正确
    crc = 0;
    crc = my_cal_crc16(crc,temp,3);
    if(memcmp((uint8_t *)&crc,temp+3,2) !=0 )
        goto cmd12_ret;

    //恢复密钥=索引和密钥全部修改成0xff
    memset((uint8_t *)&aes128.aes128_key_info[1],0xff,34);
    aes128.flag.isencryption = 0;//设置为未加密状态
    //保存密钥
    if(save_main_communication_key()==false)
    {
        gerr_info = (GERR_T)AES_SAVE_KEY_ERR;
    }

    cmd12_ret:

    return CMD12_INFO_LEN;
}


int32_t cmd13_fun(file_id_t cmdfd, UINT32 offset )
{
#define CMD13_INFO_LEN 5
    uint8_t temp[CMD13_INFO_LEN];
    uint16_t crc = 0;

    f_read(cmdfd, offset, temp, CMD13_INFO_LEN);

    //计算恢复密钥crc是否正确
    crc = my_cal_crc16(crc,(uint8_t *)&aes128.aes128_key_info[3].key,16);
    if(memcmp((uint8_t *)&crc,temp+1,2) !=0 )
        goto cmd13_ret;

    //计算命令流crc是否正确
    crc = 0;
    crc = my_cal_crc16(crc,temp,3);
    if(memcmp((uint8_t *)&crc,temp+3,2) !=0 )
        goto cmd13_ret;

    //恢复密钥=索引和密钥全部修改成0xff
    memset((uint8_t *)&aes128.aes128_key_info[1],0xff,34);
    aes128.flag.isencryption = 0;//设置为未加密状态
    //保存密钥
    if(save_main_communication_key()==false)
    {
        gerr_info = (GERR_T)AES_SAVE_KEY_ERR;
    }

    cmd13_ret:

    return CMD13_INFO_LEN;
}
