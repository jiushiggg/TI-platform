#ifndef __SRCN_IC1673_H__
#define __SRCN_IC1673_H__

#include "lattice_epd\epd_data_struct.h"

//void ic1673_deep_sleep_fun(void);
void display_lcd_IC1673(volatile SCREEN_DIS_T *tp,UINT8 temp);
void ic1673_swrest_fun(void);
void init_ic1673_fun(void);
BOOL ic1673_open_display_fun(SCREEN_COLOR_T color);

#endif


