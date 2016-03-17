#ifdef __EXAMPLE_HELLWORLD__
/***************************************************************************************************
*   Example:
*       
*           HELLWORLD Routine
*
*   Description:
*
*           This example gives an example for HELLWORLD operation.
*           
*
*   Usage:
*
*           Precondition:
*
*                   Use "make/make128 helloworld" to compile, and download bin image to module.
*           
*           Through Uart port:
*                   step1: operation module,we can see some informations from the debug port.
*                   step2: input at command, and the app can send it to core. And feed back with the response
*           
****************************************************************************************************/

#include "ql_appinit.h"
#include "ql_interface.h"
#include "ql_type.h"
#include "ql_timer.h"
#include "ql_stdlib.h"
#include "ql_fcm.h"
#include "ql_trace.h"
#include "ql_call.h"
#include "ql_tcpip.h"

void Callback_URC_Hdlr(u16 type, s32 stat_val);

QlEventBuffer g_event; // Keep this variable a global variable due to its big size
char notes[100];
void ql_entry(void)
{
    s32 ret;
    bool keepGoing = TRUE;
    QlTimer tm;
    u32 cnt = 0; 
    //s32*** triPointer;

    Ql_SetDebugMode(BASIC_MODE);    /* Only after the device reboots, 
                                     *  the set debug-mode takes effect. 
                                     */
    Ql_DebugTrace("OpenCPU: Hello World !\r\n\r\n");    /* Print out message through DEBUG port */
    Ql_OpenModemPort(ql_md_port1);  /* or ql_md_port2, two virtual modem ports are available.*/
    Ql_GenericURC_Init(Callback_URC_Hdlr);
    
    // Start a timer
    tm.timeoutPeriod = Ql_SecondToTicks(2); /* Set the interval of timer */
    Ql_StartTimer(&tm);
    Ql_DebugTrace("The timer starts.\r\n\r\n");
	    
    while(keepGoing)
    {
        Ql_GetEvent(&g_event);
        switch(g_event.eventType)
        {
            case EVENT_INTR:
            {
                // TODO: add your codes for interruption handling here 
                Intr_Event* pIntrEvt = (Intr_Event *)&g_event.eventData.intr_evt;
                Ql_DebugTrace("pinName = %d, pinState = %d", pIntrEvt->pinName, (u8)pIntrEvt->pinState);
                break;
            }
            case EVENT_KEY:
            {
                // TODO: add your codes for pressing key event here
				//Ql_DebugTrace("<-- EVENT_KEY -->\r\n");
                Key_Event* pKeyEvt = &g_event.eventData.key_evt;
                Ql_sprintf(notes, "\r\nEVENT_KEY: key_val(%x), isPressed=%d\r\n", pKeyEvt->key_val, pKeyEvt->isPressed);
                Ql_DebugTrace(notes);
                break;
            }
            case EVENT_UARTREADY:
            {
                // TODO: continue sending data to UART
				Ql_DebugTrace("<-- EVENT_UARTREADY -->\r\n");
                break;
            }
            case EVENT_UARTDATA:
            {
                // TODO: receive and handle data from UART

                /* The following code segment is written to demonstrate the usage of 'AT+QLOCPU'.
                *   If send 'AT+QLOCPU=0' through serial port tool, and reboot the device, only Core 
                *   System software is booted (the application will not be booted).
                */
                PortData_Event* pPortEvt = (PortData_Event*)&g_event.eventData.uartdata_evt;
                Ql_DebugTrace("EVENT_UARTDATA [PORT=%d]: %s\r\n", pPortEvt->port, pPortEvt->data);
                if (  Ql_strncmp((char*)pPortEvt->data, "AT+", 3) || Ql_strncmp((char*)pPortEvt->data, "AT", 2)
                    || Ql_strncmp((char*)pPortEvt->data, "at+", 3) || Ql_strncmp((char*)pPortEvt->data, "at", 2) 
                    )
                {// it's AT command, send to core.
                    Ql_SendToModem(ql_md_port1, pPortEvt->data, pPortEvt->len);
                }
                break;
            }
            case EVENT_MODEMDATA:
            {
                // TODO: receive and hanle data from CORE through virtual modem port
                PortData_Event* pPortEvt = (PortData_Event*)&g_event.eventData.modemdata_evt;
                Ql_DebugTrace("Modem Data at vPort [%d]: %s\r\n", pPortEvt->port, pPortEvt->data);
                break;
            }
            case EVENT_TIMER:
            {
                // TODO: specify what you want to happen when the interval for timer elapes

                //Ql_DebugTrace("The timer raises for %d time(s).\r\n", ++cnt);
                Ql_memset(notes, 0x0, Ql_strlen(notes));
                Ql_sprintf(notes, "The timer raises for %d time(s).\r\n", ++cnt);
                Ql_DebugTrace(notes);

                // Start the timer again
                if (cnt < 10)
                {
                    Ql_StartTimer(&tm);
                }
                else
                {
                    Ql_DebugTrace("\r\nThe timer stops.\r\n");
                }
                break;
            }
            case EVENT_SERIALSTATUS:
            {
                // TODO: specify what you want to happen when the status of CTS or DCD of serial port changes (now only for UART1)
                PortStatus_Event* pPortStatEvt = (PortStatus_Event *)&g_event.eventData.portstatus_evt;
                Ql_DebugTrace("Serial Status, port:%d, type:%d, value:%s.\r\n", pPortStatEvt->port, pPortStatEvt->type, pPortStatEvt->val);
                break;
            }
            case EVENT_MSG:
            {
                // TODO: specify what you want to happen when the message from other task arrives
				Ql_DebugTrace("<-- EVENT_MSG -->\r\n");
                break;
            }
            case EVENT_POWERKEY:
            {
                // TODO: specify what you want to happen when the power key is pressed down
                //Ql_DebugTrace("<-- EVENT_POWERKEY -->\r\n");
                Powerkey_Event* pPwrkeyEvt = &g_event.eventData.powerkey_evt;
                char* pwrKeyOn = "POWERKEY_ON";
                char* pwrKeyOff = "POWERKEY_OFF";
                if (POWERKEY_ON == pPwrkeyEvt->powerkey)
                {
                    Ql_sprintf(notes, "\r\nEVENT_KEY: POWERKEY_ON");
                }
                else
                {
                    Ql_sprintf(notes, "\r\nEVENT_KEY: POWERKEY_OFF");
                }
                if (pPwrkeyEvt->isPressed)
                {
                    Ql_sprintf(notes + Ql_strlen(notes), ", Press Key Down\r\n");
                }
                else
                {
                    Ql_sprintf(notes + Ql_strlen(notes), ", Release Key\r\n");
                }
                //Ql_SendToUart(ql_uart_port1, (u8*)buffer, Ql_strlen(buffer));  
                Ql_DebugTrace(notes);
                break;
            }
            case EVENT_HEADSET:
            {
                // TODO: specify what you want to happen when earphone is plugged in device or plugged out from device
				Ql_DebugTrace("<-- EVENT_HEADSET -->\r\n");
                break;
            }
            case EVENT_UARTESCAPE:
            {
                // TODO: specify what you want to happen when the Escape function happens
				Ql_DebugTrace("<-- EVENT_UARTESCAPE -->\r\n");
                break;
            }
            case EVENT_UARTFE:
            {
                // TODO: specify what you want to do when the EVENT_UARTFE event arrives
				Ql_DebugTrace("<-- EVENT_UARTFE -->\r\n");
                break;
            }
            
            default:
				Ql_DebugTrace("<-- Other event:%d... -->\r\n", g_event.eventType);
                break;
        }
    }
}

void sys_init_status_ind(s32 stat_val)
{
    if (SYS_STATE_ATOK == stat_val)
    {
        Ql_DebugTrace("<-- Sys init state: AT ok -->\r\n");
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
    }
    else if (SIMCARD_NOT_INSERTED == stat_val)
    {
        Ql_DebugTrace("<-- SIM: SIMCARD_NOT_INSERTED -->\r\n");
    }
    else if (SIMCARD_PIN_LOCKED == stat_val)
    {
        Ql_DebugTrace("<-- SIM: SIMCARD_PIN_LOCKED -->\r\n");
    }
    else if (SIMCARD_PUK_LOCKED == stat_val)
    {
        Ql_DebugTrace("<-- SIM: SIMCARD_PUK_LOCKED -->\r\n");
    }
    else if (SIMCARD_NOT_READY == stat_val)
    {
        Ql_DebugTrace("<-- SIM: SIMCARD_NOT_READY -->\r\n");
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
        default:
            Ql_DebugTrace("<-- urc type: %d -->\r\n", type);
            break;
    }
}

#endif // __EXAMPLE_HELLWORLD__

