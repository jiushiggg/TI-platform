#ifndef  SYS_INIT_H
#define  SYS_INIT_H

#include <stdint.h>
#include  "three_protocol.h"


#define  NO_EVENT                               (uint32_t)(1 << 0)
#define  EVENT_FLAG_INTER_REED                  (uint32_t)(1 << 1)
#define  EVENT_FLAG_SYS_HEART                   (uint32_t)(1 << 2)
#define  EVENT_FLAG_RFWORK                      (uint32_t)(1 << 3)
#define  EVENT_FLAG_WRITEID                     (uint32_t)(1 << 4)
#define  EVENT_FLAG_ERASER_BUFF                 (uint32_t)(1 << 5)
#define  EVENT_FLAG_OSD_ANALUSIS                (uint32_t)(1 << 6)
#define  EVENT_FALG_DISPLAY_PAGE                (uint32_t)(1 << 7)
#define  EVENT_FLAG_UPDATA_CHECK                (uint32_t)(1 << 8)
#define  EVENT_FLAG_FLASHUPDATE                 (uint32_t)(1 << 9)
#define  EVENT_FLAG_NETLINK                     (uint32_t)(1 << 10)
#define  EVENT_FLAG_NFC                         (uint32_t)(1 << 11)
#define  EVENT_FLAG_QUEST_HEART                 (uint32_t)(1 << 12)
#define  EVENT_FLAG_UPDATA_ROM                  (uint32_t)(1 << 13)
#define  EVENT_FLAG_TIMER_CALIBRATION           (uint32_t)(1 << 14)
#define  EVENT_FLAG_EPD_DISPLAY                 (uint32_t)(1 << 15)
#define  EVENT_ALL                              0xFFFFFFFF


/*---------------系统自定义类型----------------*/


#pragma pack(1)

struct INFO_DATA_T{
  uint16_t      idcrc;
  RFINIT        gRFInitData;            //gRFInitData   18B   RF 工作数据
};

typedef enum{
    SCREEN_COLOUR_NONE =0,
    SCREEN_COLOUR_BLACK,
    SCREEN_COLOUR_RED,
    SCREEN_COLOUR_YELLOW,
}SCREEN_COLOUR_T;
#pragma pack()


void adc_voltage(void);
void sys_load_page_display_fun(void);


#endif
