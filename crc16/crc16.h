#ifndef CRC16_H
#define CRC16_H

#include <stdint.h>
#include <string.h>
#include "global_variable.h"

uint16_t my_cal_crc16(uint16_t crc,uint8_t *ptr,uint16_t len);
bool rf_pgk_crc(uint8_t *ptr,uint16_t len,RFID rfid);
uint16_t ack_crc_fun(uint8_t *ptr,uint16_t len);
#endif
