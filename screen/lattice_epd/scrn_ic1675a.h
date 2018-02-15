#ifndef __SCRN_IC1675A_H__
#define __SCRN_IC1675A_H__

#include "lattice_epd\epd_data_struct.h"




/*-------------------º¯ÊýÉùÃ÷-----------------------*/

void ic1675a_swrest_fun(void);
void init_ic1675a_fun(SCREEN_SIZE_LT size);
//void ic1675a_deep_sleep_fun(void);
void display_lcd_ic1675A(volatile SCREEN_DIS_T *tp,UINT8 temp);
void display_lcd_red_ic1675a(volatile SCREEN_DIS_T *tp,UINT8 temp);
BOOL ic1675a_open_display_fun(SCREEN_COLOR_T color);
void ic1675a_check_busy_fun(void);
#endif
