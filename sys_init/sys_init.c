
#include "sys_init.h"
#include  "global_variable.h"
#include <ti/devices/cc26x0r2/driverlib/aon_batmon.h>
#include <ti/drivers/ADCBuf.h>
#include <ti/sysbios/knl/Task.h>


void sys_load_page_display_fun(void)
{
    rom_segment_read(ROM_FLASH_BLOCK4_ADDR, (WORD)&gSys_tp, sizeof(gSys_tp));
    if(gSys_tp.default_page_id != gSys_tp.gpage_nowid) //当前页不等于default页
     {
        gSys_tp.present_page_id = gSys_tp.default_page_id;//要显示的页码改为默认页
        Event_post(protocol_eventHandle, EVENT_FALG_DISPLAY_PAGE);
     }
    gpage.flag = FALSE;//关闭切页定时器标志
    gSys_tp.change_map =0xff;
    save_sys_load_page_info();
}

#define ADC_3_3V        ((UINT32)3300)
#define ADC_3_2V        ((UINT32)3200)
#define ADC_3_1V        ((UINT32)3100)
#define ADC_3_0V        ((UINT32)3000)
#define ADC_2_9V        ((UINT32)2900)
#define ADC_2_8V        ((UINT32)2800)
#define ADC_2_7V        ((UINT32)2700)
#define ADC_2_6V        ((UINT32)2600)

//#define ADC_BUF

#ifdef  ADC_BUF
#define ADCBUFFERSIZE    (5)

uint16_t sampleBufferOne[ADCBUFFERSIZE];
uint16_t sampleBufferTwo[ADCBUFFERSIZE];
uint32_t microVoltBuffer[ADCBUFFERSIZE];

void adcBufCallback(ADCBuf_Handle handle, ADCBuf_Conversion *conversion,void *completedADCBuffer, uint32_t completedChannel)
{

}

void adc_buf(ADCBuf_Handle handle, ADCBuf_Conversion *conversion,void *completedADCBuffer, uint32_t completedChannel)
{
    uint32_t temp;
    /* Adjust raw adc values and convert them to microvolts */
    ADCBuf_adjustRawValues(handle, completedADCBuffer, ADCBUFFERSIZE,completedChannel);
    ADCBuf_convertAdjustedToMicroVolts(handle, completedChannel,completedADCBuffer, microVoltBuffer, ADCBUFFERSIZE);

    temp = ((microVoltBuffer[0]+microVoltBuffer[1]+microVoltBuffer[2]+microVoltBuffer[3]+microVoltBuffer[4])/5000);
    if(temp >= ADC_3_3V)
        gelectric_quantity = 0x07;
    else if(temp >= ADC_3_2V)
        gelectric_quantity =  0x06;
    else if(temp >= ADC_3_1V)
        gelectric_quantity =  0x05;
    else if(temp >= ADC_3_0V)
        gelectric_quantity =  0x04;
    else if(temp >= ADC_2_9V)
        gelectric_quantity =  0x03;
    else if(temp >= ADC_2_8V)
        gelectric_quantity =  0x02;
    else if(temp >= ADC_2_7V)
        gelectric_quantity =  0x01;
    else
        gelectric_quantity =  0x00;
}

void adc_voltage(void){

    ADCBuf_Handle adcBuf;
    ADCBuf_Params adcBufParams;
    ADCBuf_Conversion continuousConversion;

    /* Set up an ADCBuf peripheral in ADCBuf_RECURRENCE_MODE_CONTINUOUS */
    ADCBuf_init();
    ADCBuf_Params_init(&adcBufParams);
    adcBufParams.callbackFxn = adcBufCallback;
    adcBufParams.recurrenceMode = ADCBuf_RECURRENCE_MODE_ONE_SHOT;
    adcBufParams.returnMode = ADCBuf_RETURN_MODE_BLOCKING;
//    adcBufParams.samplingFrequency = 200;
    adcBuf = ADCBuf_open(Board_ADCBUF0, &adcBufParams);

    /* Configure the conversion struct */
    continuousConversion.arg = NULL;
    continuousConversion.adcChannel = CC2640R2_LAUNCHXL_ADCBUF0CHANNELVDDS;
    continuousConversion.sampleBuffer = sampleBufferOne;
    continuousConversion.sampleBufferTwo = sampleBufferTwo;
    continuousConversion.samplesRequestedCount = ADCBUFFERSIZE;

    /* Start converting. */
    if (ADCBuf_convert(adcBuf, &continuousConversion, 1) != ADCBuf_STATUS_SUCCESS)
    {
        //System_abort("Did not start conversion process correctly\n");
    }
//    adc_buf(adcBuf, &continuousConversion, sampleBufferOne, Board_ADCBUF0CHANNEL0);
    ADCBuf_close(adcBuf);

}

#else

#define ADC_SAMPLE_COUNT          5
uint16_t adcValue1[ADC_SAMPLE_COUNT];
uint32_t adcValue1MicroVolt[ADC_SAMPLE_COUNT];

void adc_voltage(void)
{
    uint16_t     i;
    uint32_t temp;
    ADC_Handle   adc;
    ADC_Params   params;
    int_fast16_t res;

    ADC_Params_init(&params);
    adc = ADC_open(CC2640R2_LAUNCHXL_ADCVDDS, &params);

    for (i = 0; i < ADC_SAMPLE_COUNT; i++)
    {
        res = ADC_convert(adc, &adcValue1[i]);

        if (res == ADC_STATUS_SUCCESS)
        {

            adcValue1MicroVolt[i] = ADC_convertRawToMicroVolts(adc, adcValue1[i]);

        }

    }

    ADC_close(adc);

    temp = ((adcValue1MicroVolt[0]+adcValue1MicroVolt[1]+adcValue1MicroVolt[2]+adcValue1MicroVolt[3]+adcValue1MicroVolt[4])/5000);
    if(temp >= ADC_3_3V)
        gelectric_quantity = 0x07;
    else if(temp >= ADC_3_2V)
        gelectric_quantity =  0x06;
    else if(temp >= ADC_3_1V)
        gelectric_quantity =  0x05;
    else if(temp >= ADC_3_0V)
        gelectric_quantity =  0x04;
    else if(temp >= ADC_2_9V)
        gelectric_quantity =  0x03;
    else if(temp >= ADC_2_8V)
        gelectric_quantity =  0x02;
    else if(temp >= ADC_2_7V)
        gelectric_quantity =  0x01;
    else
        gelectric_quantity =  0x00;
}

#endif

#if 0
void adc_voltage(void)
{
    uint32_t temp;
    temp = AONBatMonBatteryVoltageGet();
    temp = (((temp & 0x700) >> 8)*1000 + (((temp & 0xF0) >> 4)*1000/16));

    gelectric_quantity = (UINT16)temp;
    if(temp >= ADC_3_3V)
        gelectric_quantity = 0x07;
    else if(temp >= ADC_3_2V)
        gelectric_quantity =  0x06;
    else if(temp >= ADC_3_1V)
        gelectric_quantity =  0x05;
    else if(temp >= ADC_3_0V)
        gelectric_quantity =  0x04;
    else if(temp >= ADC_2_9V)
        gelectric_quantity =  0x03;
    else if(temp >= ADC_2_8V)
        gelectric_quantity =  0x02;
    else if(temp >= ADC_2_7V)
        gelectric_quantity =  0x01;
    else
        gelectric_quantity =  0x00;
}
#endif
