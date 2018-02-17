/*
 * NFC_Protocl.c
 *
 *  Created on: 2018年1月15日
 *      Author: ggg
 */
#include "NFC_Protocl.h"
#include "fm11nc08.h"
#include "../bsp/bsp_spi.h"
#include "../nfc/nfc.h"
#include <ti/devices/cc26x0r2/driverlib/gpio.h>
#include <stddef.h>
#include "../nfc/uplink.h"

typedef uint16_t (*nfcFnx)(stNFCobj *nfc, stESLRecBuf *rbuf);

#define FIFO_LEN    32

//ISO 14443-4
#define PCB_IBLOCK0                              0X02
#define PCB_IBLOCK1                              0X03
#define PCB_IBLOCK0_CHAINING      0X12
#define PCB_IBLOCK1_CHAINING      0X13
#define PCB_RBLOCK0                             0XB2
#define PCB_RBLOCK1                             0XB3
#define PCB_SBLOCK_DSELECT          0XC2
#define PCB_SBLOCK_WTX                     0XF2

//Support NFCForum-TS-Type-4-Tag_2.0
#define PCB     0
#define CLA  1
#define INS  2
#define P1   3
#define P2   4
#define LC   5
#define DATA 6

#define CLA_STANDARD            0X00
#define INS_DETECTION           0xA4
#define INS_READ                0xB0
#define INS_WRITE               0xD6
#define STANDARD_READ           0X00B0
#define STANDARD_WRITE          0X00D6
#define STANDARD_SELECT         0X00A4

//Private
#define PCB_OFFSET                              1
#define PCB_LEN                                      1
#define SESSION_OFFSET                    1
#define SESSION_LEN                            1
#define SUB_SESSION_LEN                 2
#define NFC_HEAD_LEN                  (offsetof(stESLReceive, reqType) - PCB_OFFSET)// (SESSION_LEN + SUB_SESSION_LEN + 1)
#define CRC_LEN                                     2
#define CRC_LOW                 (PCB_LEN + NFC_HEAD_LEN + rbuf->dataRec.len)
#define CRC_HIGH                (PCB_LEN + NFC_HEAD_LEN + rbuf->dataRec.len+1)
#define DEC_LEN                                     2

#define NFC_EVENT_PRIVATE_ALL       (NFC_EVENT_LIGHT_LED|NFC_EVENT_BLINDING|NFC_EVENT_UPDATA|NFC_EVENT_WRITE_DATA|NFC_EVENT_READ_ID)
#define NFC_EVENT_STANDARD_ALL      (NFC_EVENT_STANDARD_READ|NFC_EVENT_STANDARD_WRITE)

#define CNT_IDLE 5

#define LED_ON(n)    GPIO_clearDio(n)
#define LED_OFF(n)   GPIO_setDio(n)

void NFC_ProtoclFnx(stNFCobj *nfc);
static void commonFnx(stNFCobj *nfc, stESLRecBuf *rbuf);
static uint16_t selectProtoclHandleFnx(stNFCobj *nfc, stESLRecBuf *rbuf);
static uint16_t privateHandleFnx(stNFCobj *nfc, stESLRecBuf *rbuf);
static uint16_t standardHandleFnx(stNFCobj *nfc, stESLRecBuf *rbuf);
static void NFC_state(stNFCobj *nfc, stESLRecBuf *rbuf);
static uint16_t privateRxDataHandle(stNFCobj *nfc, stESLRecBuf *rbuf);
static void NFC_IdleHandle(stNFCobj *nfc);
void testState(void);

uint8_t const responseErr[2] = {0x6A, 0x82};
uint8_t const responseOk[2] = {0x90, 0x00};
uint8_t const responsePageErr[2] = {0x6a, 0x83};
//const stRFInfo rfInfo={0x52, 0x56,0x78,0x53, 0x51,0x55,0x56,0x66, 0x00,0x00,0x00,0x00, 0x50,0x51,0x52,0x99, 160,160,160, 1};
/* Data structure of the capability container file */
#pragma pack(1)
typedef struct _CC_file{
    uint16_t cclen;
    uint8_t mapping_version;
    uint16_t mle;
    uint16_t mlc;
    uint8_t typeTLV;
    uint8_t length;
    uint16_t file_identifier;
    uint16_t max_NDEF;
    uint8_t read_security;
    uint8_t write_security;
}CC_file;
#pragma pack()

//与nfc通信是大端模式
const CC_file type_4_tag = {0x0F00, 0x20, 0x3B00, 0x4B00, 0x04, 0x06,0x04E1,0x3200,0,0};

stNFCobj NFCobj;
stESLRecBuf fm11nc08_buf;

static emAppFile nfcProtoclFlg = NFC_NDEF_FILE;
//static emAppFile nfcProtoclFlg = NFC_PRIVATE_PROTOCL;

//typedef  uint16_t (*nfcFnx)(stNFCobj *nfc, stESLRecBuf *rbuf);
//uint16_t rx_dataHandle(stNFCobj *nfc, stESLRecBuf *rbuf);
nfcFnx  nfcFxnTable[1];

uint8_t ndefTest[100]={0x00, 0x03, 0xD0,0,0};

static void NFC_IdleHandle(stNFCobj *nfc)
{
    uint8_t ret = 0;
    //uint8_t  B=0 ;
    nfc->curEvent = NFC_EVENT_NONE;
    nfc->error = NFC_ERR_NONE;
    bsp_spi_tm = BSP_SPI_NORMAL;
    set_nfc_timer(NFC_TIMER_ONCE, NFC_TIMEOUT_500MS);

    while(NFC_ERR_NONE  == nfc->error){
        ret = FM11_Serial_ReadReg(MAIN_IRQ);
        //B = FM11_Serial_ReadReg(MAIN_IRQ_MASK);
        if(ret & MAIN_IRQ_RX_START)  {
            nfc->curEvent = NFC_EVENT_ACTIVE;
            break;
        }
        if(ret & MAIN_IRQ_ACTIVE){
            nfc->curEvent = NFC_EVENT_ACTIVE;
            break;
        }
    }
    stop_nfc_timer();
    if (NFC_ERR_NONE  != nfc->error){
        nfc->curEvent = NFC_EVENT_TIMEOUT;
    }
}

static uint16_t selectProtoclHandleFnx(stNFCobj *nfc, stESLRecBuf *rbuf)
{
    uint16_t len;
    uint16_t offset;
    uint8_t *p = (uint8_t*)responseErr;
    if (nfcProtoclFlg==NFC_PRIVATE_PROTOCL){
        if (rbuf->cmdAPDU.ins==INS_DETECTION && CLA_STANDARD==rbuf->cmdAPDU.cla){
            len = 0;        //回错误，走私有协议
        }else if (EM_READ_ESL_ID == rbuf->dataRec.reqType){
            memcpy(&rbuf->dataRec.data, (uint8_t*)&INFO_DATA.gRFInitData, sizeof(stRFInfo));
            rbuf->dataRec.len = 21;
            len = 27;
            p = (uint8_t*)responseOk;
            nfc->nextEvent = NFC_EVENT_PRIVATE_PROTOCL;
        }else{
            len = 0;
        }
    }else {     //走标准协议
        if (INS_DETECTION==rbuf->cmdAPDU.ins && CLA_STANDARD==rbuf->cmdAPDU.cla){
            p = (uint8_t*)responseOk;        //step 1,2,4
            len = 0;
        }else if(INS_READ==rbuf->cmdAPDURead.ins && CLA_STANDARD==rbuf->cmdAPDURead.cla){ //step 3,5
            offset = ((uint16_t)rbuf->cmdAPDURead.p1<<8)|rbuf->cmdAPDURead.p2;
            len = rbuf->cmdAPDURead.le;
            if (2==len){            //read NDEF
                nfc->nextEvent = NFC_EVENT_STANDARD_PROTOCL;
                memcpy(&rbuf->buf[1], ndefTest+offset, len);
            }else{                  //read CC File
                memcpy(&rbuf->buf[1], (uint8_t*)&type_4_tag+offset, len);
            }
            p = (uint8_t*)responseOk;
        }else{
            len = 0;
        }
    }
    memcpy(&rbuf->buf[1]+len, p, sizeof(responseOk));
    len = sizeof(rbuf->dataRec.pcb) + len + sizeof(responseOk);
    return len;
}

static uint16_t standardHandleFnx(stNFCobj *nfc, stESLRecBuf *rbuf)
{
    uint8_t len;
    uint16_t offset;
    uint8_t *p=(uint8_t*)responseErr;
    uint16_t tmp = ((uint16_t)rbuf->cmdAPDURead.cla<<8)|rbuf->cmdAPDU.ins;
    offset = ((uint16_t)rbuf->cmdAPDURead.p1<<8)|rbuf->cmdAPDURead.p2;

    switch(tmp){
        case STANDARD_READ:
            len = rbuf->cmdAPDURead.le;
            memcpy(&rbuf->buf[1], ndefTest+offset, len);
            p=(uint8_t*)responseOk;
            break;
        case STANDARD_WRITE:
            len = rbuf->cmdAPDU.lc;
            memcpy(ndefTest+offset, &rbuf->buf[sizeof(stCmdAPDU)], len);
            len = 0;
            p=(uint8_t*)responseOk;
            break;
        case STANDARD_SELECT:
            nfc->nextEvent = NFC_EVENT_DETECT_CARD;
            p=(uint8_t*)responseOk;
            //no break
        default:
            len = 0;
            break;
    }

    memcpy(&rbuf->buf[1]+len, p, sizeof(responseOk));
    len = sizeof(rbuf->dataRec.pcb) + len + sizeof(responseOk);
    return len;
}

static uint16_t privateRxDataHandle(stNFCobj *nfc, stESLRecBuf *rbuf)
{
    int8_t  len = 0;
    switch(rbuf->dataRec.reqType){
        case EM_LIGHT_CFG_CMD:
            nfc->nextEvent = NFC_EVENT_LIGHT_LED;
            if (rbuf->cmdAPDU.ins==INS_DETECTION && CLA_STANDARD==rbuf->cmdAPDU.cla){
                memcpy(&rbuf->dataRec.session, responsePageErr, sizeof(responsePageErr));
                len = sizeof(rbuf->dataRec.pcb) + sizeof(responseErr);
                break;
            }
            memcpy(&rbuf->dataRec.session, responseOk, sizeof(responseOk));
            len = sizeof(rbuf->dataRec.pcb) + sizeof(responseErr);
            get_rc_pkg_fun((uint8_t*)&rbuf->buf[2]);       //NFC包的第3个字节是遥控器包的第1个字节
            break;
        case EM_BINDING_STR_FORMAT:
        case EM_BINDING_INT_FORMAT:
            memcpy(&rbuf->dataRec.session, responseOk, sizeof(responseOk));
            len = sizeof(rbuf->dataRec.pcb) + sizeof(responseErr);

            creat_uplink_request_pkg(rbuf->dataRec.reqType, rbuf->dataRec.data.cnotentData);
            uplink_request_and_rx_ack();

            nfc->nextEvent = NFC_EVENT_BLINDING;
            break;
        case EM_UPDATE_STR_FORMAT:
        case EM_UPDATE_INT_FORMAT:
        case EM_NFC_UPLINK_DATA:
            memcpy(&rbuf->dataRec.session, responseOk, sizeof(responseOk));
            len = sizeof(rbuf->dataRec.pcb) + sizeof(responseErr);

            creat_uplink_request_pkg(rbuf->dataRec.reqType, rbuf->dataRec.data.cnotentData);
            uplink_request_and_rx_ack();

           nfc->nextEvent = NFC_EVENT_UPDATA;
           break;
        case EM_READ_ESL_ID:
            memcpy(&rbuf->dataRec.data, (uint8_t*)&INFO_DATA.gRFInitData, sizeof(stRFInfo));
            rbuf->buf[28] = 0x90;
            rbuf->buf[29] = 0x00;
            rbuf->dataRec.len = 21;
            len = 30;
//            nfc->nextEvent = NFC_EVENT_NONE;
            break;
        case EM_READ_INFO:
        case EM_WRITE_DATA:
        case EM_NFC_DEV_SEND_DATA:
            break;
        default:
            memcpy(&rbuf->dataRec.session, responseErr, sizeof(responseErr));
            len = sizeof(rbuf->dataRec.pcb) + sizeof(responseErr);
            break;
    }
    return len;
}

static uint16_t dataErrHandle( stESLRecBuf *rbuf)
{
    int16_t  len = 0;
    uint16_t tmp = 0;
    switch(rbuf->dataRec.reqType){
        case EM_LIGHT_CFG_CMD:
            rbuf->dataSend.len = 3;
            rbuf->dataSend.ackValue = NFC_ACK_SOFTCRC_ERR;
            tmp = ack_crc_fun(rbuf->buf, (offsetof(stESLSend, data)-PCB_LEN));
            rbuf->dataSend.data.ackData[0] = tmp;
            rbuf->dataSend.data.ackData[1] = tmp>>8;
            rbuf->dataSend.data.ackData[2] = 0x90;
            rbuf->dataSend.data.ackData[3] = 0x0;
            len = offsetof(stESLSend, data) + 4+DEC_LEN;
            break;
        case EM_BINDING_STR_FORMAT:
        case EM_BINDING_INT_FORMAT:
        case EM_UPDATE_STR_FORMAT:
        case EM_UPDATE_INT_FORMAT:
        case EM_READ_ESL_ID:
        case EM_READ_INFO:
        case EM_WRITE_DATA:
        case EM_NFC_DEV_SEND_DATA:
        default:
            memcpy(&rbuf->dataRec.session, responseErr, sizeof(responseErr));
            len = sizeof(rbuf->dataRec.pcb) + sizeof(responseErr);
            break;
    }
    return len;
}

static uint16_t privateHandleFnx(stNFCobj *nfc, stESLRecBuf *rbuf)
{
    uint16_t tmp;
    uint16_t len;
    if (rbuf->cmdAPDU.ins==INS_DETECTION && CLA_STANDARD==rbuf->cmdAPDU.cla){
        memcpy(&rbuf->dataRec.session, responseErr, sizeof(responseErr));        //回错误，走私有协议
        len = sizeof(rbuf->dataRec.pcb) + sizeof(responseErr);
        nfc->nextEvent = NFC_EVENT_DETECT_CARD;
    }else{
        tmp = ack_crc_fun(rbuf->buf+PCB_OFFSET, rbuf->dataRec.len + NFC_HEAD_LEN + CRC_LEN);//加CRC_LEN是因为ack_crc_fun里已经减去CRC长度
        if (tmp == ((uint16_t)rbuf->buf [CRC_HIGH]<<8) | rbuf->buf [CRC_LOW]){
            len = privateRxDataHandle(nfc, rbuf);
        }else{
            len = dataErrHandle(rbuf);
        }
    }
    return len;
}

void NFC_ProtoclFnx(stNFCobj *nfc)
{

    BSP_SPI_Open(BSP_FM11_SPI_BIT_RATE,BSP_FM11_SPI_CLK);

    do {
        switch(nfc->curState)
        {
        case NFC_IDLE_STATE:
            nfc->nextState = NFC_ACTIVE_STATE;
            nfc->error = NFC_ERR_NONE;
            nfc->curEvent = NFC_EVENT_NONE;
            break;
        case NFC_ACTIVE_STATE:
            NFC_IdleHandle(nfc);
            if (NFC_EVENT_ACTIVE == nfc->curEvent){
                nfc->nextState = NFC_DETECT_PROTOCL_STATE;
            }else{
                nfc->nextState = NFC_ERR_STATE;
            }
            break;
        case NFC_DETECT_PROTOCL_STATE:
            nfcFxnTable[0] = selectProtoclHandleFnx;
            commonFnx(nfc, &fm11nc08_buf);
            if (nfc->curEvent == NFC_EVENT_PRIVATE_PROTOCL) {
                nfc->nextState = NFC_PRIVATE_PROTOCL_HANDLE;
            } else if(nfc->curEvent == NFC_EVENT_STANDARD_PROTOCL){
                nfc->nextState = NFC_STANDARD_PROTOCL_HANDLE;
            }else {
                nfc->nextState = NFC_ERR_STATE;
            }
            break;
        case NFC_STANDARD_PROTOCL_HANDLE:
            nfcFxnTable[0] = standardHandleFnx;
            commonFnx(nfc, &fm11nc08_buf);
            if (nfc->curEvent & NFC_EVENT_STANDARD_ALL){
                nfc->nextState = NFC_EVENT_FINISH_STATE;
            }else if(nfc->curEvent == NFC_EVENT_DETECT_CARD){
                nfc->nextState = NFC_DETECT_PROTOCL_STATE;
            }else{
                nfc->nextState = NFC_ERR_STATE;
            }
            break;
        case NFC_PRIVATE_PROTOCL_HANDLE:
            nfcFxnTable[0] = privateHandleFnx;
            commonFnx(nfc, &fm11nc08_buf);
            if (nfc->curEvent & NFC_EVENT_PRIVATE_ALL){
                nfc->nextState = NFC_EVENT_FINISH_STATE;
            }else if(nfc->curEvent == NFC_EVENT_DETECT_CARD){
                nfc->nextState = NFC_DETECT_PROTOCL_STATE;
            }else{
                nfc->nextState = NFC_ERR_STATE;
            }
            break;
        case NFC_ERR_STATE:
//            stop_nfc_timer();
//            nfc_int_set(NFC_INT_EN);
            nfc->nextState = NFC_EVENT_FINISH_STATE;
            break;
        case NFC_EVENT_FINISH_STATE:
            set_nfc_timer(NFC_TIMER_PERIOD,  NFC_TIMEOUT_50MS);
            nfc->nextState  = NFC_HALT_STATE;
            break;
        case NFC_HALT_STATE:
            NFC_state(nfc, &fm11nc08_buf);
            if (nfc->curEvent == NFC_EVENT_REMOVE){
                stop_nfc_timer();
                nfc_int_set(NFC_INT_EN);
                nfc->nextState = NFC_IDLE_STATE;
            }else{
                nfc->nextState = NFC_HALT_STATE;
            }
            break;
        default:
//            stop_nfc_timer();
//            nfc_int_set(NFC_INT_EN);
            nfc->nextState = NFC_EVENT_FINISH_STATE;
            break;
        }
        nfc->preState = nfc->curState;
        nfc->curState = nfc->nextState;
    }while(nfc->curState  != NFC_IDLE_STATE  && nfc->curState  != NFC_HALT_STATE);

    BSP_SPI_Close();
}


static void commonFnx(stNFCobj *nfc, stESLRecBuf *rbuf)
{
    uint8_t len = 0;
    nfc->nextEvent = NFC_EVENT_NONE;
    nfc->error = NFC_ERR_NONE;
    bsp_spi_tm = BSP_SPI_NORMAL;

    do{
        set_nfc_timer(NFC_TIMER_ONCE, NFC_TIMEOUT_50MS);
        len = FM11_rx(nfc, rbuf);
        if (nfc->error != NFC_ERR_NONE){
            nfc->nextEvent = NFC_EVENT_TIMEOUT;
            break;
        }

        switch(rbuf->dataRec.pcb){
        case PCB_IBLOCK0:
        case PCB_IBLOCK1:
            len = nfcFxnTable[0](nfc, rbuf);
            break;
        case PCB_IBLOCK0_CHAINING:
        case PCB_IBLOCK1_CHAINING:
            break;
        case PCB_RBLOCK0:
        case PCB_RBLOCK1:
            memcpy(&rbuf->dataRec.session, responseOk, sizeof(responseOk));
            len = sizeof(rbuf->dataRec.pcb) + sizeof(responseOk);
            break;
        case PCB_SBLOCK_DSELECT:
            memcpy(&rbuf->dataRec.session, responseOk, sizeof(responseOk));
            len = sizeof(rbuf->dataRec.pcb) + sizeof(responseOk);
            nfc->nextEvent = NFC_EVENT_DETECT_CARD;
            break;
        case PCB_SBLOCK_WTX:
            break;
        default:
            memcpy(&rbuf->dataRec.session , responseErr, sizeof(responseErr));
            len = sizeof(rbuf->dataRec.pcb) + sizeof(responseErr);
            break;
        }
        FM11_tx(nfc, rbuf, len);
        if (nfc->error != NFC_ERR_NONE){
            nfc->nextEvent = NFC_EVENT_TIMEOUT;
        }
        nfc->preEvent = nfc->curEvent;
    }while(nfc->nextEvent == NFC_EVENT_NONE);
    stop_nfc_timer();
    nfc->curEvent = nfc->nextEvent;
}
static void NFC_state(stNFCobj *nfc, stESLRecBuf *rbuf)
{

    static uint8_t cnt = 0;
    volatile uint8_t tmp = 0;
     tmp = FM11_Serial_ReadReg(RF_STATUS);
     //   buf1[i] = FM11_Serial_ReadReg(MAIN_IRQ);

     if (tmp==NFC_IDLE_STATUS || bsp_spi_tm==BSP_SPI_TIMEOUT){
         cnt++;
     }else{
         cnt = 0;
     }

     if (cnt >CNT_IDLE){
         cnt = 0;
         nfc->curEvent =  NFC_EVENT_REMOVE;
     }else {
         nfc->curEvent =  NFC_EVENT_APPROACH;
     }
}


