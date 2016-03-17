#ifdef __EXAMPLE_TRANSPASS__
/***************************************************************************************************
*   Example:
*       
*           TRANSPASS Routine
*
*   Description:
*
*           This example gives an example for transpass setting.
*           Through Uart port, input the special command, there will be given the response about transpass operation.
*           
*
*   Usage:
*
*           Precondition:
*
*                   Use "make/make128 transpass" to compile, and download bin image to module.
*           
*           Through Uart port:
*                   Sending data from main port that will transmit the data to modem,when the modem received it,it will be print out to debug port. 
*
*           
****************************************************************************************************/  
#include "ql_appinit.h"
#include "ql_interface.h"
#include "ql_type.h"
#include "ql_trace.h"
#include "ql_audio.h"
#include "ql_timer.h"
#include "ql_stdlib.h"
#include "ql_fcm.h"


void ql_entry()
{
    bool           keepGoing = TRUE;
    QlEventBuffer  flSignalBuffer;
    s32 period = 10;

    Ql_SetDebugMode(BASIC_MODE);   
    Ql_DebugTrace("transpass: ql_entry\r\n");
    Ql_OpenModemPort(ql_md_port1);       

    while(keepGoing)
    {    
        Ql_GetEvent(&flSignalBuffer);
        switch(flSignalBuffer.eventType)
        {
            case EVENT_MODEMDATA:
            {                
                Ql_DebugTrace("\r\nEVENT_MODEMDATA type=%d \r\n",flSignalBuffer.eventData.modemdata_evt.type);
                Ql_DebugTrace("\r\nmodemdata =%s\r\n",flSignalBuffer.eventData.uartdata_evt.data);    
                //if (flSignalBuffer.eventData.modemdata_evt.type==DATA_AT)
                Ql_SendToUart(ql_uart_port1,(u8*)flSignalBuffer.eventData.uartdata_evt.data,flSignalBuffer.eventData.uartdata_evt.len);    
                break;
            }

            case EVENT_UARTDATA:
            {            
                Ql_DebugTrace("\r\nEVENT_UARTDATA PORT=%d\r\n",flSignalBuffer.eventData.uartdata_evt.port);
                Ql_DebugTrace("\r\nuart data =%s\r\n",flSignalBuffer.eventData.uartdata_evt.data);    
                Ql_SendToModem(ql_md_port1,(u8*)flSignalBuffer.eventData.uartdata_evt.data,flSignalBuffer.eventData.uartdata_evt.len);    
                break;            
            }
            
            default:
                break;
        }
    }
}

#endif // __EXAMPLE_TRANSPASS__

