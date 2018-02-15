#ifndef __HOLTEK_MSP430_EPDIO_H__
#define __HOLTEK_MSP430_EPDIO_H__

#include <ti/drivers/PIN.h>
#include <ti/devices/cc26x0r2/driverlib/ioc.h>
#include "CC2640R2_LAUNCHXL.h"


extern PIN_State epdPinState;



//#define EPD_SPI_CS_PIN                  IOID_0
//#define EPD_RST_PIN                     IOID_25
//#define EPD_BUSY_PIN                    IOID_26
//#define EPD_DC_PIN                      IOID_27
//#define EPD_SPI_SDA_PIN                 IOID_28
//#define EPD_ENVDD_PIN                   IOID_29
//#define EPD_SPI_CK_PIN                  IOID_30

//-------------------------------FMSDO-----------------
#define EPD_WF_FMSDO_OUT               
#define EPD_WF_FMSDO_IN              
#define EPD_WF_FMSDO_IO                 
#define EPD_WF_FMSDO_REN_CLOSE        
#define EPD_WF_FMSDO_REN_OPEN         
#define EPD_WF_FMSDO_0                 
#define EPD_WF_FMSDO_1                

//------------------------------EPD_POWER--------------
//设置输入输出模式
#define EPD_WF_POWER_OUT   PIN_setConfig(&epdPinState, PIN_BM_INPUT_MODE|PIN_BM_DRVSTR, EPD_ENVDD_PIN|PIN_GPIO_OUTPUT_EN | PIN_PUSHPULL | PIN_DRVSTR_MAX);
#define EPD_WF_POWER_IN
#define EPD_WF_POWER_IO

//是否打开上下拉电阻
#define EPD_WF_POWER_REN_CLOSE
#define EPD_WF_POWERT_REN_OPEN

//供电、断电
#define EPD_W21_POWER_OFF       PIN_setOutputValue(&epdPinState, EPD_ENVDD_PIN,1);
#define EPD_W21_POWER_ON        PIN_setOutputValue(&epdPinState, EPD_ENVDD_PIN,0);

//------------------------------EPD_Rst----------------  
//设置输入输出模式
#define EPD_WF_RST_OUT
#define EPD_WF_RST_IN
#define EPD_WF_RST_IO

//是否打开上下拉电阻
#define EPD_WF_RST_REN_CLOSE
#define EPD_WF_RST_REN_OPEN

//输出高低或者是上拉下拉
#define EPD_W21_RST_0        PIN_setOutputValue(&epdPinState, EPD_RST_PIN,0)
#define EPD_W21_RST_1        PIN_setOutputValue(&epdPinState, EPD_RST_PIN,1)

//------------------------------cs----------------------
//设置输入输出模式
#define EPD_WF_CS_OUT
#define EPD_WF_CS_IN
#define EPD_WF_CS_IO

//是否打开上下拉电阻
#define EPD_WF_CS_REN_CLOSE
#define EPD_WF_CS_REN_OPEN

//输出高低或者是上拉下拉
#define EPD_W21_CS_0        PIN_setOutputValue(&epdPinState, EPD_SPI_CS_PIN,0)
#define EPD_W21_CS_1        PIN_setOutputValue(&epdPinState, EPD_SPI_CS_PIN,1)

//------------------------------dc----------------------
//设置输入输出模式
#define EPD_WF_DC_OUT
#define EPD_WF_DC_IN
#define EPD_WF_DC_IO

//是否打开上下拉电阻
#define EPD_WF_DC_REN_CLOSE
#define EPD_WF_DC_REN_OPEN

//输出高低或者是上拉下拉
#define EPD_W21_DC_0        PIN_setOutputValue(&epdPinState, EPD_DC_PIN,0)
#define EPD_W21_DC_1        PIN_setOutputValue(&epdPinState, EPD_DC_PIN,1)

//------------------------------bs----------------------
//设置输入输出模式
#define EPD_WF_BS_OUT
#define EPD_WF_BS_IN
#define EPD_WF_BS_IO

//是否打开上下拉电阻
#define EPD_WF_BS_REN_CLOSE
#define EPD_WF_BS_REN_OPEN

//输出高低或者是上拉下拉

#define EPD_W21_BS_0        PIN_setOutputValue(&epdPinState, EPD_BS_PIN,0)
#define EPD_W21_BS_1        PIN_setOutputValue(&epdPinState, EPD_BS_PIN,1)

//-----------------------------cs1----------------------
//设置输入输出模式
#define EPD_WF_CS1_OUT
#define EPD_WF_CS1_IN
#define EPD_WF_CS1_IO

//是否打开上下拉电阻
#define EPD_WF_CS1_REN_CLOSE
#define EPD_WF_CS1_REN_OPEN

//输出高低或者是上拉下拉
#define EPD_W21_CS1_0       PIN_setOutputValue(&epdPinState, EPD_SPI_CS2_PIN,0)
#define EPD_W21_CS1_1       PIN_setOutputValue(&epdPinState, EPD_SPI_CS2_PIN,1)

//---------------------------EPD_CK---------------------   
//设置输入输出模式
#define EPD_WF_CLK_OUT
#define EPD_WF_CLK_IN
#define EPD_WF_CLK_IO

//是否打开上下拉电阻
#define EPD_WF_CLK_REN_CLOSE
#define EPD_WF_CLK_REN_OPEN

//输出高低或者是上拉下拉
#define EPD_W21_CLK_0        PIN_setOutputValue(&epdPinState, EPD_SPI_CK_PIN,0)
#define EPD_W21_CLK_1        PIN_setOutputValue(&epdPinState, EPD_SPI_CK_PIN,1)

//---------------------------EPD_SDA-------------------   

//设置输入输出模式PIN_setConfig(&hStateHui, PIN_BM_INPUT_MODE|PIN_BM_DRVSTR, EPD_ENVDD_PIN|PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH | PIN_PUSHPULL | PIN_DRVSTR_MAX);
#define EPD_WF_SDA_OUT      PIN_setConfig(&epdPinState, PIN_BM_INPUT_MODE|PIN_BM_DRVSTR, EPD_SPI_SDA_PIN|PIN_GPIO_OUTPUT_EN | PIN_PUSHPULL | PIN_DRVSTR_MAX)
#define EPD_WF_SDA_IN       PIN_setConfig(&epdPinState, PIN_BM_INPUT_MODE|PIN_BM_DRVSTR, EPD_SPI_SDA_PIN|PIN_INPUT_EN       | PIN_NOPULL )
#define EPD_WF_SDA_IO

//是否打开上下拉电阻
#define EPD_WF_SDA_REN_CLOSE
#define EPD_WF_SDA_REN_OPEN

//输出高低或者是上拉下拉
#define EPD_W21_MOSI_0      PIN_setOutputValue(&epdPinState, EPD_SPI_SDA_PIN,0)
#define EPD_W21_MOSI_1      PIN_setOutputValue(&epdPinState, EPD_SPI_SDA_PIN,1)
#define EPD_READ_SDA        PIN_getInputValue(EPD_SPI_SDA_PIN)
//#define EPD_READ_SDA        (1 == PIN_getInputValue(EPD_SPI_SDA_PIN)

//---------------------------EPD_Busy-------------------
//设置输入输出模式
#define EPD_WF_BUSY_OUT
#define EPD_WF_BUSY_IN             PIN_setConfig(&epdPinState, PIN_BM_INPUT_MODE|PIN_BM_DRVSTR, EPD_BUSY_PIN|PIN_INPUT_EN | PIN_NOPULL )
#define EPD_WF_BUSY_IN_PULLDOWN      PIN_setConfig(&epdPinState, PIN_BM_INPUT_MODE|PIN_BM_DRVSTR, EPD_BUSY_PIN|PIN_INPUT_EN | PIN_PULLDOWN )
#define EPD_WF_BUSY_IN_PULLUP      PIN_setConfig(&epdPinState, PIN_BM_INPUT_MODE|PIN_BM_DRVSTR, EPD_BUSY_PIN|PIN_INPUT_EN | PIN_PULLUP )
#define EPD_WF_BUSY_IO

//是否打开上下拉电阻
#define EPD_WF_BUSY_REN_CLOSE    PIN_setConfig(&epdPinState, PIN_PUSHPULL, EPD_BUSY_PIN|PIN_NOPULL)
#define EPD_WF_BUSY_REN_OPEN     PIN_setConfig(&epdPinState, PIN_PUSHPULL, EPD_BUSY_PIN|PIN_PULLUP)

//输出高低或者是上拉下拉
#define EPD_W21_BUSY_0      PIN_setOutputValue(&epdPinState, EPD_BUSY_PIN,0)
#define EPD_W21_BUSY_1      PIN_setOutputValue(&epdPinState, EPD_BUSY_PIN,1)
#define isEPD_W21_BUSY       (1 == PIN_getInputValue(EPD_BUSY_PIN))



void epd_spi_open(void);
void epd_powerIO_init(void);
void epd_spi_init(void);
void close_epd(void);
void open_epd(void);
void EPD_IC_Rst(void);
bool screen_busy_fun(void);
void epd_deep_sleep_io(void);
void epd_power_deep_sleep_fun(void);
#endif



























