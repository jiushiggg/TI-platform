#include "typedef.h"
#include "mini_fs_conf.h"
#include "osd4_0_cmd.h"


#define CLOLOR_RED_OFFSET     (0x1000)




#pragma pack(1)


typedef enum {
 PAGE_COLOR_NONE,
 CLOLOR_BW = 0,
 CLOLOR_RED,
 CLOLOR_YELLOW,
 CLOLOR_MAX_NUM,
}SCREEN_PAGE_COLOR_T;
//文件功能说明
typedef enum
{
  FEATURES_NONE,
  FEATURES_OCCUPIED_FILE,  //
  FEATURES_DISPALY,        //
  FEATURES_CMD_BUFF,
  FEATURES_MAX_NUM,
}FILE_TYPE_T;

//文件功能说明
typedef struct _dis_page_t
{
  UINT8 page_id;       //页号
  file_id_t file_id;   //页号对应的物理文件编码
  UINT16 page_crc;
  FILE_TYPE_T file_featrue;
  SCREEN_PAGE_COLOR_T color;
  UINT8 *page_info;
  
}G_DIS_PAGE_T;

//缓存命令流文件说明
typedef struct _cmd_page_t
{
  UINT8 page_id;       //页号 大于最大显示页页号的2个文件
  file_id_t file_id;   //页号对应的物理文件编码
  UINT16 page_crc;
  FILE_TYPE_T file_featrue;
  UINT8 *page_info;
  
}G_CMD_PAGE_T;

//所有页描述
typedef struct _all_page_info_t
{
  UINT32 used_page_map;//所有页码的map表说明，1表示占用，0表示未被占用
  UINT32 need_erase_page_map;//需要擦除的页码表 1表示要擦除的页，0表示未被查处的页
  UINT8 file_id_cursor; //文件id的偏移游标
}ALL_PAGE_INFO_T;

#pragma pack()


extern ALL_PAGE_INFO_T gpage_info_st;
extern G_DIS_PAGE_T  gdsi_info_st[MAX_DISPLAY_PGAE_SUM];
extern G_DIS_PAGE_T  gdsi_info_st_red[MAX_DISPLAY_PGAE_SUM];

BOOL read_dispage_id_data(UINT8 pageid);
BOOL write_dispage_file_fun(G_DIS_PAGE_T *page_tp, OSD40_LAYER_T *tp);
BOOL check_page_correctness(UINT8 cmd_pageid ,UINT8 src_pageid);
BOOL integration_disdata_fun(UINT8 *cmd_srcbuff,G_DIS_PAGE_T *page_tp,OSD40_LAYER_T *tp);
UINT8 set_cursor_offset(void);
void storage_struct_init(void);
void set_erase_fileid_map_fun(UINT8 id_map);
void set_used_fileid_map_fun(UINT8 id_map);
void erase_file_fun(void);

