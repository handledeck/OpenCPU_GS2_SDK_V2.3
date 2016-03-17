/***************************************************************************************************
*   Example:
*       
*           GPIO Routine
*
*   Description:
*
*           This example gives an example for gpio setting.
*           Through Uart port, input the special command, there will be given the response about the gpio state.
*           The gpio used in this example is QL_PINNAME_NETLIGHT
*
*   Usage:
*
*           Precondition:
*
*                   Use "make/make128 GPIO" to compile, and download bin image to module.
*           
*           Through Uart port:
*
*               If input "1", that will subscribe QL_PINNAME_NETLIGHT pin to QL_PINMODE_1.
*               If input "2", that will subscribe QL_PINNAME_NETLIGHT pin to QL_PINMODE_2.
*               If input "u", that will unsubscribe QL_PINNAME_NETLIGHT pin.
*               If input "l",  that will set the QL_PINNAME_NETLIGHT pin level to LOW_LEVEL.
*               If input "h", that will set the QL_PINNAME_NETLIGHT pin level to HIGH_LEVEL.
*               If input "r", that will read the QL_PINNAME_NETLIGHT pin level.
*           
****************************************************************************************************/
#ifdef __EXAMPLE_GPIO__
  
#include "ql_appinit.h"
#include "ql_interface.h"
#include "ql_type.h"
#include "ql_trace.h"
#include "ql_audio.h"
#include "ql_timer.h"
#include "ql_stdlib.h"
#include "ql_pin.h"

void ql_entry()		// entry for main task
{
    bool           keepGoing = TRUE;
    QlEventBuffer  flSignalBuffer;
    char *pData, *p;
    char buffer[100];

    Ql_SetDebugMode(BASIC_MODE);	// set working mode of UART2 to BASIC_MODE
    Ql_DebugTrace("gpio: ql_entry\r\n");
    Ql_OpenModemPort(ql_md_port1);  //open virtual modem serial port ql_md_port1
     
    while(keepGoing)
    {    
        Ql_GetEvent(&flSignalBuffer);		// Get event from core system. When there is no event, 
                                            // that will go into waiting state.
        switch(flSignalBuffer.eventType)
        {
            case EVENT_UARTDATA:			// When core system has received data from uart port, this event will be triggered
            {
                if (flSignalBuffer.eventData.uartdata_evt.len>0)
                {
                    s32 pin;
                    s32 mod = 0;
                    s32 pinpullenable = 0;
                    s32 pindirection = 0;
                    s32 pinlevel = 0;
                    s32 iret;

                    pData = (char*)flSignalBuffer.eventData.uartdata_evt.data;

                    //command-->select QL_PINNAME_NETLIGHT pin function to mode1, it is netlight function
                    p = Ql_strstr(pData,"1");
                    if (p)
                    {
                        iret = Ql_pinSubscribe(QL_PINNAME_NETLIGHT, QL_PINMODE_1, NULL);    
                        Ql_sprintf(buffer, "\r\nSubscribe(%d),pin=%d,mod=%d\r\n",iret,QL_PINNAME_NETLIGHT,QL_PINMODE_1);
                        Ql_SendToUart(ql_uart_port1,buffer,Ql_strlen(buffer));  
                        break;
                    }

                    //command-->select QL_PINNAME_NETLIGHT pin function to mode2, it is gpio function
                    p = Ql_strstr(pData,"2");
                    if (p)
                    {
                        QlPinParameter pinparameter;
                        pinparameter.pinconfigversion = QL_PIN_VERSION;
                        pinparameter.pinparameterunion.gpioparameter.pinpullenable = QL_PINPULLENABLE_ENABLE;
                        pinparameter.pinparameterunion.gpioparameter.pindirection = QL_PINDIRECTION_OUT;
                        pinparameter.pinparameterunion.gpioparameter.pinlevel = QL_PINLEVEL_HIGH;
                        iret = Ql_pinSubscribe(QL_PINNAME_NETLIGHT, QL_PINMODE_2, &pinparameter);    
                        Ql_sprintf(buffer, "\r\nSubscribe(%d),pin=%d,mod=%d,pul=%d,dir=%d,lev=%d\r\n",iret,QL_PINNAME_NETLIGHT,QL_PINMODE_2,QL_PINPULLENABLE_ENABLE,QL_PINDIRECTION_OUT,QL_PINLEVEL_HIGH);
                        Ql_SendToUart(ql_uart_port1,buffer,Ql_strlen(buffer));  
                        break;
                    }                        

                    //command-->unsubscribe
                    p = Ql_strstr(pData,"u");
                    if (p)
                    {
                        iret = Ql_pinUnSubscribe(QL_PINNAME_NETLIGHT);
                        Ql_sprintf(buffer, "\r\nUnSubscribe(%d),pin=%d\r\n",iret,QL_PINNAME_NETLIGHT);
                        Ql_SendToUart(ql_uart_port1,buffer,Ql_strlen(buffer));  
                        break;
                    }  

                    //command-->write gpio low level, when pin select gpio function.
                    p = Ql_strstr(pData,"l");
                    if (p)
                    {
                        iret = Ql_pinWrite(QL_PINNAME_NETLIGHT, QL_PINLEVEL_LOW);
                        Ql_sprintf(buffer, "\r\nWriteLow(%d),pin=%d\r\n",iret,QL_PINNAME_NETLIGHT);
                        Ql_SendToUart(ql_uart_port1,buffer,Ql_strlen(buffer));  
                        break;
                    }  

                    //command-->write gpio high level, when pin select gpio function.
                    p = Ql_strstr(pData,"h");
                    if (p)
                    {
                        iret = Ql_pinWrite(QL_PINNAME_NETLIGHT, QL_PINLEVEL_HIGH);
                        Ql_sprintf(buffer, "\r\nWriteHigh(%d),pin=%d\r\n",iret,QL_PINNAME_NETLIGHT);
                        Ql_SendToUart(ql_uart_port1,buffer,Ql_strlen(buffer));  
                        break;
                    }  

                    //command-->read gpio level, when pin select gpio function.
                    p = Ql_strstr(pData,"r");
                    if (p)
                    {
                        QlPinLevel pinlevel;
                        iret = Ql_pinRead(QL_PINNAME_NETLIGHT, &pinlevel);
                        Ql_sprintf(buffer, "\r\nRead(%d),pin=%d,level=%d\r\n",iret,QL_PINNAME_NETLIGHT,pinlevel);
                        Ql_SendToUart(ql_uart_port1,buffer,Ql_strlen(buffer));  
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

#endif //__EXAMPLE_GPIO__

