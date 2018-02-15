#ifndef __dddd__
#define __dddd__
#include <stdint.h>

#include "typedef.h"




#pragma pack(1)
typedef struct  {
    UINT32 color:4;
    UINT32 iszip:1;
    UINT32 src_dis_directio:2;
    UINT32 reserve1:1;
    UINT32 reserve2:8;

    UINT32 operation_type:4;
    UINT32 reserve3:4;
    UINT32 reserve4:8;
}OSD_PROPERTY_T;

typedef struct
{
    UINT8  cmd;
    UINT32 len;
    UINT8  pageid;
    UINT8  page_map;
    OSD_PROPERTY_T property;
    UINT16 page_crc;
    UINT32 uzip_len;
    UINT16 unzip_data_crc;

}OSD40_LAYER_T;

typedef struct page_crc_t
{
    UINT8 default_page_id;
    UINT8 crc_page0[2];
    UINT8 crc_page1[2];
    UINT8 crc_page2[2];
    UINT8 crc_page3[2];
    UINT8 crc_page4[2];
    UINT8 crc_page5[2];
    UINT8 crc_page6[2];
    UINT8 crc_page7[2];
}page_crc_t;

typedef struct osd_cmd_crc_t
{
    UINT8 crc_cmd[2];
}osd_cmd_crc_t;

#pragma pack()

extern page_crc_t osd4_g_crc;
extern osd_cmd_crc_t  g_cmd_crc;



void osd_init(void);
void sid_change_eraflag(void);
void upgread_id_change_eraflag(void);
void event_128_fun(void);
RF_CMD_T get_osd_76cmd_fun(UINT8 *buf);


#endif

