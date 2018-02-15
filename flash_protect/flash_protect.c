#include "flash_protect.h"


void flash_wirte_protect_fun(void)
{
 const uint8_t flash_write_block_arry[]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,30,31};

    uint32_t i=0;
    uint32_t addr = 0;

    for(i=0;i < sizeof(flash_write_block_arry);i++)
    {
        addr  =  (flash_write_block_arry[i])* MY_FLASH_SECT_BLOCK_SIZE;
        FlashProtectionSet(addr,FLASH_WRITE_PROTECT);
    }
//    for(i=0;i < sizeof(flash_write_block_arry);i++)
//        rom_segment_erase(flash_write_block_arry[i]*MY_FLASH_SECT_BLOCK_SIZE);
}

//void flash_no_protect_fun(uint8_t *block)
//{
//    uint32_t i=0;
//    uint32_t addr = 0;
//
//    for(i=0;i <strlen((const char *)block);i++)
//    {
//        addr  =   (flash_write_block_arry[i])* MY_FLASH_SECT_BLOCK_SIZE;
//        FlashProtectionSet(addr,FLASH_NO_PROTECT);
//    }
//
//}
//void my_flash_write_protect_test(void)
//{
//    uint8_t tp0[100],tp1[100],tp2[100];
//    memset(tp0,0x00,100);
//    memset(tp1,0x00,100);
//    memset(tp2,0x00,100);
//
//
//    rom_segment_read(14*MY_FLASH_SECT_BLOCK_SIZE, (WORD)tp0, 100);
//    memset(tp0,0x55,100);
//
//    rom_segment_erase(14*MY_FLASH_SECT_BLOCK_SIZE);
//    rom_segment_write(14*MY_FLASH_SECT_BLOCK_SIZE, (WORD)tp0, 100);
//    rom_segment_read(14*MY_FLASH_SECT_BLOCK_SIZE, (WORD)tp1, 100);
//
//    flash_wirte_protect_fun((uint8_t *)flash_write_block_arry);
//    //FlashProtectionSet(14*MY_FLASH_SECT_BLOCK_SIZE,FLASH_WRITE_PROTECT);
//
//    rom_segment_read(14*MY_FLASH_SECT_BLOCK_SIZE, (WORD)tp2, 100);
//    rom_segment_erase(14*MY_FLASH_SECT_BLOCK_SIZE);
//    memset(tp2,0x11,100);
//    rom_segment_write(14*MY_FLASH_SECT_BLOCK_SIZE, (WORD)tp2, 100);
//    memset(tp2,0x00,100);
//    rom_segment_read(14*MY_FLASH_SECT_BLOCK_SIZE, (WORD)tp2, 100);
//    memset(tp2,0x00,100);
//}
