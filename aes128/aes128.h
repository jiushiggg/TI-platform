#ifndef  __AES128_H__
#define  __AES128_H__

#include <ti/drivers/crypto/CryptoCC26XX.h>
#include <string.h>

#define AES128_KEY_TYPE_NUMBER 4
#define NORMALDATA_DECRYPT_LEN 16

#define Board_CRYPTO                CC2640_LAUNCHXL_CRYPTO0

#pragma pack(1)
/*!
 *  @def    CC2650_LAUNCHXL_CryptoName
 *  @brief  Enum of Crypto names on the CC2650 dev board
 */
typedef enum CC2650_LAUNCHXL_CryptoName {
    CC2640_LAUNCHXL_CRYPTO0 = 0,
    CC2640_LAUNCHXL_CRYPTOCOUNT
} CC2640_LAUNCHXL_CryptoName;


typedef struct
{
    CryptoCC26XX_KeyLocation keyLocation;           // Location in Key RAM
    uint8_t key[AES_ECB_LENGTH];                      // Stores the Aes Key
}AESECB_KEY;
typedef struct
{
    uint8_t aes_enable:4;//是否支持加密
    uint8_t isencryption:4;//是否加密
}ENCRYPTIN_T;
typedef struct
{
    ENCRYPTIN_T flag;
    AESECB_KEY aes128_key_info[AES128_KEY_TYPE_NUMBER];
}AESECB_T;

#pragma pack()
extern AESECB_T  aes128;//高四位表示是否加密，第四为表示是否支持加密
void key_load_fun(void);
void aes_ack_clean_fun(void);
bool glbcmd_encryption_fun(uint8_t *srcbuff,bool flag,uint8_t isencryption);
bool aes128_decrypt_fun(AESECB_KEY *usdkey,uint8_t *srcbuff,uint8_t len,bool flag);
bool aes128_encryption_fun(AESECB_KEY *usdkey,uint8_t *srcbuff,uint8_t len,bool flag);
bool normaldata_decrypt_fun(uint8_t *srcbuff,bool flag,uint8_t type,uint8_t isencryption);
bool romupdataldata_decrypt_fun(uint8_t *srcbuff,bool flag,uint8_t isencryption);
bool save_main_communication_key(void);
void check_26info_key_err_fun(uint8_t type);
bool check_initkey_permissions_fun(uint8_t type);
#endif
