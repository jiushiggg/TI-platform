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
#include "../sys_init/sys_config.h"

AESECB_T  aes128={0};


bool check_initkey_permissions_fun(uint8_t type)
{
    if( (aes128.flag.isencryption == 1) && (type == 1) && (aes128.flag.aes_enable == 1) )//支持加密，加了密，是初始化密钥指令
    {
        gerr_info = (GERR_T)AES_PERMISSIONS_ERR;
        return false;
    }
    if((aes128.flag.isencryption == 0) && (type == 3)&& (aes128.flag.aes_enable == 1) )//支持加密，没加密，恢复米密钥
    {
        gerr_info = (GERR_T)AES_PERMISSIONS_ERR;
        return false;
    }
    return true;
}

void check_26info_key_err_fun(uint8_t type)
{
    if((aes128.flag.aes_enable ==1) &&(aes128.flag.isencryption==1))//若果是加密价签，且是加密状态
    {
        if( (type == 2) || (type == 4) )
            gerr_info = (GERR_T)AES_MAIN_KEY_ERR;//主密钥的密钥错误
        else
            gerr_info = (GERR_T)AES_KEY_ERR;//通讯时使用的密钥错误
    }
}
//aes 密钥错误标志在射频通讯四秒后清除
void aes_ack_clean_fun(void)
{
    if((GERR_T)gerr_info > AES_ACK_ERR_RF_TIMEOUT_CLEAN_FLAG)
        gerr_info = NONEERR;
}

bool save_sys_key_info_fun(uint8_t tp[],uint8_t len)
{
    uint8_t ret, i = 0;
    for(i=0;i<3; i++)
    {
        ret = block_data_check(ROM_FLASH_BLOCK0_ADDR, ROM_FLASH_BLOCK5_ADDR - ROM_FLASH_BLOCK0_ADDR, tp, len);
        if(ret == TRUE)
            break;
    }
    return ret;
}

bool load_main_communication_key(void)
{
    uint8_t tp[35+2];//aes128.flag+主密钥索引+主密钥+通讯密钥索引+通讯密钥+crc
    uint8_t i = 0;
    uint16_t crc = 0;

    for(i=0;i<3; i++)
    {
        crc = 0;
        memset(tp,0x00,37);
        rom_segment_read(ROM_FLASH_BLOCK5_ADDR, (WORD)tp, 37);
        crc = my_cal_crc16(crc,tp,35);
        if(0 == memcmp((uint8_t *)&crc, tp+35, 2))
        {
            memcpy( (uint8_t *)&aes128.flag,tp,1);
            memcpy( (uint8_t *)&aes128.aes128_key_info[1],tp+1,34);
            memcpy( tp+1,(uint8_t *)&aes128.aes128_key_info[2],17);
            return true;
        }
    }
    return false;
}
bool save_main_communication_key(void)
{
    uint8_t tp[35+2];//aes128.flag+主密钥+通讯密钥+crc
    uint16_t crc = 0;

    memcpy( tp,(uint8_t *)&aes128.flag,1);
    memcpy( tp+1,(uint8_t *)&aes128.aes128_key_info[1],34);
    //添加保存函数
    crc = my_cal_crc16(crc,tp,35);
    memcpy( tp+35,(uint8_t *)&crc,2);
    return (save_sys_key_info_fun(tp,37));

}

void key_load_fun(void)
{
    uint8_t temp[16];
    memcpy(temp,(void *)&INFO_DATA.gRFInitData.esl_id,4);
    memcpy(temp+4,(void *)&INFO_DATA.gRFInitData.esl_id,4);
    memcpy(temp+8,(void *)&INFO_DATA.gRFInitData.esl_id,4);
    memcpy(temp+12,(void *)&INFO_DATA.gRFInitData.esl_id,4);

    memset((uint8_t *)&aes128,0x00,sizeof(aes128));
    //初始密钥
    aes128.aes128_key_info[0].keyLocation = CRYPTOCC26XX_KEY_1;
    memcpy( aes128.aes128_key_info[0].key,temp,16);

    uint32_t tp = 0x55555555;
    memcpy(temp+4,(uint8_t *)&tp,4);
    memcpy(temp+8,(uint8_t *)&tp,4);

    //恢复密钥
    aes128.aes128_key_info[3].keyLocation = CRYPTOCC26XX_KEY_4;
    memcpy( aes128.aes128_key_info[3].key,temp,16);


    if(load_main_communication_key() == false)
    {
        aes128.flag.isencryption = 0;
        memset((uint8_t *)&aes128.aes128_key_info[1],0xff,34);
    }


    aes128.flag.aes_enable =1;
}

bool aes128_encryption_fun(AESECB_KEY *usdkey,uint8_t *srcbuff,uint8_t len,bool flag)
{
    CryptoCC26XX_Handle             handle;
    int32_t                         keyIndex;

    int32_t                         status;
    CryptoCC26XX_AESECB_Transaction trans;
    uint8_t mydstbuff[AES_ECB_LENGTH];

    memset(mydstbuff,0x00,AES_ECB_LENGTH);
    if(flag == false)
    {
        return true;
    }

    // Initialize Crypto driver
    CryptoCC26XX_init();

    // Attempt to open CryptoCC26XX.
    handle = CryptoCC26XX_open(Board_CRYPTO, false, NULL);
    if (!handle)
        return false;

    keyIndex = CryptoCC26XX_allocateKey(handle, usdkey->keyLocation,(const uint32_t *) usdkey->key);

    if (keyIndex == CRYPTOCC26XX_STATUS_ERROR)
        return false;

    // Initialize transaction
    CryptoCC26XX_Transac_init((CryptoCC26XX_Transaction *) &trans, CRYPTOCC26XX_OP_AES_ECB_ENCRYPT);
    // Setup transaction
    trans.keyIndex         = keyIndex;
    trans.msgIn            = (uint32_t *) srcbuff;
    trans.msgOut           = (uint32_t *) mydstbuff;

    // Encrypt the plaintext with AES ECB
    status = CryptoCC26XX_transact(handle, (CryptoCC26XX_Transaction *) &trans);
    if(status != CRYPTOCC26XX_STATUS_SUCCESS)
        return false;

    CryptoCC26XX_releaseKey(handle, &keyIndex);
    CryptoCC26XX_close(handle);
    memcpy(srcbuff,mydstbuff,len);
    return true;
}

bool aes128_decrypt_fun(AESECB_KEY *usdkey,uint8_t *srcbuff,uint8_t len,bool flag)
{

    CryptoCC26XX_Handle             handle;
    int32_t                         keyIndex;

    int32_t                         status;
    CryptoCC26XX_AESECB_Transaction trans;

    uint8_t mydstbuff[AES_ECB_LENGTH];

    memset(mydstbuff,0x00,AES_ECB_LENGTH);
    if(flag == false)
    {
        return true;
    }

    // Initialize Crypto driver
    CryptoCC26XX_init();

    // Attempt to open CryptoCC26XX.
    handle = CryptoCC26XX_open(Board_CRYPTO, false, NULL);
    if (!handle)
        return false;

    keyIndex = CryptoCC26XX_allocateKey(handle, usdkey->keyLocation,(const uint32_t *) usdkey->key);

    if (keyIndex == CRYPTOCC26XX_STATUS_ERROR)
        return false;

    // Initialize transaction
    CryptoCC26XX_Transac_init((CryptoCC26XX_Transaction *) &trans, CRYPTOCC26XX_OP_AES_ECB_DECRYPT);
    // Setup transaction
    trans.keyIndex         = keyIndex;
    trans.msgIn            = (uint32_t *) srcbuff;
    trans.msgOut           = (uint32_t *) mydstbuff;

    // Decrypt the plaintext with AES ECB
    status = CryptoCC26XX_transact(handle, (CryptoCC26XX_Transaction *) &trans);
    if(status != CRYPTOCC26XX_STATUS_SUCCESS)
        return false;

    CryptoCC26XX_releaseKey(handle, &keyIndex);
    CryptoCC26XX_close(handle);
    memcpy(srcbuff,mydstbuff,len);
    return true;

}


/* 解析-全局命令加密和修改信道的全局命令加密
 * 主密钥解密
 * 数据大小5个字节，为全局命令的最后最后5个byte
 */
bool glbcmd_encryption_fun(uint8_t *srcbuff,bool flag,uint8_t isencryption)
{
    uint8_t temp[16];
    RFID tp;
    uint16_t crc =0;
    if((flag == false) || (isencryption == 0))//不支持加密,或者未加密
        return true;

    //加密，主密钥解密
    memset(temp,0x00,16);

    memcpy(temp,srcbuff,4);
    memcpy(temp+4,srcbuff,4);
    memcpy(temp+8,srcbuff,4);
    memcpy(temp+12,srcbuff,4);

    if(aes128_encryption_fun(&aes128.aes128_key_info[1],temp,16,isencryption)== false)
        return false;
    crc = my_cal_crc16(crc,temp,16);


    tp =INFO_DATA.gRFInitData.wakeup_id;
    tp.id2 = 0x00;
    crc= my_cal_crc16(crc,(UINT8 *)&tp,sizeof(RFID));

    if(memcmp((uint8_t *)&crc,srcbuff + 4,2) != 0 )
        return false;
    //解密成功
    return true;
}

/* 解析-普通数据包的ase加密
 * 通讯密钥解密
 * 数据大小16个字节，为26个字节中的低9~24byte,不加密crc的目的是保证解密后crc不对时可以知道是密钥不对
 * 特别注意：传入的参数必须是16字节，大于会把后面的清空
 */
bool normaldata_decrypt_fun(uint8_t *srcbuff,bool flag,uint8_t type,uint8_t isencryption)
{
#define NOR_TEMP_KEY_INFO_LEN  (NORMALDATA_DECRYPT_LEN + 1)
    uint8_t key_temp[NOR_TEMP_KEY_INFO_LEN];
    uint8_t buff[NORMALDATA_DECRYPT_LEN];


    if(flag == false)//不支持加密
        return true;

    if( (isencryption == 0) && ( type == 0 ))//不加密，且是正常通讯
        return true;

    //加密，或者未加密且不是正常通讯
    isencryption = 1;
    memcpy(buff,srcbuff,16);

    //    if(type > 4)//非法操作
    //    {
    //        //设置全局错误，表示非法操作
    //        gerr_info = (GERR_T)AES_INDEX_NUM_ERR;
    //        return false;
    //    }
    //合法操作

    //支持加密，已经加密，
    switch(type)//type为26字节中flag的高4bit
    {
    case 0://正常通讯，使用通讯密钥
        memcpy( key_temp,&aes128.aes128_key_info[2],NOR_TEMP_KEY_INFO_LEN);//index+key
        break;
    case 2://修改通讯密钥，使用主密钥
    case 4://主密钥恢复密钥
        memcpy( key_temp,&aes128.aes128_key_info[1],NOR_TEMP_KEY_INFO_LEN);
        break;
    case 3://恢复密钥出厂密钥，使用恢复密钥
        memcpy( key_temp,&aes128.aes128_key_info[3],NOR_TEMP_KEY_INFO_LEN);
        break;
    default://初始化密钥
        memcpy( key_temp,&aes128.aes128_key_info[0],NOR_TEMP_KEY_INFO_LEN);
        break;
    }
    if(aes128_decrypt_fun((AESECB_KEY *)key_temp,buff,NORMALDATA_DECRYPT_LEN,isencryption) ==true)
    {
        memcpy(srcbuff,buff,16);
        return true;
    }
    return false;

}

bool romupdataldata_decrypt_fun(uint8_t *srcbuff,bool flag,uint8_t isencryption)
{
#define NOR_TEMP_KEY_INFO_LEN  (NORMALDATA_DECRYPT_LEN + 1)
    uint8_t key_temp[NOR_TEMP_KEY_INFO_LEN];
    uint8_t buff[NORMALDATA_DECRYPT_LEN];


    if((flag == false) || (isencryption == 0))//不支持加密或者不加密
        return true;


    memcpy(buff,srcbuff,16);
    memcpy( key_temp,&aes128.aes128_key_info[1],NOR_TEMP_KEY_INFO_LEN);

    if(aes128_decrypt_fun((AESECB_KEY *)key_temp,buff,NORMALDATA_DECRYPT_LEN,isencryption) ==true)
    {
        memcpy(srcbuff,buff,16);
        return true;
    }
    return false;

}
