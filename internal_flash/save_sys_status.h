#ifndef SAVE_SYS_STATUS_H
#define SAVE_SYS_STATUS_H

#include <string.h>
#include <stdint.h>
#include "internal_flash.h"
#include "crc16.h"
#include "global_variable.h"
#include "stdbool.h"

bool load_info_sys_parameter(void);
void save_info_sys_parameter(void);
bool load_id(uint32_t addr);
void test_save_id(void);



#endif
