#ifndef __HOLTEK_MSP430_OSD2__0CMD_H__
#define __HOLTEK_MSP430_OSD2__0CMD_H__

#include "mini_fs.h"
#include <stdint.h>

#define OSD_CMD_SUN     (13)
#define OSD_END_CMD     (0x76)
#define PAGE_LEN        (512)
#define ALL_PAGE_SUM    (0x07)
#define PAGE_TIME_OFF   (0XFFF0)
#define NUMBER_ALL   (12)                       //数字缓冲区长度


#pragma pack(1)

typedef struct write_layer_t
{
  UINT8  cmd;
  UINT32 len;
  UINT16  layerid;
  UINT32 property; 
} write_layer_t;

typedef struct write_layer_zip_t
{
  UINT8  cmd;
  UINT32 len;
  UINT16  layerid;
  UINT32 property;
  UINT32 all_zip_len;
  UINT32 all_unzip_len;
} write_layer_zip_t;

typedef struct block_zip_t
{
  UINT32 block_zip_len;
  UINT32 block_unzip_len;
} block_zip_t;

typedef struct wrt_layer_arrt_t
{
  UINT32 offset;
  UINT32 len;
  file_id_t src_file;
}wrt_layer_arrt_t;

typedef struct store_page_cmd_t
{
  UINT8 cmd;
  UINT16 len;
  UINT8 page_sum;
}store_page_cmd_t;

typedef struct lcd_store_page_cmd_t
{
  UINT8 cmd;
  UINT32 len;
  UINT8 page_sum;
}lcd_store_page_cmd_t;

typedef struct lcd_page_num_t
{
  UINT8  page_id;
  UINT16  page_data_len;
  UINT8  page_data;
}lcd_page_num_t;

typedef struct page_into_t
{
  UINT16 page_len;
  UINT8  page_id;
  UINT16 layer_len;
}page_into_t;

typedef struct dis_page_cmd_t
{
  UINT8  cmd;
  UINT16 len;
  UINT8  default_page_id;
  UINT8  page_id_bit_map;
 
}dis_page_cmd_t;
typedef struct rom_updata_cmd_t
{
    UINT8  cmd;
    UINT32 len;
    UINT16 crc;
    UINT8  rom_version;
    UINT16 screen_id_range;
    UINT32 rom_len;

}rom_updata_cmd_t;

typedef struct alter_num_t
{
  UINT8 cmd;
  UINT16 len;
  UINT8 flag;
}alter_num_t;
typedef struct page_num_sum_t
{
  UINT8  page_id;
  UINT16 page_num_len;
}page_num_sum_t;

typedef struct number_t
{
  UINT16 layer_id;
  UINT32 number;
}number_t;

typedef struct cmd1_t
{
  UINT8 cmd;
  UINT16 len;
  UINT8 default_page_id;
}cmd1_t;

typedef struct font_arrt_t
{
  UINT16 layer_id;
  UINT32 property;
  UINT16 sx;
  UINT16 sy;
  UINT16 ex;
  UINT16 ey;
  UINT16 font_layerid_i;
  UINT16 font_layerid_d; 
  UINT16 font_layerid_s; 
  UINT32 format; 
}font_arrt_t;

typedef struct num_ft
{  
  UINT16 layerid;
  UINT32 number; 
  UINT32 offset;
  UINT32 dstoffset; 
  UINT8  font_h;
  UINT8  font_w;
  UINT8  type;
}num_ft;
typedef struct property_t
{
    UINT8 color;
    UINT8 direction;
    UINT8 font_color;
    UINT8 background_color;
}property_t;
//-----------------------

#pragma pack()


bool process_cmd(file_id_t cmdfd);
bool process_zip_cmd(file_id_t cmdfd);
void sid_change_eraflag(void);
void osd_init(void);
void event_128_fun(void);
void erase_file_fun(void);
RF_CMD_T get_osd_76cmd_fun(UINT8 *buf);

#endif
