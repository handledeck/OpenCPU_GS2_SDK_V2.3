#ifdef __EXAMPLE_NEWAPI__
/***************************************************************************************************
*   Example:
*       
*           new API Routine
*   
*   Description:
*
*            used for new API test 和OpenCPU 修改点测试
*           
*
*   Usage:
*
*           Precondition:
*
*                   Use "make newAPI" to compile, and download bin image to module.
*
*           
****************************************************************************************************/
#include "ql_appinit.h"
#include "ql_interface.h"
#include "ql_type.h"
#include "ql_timer.h"
#include "ql_stdlib.h"
#include "ql_fcm.h"
#include "ql_trace.h"
#include "ql_api_type.h"

QlEventBuffer g_event; // Keep this variable a global variable due to its big size
char notes[100]={0x0};

void Ql_volInd_callback(s32 cause)
{

    Ql_DebugTrace("\r\nQl_volInd_callback Read(%d)\r\n",cause);
}
void  callback_cfun_switch(s32 cause)
{
    Ql_DebugTrace("OCPU: cfun switch: %d\r\n", cause);
}


void ql_entry(void)
{
    s32 ret;
    bool keepGoing = TRUE;
    QlTimer tm;
    u32 cnt = 0; 
    char *pChar;
    // Start a timer
    tm.timeoutPeriod = Ql_SecondToTicks(2); /* Set the interval of timer */
        
    Ql_SetDebugMode(BASIC_MODE);    /* Only after the device reboots, */
    Ql_DebugTrace("OpenCPU:New API test!\r\n\r\n");   
    Ql_OpenModemPort(ql_md_port1);  /* or ql_md_port2, two virtual modem ports are available.*/
    Ql_VoltageInd_Init((OCPU_CB_VOL_IND)Ql_volInd_callback); //测试Ql_VoltageInd_Init函数，电压在几个门槛电压时会回调Ql_volInd_callback
    while(keepGoing)
    {
        Ql_GetEvent(&g_event);
        switch(g_event.eventType)
        {
            case EVENT_UARTDATA:
            {
                PortData_Event* pDataEvt = (PortData_Event*)&g_event.eventData.uartdata_evt;
           // 测试Ql_WDTMF() API
                pChar = Ql_strstr(pDataEvt->data,"Ql_WDTMF");
                if(pChar)
                {
                  ret = Ql_WDTMF(7,7,"123456789122456789954122,50,50,aaaaa,60,60",52);
                  Ql_DebugTrace("Ql_WDTMF return %d\r\n",ret);
                  break;
                }
          // start timer for Ql_ReadModuleUID test
                pChar= Ql_strstr(pDataEvt->data,"Module_UID");
                if(pChar)
                {
                    Ql_StartTimer(&tm);
                }
          // 在串口输入Ql_SetSleepMode来测试 Ql_SetSleepMode()函数
                pChar= Ql_strstr(pDataEvt->data,"Ql_SetSleepMode");
                if(pChar)
                {
                    s32 ret;
                    ret = Ql_SetSleepMode(1);
                    Ql_DebugTrace("Ql_SetSleepMode(1)=%d\r\n", ret);
                    break;
                }
                
           // cfun switch  Ql_SetCFUN
                pChar = Ql_strstr(pDataEvt->data, "cfun=");
                if (pChar)
                {
                    pChar = Ql_strstr((char*)pDataEvt->data, "=");
                    if (pChar)
                    {
                        char* p2;
                        u8 cfun;
                        
                        p2 = Ql_strstr((char*)pDataEvt->data, "\r\n");
                        Ql_memcpy(notes, pChar + 1, p2 - pChar -1);
                        cfun = Ql_atoi(notes);
                        
                        ret = Ql_SetCFUN(cfun, callback_cfun_switch);
                        Ql_DebugTrace("Ql_SetCFUN(%d)=%d\r\n", cfun, ret);
                    }
                    break;
                }
             //cfun get
                pChar = Ql_strstr(pDataEvt->data, "Ql_GetCFUN");
                if(pChar)
                {
                    s32 ret;
                    ret = Ql_GetCFUN();
                    Ql_DebugTrace("Cfun=%d\r\n", ret);
                    break;
                }
                
               // power supply
                pChar = Ql_strstr(pDataEvt->data, "Ql_GetPowerSupply");
                if (pChar)
                {
                    u8 chgStat;
                    u32 cap;
                    u32 vol;
                    ret = Ql_GetPowerSupply(&chgStat, &cap, &vol);
                    Ql_DebugTrace("Ql_GetPowerSupply()=%d: chgStat=%d,cap=%d,vol=%d\r\n", ret, chgStat, cap, vol);
                    break;
                }
             
                break;
            }
            case EVENT_TIMER:
            {
                u16 md_uid[20] = {0x0};
                u16 len, i = 0;
                u8* pUid;
                //Ql_DebugTrace("The timer raises for %d time(s).\r\n", ++cnt);
                Ql_memset(notes, 0x0, Ql_strlen(notes));
                len = Ql_ReadModuleUID(md_uid);
                Ql_sprintf(notes, "Module UID: ");
                Ql_DebugTrace(notes);
                Ql_memset(notes, 0x0, Ql_strlen(notes));
                pUid = (u8*)md_uid;
                for (i = 0; i < len; i++)
                {
                    Ql_sprintf(notes + i*2, "%02X", *pUid);
                    pUid++;
                }
                Ql_DebugTrace(notes);
                if (cnt < 10)
                {
                    Ql_StartTimer(&tm);
                }
                else
                {
                    Ql_StopTimer(&tm);
                    Ql_DebugTrace("\r\nThe timer stops.\r\n");
                }
                break;
            }
            default:
                break;
        }

    }  

}


#endif
