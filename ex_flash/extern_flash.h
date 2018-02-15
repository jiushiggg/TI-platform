
#ifndef  EXTERN_FLASH_H
#define  EXTERN_FLASH_H

#include "global_variable.h"
#include "ExtFlash.h"
#include <ti/drivers/NVS.h>
#include <ti/drivers/nvs/NVSSPI25X.h>


///*------------------参数定义---------------*/
//
//#define  EF_STAA_DDR        ((WORD)0x5000)//20k
//#define  EF_FS_USED      ((WORD)0x2000)//8k
//
//#ifdef  EXTERN_FALSH_4M
//    #define  EF_EADDR        ((WORD)0x80000)
//#else
//    #define  EF_EADDR        ((WORD)0x40000)//256k == 0x40000  128k =0x20000 //512k=0x80000
//#endif
//
//#define  FLASH_ALL_SIZE   (WORD)(EF_EADDR - EF_FS_USED - EF_STAA_DDR)        //8k的文件系统，20k的系统保留
//#define  EF_BLOCK_SIZE   ((WORD)0x1000)   //4k


extern NVS_Handle nvsHandle;
extern NVS_Attrs regionAttrs;
extern NVS_Params nvsParams;

void init_nvs_spi_flash(void);
void extern_flash_open(void);
void extern_flash_close(void);
void segment_erase(WORD seg_addr);
void segment_write(WORD addr, WORD data, WORD len);
void segment_read(WORD addr, WORD buf, WORD len);

#endif
