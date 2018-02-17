/*
 * NFC_Protocl.h
 *
 *  Created on: 2018年1月15日
 *      Author: ggg
 */

#ifndef NFC_PROTOCL_H_
#define NFC_PROTOCL_H_

#include <stdint.h>
#include "../three_protocol/three_protocol.h"

#define     FIFO_BUF_LEN    250
#define     REQ_CONTENT_LEN  (26)   //32-6
#define     ACK_DATA_LEN    20


//-----------------RECEIVE---------------------
/*
0x00: light status sync;
0x81: a new binding with REQ_Content in int format;
0x82: a new binding with REQ_Content in str format;
0x83: update the storage with REQ_Content in int format;
0x84: update the storage with REQ_Content in str format
 */
#pragma pack(1)

typedef enum _emReqType{
EM_LIGHT_CFG_CMD = 0x00,
EM_BINDING_STR_FORMAT= 0x81,
EM_BINDING_INT_FORMAT= 0x82,
EM_UPDATE_STR_FORMAT =  0x83,
EM_UPDATE_INT_FORMAT =  0x84,
EM_READ_ESL_ID                  = 0x05,
EM_READ_INFO                              = 0x06,
EM_WRITE_DATA                 =0x07,
EM_NFC_DEV_SEND_DATA =0x85,
EM_NFC_UPLINK_DATA =0x86
}emReqType;

typedef struct _stReaderCtrLED{
    uint8_t pcb;
    uint8_t session;
    uint16_t subSession;
    uint8_t len;
    emReqType reqType;
    uint16_t  secur_code;
    uint8_t  t0_cont;
    uint8_t  led_num;
    uint8_t  t0_time;
    uint8_t  t1_time;
    uint16_t t2_time;
    uint16_t period;
    uint16_t display_time;
    uint8_t page_num;
    uint8_t led_map[4];
    uint8_t default_page;
    uint8_t reserved[2];
}stReaderCtrLED;

typedef struct _stReaderRdID{
    uint16_t  secur_code;
    uint8_t ctrl;
}stReaderRdID;

typedef struct _stReaderWrDataCmd{
    uint16_t  secur_code;
    uint8_t ctrl;
}stReaderWrDataCmd;


typedef union _unReqContent{
    uint8_t                    cnotentData[REQ_CONTENT_LEN];
    //stReaderCtrLED     cnotentCtrLED;
    stReaderRdID          cnotentRdID;
    stReaderWrDataCmd    cnotentWrDataCmd;
}unReqContent;

typedef struct _stESLReceive{
    uint8_t pcb;
    uint8_t session;
    uint16_t subSession;
    uint8_t len;
    emReqType reqType;
    unReqContent data;
//    uint16_t crc;
//    uint16_t fm11nc08_crc;
}stESLReceive;

typedef struct _stCmdAPDU{
    uint8_t pcb;
    uint8_t cla;
    uint8_t ins;
    uint8_t p1;
    uint8_t p2;
    uint8_t lc;
}stCmdAPDU;
typedef struct _stCmdAPDURead{
    uint8_t pcb;
    uint8_t cla;
    uint8_t ins;
    uint8_t p1;
    uint8_t p2;
    uint8_t le;
}stCmdAPDURead;

//-------------SEND----------
typedef struct _stRFInfo
{
    RFID master_id;
    RFID wakeup_id;
    RFID extend_esl_id;
    RFID esl_id;
    uint8_t  set_wkup_ch;
    uint8_t  grp_wkup_ch;
    uint8_t  esl_data_ch;
    uint8_t  esl_netmask;
} stRFInfo;

typedef struct _stWrDataAck{
    uint8_t defaultChannel;
}stWrDataAck;


typedef union _unAckData{
    uint8_t                    ackData[20];
    stRFInfo     RFinfo;
    stWrDataAck     wrDataAck;
}unAckData;

typedef enum _emAckValue{
    NFC_ACK_SUCCESS = (uint8_t)1,
    NFC_ACK_SECURITY_ERR,
    NFC_ACK_SESSION_ERR,
    NFC_ACK_SUBSESSION_ERR,
    NFC_ACK_SOFTCRC_ERR
}emAckValue;

typedef struct _stESLSend{
    uint8_t pcb;
    uint8_t session;
    uint16_t subSession;
    uint8_t len;
    emReqType ackType;
    emAckValue ackValue;
    unAckData data;
//    uint16_t crc;
//    uint16_t fm11nc08_crc;
}stESLSend;

typedef union _stESLRecBuf{
    stESLReceive    dataRec;
    uint8_t                 buf[FIFO_BUF_LEN];
    stCmdAPDURead  cmdAPDURead;
    stCmdAPDU         cmdAPDU;
    stReaderCtrLED      cnotentCtrLED;
    stESLSend          dataSend;
}stESLRecBuf;



#pragma pack()

typedef enum _emAppFile{
    NFC_PRIVATE_PROTOCL,
    NFC_NDEF_FILE,
    NFC_FILE_COUNT
}emAppFile;

typedef enum _emNFC_State{
    NFC_IDLE_STATE,
    NFC_HALT_STATE,
    NFC_WAIT_IRQ_STATE,
    NFC_ACTIVE_STATE,
    NFC_DETECT_PROTOCL_STATE,
    NFC_STANDARD_PROTOCL_HANDLE,
    NFC_PRIVATE_PROTOCL_HANDLE,
    NFC_REC_START,
    NFC_REC_STATE,
    NFC_REC_FINISH,
    NFC_SEND_START,
    NFC_SEND_STATE,
    NFC_SEND_FINISH,
    NFC_FWT_START,
    NFC_FWT_STATE,      //frame waiting time
    NFC_FWT_FINISH,
    NFC_EVENT_FINISH_STATE,
    NFC_DATA_HADLE,
    NFC_ERR_STATE,
    NFC_UNKNOW_STATE
}emNFC_State;

typedef enum _emNFC_Event{
    NFC_EVENT_NONE = 0,
    NFC_EVENT_DETECT_CARD = (1<<0),
    NFC_EVENT_STANDARD_PROTOCL = (1<<1),
    NFC_EVENT_PRIVATE_PROTOCL = (1<<2),
    NFC_EVENT_REC_CHAINING = (1<<3),
    NFC_EVENT_REC_FIFO_WL = (1<<4),          //Water Level
    NFC_EVENT_REC_DONE = (1<<5),
    NFC_EVENT_REC_ERR = (1<<6),
    NFC_EVENT_SEND_DONE = (1<<7),
    NFC_EVENT_ACTIVE = (1<<8),
    NFC_EVENT_LIGHT_LED = (1<<9),
    NFC_EVENT_BLINDING = (1<<10),
    NFC_EVENT_UPDATA = (1<<11),
    NFC_EVENT_WRITE_DATA = (1<<12),
    NFC_EVENT_READ_ID = (1<<13),
    NFC_EVENT_REMOVE = (1<<14),
    NFC_EVENT_APPROACH = (1<<15),
    NFC_EVENT_TIMEOUT = (1<<16),
    NFC_EVENT_STANDARD_READ = (1<<17),
    NFC_EVENT_STANDARD_WRITE = (1<<18),
}emNFC_Event;

typedef enum    _emErr{
    NFC_ERR_NONE,
    NFC_ERR_TIMEOUT,
    NFC_ERR_UNKNOW,
    NFC_ERR_CNT
}emErr;

typedef struct _stNFCobj {
    emNFC_Event  preEvent;         //记录上一次事件
    emNFC_Event  curEvent;                  //记录当前事件
    emNFC_Event  nextEvent;

    emNFC_State preState;        //记录上一次State
    emNFC_State curState;             //记录当前State
    emNFC_State nextState;        //记录下一次State

    emErr error;         //记录RF错误标识
} stNFCobj;

extern stNFCobj NFCobj;

extern void NFC_ProtoclFnx(stNFCobj *nfc);



#endif /* NFC_PROTOCL_H_ */
