#include "three_protocol.h"
#include "rf.h"
#include "three_event.h"
#include <string.h>
#include "global_variable.h"
#include "storage_struct.h"
#include "../aes128/aes128.h"
#include "../aes128/aes128_osdcmd.h"

const rf_cmd_t cmd_rf[] = {globle_evert_fun,set_get_frame1_fun,set_pkg_trn_fun,set_pkg_chn_fun,set_broadcast_fun,set_pkg_quick_led_page_fun,set_pkg_uplink_ctrl_fun,set_ctrl_err_fun,get_data_query_fun,\
                           get_netlink_fun,get_osd_updata_fun,get_osd_broadcast_pkg_fun,get_osd_128cmd_fun,get_osd_76cmd_fun,get_link_query_fun,get_broadcast_fun,get_rc_pkg_fun,get_sleep_fun};


int rf_state_machine(RF_T *rf)
{
    uint8_t loop = 1;
    uint8_t rf_buf[32];
    RF_ERROR_T err;
    MsgObj  clear_msg;

    while (loop)
    {
        switch (rf->cmd)
        {
        //***********************************rf �쳣��������********************************
        case RF_FSM_CMD_INIT:                                        //��ʼ��rf
            RF_close(rfHandle);
            rf_init();
            gsid.old_sid  = 0;
            gsid.old_sub_sid = 0;
            rf->next_cmd = RF_FSM_CMD_SLEEP;

            //***********************************set��״̬����**********************************
        case RF_FSM_CMD_SET_EXIT_LOOP:                              //�˳���������set��֡ģʽ�£�����ģʽ��֡1��ʱ���յ�sleep֡���˳�����set����
            set_wor_flag_fun(RF_FSM_CMD_SET_EXIT_LOOP);
            rf->next_cmd = RF_FSM_CMD_SLEEP;
            break;
        case RF_FSM_CMD_SET_WAKEUP:
            set_wor_flag_fun(RF_FSM_CMD_SET_EXIT_LOOP);
            rf->next_cmd = RF_FSM_CMD_SET_RX;
            break;
        case RF_FSM_CMD_SET_RX:                     //����6�ֽڵ�set

            Mailbox_pend(rf_rx_timeout_mailbox, &clear_msg, 0);     //�崦��ʱ����ʱpost���ź���
            memset(rf_buf, 0, RF_SET_WAKEUP_PACKET);
            err = rf_cmd_set_rx(rf, rf_buf);
            if (err == RF_ERROR_NONE)
            {
                set_wkup_event(rf, rf_buf[0]);
                rf->next_cmd = RF_FSM_EVENT_SET_HANDLE;
            }
            else
                rf->next_cmd = RF_FSM_CMD_SLEEP;
            break;
        case RF_FSM_EVENT_SET_HANDLE:

            set_wor_flag_fun(RF_FSM_CMD_SET_EXIT_LOOP);
            rf->next_cmd  =  cmd_rf[rf->event](rf_buf);
            rf->prev_event = rf->event;
            break;


            //***********************************group��״̬����********************************

        case RF_FSM_CMD_GRP_EXIT_LOOP:
            set_wor_flag_fun(RF_FSM_CMD_GRP_EXIT_LOOP);
            rf->next_cmd = RF_FSM_CMD_SLEEP;
            break;

        case RF_FSM_CMD_GRP_WAKEUP:
            Mailbox_pend(rf_rx_timeout_mailbox, &clear_msg, 0);
            set_clock_timeout(GRP_RX_TIMEOUT);     //���ö�ʱ��
            rf->next_cmd = RF_FSM_CMD_GRP_RX;
            break;

        case RF_FSM_CMD_GRP_RX:
            set_wor_flag_fun(RF_FSM_CMD_GRP_EXIT_LOOP);
            if(gro_wor_times > def_sys_attr.exit_grp_wkup_cont)
            {
                gro_wor_times= 0;
                rf->next_cmd = RF_FSM_CMD_SET_WAKEUP;
                break;
            }
            gro_wor_times++;
            memset(rf_buf, 0, RF_GRP_WAKEUP_PACKET);
            err = rf_cmd_grp_rx(rf, rf_buf);
            if (err == RF_ERROR_NONE)
            {
                rf->next_cmd =  grp_wkup_event(rf, rf_buf);
            }
            else
            {
                rf->next_cmd = RF_FSM_CMD_SET_RX;

            }
            break;

            //***********************************ң��������***********************************
        case RF_FSM_CMD_RC_EXIT_LOOP:
            set_wor_flag_fun(RF_FSM_CMD_RC_EXIT_LOOP);
            rf->next_cmd = RF_FSM_CMD_SLEEP;
            break;

        case RF_FSM_CMD_RC_WAKEUP:
            Mailbox_pend(rf_rx_timeout_mailbox, &clear_msg, 0);
            rf->next_cmd = RF_FSM_CMD_RC_RX;
            break;

        case RF_FSM_CMD_RC_RX:
            set_wor_flag_fun(RF_FSM_CMD_RC_EXIT_LOOP);
            if(rc_wor_times > RC_WOR_CONT)
            {
                rc_wor_times= 0;
                rf->next_cmd = rc_wor_exit_fun();
                break;
            }
            rc_wor_times++;
            memset(rf_buf, 0, RF_GRP_WAKEUP_PACKET);
            err = rf_rc_grp_rx(rf, rf_buf);
            if (err == RF_ERROR_NONE)
            {
                rf->next_cmd =  rc_wkup_event(rf, rf_buf);
            }
            else
                rf->next_cmd = RF_FSM_CMD_SLEEP;
            break;

            //***********************************uplink����************************************

        case RF_FSM_CMD_UPLINK_EXIT_LOOP:
            set_wor_flag_fun(RF_FSM_CMD_UPLINK_EXIT_LOOP);
            rf->next_cmd = RF_FSM_CMD_SLEEP;
            break;

        case RF_FSM_CMD_UPLINK_WAKEUP:
            Mailbox_pend(rf_rx_timeout_mailbox, &clear_msg, 0);
            rf->next_cmd = RF_FSM_CMD_UPLINK_RX;
            break;

        case RF_FSM_CMD_UPLINK_RX:
            set_wor_flag_fun(RF_FSM_CMD_UPLINK_EXIT_LOOP);
            if((uplink_wor_times > UPLINK_WOR_CONT)||(exit_uplink_status_flag == TRUE))
            {
                exit_uplink_status_flag = FALSE;
                uplink_wor_times= 0;
                rf->next_cmd = rc_wor_exit_fun();
                break;
            }
            uplink_wor_times++;
            memset(rf_buf, 0, RF_NORMAL_PACKET);
            err = rf_uplink_set_rx(rf, rf_buf);
            if (err == RF_ERROR_NONE)
            {
                uplink_wor_times= 0;
                rf->next_cmd =  uplink_wkup_event(rf, rf_buf);
            }
            else
                rf->next_cmd = RF_FSM_CMD_SLEEP;
            break;

            //***********************************����֡1״̬����********************************
        case RF_FSM_CONFG_FRAME1_PER:
            Mailbox_pend(rf_rx_timeout_mailbox, &clear_msg, 0);
            set_clock_timeout(FRAME1_RX_TIMEOUT);     //���ö�ʱ��
            rf->next_cmd = RF_FSM_CMD_RX_FRAME1;
            break;

        case RF_FSM_CMD_RX_FRAME1:
            err = rf_cmd_rx_fram1(rf, rf_buf);
            if (err == RF_ERROR_NONE)
            {
                rf->next_cmd = rx_frame1_event(rf, rf_buf);
            }
            else
            {

                rf->next_cmd = rx_frame1_mask_exit_fun();
            }
            break;

            //***********************************����֡2(�������԰�)״̬����********************************
        case RF_FSM_CONFG_FRAME2_PER:
            Mailbox_pend(rf_rx_timeout_mailbox, &clear_msg, 0);
            set_clock_timeout(FRAME2_RX_TIMEOUT);     //���ö�ʱ��
            rf->next_cmd = RF_FSM_CMD_RX_FRAME2;
            break;

        case RF_FSM_CMD_RX_FRAME2:
            err = rf_cmd_rx_fram2(rf, rf_buf);
            if (err == RF_ERROR_NONE)
            {
                rf->next_cmd = rx_frame2_event(rf, rf_buf);
            }
            else
            {

                rf->next_cmd = RF_FSM_CMD_SET_EXIT_LOOP;
            }
            break;

            //***********************************��������״̬����********************************
        case RF_FSM_CMD_RX_DATA_CONFIG:
            rf->next_cmd = RF_FSM_CMD_RX_DATA;
            break;
        case RF_FSM_CMD_RX_DATA:                    //�������ն�����ݰ�ʱ������С���ĵĿ��ǵ㣬ԭ����Ƶ�����л�״̬����ת
            err = rf_cmd_rx_data(rf, rf_buf);
            if (err == RF_ERROR_NONE)
            {
                uint8_t ctrl_tp,type;
                type = (rf_buf[3]&0xf0)>>4;
                ctrl_tp = rf_buf[0]&(UINT8)SET_CTRL_INFO;
                if( ( ctrl_tp== CTRL_LINK_QUERY) || (ctrl_tp == CTRL_SLEEP) || (ctrl_tp == CTRL_RC_PKG))
                    goto not_encrypted;

                if( ctrl_tp== CTRL_NETLINK)
                    type = 0;
          
                if(check_initkey_permissions_fun(type) == false)//����״̬�³�ʼ����Կ��Ȩ�޴���
                {
                    rf->next_cmd = RF_FSM_CMD_RX_DATA;
                    break;
                }

                if(false == normaldata_decrypt_fun(rf_buf + 8  , aes128.flag.aes_enable  ,type ,aes128.flag.isencryption))
                {
                    gerr_info = (GERR_T)AES_HARDWARE_ERR;
                    rf->next_cmd = RF_FSM_CMD_RX_DATA;
                    break;
                }
                not_encrypted:
                if(TRUE != crc_normal_pkg_fun(rf_buf))//����ʱ����ʾ��Կ���󣬲�����ʱ��ʾcrc��
                {
                    check_26info_key_err_fun(type);
                    rf->next_cmd = RF_FSM_CMD_RX_DATA;
                    break;
                }
                rx_data_event(rf, rf_buf);
                rf->next_cmd = RF_FSM_EVENT_RX_DATA_HANDLE;

            }
            else
            {
                clear_pkg_nosd_fun();
                rf->next_cmd = RF_FSM_CMD_SLEEP;
            }
            break;
        case RF_FSM_EVENT_RX_DATA_HANDLE:
            rf->next_cmd  =  cmd_rf[rf->event](rf_buf);
            rf->prev_event = rf->event;
            break;

            //***********************************�㲥��������״̬����********************************
        case RF_FSM_CMD_BROADCAST_RX_DATA_CONFIG:
            rf->next_cmd = RF_FSM_CMD_BROADCAST_RX_DATA;
            break;
        case RF_FSM_CMD_BROADCAST_RX_DATA:                    //�������ն�����ݰ�ʱ������С���ĵĿ��ǵ㣬ԭ����Ƶ�����л�״̬����ת
            err = rf_cmd_broadcast_rx_data(rf, rf_buf);
            if (err == RF_ERROR_NONE)
            {
                if((rf_buf[0] & SET_CTRL_INFO) !=CTRL_BROADCAST_UPDATA)//��ֹ��ǰ�����գ��ٴ��յ��������԰����յ����������¿����������ݰ�
                {
                    rf->next_cmd = RF_FSM_CMD_BROADCAST_RX_DATA;
                    break;
                }
                if(false == romupdataldata_decrypt_fun(rf_buf + 8 , aes128.flag.aes_enable  ,aes128.flag.isencryption))
                {
                    gerr_info = (GERR_T)AES_HARDWARE_ERR;
                    rf->next_cmd = RF_FSM_CMD_BROADCAST_RX_DATA;
                    break;
                }

                if(TRUE != crc_broadcast_normal_pkg_fun(rf_buf))
                {
                    check_26info_key_err_fun(2);
                    rf->next_cmd = RF_FSM_CMD_BROADCAST_RX_DATA;
                    break;
                }
                broadcast_rx_data_event(rf, rf_buf);
                rf->next_cmd = RF_FSM_EVENT_BROADCAST_RX_DATA_HANDLE;

            }
            else
            {
                //                clear_pkg_nosd_fun();
                rf->next_cmd = RF_FSM_CMD_SLEEP;
            }
            break;
        case RF_FSM_EVENT_BROADCAST_RX_DATA_HANDLE:
            rf->next_cmd  =  cmd_rf[rf->event](rf_buf);
            rf->prev_event = rf->event;
            break;

            //***********************************��ACK��״̬����********************************
        case RF_FSM_EVENT_TX_ACK_HANDLE:
            rf->next_cmd  = tx_ack_info(rf_buf);
            break;
        case RF_FSM_CMD_TX_DATA:

            err = ack_tx_data(RF_NORMAL_PACKET, rf_buf);
            if (err == RF_ERROR_NONE)
            {
                clear_pkg_nosd_fun();
                rf->next_cmd = RF_FSM_CMD_RX_DATA;
            }
            else
            {
                clear_pkg_nosd_fun();
                rf->next_cmd = RF_FSM_CMD_SLEEP;
            }

            break;

            //***********************************rf����״̬����********************************
        case  RF_FSM_CMD_SLEEP:
            start_set_group_rc_timer();
            clear_pkg_nosd_fun();
            erase_file_fun();
            //      RF_yield(rfHandle);
            loop = 0;
            aes_ack_clean_fun();
            break;

        default:
            rf->next_cmd = RF_FSM_CMD_RF_POR_RST;
            break;
        }
        rf->prev_cmd = rf->cmd;
        rf->cmd = rf->next_cmd;
    }

    return 0;
}
