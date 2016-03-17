/*******************************************************
* Example: 
*   At command pipe
*
* Description:
*   This example demonstrates how to establish a AT command pipe
*   between application and core. So, this example only
*   receives AT command from MAIN port and hands it to
*   CORE, or prints responses from CORE to DEBUG port.
*
* Usage:
*   Input AT command termined by '\n' or '\r\n' through
*   UART1(MAIN) port, and the response will be returned
*   through this port.
********************************************************/
#ifdef __ATC_PIPE__
#include "ql_type.h"
#include "ql_appinit.h"
#include "ql_trace.h"
#include "ql_interface.h"
#include "ql_fcm.h"
#include "ql_stdlib.h"
#include "ql_call.h"
#include "Ql_tcpip.h"

#define BOOT_MSG_RDY                "\r\nRDY\r\n\0"
#define BOOT_MSG_SIM_READY          "\r\n+CPIN: READY\r\n\0"
#define BOOT_MSG_SIM_NOT_READY      "\r\n+CPIN: NOT READY\r\n\0"
#define BOOT_MSG_SIM_NOT_INSERT     "\r\n+CPIN: NOT INSERTED\r\n\0"
#define BOOT_MSG_SIM_PIN_CODE       "\r\n+CPIN: SIM PIN\r\n\0"
#define BOOT_MSG_SIM_PUK_CODE       "\r\n+CPIN: PUK PIN\r\n\0"
#define BOOT_MSG_CFUN_0             "\r\n+CFUN: 0\r\n\0"
#define BOOT_MSG_CFUN_1             "\r\n+CFUN: 1\r\n\0"

#define UART_TRANSPARENT_TRANSFER_EN  1
#if UART_TRANSPARENT_TRANSFER_EN > 0
#define UART_TRANSPARENT_TRANSFER(PORT,DATA,LEN) \
    Ql_SendToUart(PORT, (u8*)DATA, (u16)LEN)
#else
#define UART_TRANSPARENT_TRANSFER(PORT,DATA,LEN)
#endif

QlEventBuffer g_event;
QlPort g_uart_port = ql_uart_port1;
void Callback_URC_Hdlr(u16 type, s32 stat_val);
void ql_entry(void)
{
    Ql_SetDebugMode(BASIC_MODE);
    Ql_DebugTrace("OpenCPU: AT Commands Pipe.\r\n");
    Ql_OpenModemPort(ql_md_port1);
    Ql_SetPortOwner(ql_md_port1, ql_main_task);
    Ql_GenericURC_Init(Callback_URC_Hdlr);
    while(TRUE)
    {
        Ql_GetEvent(&g_event);
        switch(g_event.eventType)
        {
            case EVENT_UARTDATA:
            {
                //TODO: receive and handle data from UART
                PortData_Event* pDataEvt = (PortData_Event*)&g_event.eventData.uartdata_evt;
                s32 dataLen = pDataEvt->len;
                g_uart_port = pDataEvt->port;
                if (ql_usb_com_port1== pDataEvt->port || ql_usb_com_port2== pDataEvt->port)
                {
                    Ql_memset(pDataEvt->data, 0x0, sizeof(pDataEvt->data));
                    dataLen = Ql_UartDirectnessReadData(pDataEvt->port, pDataEvt->data, EVENT_MAX_DATA);
                }
                Ql_SendToModem(ql_md_port1, (u8*)pDataEvt->data, dataLen);
                break;
            }
            case EVENT_MODEMDATA:
            {
                //TODO: receive and hanle data from CORE through virtual modem port
                PortData_Event* pPortEvt = (PortData_Event*)&g_event.eventData.modemdata_evt;
                //Ql_DebugTrace("> %s", pPortEvt->data);
                UART_TRANSPARENT_TRANSFER(g_uart_port, (u8*)pPortEvt->data, pPortEvt->len);
                break;
            }
            
            default:
                break;
        }
    }
}

void sys_init_status_ind(s32 stat_val)
{
    if (SYS_STATE_ATOK == stat_val)
    {
        Ql_DebugTrace("<-- Sys init state: AT ok -->\r\n");
        UART_TRANSPARENT_TRANSFER(g_uart_port, (u8*)BOOT_MSG_RDY, Ql_strlen(BOOT_MSG_RDY));
    } 
    else if (SYS_STATE_PHBOK == stat_val)
    {
        Ql_DebugTrace("<-- Sys init state: Phb ok -->\r\n");
    } 
    else if (SYS_STATE_SMSOK == stat_val)
    {
        Ql_DebugTrace("<-- Sys init state: SMS ok -->\r\n");
    }
}

void sim_status_ind(s32 stat_val)
{
    if (SIMCARD_ALREADY == stat_val)
    {
        Ql_DebugTrace("<-- SIM: SIMCARD_ALREADY -->\r\n");
        UART_TRANSPARENT_TRANSFER(g_uart_port, (u8*)BOOT_MSG_SIM_READY, Ql_strlen(BOOT_MSG_SIM_READY));
    }
    else if (SIMCARD_NOT_INSERTED == stat_val)
    {
        Ql_DebugTrace("<-- SIM: SIMCARD_NOT_INSERTED -->\r\n");
        UART_TRANSPARENT_TRANSFER(g_uart_port, (u8*)BOOT_MSG_SIM_NOT_INSERT, Ql_strlen(BOOT_MSG_SIM_NOT_INSERT));
    }
    else if (SIMCARD_PIN_LOCKED == stat_val)
    {
        Ql_DebugTrace("<-- SIM: SIMCARD_PIN_LOCKED -->\r\n");
        UART_TRANSPARENT_TRANSFER(g_uart_port, (u8*)BOOT_MSG_SIM_PIN_CODE, Ql_strlen(BOOT_MSG_SIM_PIN_CODE));
    }
    else if (SIMCARD_PUK_LOCKED == stat_val)
    {
        Ql_DebugTrace("<-- SIM: SIMCARD_PUK_LOCKED -->\r\n");
        UART_TRANSPARENT_TRANSFER(g_uart_port, (u8*)BOOT_MSG_SIM_PUK_CODE, Ql_strlen(BOOT_MSG_SIM_PUK_CODE));
    }
    else if (SIMCARD_NOT_READY == stat_val)
    {
        Ql_DebugTrace("<-- SIM: SIMCARD_NOT_READY -->\r\n");
        UART_TRANSPARENT_TRANSFER(g_uart_port, (u8*)BOOT_MSG_SIM_NOT_READY, Ql_strlen(BOOT_MSG_SIM_NOT_READY));
    }
    else
    {
        Ql_DebugTrace("<-- SIM: Invalid SIM Indication: %d -->\r\n", stat_val);
    }
}

void network_status_ind(u16 nw_type, s32 stat_val)
{
    s32 len = 0;
    s32 totalLen = 0;
    char notes[100];
    Ql_memset(notes, 0x0, sizeof(notes));
    len = Ql_sprintf(notes, "<-- ");
    totalLen += len;
    if (2 == nw_type)
    {
        len = Ql_sprintf(notes + totalLen, "GSM: ");
        totalLen += len;
    }
    else if (3 == nw_type)
    {
        len = Ql_sprintf(notes + totalLen, "GPRS: ");
        totalLen += len;
    }
    len = 0;
    switch (stat_val)
    {
        case Ql_REG_STATE_NOT_REGISTERED:
            len = Ql_sprintf(notes + totalLen, "NOT_REGISTERED");
            break;
        case Ql_REG_STATE_REGISTERED:
            len = Ql_sprintf(notes + totalLen, "REGISTERED");
            break;
        case Ql_REG_STATE_SEARCHING:
            len = Ql_sprintf(notes + totalLen, "SEARCHING");
            break;
        case Ql_REG_STATE_REG_DENIED:
            len = Ql_sprintf(notes + totalLen, "REG_DENIED");
            break;
        case Ql_REG_STATE_UNKNOWN:
            len = Ql_sprintf(notes + totalLen, "UNKNOWN");
            break;
        case Ql_REG_STATE_NOT_ACTIVE:
            len = Ql_sprintf(notes + totalLen, "NOT_ACTIVE");
            break;
        default:
            break;
    }
    totalLen += len;
    len = Ql_sprintf(notes + totalLen, " -->\r\n");
    Ql_DebugTrace(notes);
}
void Callback_URC_Hdlr(u16 type, s32 stat_val)
{
    switch (type)
    {
        case SYS_INIT_STATE_IND:
            sys_init_status_ind(stat_val);
            break;
        case SIM_CARD_STATE_IND:
            sim_status_ind(stat_val);
            break;
        case GSM_NW_STATE_IND:
        case GPRS_NW_STATE_IND:
            network_status_ind(type, stat_val);
            break;
        case CFUN_IND:
        {
            if (0 == stat_val)
            {
                UART_TRANSPARENT_TRANSFER(g_uart_port, (u8*)BOOT_MSG_CFUN_0, Ql_strlen(BOOT_MSG_CFUN_0));
            }else{
                UART_TRANSPARENT_TRANSFER(g_uart_port, (u8*)BOOT_MSG_CFUN_1, Ql_strlen(BOOT_MSG_CFUN_1));
            }
            Ql_DebugTrace("<-- CFUN state: %d -->\r\n", stat_val);
            break;
        }
        default:
            Ql_DebugTrace("<-- urc type: %d -->\r\n", type);
            break;
    }
}

#endif // __ATC_PIPE__

