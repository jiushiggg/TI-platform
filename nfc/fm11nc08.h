/*
 * fm11nc08.h
 *
 *  Created on: 2018年1月5日
 *      Author: ggg
 */

#ifndef FM11NC08_H_
#define FM11NC08_H_
#include <stdint.h>
#include "NFC_Protocl.h"

/**
 * 定义FM11NX08寄存器偏移地址
 */
#define FIFO_ACCESS     0x00        //MCU读写FIFO的入口地址，SPI读写FIFO使用专用指令码
#define FIFO_FLUSH      0x01        //供MCU清空FIFO内容
#define FIFO_WORDCNT    0x02        //供MCU查询FIFO剩余字节
#define RF_STATUS       0x03        //非接触口工作状态
#define RF_TXEN         0x04        //FIFO访问寄存器
#define RF_BAUD         0x05        //非接通信波特率选择
#define RF_RATS         0x06        //非接通信收到的RATS数据
#define MAIN_IRQ        0x07        //主要中断标志寄存器
#define FIFO_IRQ        0x08        //FIFO中断标志寄存器
#define AUX_IRQ         0x09        //辅助中断标志寄存器
#define MAIN_IRQ_MASK   0x0A        //主中断屏蔽寄存器
#define FIFO_IRQ_MASK   0x0B        //FIFO中断屏蔽寄存器
#define AUX_IRQ_MASK    0x0C        //辅助中断屏蔽寄存器
#define NFC_CFG         0x0D        //NFC配置寄存器
#define REGU_CFG        0x0E        //Regulator配置寄存器

#define MAIN_IRQ_RF_PWON  0x80
#define MAIN_IRQ_ACTIVE   0x40
#define MAIN_IRQ_RX_START 0x20
#define MAIN_IRQ_RX_DONE  0x10
#define MAIN_IRQ_TX_DONE  0x08
#define MAIN_IRQ_ARBIT    0x04
#define MAIN_IRQ_FIFO     0x02
#define MAIN_IRQ_AUX      0x01
#define FIFO_IRQ_WL 0x08

#define NFC_L4_STATUS       0XA0
#define NFC_IDLE_STATUS       0X04

void FM11_RF_Tx(uint32_t ilen,uint8_t *ibuf);
/*********************************************************************************************************
** 函数名称:    FM11_RF_Rx
** 函数功能:    写FIFO
** 输入参数:    rbuf:读取数据
** 输出参数:    无
** 返回值:      读取的数据长度
*********************************************************************************************************/
uint32_t FM11_RF_Rx(uint8_t *rbuf);
/*********************************************************************************************************
** 函数名称:    FM11_Set_RatsCfg
** 函数功能:    配置卡片rats相关数据
** 输入参数:    rats 参数
** 输出参数:    无
** 返回值:      无
*********************************************************************************************************/
extern void FM11_Set_RatsCfg(uint8_t rats);


/*********************************************************************************************************
** 函数名称:    FM11_CS_ON
** 函数功能:    FM11的spi口片选ON
** 输入参数:    无
** 输出参数:    无
** 返回值:      无
*********************************************************************************************************/
extern void  FM11_CS_ON(void);
/*********************************************************************************************************
** 函数名称:    FM11_CS_OFF
** 函数功能:    FM11的spi口片选OFF
** 输入参数:    无
** 输出参数:    无
** 返回值:      无
*********************************************************************************************************/
extern void FM11_CS_OFF(void);
/*********************************************************************************************************
** 函数名称:    FM11_Write_E2_Enable
** 函数功能:    FM11的spi口E2写使能
** 输入参数:    无
** 输出参数:    无
** 返回值:      无
*********************************************************************************************************/
extern void FM11_Write_E2_Enable(void);
/*********************************************************************************************************
** 函数名称:    FM11_Write_E2_Disable
** 函数功能:    FM11的spi口E2写禁止
** 输入参数:    无
** 输出参数:    无
** 返回值:      无
*********************************************************************************************************/
extern void FM11_Write_E2_Disable(void);
/*********************************************************************************************************
** 函数名称:    FM11_Serial_WriteReg
** 函数功能:    写FM11寄存器
** 输入参数:    reg:寄存器地址
**                  val:写入的参数
** 输出参数:    无
** 返回值:      无
*********************************************************************************************************/
extern void FM11_Serial_WriteReg(uint8_t reg,uint8_t val);
/*********************************************************************************************************
** 函数名称:    FM11_Sreial_ReadReg
** 函数功能:    读寄存器值
** 输入参数:    reg:寄存器地址
** 输出参数:    无
** 返回值:      val,读出的寄存器值
*********************************************************************************************************/
extern uint8_t FM11_Serial_ReadReg(uint8_t reg);

/*********************************************************************************************************
** 函数名称:    FM11_Serial_Write_E2Page
** 函数功能:    写E2数据
** 输入参数:    addr:E2地址
**           len:写入的数据长度(<=16)
**           *buf:写入的数据
** 输出参数:    无
** 返回值:     无
*********************************************************************************************************/
extern void FM11_Serial_Write_E2Page(uint16_t addr,uint32_t len,uint8_t *buf);

/*********************************************************************************************************
** 函数名称:    FM11_Serial_Write_Eeprom
** 函数功能:    写E2数据
** 输入参数:    addr:E2地址
**           len:写入的数据长度可以大于16字节
**           *wbuf:写入的数据
** 输出参数:    无
** 返回值:     无
*********************************************************************************************************/
extern int FM11_Serial_Write_Eeprom(uint16_t addr,uint32_t len,uint8_t *wbuf);

/*********************************************************************************************************
** 函数名称:    FM11_Serial_Read_Eeprom
** 函数功能:    读取的E2数据
** 输入参数:    addr:E2地址
**           len:读取的数据长度，可以大于16个字节
**           *rbuf:读取的数据
** 输出参数:    无
** 返回值:     无
*********************************************************************************************************/
extern void FM11_Serial_Read_Eeprom(uint16_t addr,uint32_t len,uint8_t *rbuf);

/*********************************************************************************************************
** 函数名称:    FM11_Serial_Write_FIFO
** 函数功能:    写FIFO
** 输入参数:    wlen:写数据长度(<=32 fifo空间)
**           wbuf:写的数据
** 输出参数:    无
** 返回值:      无
*********************************************************************************************************/
extern void FM11_Serial_Write_FIFO(uint8_t *wbuf,uint32_t wlen);
/*********************************************************************************************************
** 函数名称:    FM11_Serial_Read_FIFO
** 函数功能:    发送NAK帧
** 输入参数:    *rlen:待读取的数据长度
** 输出参数:    *rbuf:读取的数据
** 返回值:      读取的数据长度
*********************************************************************************************************/
extern void FM11_Serial_Read_FIFO(uint32_t rlen,uint8_t *rbuf);

/*********************************************************************************************************
** 函数名称:    FM11_Init
** 函数功能:    FM11的spi口初始化程序
** 输入参数:    无
** 输出参数:    无
** 返回值:      无
*********************************************************************************************************/
extern void FM11_Init(uint32_t bitRate, uint32_t clkPin);

extern void pt_delay_us(uint32_t delayUs);
void FM11_tx(stNFCobj *nfc, stESLRecBuf *sbuf, uint8_t len);
uint32_t FM11_rx(stNFCobj *nfc, stESLRecBuf *rbuf);

#endif /* FM11NC08_H_ */
