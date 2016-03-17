#ifdef __EXAMPLE_ALARM__
/***************************************************************************************************
*   Example:
*       
*           ALARM Routine
*
*   Description:
*
*           This example gives an example for ALARM operation.it use to start up module when time coming.
*           Through Uart port, input the special command, there will be given the response about module operation.
*
*   Usage:
*
*           Precondition:
*
*                   Use "make/make128 ALARM" to compile, and download bin image to module.
*           
*           Through Uart port:
*
*               If input "Ql_GetLocalTime", that will retrieves the current local date and time.
*               If input "Ql_SetLocalTime", that will set the current local date and time.
*               If input "Ql_Alarm_StartUp", that will set the alarm start time and repeat type.
*               If input "Ql_PowerDown", that will switch off the power of the module.
*               If input "Ql_Alarm_Remove".that will switch off alarm function.
*       
****************************************************************************************************/
#include "ql_type.h"
#include "ql_appinit.h"
#include "ql_trace.h"
#include "ql_interface.h"
#include "ql_fcm.h"
#include "ql_stdlib.h"
#include "ql_timer.h"

QlSysTimer SetTime( u8 *str)
{
    s8 *p = NULL;
    s8 *q = NULL;
	QlSysTimer SysTime;
                        // Year
	p = Ql_strstr(str, "=");
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
    p = Ql_strstr(q, "#");
    if(p)
    {
        (*p)=0;
        SysTime.second = Ql_atoi(q);
    }else
    {
        SysTime.second = Ql_atoi(q);
    }
    
    return SysTime;
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
    QlEventBuffer signal;

	Ql_SetDebugMode(BASIC_MODE);
    //Ql_SetDebugMode(ADVANCE_MODE);
    
	Ql_DebugTrace("alarm: ql_entry\r\n");
    Ql_SetPortOwner(ql_md_port1, ql_main_task);
	Ql_OpenModemPort(ql_md_port1);
    
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
                    
                    /*CMD Format:Ql_SetLocalTime=year,month,day,hour,minute,second */
					p = Ql_strstr(pData, "Ql_SetLocalTime=");
                    if(p)
                    {
						Ql_memset(&SysTime, 0, sizeof(SysTime));
                        SysTime = SetTime((u8 *)pData);

						bRet = Ql_SetLocalTime(&SysTime);
						Ql_DebugTrace("\r\nQl_SetLocalTime(bRet = %d)", bRet);
                        break;
                    }

                    /*Ql_GetLocalTime*/
                    p = Ql_strstr(pData, "Ql_GetLocalTime");
                    if(p)
                    {
						Ql_memset(&SysTime, 0, sizeof(SysTime));
						Ql_GetLocalTime(&SysTime);
						Ql_DebugTrace("\r\nCurrent Time :\r\n%d/%d/%d %d:%d:%d\r\n",SysTime.year, SysTime.month, SysTime.day, SysTime.hour, SysTime.minute, SysTime.second);
                        break;
                    }
                    
                    //Ql_Alarm_StartUp=year,month,day,hour,minute,second#repeattype
                    p = Ql_strstr(pData, "Ql_Alarm_StartUp=");
                    if(p)
                    {
                        u32 ret;
                        s32 recurr;
                        s8 buffer[30];
                        Ql_strcpy(buffer,pData);

						Ql_memset(&SysTime, 0, sizeof(SysTime));
                        SysTime = SetTime((u8 *)pData);

                        p = Ql_strstr(buffer, "#");
                        p = p + 1;
                        recurr = Ql_atoi(p);
                        ret = Ql_Alarm_StartUp(&SysTime,(u8)recurr);
                        Ql_DebugTrace("\r\nSet Alarm :\r\n%d/%d/%d %d:%d:%d,recurr=%d,ret=%d\r\n",SysTime.year, SysTime.month,
                                                            SysTime.day, SysTime.hour, SysTime.minute, SysTime.second,recurr,ret);
                         break;
                    }
                    
                    //Ql_PowerDown=0(1)
                    p = Ql_strstr(pData, "Ql_PowerDown=");
                    if(p)
                    {
                        s32 powertype;
                        p = Ql_strstr(pData, "=");
                        p = p + 1;
                        powertype = Ql_atoi(p);
                        Ql_PowerDown((u8)powertype);
                        Ql_DebugTrace("powertype=%d\r\n",powertype);
                         break;
                        
                     }


                    //Ql_Alarm_Remove
                    p = Ql_strstr(pData, "Ql_Alarm_Remove");
                    if(p)
                    {
                        
                        u32 ret;
                        ret = Ql_Alarm_Remove();
                        
                        Ql_DebugTrace("Ql_Alarm_Remove=%d\r\n",ret);
                         break;
                     }

				}
				break;
			}
			case EVENT_TIMER:
			{
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


