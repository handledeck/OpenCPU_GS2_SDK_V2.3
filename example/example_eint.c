/***************************************************************************************************
*   Example:
*       
*           Eint Routine
*
*   Description:
*
*           This example gives an example for eint operation.
*           Through Main Uart port, input the special command, and the response message will be 
*           printed out through MAIN Uart port. The gpio used in this example is QL_PINNAME_DTR.
*           
*   Usage:
*
*           Compile & Run:
*
*                   Use "make/make128 eint" to compile, and download bin image to module.
*           
*           Through Uart port:
*
*               If input "1", that will subscribe QL_PINNAME_DTR pin to QL_PINMODE_1,it is DTR function.
*               If input "3", that will subscribe QL_PINNAME_DTR pin to QL_PINMODE_3, it is EINT function.
*               If input "u", that will unsubscribe QL_PINNAME_DTR pin.
*               If input "r", that will read pin level.
*           
****************************************************************************************************/
#ifdef __EXAMPLE_EINT__
  
#include "ql_appinit.h"
#include "ql_interface.h"
#include "ql_type.h"
#include "ql_trace.h"
#include "ql_audio.h"
#include "ql_timer.h"
#include "ql_stdlib.h"
#include "ql_pin.h"
#include "ql_fcm.h"


void ql_entry()
{
    bool           keepGoing = TRUE;
    QlEventBuffer  flSignalBuffer;
    s32 period = 10;
    char buffer[100];
    char *pData, *p;

    Ql_SetDebugMode(BASIC_MODE);   
    Ql_DebugTrace("eint: ql_entry\r\n");
    Ql_OpenModemPort(ql_md_port1);       

    while(keepGoing)
    {    
        Ql_GetEvent(&flSignalBuffer);
        switch(flSignalBuffer.eventType)
        {
            case EVENT_INTR:
            {
                //the EVENT will report when you execute command 3.
                Ql_sprintf(buffer, "\r\nEVENT_INTR=pinName(%d),pinState=%d\r\n",flSignalBuffer.eventData.intr_evt.pinName,flSignalBuffer.eventData.intr_evt.pinState);
                Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  
                break;
            }
            
            case EVENT_UARTDATA:
            {
                if (flSignalBuffer.eventData.uartdata_evt.len>0)
                {
                    s32              pin;
                    s32 mod = 0;
                    s32 pinpullenable = 0;
                    s32 pindirection = 0;
                    s32 pinlevel = 0;
                    s32 iret;

                    pData = (char*)flSignalBuffer.eventData.uartdata_evt.data;

                    //command-->select QL_PINNAME_DTR pin function to mode1, it is DTR function
                    p = Ql_strstr(pData,"1");
                    if (p)
                    {
                        iret = Ql_pinSubscribe(QL_PINNAME_DTR, QL_PINMODE_1, NULL);    
                        Ql_sprintf(buffer, "\r\nSubscribe(%d),pin=%d,mod=%d\r\n",iret,QL_PINNAME_DTR,QL_PINMODE_1);
                        Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  
                        break;
                    }
                    
                    //command-->select QL_PINNAME_DTR pin function to mode3, it is EINT function
                    p = Ql_strstr(pData,"3");
                    if (p)
                    {
                        QlPinParameter pinparameter;
                        pinparameter.pinconfigversion = QL_PIN_VERSION;
                        pinparameter.pinparameterunion.eintparameter.eintsensitivetype = QL_EINTSENSITIVETYPE_LEVEL;
                        pinparameter.pinparameterunion.eintparameter.hardware_de_bounce = 10; //10 ms
                        pinparameter.pinparameterunion.eintparameter.software_de_bounce = 0; // unit is ms , max is 2559
                        iret = Ql_pinSubscribe(QL_PINNAME_DTR, QL_PINMODE_3, &pinparameter); 
                        Ql_sprintf(buffer, "\r\nSubscribe(%d),pin=%d,mod=%d\r\n",iret,QL_PINNAME_DTR,QL_PINMODE_3);
                        Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  
                        break;
                    }

                    //command-->unsubscribe
                    p = Ql_strstr(pData,"u");
                    if (p)
                    {
                        iret = Ql_pinUnSubscribe(QL_PINNAME_DTR);
                        Ql_sprintf(buffer, "\r\nUnSubscribe(%d),pin=%d\r\n",iret,QL_PINNAME_DTR);
                        Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  
                        break;
                    }
                    
                    //command-->read pin level
                    p = Ql_strstr(pData,"r");
                    if (p)
                    {
                        QlPinLevel pinlevel;
                        iret = Ql_pinRead(QL_PINNAME_DTR, &pinlevel);
                        Ql_sprintf(buffer, "\r\nRead(%d),pin=%d,level=%d\r\n",iret,QL_PINNAME_DTR,pinlevel);
                        Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  
                        break;
                    }  
                }
                break;            
            }
            
            default:
                break;
        }
    }
}

#endif // __EXAMPLE_EINT__

