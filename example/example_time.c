#ifdef __EXAMPLE_TIME__
/***************************************************************************************************
*   Example:
*       
*           TIME Routine
*
*   Description:
*
*           This example gives an example for time operation.
*           Through Uart port, input the special command, there will be given the response about time operation.
*
*   Usage:
*
*           Precondition:
*
*                   Use "make/make128 time" to compile, and download bin image to module.
*           
*           Through Uart port:
*
*               If input "s", that will set the current time.
*               If input "g", that will get the local time and convert to seconds.
*           
****************************************************************************************************/

#include "ql_type.h"
#include "ql_appinit.h"
#include "ql_trace.h"
#include "ql_interface.h"
#include "ql_fcm.h"
#include "ql_stdlib.h"
#include "ql_timer.h"

char textBuf[100];

#define DEBUG(...)  {\
	Ql_memset(textBuf,0,100);  \
	Ql_sprintf(textBuf,__VA_ARGS__);   \
	Ql_SendToUart(ql_uart_port1,(u8*)(textBuf),Ql_strlen(textBuf));}

QlEventBuffer signal;

void ql_entry(){
	u32 i;
	QlSysTimer time;
    bool bRet = FALSE;

	while(1){	
		Ql_GetEvent(&signal);
		switch(signal.eventType){
			case EVENT_UARTDATA:
				if(signal.eventData.uartdata_evt.data[0] == 's'){
					time.year = 11;
					time.month = 10;
					time.day = 26;
					time.hour = 17;
					time.minute = 30;
					time.second = 18;
					i = Ql_SetLocalTime(&time);
					DEBUG("Set local time result: %i(%i.%i.%i %i:%i:%i)\n\r", i, time.day, time.month, time.year, time.hour, time.minute, time.second);
				}
				else if(signal.eventData.uartdata_evt.data[0] == 'g'){
					if(Ql_GetLocalTime(&time)){
						DEBUG("Local time successfuly determined: %i.%i.%i %i:%i:%i = ", time.day, time.month, time.year, time.hour, time.minute, time.second);
						time.year += 2000;	
						i = Ql_Mktime(&time);
						DEBUG("%i\n\r", i);
						
                        i = Ql_LocalTime2CalendarTime(&time, 0);//The above years have been added 2000, so baseyear set to 0
                        DEBUG("%i\n\r", i);
						
                        bRet = Ql_CalendarTime2LocalTime(i, &time, 2000);
                        
                        if(FALSE == bRet)
                        {
                            DEBUG("Ql_CalendarTime2LocalTime failed !!\r\n");
                        }
                        else
                        {
                            DEBUG("Ql_CalendarTime2LocalTime: (%i.%i.%i %i:%i:%i)\r\n", time.day, time.month, time.year, time.hour, time.minute, time.second);
                        }
					}
					else
						DEBUG("Local time not determined");
				}
			break;
		}
	}
}

#endif
