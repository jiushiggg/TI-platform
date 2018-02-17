/*
 * fm11nc08.c
 *
 *  Created on: 2018��1��5��
 *      Author: ggg
 */
#include "fm11nc08.h"
#include "../bsp/bsp_spi.h"
#include "NFC_Protocl.h"

#define OFF 0
#define ON  1
#define  FM11_E2_BLOCK_SIZE  16
#define  FM11_E2_USER_ADDR   0x000C
#define  FM11_E2_MANUF_ADDR  0x039F


/**
 * ����FM11NX08 ��������
 */
#define CS_DELAY        10   //task_sleep ��10usΪ��λ

#define MIN_LEN             3   //PCB+CRC16

void NFC_IRQ_occur(void);
void FM11_tx(stNFCobj *nfc, stESLRecBuf *sbuf, uint8_t len);
uint32_t FM11_rx(stNFCobj *nfc, stESLRecBuf *rbuf);


void pt_delay_ms(uint32_t delayMs)
{
    volatile uint32_t j;
    /* by experimination, this is in ms (approx) */
    for (j = 0; j < 4010 * delayMs; j++)
    {
        asm(" NOP");
    }
}

void pt_delay_us(uint32_t delayUs)
{
    volatile  uint32_t j;
    for (j = 0; j < 4 * delayUs; j++)
    {
        asm(" NOP");
    }
}


#define Delay_10us(val)        {pt_delay_us(val*10);}

/*********************************************************************************************************
** ��������:    FM11_Set_RatsCfg
** ��������:    ���ÿ�Ƭrats�������
** �������:    rats ����
** �������:    ��
** ����ֵ:      ��
*********************************************************************************************************/
//void FM11_Set_RatsCfg(uint8_t rats)
//{
//    uint8_t temp;
//
//    CID = rats & 0x0F;
//    temp = (rats >> 4) & 0x0F;
//
//    if(temp < 5)
//        FSDI = 8*(temp+2);
//    else if((temp >= 5)&&(temp <= 7))
//        FSDI = 32*(temp-3);
//    else
//        FSDI = 256;
//
//    FSDI -= 2;  //��ȥ2�ֽ�EDC
//    block_num = 0x01;   //��ʼΪ1������ǰ��һֱΪ��һ֡�Ŀ��
//}



/*********************************************************************************************************
** ��������:    FM11_CS_ON
** ��������:    FM11��spi��ƬѡON
** �������:    ��
** �������:    ��
** ����ֵ:      ��
*********************************************************************************************************/
void FM11_CS_ON(void)
{
    //__disable_irq();
    BSP_FM11_Sel();        //�������Ч
    Delay_10us(100);
}

/*********************************************************************************************************
** ��������:    FM11_CS_OFF
** ��������:    FM11��spi��ƬѡOFF
** �������:    ��
** �������:    ��
** ����ֵ:      ��
*********************************************************************************************************/
void FM11_CS_OFF(void)
{
    BSP_FM11_DeSel()   //�������Ч
    //__enable_irq();
}


/**********************************************************************************************************
** �������ƣ�FM11��дһ���ֽ�(SPI)
*********************************************************************************************************
uint8_t FM11_Single_RW(uint8_t wData)
{
    //��������
    while (SPI_I2S_GetFlagStatus(FM11_SPI, SPI_I2S_FLAG_TXE) == RESET);

    SPI_I2S_SendData(FM11_SPI,wData);

    while (SPI_I2S_GetFlagStatus(FM11_SPI, SPI_I2S_FLAG_RXNE) == RESET);
    //��������
    return SPI_I2S_ReceiveData(FM11_SPI);
}
*/

/**********************************************************************************************************
** �������ƣ�FM11 д����ֽ� <=16���ֽ�
********************************************************************************************************
void SPI_WriteByte(uint8_t *pData, uint32_t length)
{

  for(; length>0; length--)
    {
        FM11_SPI_SingleRW(*pData);
        pData++;
    }


}
**/
/**********************************************************************************************************
** �������ƣ�FM11������ֽ� <=16���ֽ�
********************************************************************************************************
void  SPI_readByte(uint8_t *pData, uint32_t length)
{

  while(length--)
        {
                *pData = FM11_SPI_SingleRW(0xFF);
          pData++;
        }

}
**/

/*********************************************************************************************************
** ��������:    FM11_Write_E2_Enable
** ��������:    FM11��spi��E2дʹ��
** �������:    ��
** �������:    ��
** ����ֵ:      ��
*********************************************************************************************************/
void FM11_Write_E2_Enable(void)
{
    FM11_CS_ON();
    Delay_10us(CS_DELAY);         //��ʱȷ��FM11�ϵ����

    BSP_SPI_RW_One(0xCE); //ʹ��E2����ָ��ȷ���CE
    BSP_SPI_RW_One(0x55); //ʹ��E2����ָ��ٷ���55�����ĵ���30ҳ
    FM11_CS_OFF();
}

/*********************************************************************************************************
** ��������:    FM11_Write_E2_Disable
** ��������:    FM11��spi��E2д��ֹ
** �������:    ��
** �������:    ��
** ����ֵ:      ��
*********************************************************************************************************/
void FM11_Write_E2_Disable(void)
{
    FM11_CS_ON();
    Delay_10us(CS_DELAY);         //��ʱȷ��FM11�ϵ����
    BSP_SPI_RW_One(0xCE); //��ֹE2����ָ��
    BSP_SPI_RW_One(0xAA); //��ֹE2����ָ��
    FM11_CS_OFF();
}


/*********************************************************************************************************
** ��������:    FM11_Serial_WriteReg
** ��������:    дFM11�Ĵ���
** �������:    reg:�Ĵ�����ַ
**                  val:д��Ĳ���
** �������:    ��
** ����ֵ:      ��
*********************************************************************************************************/
void FM11_Serial_WriteReg(uint8_t reg,uint8_t val)
{
    uint8_t mode;
    mode = reg & 0x0F;
    FM11_CS_ON();
    Delay_10us(CS_DELAY);
    BSP_SPI_RW_One(mode);    //ָ����ֲ��28ҳ
    BSP_SPI_RW_One(val);
    FM11_CS_OFF();
}


/*********************************************************************************************************
** ��������:    FM11_Serial_ReadReg
** ��������:    ���Ĵ���ֵ
** �������:    reg:�Ĵ�����ַ
** �������:    ��
** ����ֵ:      val,�����ļĴ���ֵ
*********************************************************************************************************/
uint8_t FM11_Serial_ReadReg(uint8_t reg)
{
    uint8_t mode;
    uint8_t val;

    mode = reg&0x0F; //ָ����ֲ��28ҳ
    mode = mode | 0x20; //ָ����ֲ��28ҳ

    FM11_CS_ON();
    BSP_SPI_RW_One(mode);
    val = BSP_SPI_RW_One(0xFF);
    FM11_CS_OFF();
    return val;
}

/*********************************************************************************************************
** ��������:    FM11_Serial_Write_E2Page
** ��������:    дE2����
** �������:    addr:E2��ַ
**           len:д������ݳ���(<=16)
**           *buf:д�������
** �������:    ��
** ����ֵ:     ��
*********************************************************************************************************/
uint8_t w_len=0;
uint8_t w_cnt=0;
void FM11_Serial_Write_E2Page(uint16_t addr,uint32_t len,uint8_t *buf)
{
    uint8_t cmd[2];
    cmd[0] = (addr >> 8) & 0x03| 0x40; //�˴�ȷ��һ�£��費��Ҫ�ٴη���дee�ĵ�һ���ֽڵ�����ָ��
    cmd[1] = addr & 0xFF;
//  printf("%s: adr = 0x%3x, len = %d, ibuf[0] = 0x%2x\r\n", __func__, adr, len, ibuf[0]);
    FM11_CS_ON();
    Delay_10us(10);
    BSP_SPI_RW_One(cmd[0]);
    BSP_SPI_RW_One(cmd[1]); //E2��ַΪ10bit������ֽ�ֻռ8bit��������λ��addr0
    BSP_SPI_Write(buf,len);
    FM11_CS_OFF();
    Delay_10us(2000);          //�˴�����Ҫ����ʱ�����ֲ��30ҳ
}

/*********************************************************************************************************
** ��������:    FM11_Serial_Write_Eeprom
** ��������:    дE2����
** �������:    addr:E2��ַ
**           len:д������ݳ��ȿ��Դ���16�ֽ�
**           *wbuf:д�������
** �������:    ��
** ����ֵ:     ��
*********************************************************************************************************/
int FM11_Serial_Write_Eeprom(uint16_t addr,uint32_t len,uint8_t *wbuf)
{
    uint8_t offset;
   FM11_Write_E2_Enable();

//  printf("%s: adr = 0x%3x, len = %d, wbuf[0] = 0x%2x\r\n", __func__, adr, len, wbuf[0]);
    if(addr < FM11_E2_USER_ADDR || addr >= FM11_E2_MANUF_ADDR)
    {
        return -1;
    }
    if(addr % FM11_E2_BLOCK_SIZE)
    {
        offset = FM11_E2_BLOCK_SIZE - (addr % FM11_E2_BLOCK_SIZE);
        if(len > offset)
        {
            FM11_Serial_Write_E2Page(addr,offset,wbuf);
            addr += offset;
            wbuf += offset;
            len -= offset;
        }
        else
        {
            FM11_Serial_Write_E2Page(addr,len,wbuf);
            len = 0;
        }
    }
    while(len)
    {
        if(len >= FM11_E2_BLOCK_SIZE)
        {
            FM11_Serial_Write_E2Page(addr,FM11_E2_BLOCK_SIZE,wbuf);
            addr += FM11_E2_BLOCK_SIZE;
            wbuf += FM11_E2_BLOCK_SIZE;
            len -= FM11_E2_BLOCK_SIZE;
        }
        else
        {
            FM11_Serial_Write_E2Page(addr,len,wbuf);
            len = 0;
        }
    }
    return 0;
}


/*********************************************************************************************************
** ��������:    FM11_Serial_Read_Eeprom
** ��������:    ��ȡ��E2����
** �������:    addr:E2��ַ
**           len:��ȡ�����ݳ��ȣ����Դ���16���ֽ�
**           *rbuf:��ȡ������
** �������:    ��
** ����ֵ:     ��
*********************************************************************************************************/
void FM11_Serial_Read_Eeprom(uint16_t addr,uint32_t len,uint8_t *rbuf)
{
    uint8_t buf[2];
    buf[0] = ((addr >> 8) & 0x03)| 0x60; //ָ����ֲ��28ҳ
    buf[1] = addr & 0xFF;

    FM11_CS_ON();
    Delay_10us(CS_DELAY);         //��ʱȷ��FM11�ϵ����
    BSP_SPI_RW_One(buf[0]);
    BSP_SPI_RW_One(buf[1]);
    BSP_SPI_Read(rbuf,len);
    FM11_CS_OFF();
}


/*********************************************************************************************************
** ��������:    FM11_Serial_Write_FIFO
** ��������:    дFIFO
** �������:    wlen:д���ݳ���(<=32 fifo�ռ�)
**           wbuf:д������
** �������:    ��
** ����ֵ:      ��
*********************************************************************************************************/
void FM11_Serial_Write_FIFO(uint8_t *wbuf,uint32_t wlen)
{
    FM11_CS_ON();
    BSP_SPI_RW_One(0x80);
    BSP_SPI_Write(wbuf,wlen);
    FM11_CS_OFF();
}

/*********************************************************************************************************
** ��������:    FM11_Serial_Read_FIFO
** ��������:    ����NAK֡
** �������:    *rlen:����ȡ�����ݳ���
** �������:    *rbuf:��ȡ������
** ����ֵ:      ��ȡ�����ݳ���
*********************************************************************************************************/
void FM11_Serial_Read_FIFO(uint32_t rlen,uint8_t *rbuf)
{
    FM11_CS_ON();
    BSP_SPI_RW_One(0xA0);
    BSP_SPI_Read(rbuf,rlen);
    FM11_CS_OFF();
}


/*********************************************************************************************************
** ��������:    FM11_Init
** ��������:    FM11��spi�ڳ�ʼ������
** �������:    ��
** �������:    ��
** ����ֵ:      ��
*********************************************************************************************************/
void FM11_Init(uint32_t bitRate, uint32_t clkPin)
{
    BSP_SPI_Open(bitRate,clkPin);

    FM11_CS_OFF();
    FM11_Serial_WriteReg(FIFO_FLUSH,0xFF);     //��fifo�Ĵ���

#ifdef FIFO_TEST
    while(1)
    {
        FM11_Serial_Write_FIFO(test_WriteBuf, 8);
        FM11_Serial_Read_FIFO(8, test_ReadBuf);
        FM11_Serial_ReadReg(NFC_CFG);
    }
#endif

#ifdef REG_TEST
    FM11_Serial_WriteReg(NFC_CFG,0x3);
    g_reg=FM11_Serial_ReadReg(NFC_CFG);

    FM11_Serial_WriteReg(NFC_CFG,0x2);
    g_reg=FM11_Serial_ReadReg(NFC_CFG);
#endif
}

uint32_t FM11_rx(stNFCobj *nfc, stESLRecBuf *rbuf)
{
    uint32_t len_cnt=0;
    uint8_t tmp = 0;
    //uint8_t debug = 0;

//    FM11_Serial_WriteReg(FIFO_FLUSH,0xFF);     //��fifo�Ĵ���
    while(NFC_ERR_NONE == nfc->error){
        tmp = FM11_Serial_ReadReg(MAIN_IRQ);
//        debug = FM11_Serial_ReadReg(MAIN_IRQ_MASK);
        if(tmp & MAIN_IRQ_RX_DONE){
            len_cnt =  FM11_Serial_ReadReg(FIFO_WORDCNT) & 0x3F;
            if (len_cnt < MIN_LEN){
                continue;
            }
            nfc->curEvent = NFC_EVENT_REC_DONE;
            break;
        }
    }
    if (NFC_ERR_NONE != nfc->error){
        return 0;
    }

    if (nfc->curEvent == NFC_EVENT_REC_DONE){    //������һֱ���գ�д��RAM��
        FM11_Serial_Read_FIFO(len_cnt, rbuf->buf);
    }else{
        nfc->error = NFC_ERR_UNKNOW;
    }
    return len_cnt;
}
void FM11_tx(stNFCobj *nfc, stESLRecBuf *sbuf, uint8_t len)
{
    uint8_t tmp = 0;
    FM11_Serial_Write_FIFO(sbuf->buf,  len);
    FM11_Serial_WriteReg(RF_TXEN, 0x55);
    while(NFC_ERR_NONE == nfc->error){
        tmp = FM11_Serial_ReadReg(MAIN_IRQ);
//                        debug = FM11_Serial_ReadReg(MAIN_IRQ_MASK);
        if(tmp & MAIN_IRQ_TX_DONE){
            nfc->curEvent = NFC_EVENT_SEND_DONE;
            break;
        }
    }
}



