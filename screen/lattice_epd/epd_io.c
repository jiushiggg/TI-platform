#include "epd_io.h"
#include <ti/sysbios/knl/Task.h>
#include <xdc/runtime/System.h>
#include <ti/drivers/PIN.h>
#include "global_variable.h"


/* Pin driver handle */
PIN_Handle epdPinHandle;
PIN_State  epdPinState;

/*
 * Application LED pin configuration table:
 *   - All LEDs board LEDs are off.
 */

PIN_Config epdPinTable[] = {
    EPD_BS_PIN      | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    EPD_SPI_CS2_PIN | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    EPD_SPI_CS_PIN  | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    EPD_RST_PIN     | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    EPD_SPI_CK_PIN  | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    EPD_BUSY_PIN    | PIN_INPUT_EN       | PIN_GPIO_LOW | PIN_NOPULL,
    EPD_DC_PIN      | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    EPD_SPI_SDA_PIN | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    EPD_ENVDD_PIN   | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    PIN_TERMINATE
};

void EPD_IC_Rst(void)
{
    EPD_W21_RST_1;
    Task_sleep(20000);
    EPD_W21_RST_0;
    Task_sleep(20000);
    EPD_W21_RST_1;
    Task_sleep(20000);
}
void epd_powerIO_init(void)

{
  EPD_WF_POWER_OUT;
  EPD_WF_POWER_IO;
  EPD_WF_POWER_REN_CLOSE;
}

void epd_spiio_close(void)
{ 

  //rst 
  EPD_WF_RST_OUT;
  EPD_WF_RST_IO;
  EPD_WF_RST_REN_CLOSE;
  EPD_W21_RST_0;
  //cs
  EPD_WF_CS_OUT;
  EPD_WF_CS_IO;
  EPD_WF_CS_REN_CLOSE;
  EPD_W21_CS_0;
  //cs2
  EPD_WF_CS1_OUT;
  EPD_WF_CS1_IO;
  EPD_WF_CS1_REN_CLOSE;
  EPD_W21_CS1_0;
  //dc
  EPD_WF_DC_OUT;
  EPD_WF_DC_IO;
  EPD_WF_DC_REN_CLOSE;
  EPD_W21_DC_0;
  //bs
  EPD_WF_BS_OUT;
  EPD_WF_BS_IO;
  EPD_WF_BS_REN_CLOSE;
  EPD_W21_BS_0;
  //cs2
  EPD_WF_CS1_OUT;
  EPD_WF_CS1_IO;
  EPD_WF_CS1_REN_CLOSE;
  EPD_W21_CS1_0;
  //clk  
  EPD_WF_CLK_OUT;
  EPD_WF_CLK_IO;
  EPD_WF_CLK_REN_CLOSE;
  EPD_W21_CLK_0;
  //sda
  EPD_WF_SDA_OUT;
  EPD_WF_SDA_IO;
  EPD_WF_SDA_REN_CLOSE;
  EPD_W21_MOSI_0;
  
  //busy  
  EPD_WF_BUSY_IN_PULLDOWN;
  EPD_WF_BUSY_IO;
  //EPD_WF_BUSY_REN_OPEN;
  //EPD_W21_BUSY_0;
}

void epd_spi_open(void)
{
  epdPinHandle = PIN_open(&epdPinState, epdPinTable);
  if(!epdPinHandle)
  {
      while(1);
  }
  EPD_IC_Rst();
}
void epd_spi_init(void)
{

      //rst
      EPD_WF_RST_OUT;
      EPD_W21_RST_0;

      //cs
      EPD_WF_CS_OUT;
      EPD_W21_CS_1;

      //cs2
      EPD_WF_CS1_OUT;
      EPD_W21_CS1_1;

      //bs
      EPD_WF_BS_OUT;
      EPD_W21_BS_0;     // 4 wire spi mode selected

      //dc
      EPD_WF_DC_OUT;

      //clk
      EPD_WF_CLK_OUT;
      EPD_W21_CLK_0;

      //sda
      EPD_WF_SDA_OUT;

      //busy
      EPD_WF_BUSY_IN;
      EPD_WF_BUSY_REN_CLOSE;
     // EPD_W21_BUSY_0;
      EPD_IC_Rst();

}

void epd_deep_sleep_io(void)
{

    //rst 
  EPD_WF_RST_OUT;
  EPD_WF_RST_IO;
  EPD_WF_RST_REN_CLOSE;
  EPD_W21_RST_1;
  //cs
  EPD_WF_CS_OUT;
  EPD_WF_CS_IO;
  EPD_WF_CS_REN_CLOSE;
  EPD_W21_CS_1;
  //cs2
  EPD_WF_CS1_OUT;
  EPD_WF_CS1_IO;
  EPD_WF_CS1_REN_CLOSE;
  EPD_W21_CS1_1;
  //dc
  EPD_WF_DC_OUT;
  EPD_WF_DC_IO;
  EPD_WF_DC_REN_CLOSE;
  EPD_W21_DC_1;
  //bs
  EPD_WF_BS_OUT;
  EPD_WF_BS_IO;
  EPD_WF_BS_REN_CLOSE;
  EPD_W21_BS_0;
  //clk  
  EPD_WF_CLK_OUT;
  EPD_WF_CLK_IO;
  EPD_WF_CLK_REN_CLOSE;
  EPD_W21_CLK_1;
  //sda
  EPD_WF_SDA_OUT;
  EPD_WF_SDA_IO;
  EPD_WF_SDA_REN_CLOSE;
  EPD_W21_MOSI_1;
  
  //busy  
  EPD_WF_BUSY_IN_PULLUP;
  EPD_WF_BUSY_IO;
  //EPD_WF_BUSY_REN_OPEN;
  //EPD_W21_BUSY_1;
  
}


void close_epd(void)
{
  EPD_W21_POWER_OFF;
  epd_spiio_close();
}
void open_epd(void)
{
//  EPD_W21_MOSI_1;
  EPD_W21_POWER_ON;
  Task_sleep(100);//目的为为了确保屏幕上电时，会把电池电量拉低，导致系统重启
}


