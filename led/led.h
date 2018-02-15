
#ifndef LED_H
#define LED_H

#include "stdbool.h"
#include <stdint.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/drivers/PIN.h>
#include "Board.h"

extern PIN_Handle ledPinHandle;

#define LED_RED_OFF    PIN_setOutputValue(ledPinHandle, Board_LED0, 1)
#define LED_RED_ON     PIN_setOutputValue(ledPinHandle, Board_LED0, 0)

#define LED_GREEN_OFF  PIN_setOutputValue(ledPinHandle, Board_LED1, 1)
#define LED_GREEN_ON   PIN_setOutputValue(ledPinHandle, Board_LED1, 0)

#define LED_BLUE_OFF   PIN_setOutputValue(ledPinHandle, Board_LED2, 1)
#define LED_BLUE_ON    PIN_setOutputValue(ledPinHandle, Board_LED2, 0)


#pragma pack(1)

typedef struct LED_STATE
{
  bool led_off;
  bool timer_enable;
  bool  horselamp;
  uint32_t all_times;
} LED_STATE;

typedef struct EPD_ATTRIBUTE_PKG_ST
{

  uint8_t t0_cont;
  uint8_t secur_code[2];
  uint8_t display_time[4];
  uint8_t  mag_dis_time;
  uint8_t reseved[2];
  uint8_t led_num;
  uint8_t t0_time;
  uint8_t t1_time;
  uint16_t t2_time;
  uint16_t period;
  uint16_t magnet;
  uint16_t global_crc;

}EPD_ATTRIBUTE_PKG_ST;

typedef struct EPD_RC_ST
{
  uint8_t  ctrl;
  uint8_t  pkg_num[2];
  uint8_t  reseved;
  uint8_t  secur_code[2];
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
  uint8_t reserved1[2];
  uint16_t crc;
}EPD_RC_ST;

typedef struct LED_LOOP_ST
{
  uint8_t  t0_cont;
  uint8_t  led_num;
  uint8_t  t0_time;
  uint8_t  t1_time;
  uint16_t t2_time;
  uint16_t period;
  uint8_t led_map[4];
}LED_LOOP_ST;
#pragma pack()

extern Semaphore_Handle  led_off_sem;
extern Semaphore_Handle  led_flash_sem;



void led_all_off(void);
void normal_led_color_define(uint8_t tp);
void horse_led_color_define(uint8_t tp);
bool one_cycle_horse_led(void);
bool one_cycle_normal_led(void);
bool normal_or_horse_led_Fxn(void);
void def_led_attr_info_fun(void);
void def_rc_attr_info_fun(void);
void load_attribute_pkg_fun(void);
void rc_led_init(void);
void recover_led_status(uint32_t tp);

#endif
