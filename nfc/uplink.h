
#ifndef _UPLINK_H_
#define _UPLINK_H_

#include "global_variable.h"

#define UPLINK_DATA_LEN                  26
typedef enum {

    LED_STATUS = 0,
    NEW_BINDEING_INT = 0x81,
    NEW_BINDEING_STR = 0x82,
    UPDATAE_STORAGE_INT = 0x83,
    UPDATAE_STORAGE_STR = 0x84,

} UPLINK_REQ_TYPE;

typedef enum {

    UPLINK_ACK_OK =0,
    CTRL_0_ERR,
    CRC_ERR,
    SESSION_ID_ERR,
    ESL_ID_ERR,

}UPLINK_REQ_ACK_ERR_TYPE;

typedef enum{

    UPLINK_OK,
    AP_OFFLINE,

}UPLINK_STATUS_TYPE;

extern UINT8 g_uplink_status;
void creat_uplink_request_pkg(UINT8 request_type, UINT8 *buf);
void uplink_request_and_rx_ack(void);
#endif
