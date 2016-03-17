#ifdef __EXAMPLE_TIMER__
/***************************************************************************************************
*   Example:
*       
*           TIMER Routine
*
*   Description:
*
*           This example gives an example for timer operation.
*           Through Uart port, input the special command, there will be given the response about timer operation.
*
*   Usage:
*
*           Precondition:
*
*                   Use "make/make128 timer" to compile, and download bin image to module.
*           
*           Through Uart port:
*
*               If input "Ql_StartTimer", that will start timer.
*               If input "Ql_StopTimer", that will stop timer.
*               If input "Ql_GetRelativeTime_Counter", that will get the number of MCU counters since the device booted.
*               If input "Ql_GetRelativeTime", that will get the number of milliseconds since the device booted.
*               If input "Ql_GetLocalTime", that will retrieves the current local date and time.
*               If input "Ql_SetLocalTime", that will set the current local date and time.
*               If input "Ql_Mktime", that will get total seconds elapsed since 1970.01.01 00:00:00.
*       
****************************************************************************************************/
#include "ql_type.h"
#include "ql_appinit.h"
#include "ql_trace.h"
#include "ql_interface.h"
#include "ql_fcm.h"
#include "ql_stdlib.h"
#include "ql_timer.h"

char textBuf[512];

#define DEBUG(...)  {\
	Ql_memset(textBuf,0,512);  \
	Ql_sprintf(textBuf,__VA_ARGS__);   \
	Ql_SendToUart(ql_uart_port1,(u8*)(textBuf),Ql_strlen(textBuf));}

QlEventBuffer signal;

static u64 s_u64PreRelativerTime = 0;
static u64 s_u64RelativeTime = 0;
static u64 s_u64PreRelativerTime_Counter = 0;
static u64 s_u64RelativeTime_Counter = 0;

s32 g_param = 2012;
void hwTimer_handler(void* param)
{
    if (param != NULL)
    {
        DEBUG("<-- hwTimer_handler, param:%d -->\r\n", *((s32*)param));
    }else{
        DEBUG("<-- hwTimer_handler, no param -->\r\n");
    }
    Ql_StartGPTimer(300, hwTimer_handler, &g_param);   // loop the timer
}

void ql_entry()
{
	char *pData = NULL;
	char *p = NULL;
	char *q = NULL;
	
	QlSysTimer SysTime;
	QlTimer MyTimer;
	u32 uiIntervalx=0;
	u32 uiIntervaly=0;
	u32 uiRet = 0;
	bool bRet = 0;

	Ql_SetDebugMode(BASIC_MODE);
	Ql_DebugTrace("timer: ql_entry\r\n");
    Ql_SetPortOwner(ql_md_port1, ql_main_task);
	Ql_OpenModemPort(ql_md_port1);

	uiIntervalx = Ql_MillisecondToTicks(3000);
    uiIntervaly = Ql_SecondToTicks(3);
    bRet = Ql_StartGPTimer(300, hwTimer_handler, &g_param);
    DEBUG("\r\nQl_StartGPTimer(300,cb)=%d\r\n", bRet);

	DEBUG("\r\nuiIntervalx=%d, uiIntervaly=%d\r\n", uiIntervalx, uiIntervaly);
	if(uiIntervalx != uiIntervaly)
	{
		DEBUG("\r\n Ql_XToTicks is Bad! \r\n");
	}
	else
	{
		DEBUG("\r\n Ql_XToTicks is OK! \r\n");
	}

	MyTimer.timeoutPeriod = uiIntervalx;
	
	while(1)
	{	
		Ql_GetEvent(&signal);
		switch(signal.eventType)
		{
			case EVENT_UARTDATA:
			{
				if(signal.eventData.uartdata_evt.len > 0)
				{
					pData = (char *)signal.eventData.uartdata_evt.data;

					if(NULL != (p = Ql_strstr(pData, "Ql_StartTimer")))
					{
						Ql_StartTimer(&MyTimer);
						DEBUG("\r\nStart Timer successfully!\r\n");
					}
					else if(NULL != (p = Ql_strstr(pData, "Ql_StopTimer")))
					{
						Ql_StopTimer(&MyTimer);
						DEBUG("\r\nStop Timer successfully!\r\n");
					}
					else if(NULL != (p = Ql_strstr(pData, "Ql_GetRelativeTime_Counter")))
					{
						s_u64RelativeTime_Counter = Ql_GetRelativeTime_Counter();
						DEBUG("\r\ns_u64RelativeTime_Counter = %ld\r\n", s_u64RelativeTime_Counter);
						if(s_u64PreRelativerTime_Counter != 0)
						{
							DEBUG("\r\nus_u64RelativerTime_Counter - s_u64PreRelativerTime_Counter = %ld\r\n", 
								s_u64RelativeTime_Counter - s_u64PreRelativerTime_Counter);
						}
						else 
						{
							DEBUG("\r\nFirst Time To Run Ql_GetRelativerTime_Counter\r\n");
						}
						s_u64PreRelativerTime_Counter =  s_u64RelativeTime_Counter;
					}
					else if(NULL != (p = Ql_strstr(pData, "Ql_GetRelativeTime")))
					{
						s_u64RelativeTime = Ql_GetRelativeTime();
						DEBUG("\r\ns_u64RelativeTime = %ld\r\n", s_u64RelativeTime);
						if(s_u64PreRelativerTime != 0)
						{
							DEBUG( "\r\nus_u64RelativerTime - s_u64PreRelativerTime = %ld\r\n", s_u64RelativeTime - s_u64PreRelativerTime);
						}
						else 
						{
							DEBUG("\r\nFirst Time To Run Ql_GetRelativeTime\r\n");
						}
						s_u64PreRelativerTime =  s_u64RelativeTime;
					}
					else if(NULL != (p = Ql_strstr(pData, "Ql_GetLocalTime")))
					{
						bRet = Ql_GetLocalTime(&SysTime);
						DEBUG("\r\nQl_GetLocalTime(bRet = %d)", bRet);
						if(bRet == TRUE)
						{
							DEBUG("\r\ny-m-d h:m:s=%d-%d-%d %d:%d:%d\r\n",SysTime.year, SysTime.month, SysTime.day, SysTime.hour, SysTime.minute, SysTime.second);
						}
						else
						{
							DEBUG("\r\nQl_GetLocalTime -> Failed!\r\n");
						}
					}                                    /*CMD Format:Ql_SetLocalTime=year,month,day,hour,minute,second */
					else if(NULL != (p = Ql_strstr(pData, "Ql_SetLocalTime=")))
					{
						Ql_memset(&SysTime, 0, sizeof(SysTime));
											  // Year
						p = Ql_strstr(pData, "=");
						p++;
						q = Ql_strstr(p, ",");
						(*q) = 0;
						SysTime.year = Ql_atoi(p);
											// Month
						q++;
						p = Ql_strstr(q, ",");
						(*p) = 0;
						SysTime.month = Ql_atoi(q);
										   // Day
						p++;
						q = Ql_strstr(p, ",");
						(*q)=0;
						SysTime.day = Ql_atoi(p);
                                           // Hour
						q++;
						p = Ql_strstr(q, ",");
						(*p)=0;
						SysTime.hour = Ql_atoi(q);
										   // Minute
						p++;
						q = Ql_strstr(p, ",");
						(*q)=0;
						SysTime.minute = Ql_atoi(p);
                                           // Second
						q++;
						SysTime.second = Ql_atoi(q);
						bRet = Ql_SetLocalTime(&SysTime);
						DEBUG("\r\nQl_SetLocalTime(bRet = %d)", bRet);

										// Get time to conform
						Ql_memset(&SysTime, 0, sizeof(SysTime));
						Ql_GetLocalTime(&SysTime);
						DEBUG("\r\nFollow is Current Time :\r\ny-m-d h:m:s=%d-%d-%d %d:%d:%d\r\n",SysTime.year, SysTime.month, SysTime.day, SysTime.hour, SysTime.minute, SysTime.second);
					}
					else if(NULL != (p = Ql_strstr(pData, "Ql_Mktime")))
					{
						Ql_memset(&SysTime, 0, sizeof(SysTime));
						Ql_GetLocalTime(&SysTime);
						SysTime.year += 2000;
						uiRet = Ql_Mktime(&SysTime);
						DEBUG("\r\nQl_Mktime(uiRet = %d)\r\n", uiRet);
					}
					else
					{
						Ql_SendToModem(ql_md_port1, (u8 *)signal.eventData.uartdata_evt.data, signal.eventData.uartdata_evt.len);
					}
				}
				break;
			}
			case EVENT_TIMER:
			{
				if(signal.eventData.timer_evt.timer_id == MyTimer.timerId)
				{
					DEBUG("\r\nTimer is going...\r\n");
					Ql_StartTimer(&MyTimer);
				}
				break;
			}
			case EVENT_MODEMDATA:
			{
				Ql_SendToUart(ql_uart_port1, (u8 *)signal.eventData.modemdata_evt.data, signal.eventData.modemdata_evt.len);
				break;
			}
			default :
				break;
		}
	}
}

#endif
