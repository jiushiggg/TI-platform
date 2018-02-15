#ifndef __SCRN_PUBLIC_FUN__
#define __SCRN_PUBLIC_FUN__
#include "epd_data_struct.h"


#define SCREEN_PROWR_COLES_DISABLE

extern PIN_Handle epdPinHandle;
extern volatile LATTICE_SCREEN_ATTR_T  screen_t;
extern volatile SCREEN_DIS_T screen_dis_t;
extern const UINT8 screen_1675A_tp[][20];

#define HOLITECH_IC1675A        0x00000103
#define DKE_IC1675A             0x00000203
#define DKE_IC_1673_BW          0x00000202
#define HOLITECH_IC_1673_BW     0x00000102

UINT8 EPD_W21_ReadDATA(void);
UINT8 EPD_W21_ReadDATA_2(void);
void EPD_W21_WriteCMD(UINT8 command);
void EPD_W21_WriteDATA(UINT8 command);
void sys_rst_close_epd_fun(void);
void screen_analysis_fun(UINT32 screen_id);
void screen_dispaly_fun(volatile SCREEN_DIS_T *tp2);
void epd_power_deep_sleep_fun(void);
void screenic_deep_sleep_fun(void);
#endif
