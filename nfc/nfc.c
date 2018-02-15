#include "../nfc/nfc.h"
#include "nfc.h"
#include "Board.h"
#include "../nfc/NFC_Protocl.h"
#include <ti/sysbios/knl/Clock.h>
#include "../bsp/bsp_spi.h"

static PIN_State  nfcPinState;

PIN_Config nfcPinTable[] = {
                             NFC_INT_PIN | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_NEGEDGE,
                             PIN_TERMINATE
};

void nfcCallbackFxn(PIN_Handle handle, PIN_Id pinId)
{
    if (NFCobj.curState  == NFC_IDLE_STATE){
       Event_post(protocol_eventHandle, EVENT_FLAG_NFC);
       nfc_int_set(NFC_INT_DIS);
        //events |= EVENT_FLAG_NFC;
    }
}

void isr_nfc_init(void)
{
    PIN_Handle nfcPinHandle;
    nfcPinHandle = PIN_open(&nfcPinState, nfcPinTable);
    if(!nfcPinHandle)
    {
        while(1);
    }
    if (PIN_registerIntCb(nfcPinHandle, &nfcCallbackFxn) != 0)
    {
        while(1);
    }

    creat_nfc_timer();
    NFCobj.curState  = NFC_IDLE_STATE;
    NFCobj.nextState  = NFC_IDLE_STATE;
    NFCobj.nextEvent  = NFC_EVENT_NONE;
    NFCobj.curEvent  = NFC_EVENT_NONE;
    NFCobj.error  = NFC_ERR_NONE;
}

void nfc_int_set(uint8_t tmp)
{
    if (tmp == NFC_INT_EN){
        PIN_clrPendInterrupt(&nfcPinState, NFC_INT_PIN);
        PIN_setInterrupt(&nfcPinState, NFC_INT_PIN|PIN_IRQ_NEGEDGE);
    }else{
        PIN_setInterrupt(&nfcPinState, NFC_INT_PIN|PIN_IRQ_DIS);
        PIN_clrPendInterrupt(&nfcPinState, NFC_INT_PIN);
    }
}

//----------------------clock-------------
Clock_Params clk_nfc_Params;
Clock_Handle clk_nfc_Handle;
void clk_nfc_Fxn(void)
{
    switch(NFCobj.curState){
        case NFC_HALT_STATE:
            Event_post(protocol_eventHandle, EVENT_FLAG_NFC);
            break;
        case NFC_ACTIVE_STATE:
        case NFC_DETECT_PROTOCL_STATE:
        case NFC_STANDARD_PROTOCL_HANDLE:
        case NFC_PRIVATE_PROTOCL_HANDLE:
            NFCobj.error = NFC_ERR_TIMEOUT;
            bsp_spi_tm = BSP_SPI_TIMEOUT;
            break;
        default:
            break;
    }

}

void creat_nfc_timer(void)
{

    clk_nfc_Params.period = 0;
    clk_nfc_Params.startFlag = FALSE;
    clk_nfc_Handle = Clock_create((Clock_FuncPtr)clk_nfc_Fxn, NFC_TIMEOUT_BASE, &clk_nfc_Params, 0);
}


void set_nfc_timer(uint32_t period, uint32_t timeout)  
{
    uint16_t hwi_key = 0;
    hwi_key = Hwi_disable();
    Clock_stop(clk_nfc_Handle);

    switch(period)
    {
        case NFC_TIMER_ONCE:
            Clock_setPeriod(clk_nfc_Handle, 0);
            break;
        case NFC_TIMER_PERIOD:
            Clock_setPeriod(clk_nfc_Handle, NFC_TIMER_PERIOD_VALUE);
            break;
        default:
            break;
    }
    Clock_setTimeout(clk_nfc_Handle, NFC_TIMEOUT_BASE*timeout);
    Clock_start(clk_nfc_Handle);
    Hwi_restore(hwi_key);
}

void stop_nfc_timer(void)
{
    Clock_stop(clk_nfc_Handle);
}


