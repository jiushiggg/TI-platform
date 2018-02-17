
#include "app.h"
#include "../screen/lattice_epd/scrn_public_fun.h"
#include "../aes128/aes128.h"
#include "../flash_protect/flash_protect.h"
#include "../nfc/NFC_Protocl.h"
#include "../nfc/nfc.h"

#define TASK0_STACKSIZE   1024
#define TASK2_STACKSIZE   (6*1024)
Char task0_Stack[TASK0_STACKSIZE];
Char task2_Stack[TASK2_STACKSIZE];
Task_Struct task0_Struct;
Task_Struct task2_Struct;



PIN_Handle ledPinHandle;
PIN_State ledPinState;


PIN_Config ledPinTable[] = {

                            Board_LED0 | PIN_GPIO_OUTPUT_EN | PIN_INPUT_DIS| PIN_GPIO_HIGH | PIN_PUSHPULL | PIN_DRVSTR_MED,
                            Board_LED1 | PIN_GPIO_OUTPUT_EN | PIN_INPUT_DIS| PIN_GPIO_HIGH | PIN_PUSHPULL | PIN_DRVSTR_MED,
                            Board_LED2 | PIN_GPIO_OUTPUT_EN | PIN_INPUT_DIS| PIN_GPIO_HIGH | PIN_PUSHPULL | PIN_DRVSTR_MED,
                            IOID_16 | PIN_GPIO_OUTPUT_EN | PIN_INPUT_DIS| PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MED,
                            IOID_17 | PIN_GPIO_OUTPUT_EN | PIN_INPUT_DIS| PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MED,
                            IOID_28 | PIN_GPIO_OUTPUT_EN | PIN_INPUT_DIS| PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MED,
                            IOID_29 | PIN_GPIO_OUTPUT_EN | PIN_INPUT_DIS| PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MED,
                            PIN_TERMINATE
};


#define NUMMSGS    20
Error_Block eb;

//Mailbox_Struct  Protocol_Struct;
//Mailbox_Params  protocol_params;
//Mailbox_Handle  Protocol_Mailbox;

Mailbox_Struct  rf_rx_timeout_Struct;
Mailbox_Params  rf_rx_timeout_params;
Mailbox_Handle  rf_rx_timeout_mailbox;

Semaphore_Struct  led_semStruct;
Semaphore_Params  led_semparam;
Semaphore_Handle  led_flash_sem;

Semaphore_Struct  led_off_semStruct;
Semaphore_Params  led_off_semparam;
Semaphore_Handle  led_off_sem;

Semaphore_Struct  reed_semStruct;
Semaphore_Params  reed_semparam;
Semaphore_Handle  reed_sem;

Event_Params protocol_eventParam;
Event_Struct protocol_eventStruct;
Event_Handle protocol_eventHandle;


//MailboxMsgObj mailboxBuffer_1[NUMMSGS];
MailboxMsgObj mailboxBuffer_2[NUMMSGS];


Clock_Params  set_wakeup_timer_params;
Clock_Handle  set_wakeup_timer_handle;
Clock_Params  group_wakeup_timer_params;
Clock_Handle  group_wakeup_timer_handle;
Clock_Params  rc_wakeup_timer_params;
Clock_Handle  rc_wakeup_timer_handle;
Clock_Params  uplink_wakeup_timer_params;
Clock_Handle  uplink_wakeup_timer_handle;
Clock_Params  heartbeat_timer_params;
Clock_Handle  heartbeat_timer_handle;

void mailbox_semaphore_init(void)
{
    //    Mailbox_Params_init(&protocol_params);
    //    protocol_params.buf = (Ptr)mailboxBuffer_1;
    //    protocol_params.bufSize = sizeof(mailboxBuffer_1);
    //    Mailbox_construct(&Protocol_Struct, sizeof(MsgObj), NUMMSGS, &protocol_params, NULL);
    //    Protocol_Mailbox = Mailbox_handle(&Protocol_Struct);

    Mailbox_Params_init(&rf_rx_timeout_params);
    rf_rx_timeout_params.buf = (Ptr)mailboxBuffer_2;
    rf_rx_timeout_params.bufSize = sizeof(mailboxBuffer_2);
    Mailbox_construct(&rf_rx_timeout_Struct, sizeof(MsgObj), NUMMSGS, &rf_rx_timeout_params, NULL);
    rf_rx_timeout_mailbox = Mailbox_handle(&rf_rx_timeout_Struct);

    Semaphore_Params_init(&led_semparam);
    led_semparam.mode = ti_sysbios_knl_Semaphore_Mode_BINARY;
    Semaphore_construct(&led_semStruct, 0, &led_semparam);
    led_flash_sem = Semaphore_handle(&led_semStruct);

    Semaphore_Params_init(&led_off_semparam);
    led_off_semparam.mode = ti_sysbios_knl_Semaphore_Mode_BINARY;
    Semaphore_construct(&led_off_semStruct, 0, &led_off_semparam);
    led_off_sem = Semaphore_handle(&led_off_semStruct);

    Semaphore_Params_init(&reed_semparam);
    reed_semparam.mode = ti_sysbios_knl_Semaphore_Mode_BINARY;
    Semaphore_construct(&reed_semStruct, 0, &reed_semparam);
    reed_sem = Semaphore_handle(&reed_semStruct);

    Event_Params_init(&protocol_eventParam);
    Event_construct(&protocol_eventStruct, &protocol_eventParam);
    protocol_eventHandle = Event_handle(&protocol_eventStruct);

}


Void led_flash_Fxn()
{
    while(1)
    {
        Semaphore_pend(led_flash_sem, BIOS_WAIT_FOREVER);
        led_all_off();
        led_flash_flag = TRUE;
        normal_or_horse_led_Fxn();
        led_flash_flag = FALSE;
        led_all_off();
    }
}

void protocol_Fxn(void)
{
    uint32_t heartbeat_count, events;
    init_prama();

    while (1)
    {
        events = Event_pend(protocol_eventHandle, 0, EVENT_ALL, BIOS_WAIT_FOREVER);
        SetWatchDog();                       //看门狗的喂狗
//        extern_flash_open();                 //如果有外部flash，在这里使能外部flash
//        screen_repeat_display();             //24小时自动刷新屏幕的函数
//        if(clear_event_flag_epd_display == 1)
//        {
//            events &= (~EVENT_FLAG_EPD_DISPLAY);
//            clear_event_flag_epd_display = 0;
//        }
//        if(events & EVENT_FLAG_EPD_DISPLAY)  //屏幕显示任务
//        {
//            screen_display();
//        }
        if(events & EVENT_FLAG_NFC)
        {
            NFC_ProtoclFnx(&NFCobj);
        }

//        if(events & EVENT_FLAG_INTER_REED)
//        {
//            Task_sleep(10000);//100ms
//            if(0 == PIN_getInputValue(Board_BUTTON0))
//            {
//                reed_fun_select(epd_attr_info.magnet);
//            }
//        }
//        if(events & EVENT_FLAG_SYS_HEART)
//        {
//            heartbeat_count ++;
//            if(heartbeat_count > 3)
//            {
//                heartbeat_count =0;
//                Send_table_heartbeat(CTRL_TABLE_HBR,1);
//            }else
//            {
//                Send_heartbeat(CTRL_HBR_NORMAL,1);
//            }
//        }
//        if(events & EVENT_FLAG_RFWORK)
//        {
//            rf_interrupt_into_fun();
//        }
//        if(events & EVENT_FLAG_ERASER_BUFF)
//        {
//            event_128_fun();
//        }
//        if(events & EVENT_FLAG_OSD_ANALUSIS)
//        {
//            main_osd_cmd();
//        }
//
//        if(events & EVENT_FALG_DISPLAY_PAGE)
//        {
//            display_page_analysis_fun();
//        }
//        if(events & EVENT_FLAG_UPDATA_CHECK)
//        {
//            erase_pkg_area_fun();
//        }
//        if(events & EVENT_FLAG_NETLINK)
//        {
//            netlink_info_write_fun();
//        }
//        if(events & EVENT_FLAG_QUEST_HEART)
//        {
//            request_heartbeat_fun();
//        }
//        if(events & EVENT_FLAG_UPDATA_ROM)
//        {
//            write_upgread_flag_fun();
//        }
//        if(events & EVENT_FLAG_TIMER_CALIBRATION)
//        {
//
//        }
        f_sync();                             //保存文件系统
        extern_flash_close();                 //如果有外部flash，失能外部flash
    }
}

void init_flash_param_fun(void)
{
#ifdef SCREEN_PROWR_COLES_DISABLE
   // epd_power_deep_sleep_fun();
#else
    sys_rst_close_epd_fun();     //初始化屏幕
#endif
    init_nvs_spi_flash();

    //    test_save_id();
    //    rom_segment_erase(ROM_FLASH_BLOCK0_ADDR);
    //    rom_segment_erase(EF_EADDR - EF_FS_USED);
    //    fs_erase_all();

    sys_load_config_info();                //加载系统默认值
    load_attribute_pkg_fun();                 //加载rc包设置、加载属性报
    key_load_fun();//加载密钥
    screen_init();
    ledPinHandle = PIN_open(&ledPinState, ledPinTable);
    if(!ledPinHandle)
    {
        while(1);
    }
}

void init_mini_file_osd_fun(void)
{
 //   f_init_check(4);
 //   f_init();
//    sys_load_page_display_fun();          //当文件系统重建时，保证价签也号全部清空防止出现反成功不切页
 //   osd_init();
 //   isr_reed_init();
    isr_nfc_init();
//    adc_voltage();
}

void init_prama(void)
{
    open_watchdog();
    flash_wirte_protect_fun();   //APP区和最后8K区域flash写保护
    init_flash_param_fun();
    init_mini_file_osd_fun();
    creat_timer();
    creat_uplink_timer();
    creat_cut_page_timer();
    extern_flash_close();           //flash进入sleep状态


    set_wakeup_timer_params.period = (def_sys_attr.sys_def_attribute.set_wkup_time *1000000)/Clock_tickPeriod;
    set_wakeup_timer_params.startFlag = TRUE;
    set_wakeup_timer_handle = Clock_create((Clock_FuncPtr)set_wakeup_timer_Fxn, set_wakeup_timer_params.period, &set_wakeup_timer_params, &eb);
    Clock_start(set_wakeup_timer_handle);

    group_wakeup_timer_params.period = (def_sys_attr.sys_def_attribute.grp_wkup_time *1000000)/Clock_tickPeriod;
    group_wakeup_timer_params.startFlag = TRUE;
    group_wakeup_timer_handle = Clock_create((Clock_FuncPtr)group_wakeup_timer_Fxn, group_wakeup_timer_params.period, &group_wakeup_timer_params, &eb);
    Clock_stop(group_wakeup_timer_handle);

    rc_wakeup_timer_params.period = 200000/Clock_tickPeriod;
    rc_wakeup_timer_params.startFlag = TRUE;
    rc_wakeup_timer_handle = Clock_create((Clock_FuncPtr)rc_wakeup_timer_Fxn, rc_wakeup_timer_params.period, &rc_wakeup_timer_params, &eb);
    Clock_stop(rc_wakeup_timer_handle);

    uplink_wakeup_timer_params.period = 100000/Clock_tickPeriod;
    uplink_wakeup_timer_params.startFlag = TRUE;
    uplink_wakeup_timer_handle = Clock_create((Clock_FuncPtr)uplink_wakeup_timer_Fxn, uplink_wakeup_timer_params.period, &uplink_wakeup_timer_params, &eb);
    Clock_stop(uplink_wakeup_timer_handle);

    heartbeat_timer_params.period = (def_sys_attr.sys_def_attribute.heartbit_time * 1000000)/Clock_tickPeriod;
    heartbeat_timer_params.startFlag = TRUE;
    heartbeat_timer_handle = Clock_create((Clock_FuncPtr)heartbeat_timer_Fxn,(INFO_DATA.gRFInitData.esl_netmask * 256 + INFO_DATA.gRFInitData.esl_id.id2)*300, &heartbeat_timer_params, &eb);
    Clock_start(heartbeat_timer_handle);
}

void app_init(void)
{

    Task_Params taskParams_0,taskParams_2;

    Task_Params_init(&taskParams_0);
    taskParams_0.arg0 = 1000000 / Clock_tickPeriod;
    taskParams_0.stackSize = TASK0_STACKSIZE;
    taskParams_0.stack = &task0_Stack;
    taskParams_0.priority = 2;
    Task_construct(&task0_Struct, (Task_FuncPtr)led_flash_Fxn, &taskParams_0, NULL);

    Task_Params_init(&taskParams_2);
    taskParams_2.arg0 = 1000000 / Clock_tickPeriod;
    taskParams_2.stackSize = TASK2_STACKSIZE;
    taskParams_2.stack = &task2_Stack;
    taskParams_2.priority = 1;
    Task_construct(&task2_Struct, (Task_FuncPtr)protocol_Fxn, &taskParams_2, NULL);

}
