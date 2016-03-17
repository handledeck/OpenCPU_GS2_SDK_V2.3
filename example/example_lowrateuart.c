#ifdef __EXAMPLE_LOWRATEUART__
/***************************************************************************************************
*   Example:
*       
*           LOWRATEUART Routine
*
*   Description:
*
*           This example gives an example for lowrateuart operation.
*           
*
*   Usage:
*
*           Precondition:
*
*                   Use "make/make128 lowrateuart" to compile, and download bin image to module.
*           
*           Through Uart port:
*			 Running the routine that will print out some informations from debug port.
*           
****************************************************************************************************/

#include "ql_appinit.h"
#include "ql_interface.h"
#include "ql_type.h"
#include "ql_trace.h"
#include "ql_audio.h"
#include "ql_timer.h"
#include "ql_stdlib.h"
#include "ql_pin.h"
#include "Ql_multitask.h"
#include "Ql_tcpip.h"
#include "ql_fcm.h"


u8 data[500];

QlEventBuffer    flSignalBuffer; // Set flSignalBuffer to global variables  may as well, otherwise it will occupy stack space
void ql_entry()
{
    bool keepGoing = TRUE;
    s32 ret;
    QlTimer timer;

    Ql_SetDebugMode(BASIC_MODE);   
    Ql_DebugTrace("lowrateuart: ql_entry\r\n");

    timer.timeoutPeriod = Ql_MillisecondToTicks(300);
    Ql_StartTimer(&timer);

    Ql_SetUartBaudRate(ql_uart_port1,1200);
	
    while(keepGoing)
    {    
        Ql_GetEvent(&flSignalBuffer);
        switch(flSignalBuffer.eventType)
        {
            case EVENT_TIMER:
            {
                if( timer.timerId  == flSignalBuffer.eventData.timer_evt.timer_id)
                {
                    u32 total;
                    u32 rest_size;

                    timer.timeoutPeriod = Ql_MillisecondToTicks(300); 
                    Ql_StartTimer(&timer); 

                    Ql_DebugTrace("Ql_UartGetBytesAvail()=%d,Ql_UartGetTxRoomLeft()=%d",Ql_UartGetBytesAvail(ql_uart_port1),Ql_UartGetTxRoomLeft(ql_uart_port1));
                    rest_size = Ql_UartGetTxRestBytes(ql_uart_port1, &total);
                    Ql_DebugTrace("Ql_UartGetTxRestBytes()=%d,rest_size=%d",total,rest_size);

                    /*tx buffer is empty*/
                    if(rest_size == 0)
                    {
                        Ql_memset((void*)data, (u8)'A', 500);
                        Ql_SendToUart(ql_uart_port1, data, 500);        
                    }
                }
                break;
            }
            
            default:
                Ql_DebugTrace("eventType=%d",flSignalBuffer.eventType);
                break;
        }
    }
}

#endif // __EXAMPLE_LOWRATEUART__

