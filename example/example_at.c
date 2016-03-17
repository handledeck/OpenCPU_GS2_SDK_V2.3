#ifdef __EXAMPLE_AT__
 /***************************************************************************************************
 *	 Example:
 *		 
 *			 AT Routine
 *
 *	 Description:
 *
 *			 This example demonstrates how to use AT function with APIs in OpenCPU.
 *			 Through MAIN Uart port, input the specified command, and the response message will be 
 *			 printed out through MAIN Uart port or debug port.

 *
 *	 Usage:
 *
 *			 Precondition:
 *
 *					 Use "make AT" to compile, and download bin image to module.
 *			 
 *			 Through Uart port:
 *                              
 *	Note:
 *			 run the routine and observe the message from debug port.
 *
 ****************************************************************************************************/
 
#include "ql_appinit.h"
#include "ql_interface.h"
#include "ql_type.h"
#include "ql_trace.h"
#include "ql_audio.h"
#include "ql_timer.h"
#include "ql_stdlib.h"
#include "ql_trace.h"
#include "ql_fcm.h"

typedef enum tagATCmdType {
    AT_General,
    AT_CPIN,
    AT_CSQ,
    AT_CREG,
    AT_CGREG
}ATCmdType;

u8      g_cmd_type;
u32     g_cmd_idx;
bool    g_bDoNexAT = TRUE;
QlPort  g_AT_vPort = ql_md_port1;
char    g_buffer[100];
void QuerySysState(void)
{
    switch (g_cmd_idx)
    {
    case 1:// Echo mode off
        Ql_sprintf((char *)g_buffer, "ATE0\n");
        g_cmd_type = AT_General;
        break;
        
    case 2:// Query the FW version of core
        Ql_sprintf((char *)g_buffer, "ATI\n");
        g_cmd_type = AT_General;
        break;
        
    case 3:// Query the state of SIM card
        Ql_sprintf((char *)g_buffer, "AT+CPIN?\n");
        g_cmd_type = AT_CPIN;
        break;
        
    case 4:// Query GSM network state
        Ql_sprintf((char *)g_buffer, "AT+CREG?\n");
        g_cmd_type = AT_CREG;
        break;
        
    case 5:// Query GPRS network state
        Ql_sprintf((char *)g_buffer, "AT+CGREG?\n");
        g_cmd_type = AT_CGREG;
        break;
        
    case 6:// Query CSQ value
        Ql_sprintf((char *)g_buffer, "AT+CSQ\n");
        g_cmd_type = AT_CSQ;
        break;
        
    case 7:// Set audio channel
        Ql_sprintf((char *)g_buffer, "AT+QAUDCH=1\n");
        g_cmd_type = AT_General;
        g_bDoNexAT = FALSE;
        break;

    default:
        Ql_DebugTrace("at commands finished.\r\n");
        g_cmd_type = AT_General;
        return;
    }
  Ql_DebugTrace((char *)g_buffer);
  Ql_SendToModem(g_AT_vPort, (u8*)g_buffer, Ql_strlen(g_buffer));
}
    

QlEventBuffer  g_event; // 1K size
void ql_entry()
{
    bool keepGoing = TRUE;
    s32  period = 10;
    QlTimer  timer;
    QlTimer  qryTimer;
    u8       qryCnt = 0;

    Ql_DebugTrace("OpenCPU: AT Example \r\n\rn");
    
    Ql_SetDebugMode(BASIC_MODE);   
    Ql_OpenModemPort(g_AT_vPort);   
    qryTimer.timeoutPeriod = Ql_SecondToTicks(1);
        
    while(keepGoing)
    {    
        Ql_GetEvent(&g_event);
        switch(g_event.eventType)
        {
            case EVENT_TIMER:
            {
                Timer_Event* pTimerEvt = (Timer_Event*)&g_event.eventData.modemdata_evt;
                if (qryTimer.timerId == pTimerEvt->timer_id)
                {
                    qryCnt++;
                    if (qryCnt >= 10)
                    {
                        g_cmd_idx++;
                    }
                    QuerySysState();
                }
                else if (timer.timerId == pTimerEvt->timer_id)
                {
                    Ql_sprintf((char *)g_buffer,"\r\nTimer expired\r\n");
                    Ql_SendToUart(g_AT_vPort, (u8*)g_buffer,Ql_strlen(g_buffer));

                    // Start to work...
                    g_cmd_idx = 1;
                    QuerySysState();
                }

                break;
            }

            case EVENT_MODEMDATA:
            {
                PortData_Event* pPortEvt = (PortData_Event*)&g_event.eventData.modemdata_evt;
                if (DATA_AT == pPortEvt->type)
                {
                    Ql_DebugTrace("%s\r\n", pPortEvt->data);
                    if (Ql_strstr((char *)pPortEvt->data, "Call Ready") != NULL)
                    {
                        Ql_SendToUart(g_AT_vPort, pPortEvt->data, pPortEvt->len);
                        
                        // Wait 2s for the stable signal quality
                        timer.timeoutPeriod = Ql_SecondToTicks(2);
                        Ql_StartTimer(&timer);
                    }
                    else if ((AT_CSQ ==   g_cmd_type && Ql_strstr((char*)pPortEvt->data,"+CSQ:") != NULL)
                           || (AT_CPIN ==  g_cmd_type && Ql_strstr((char*)pPortEvt->data, "+CPIN:")   != NULL)
                           || (AT_CREG ==  g_cmd_type && Ql_strstr((char*)pPortEvt->data, "+CREG:") != NULL)
                           || (AT_CGREG == g_cmd_type && Ql_strstr((char*)pPortEvt->data, "+CGREG:")   != NULL)
                           )
                    {
                        if ( (AT_CREG ==  g_cmd_type && Ql_strstr((char*)pPortEvt->data, "+CREG: 0,1") == NULL)
                            ||(AT_CGREG ==  g_cmd_type && Ql_strstr((char*)pPortEvt->data, "+CGREG: 0,1") == NULL)
                            )
                        {
                            Ql_StartTimer(&qryTimer);
                        }else{                        
                            g_cmd_idx++;
                            g_bDoNexAT = TRUE;
                            QuerySysState();
                        }
                    }
                    else if ((  Ql_strstr((char*)pPortEvt->data, "\r\nOK") != NULL 
                              || Ql_strstr((char*)pPortEvt->data, "OK\r\n") != NULL 
                              || Ql_strstr((char*)pPortEvt->data, "ERROR") != NULL)
                            && g_bDoNexAT != FALSE)
                    {
                        g_cmd_idx++;
                        QuerySysState();
                    }
                    else if ((Ql_strstr((char*)pPortEvt->data, "ERROR") != NULL)
                            && g_bDoNexAT != FALSE)
                    {
                        g_bDoNexAT = FALSE;
                    }
                }
                break;
            }

            case EVENT_UARTDATA:
            {
                PortData_Event* pPortEvt = (PortData_Event*)&g_event.eventData.modemdata_evt;
                Ql_DebugTrace("EVENT_UARTDATA [PORT=%d]: %s\r\n", pPortEvt->port, pPortEvt->data);
                if (  Ql_strncmp((char*)pPortEvt->data, "AT+", 3) || Ql_strncmp((char*)pPortEvt->data, "AT", 2)
                    || Ql_strncmp((char*)pPortEvt->data, "at+", 3) || Ql_strncmp((char*)pPortEvt->data, "at", 2) 
                    )
                {// it's AT command, send to core.
                    Ql_SendToModem(g_AT_vPort, pPortEvt->data, pPortEvt->len);
                }
                break;            
            }
            
            default:
                break;
        }
    }
}

#endif //__EXAMPLE_AT__

