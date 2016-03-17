/***************************************************************************************************
*   Example:
*       
*            电源有关的API
*
*   Description:
*
*         
*
*   Usage:
*
*           Precondition:
*
*                   Use "make/make128 power" to compile, and download bin image to module.
*           
*           Through Uart port:
*
*            
* Author: Ramos.Zhang         
****************************************************************************************************/
#ifdef __EXAMPLE_POWERKEY__
#include "ql_appinit.h"
#include "ql_interface.h"
#include "ql_type.h"
#include "ql_trace.h"
#include "ql_stdlib.h"

void Ql_pwrKey_callback(s32 cause);
void Ql_volInd_callback(s32 cause);
void ql_entry()		// entry for main task
{
    bool           keepGoing = TRUE;
    QlEventBuffer  flSignalBuffer;
    char *pData, *p;
    char buffer[100];

    Ql_SetDebugMode(BASIC_MODE);	// set working mode of UART2 to BASIC_MODE
    Ql_DebugTrace("Power: ql_entry\r\n");
    Ql_OpenModemPort(ql_md_port1);  //open virtual modem serial port ql_md_port1
    Ql_PwrKey_Init((OCPU_CB_PWRKEY_IND)Ql_pwrKey_callback);
    Ql_VoltageInd_Init((OCPU_CB_VOL_IND)Ql_volInd_callback);
    while(keepGoing)
    {    
        Ql_GetEvent(&flSignalBuffer);		// Get event from core system. When there is no event, 
        switch(flSignalBuffer.eventType)
        {
            case EVENT_UARTDATA:
            {
                pData = flSignalBuffer.eventData.uartdata_evt.data;
                Ql_DebugTrace("UART data %s\r\n",pData);

                break;
            }
            default :
                break;
        }
    }
}
//20120509 Ramos.Zhang
void Ql_pwrKey_callback(s32 cause)
{

   Ql_DebugTrace("\r\nQl_pwrKey_callback Read(%d)\r\n",cause);
   // 1:PowerKEY pressed  0:PowerKEY release
   if(1 == cause)
   {
        Ql_DebugTrace("\r\nPower KEY Pressed !!!!\r\n");
   }
   else if(0 == cause)
   {
        Ql_DebugTrace("\r\nPower KEY Released !!!!\r\n");
   }
}
void Ql_volInd_callback(s32 cause)
{

    Ql_DebugTrace("\r\nQl_volInd_callback Read(%d)\r\n",cause);
}

#endif //__EXAMPLE_POWER__