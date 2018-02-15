
#ifndef INTERNAL_FLASH_H
#define INTERNAL_FLASH_H

#include <xdc/std.h>
#include <ti/devices/cc26x0r2/driverlib\rom.h>
#include <ti\devices\cc26x0\inc\hw_types.h>
#include "global_variable.h"


/*�������ڲ�flash����������Ϣ */



/*------------------��������---------------*/
#ifdef HAVE_EX_FLASH

#define  EF_STAA_DDR        ((WORD)0x5000)//20k
#define  EF_FS_USED      ((WORD)0x2000)//8k

#ifdef  EXTERN_FALSH_4M
    #define  EF_EADDR        ((WORD)0x80000)
#else
    #define  EF_EADDR        ((WORD)0x40000)//256k == 0x40000  128k =0x20000 //512k=0x80000
#endif

#define  FLASH_ALL_SIZE   (WORD)(EF_EADDR - EF_FS_USED - EF_STAA_DDR)        //8k���ļ�ϵͳ��20k��ϵͳ����
#define  EF_BLOCK_SIZE   ((WORD)0x1000)   //4k

#else

#define  EF_STAA_DDR        ((WORD)0xE000)//56k   �ڲ�flash ��56k��ʼ���ļ�ϵͳ����
#define  EF_FS_USED      ((WORD)0x1000)//4k

#define  EF_EADDR        ((WORD)0x1D000)  //�ļ�ϵͳ�Ľ�����ַ���ļ�ϵͳ������56K
#define  FLASH_ALL_SIZE   (WORD)(EF_EADDR - EF_FS_USED - EF_STAA_DDR)        //4k���ļ�ϵͳ
#define  EF_BLOCK_SIZE   ((WORD)0x1000)   //4k

#endif


void rom_segment_erase(WORD seg_addr);
void rom_segment_write(WORD addr, WORD data, WORD len);
void rom_segment_read(WORD addr,WORD buf, WORD len) ;

#endif

