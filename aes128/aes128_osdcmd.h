#ifndef  __AES128_OSDCMD2_H__
#define  __AES128_OSDCMD2_H__

typedef enum
{
    AES_NONEERR = 100,

    AES_SAVE_KEY_ERR,
    AES_HARDWARE_ERR,
    AES_ACK_ERR_RF_TIMEOUT_CLEAN_FLAG = 150,
    AES_KEY_ERR,//通讯是使用密钥错误
    AES_MAIN_KEY_ERR,//主密钥错误
    AES_PERMISSIONS_ERR,


}AES_ACK_T;

int32_t cmd10_fun(file_id_t cmdfd, UINT32 offset );
int32_t cmd11_fun(file_id_t cmdfd, UINT32 offset );
int32_t cmd12_fun(file_id_t cmdfd, UINT32 offset );
int32_t cmd13_fun(file_id_t cmdfd, UINT32 offset );
#endif
