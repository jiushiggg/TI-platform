#ifndef      __FLASH_PROTECH_H__
#define      __FLASH_PROTECH_H__

#include "../internal_flash/internal_flash.h"
#include "ti\devices\cc26x0r2\driverlib\flash.h"

extern const uint8_t flash_write_block_arry[];
#define MY_FLASH_SECT_BLOCK_SIZE  ((uint32_t)4096)

void flash_wirte_protect_fun(void);



#endif
