#ifdef __EXAMPLE_PWM__
 /***************************************************************************************************
 *	 Example:
 *		 
 *			 PWM Routine
 *
 *	 Description:
 *
 *			 This example gives an example for PWM setting.
 *			 Through Uart port, input the special command, there will be given the response about the gpio state.
 *			 The gpio used in this example is QL_PINNAME_NETLIGHT
 *
 *	 Usage:
 *
 *			 Precondition:
 *
 *					 Use "make PWM" to compile, and download bin image to module.
 *			 
 *			 Through Uart port:
 *
 *				 If input "1", that will subscribe QL_PINNAME_NETLIGHT pin to QL_PINMODE_1.
 *				 If input "3", that will subscribe QL_PINNAME_NETLIGHT pin to QL_PINMODE_3.
 *				 If input "u", that will unsubscribe QL_PINNAME_NETLIGHT pin.
 *				 If input "s", that will start alert when you set to QL_PINMODE_3.
 *				 If input "t", that will stop alert .
 *	Note:
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
#include "ql_pin.h"
#include "ql_fcm.h"

void ql_entry()
{
    bool keepGoing = TRUE;
    QlEventBuffer flSignalBuffer;
    s32 period = 10;
    char buffer[100];
    char *pData, *p;

    Ql_SetDebugMode(BASIC_MODE);   
    Ql_DebugTrace("pwm: ql_entry\r\n");
    Ql_OpenModemPort(ql_md_port1);       

    while(keepGoing)
    {    
        Ql_GetEvent(&flSignalBuffer);
        switch(flSignalBuffer.eventType)
        {
            case EVENT_UARTDATA:
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

                    //command-->select QL_PINNAME_NETLIGHT pin function to mode1, it is Net light function
                    p = Ql_strstr(pData,"1");
                    if (p)
                    {
                        iret = Ql_pinSubscribe(QL_PINNAME_NETLIGHT, QL_PINMODE_1, NULL);    
                        Ql_sprintf(buffer, "\r\nSubscribe(%d),pin=%d,mod=%d\r\n",iret,QL_PINNAME_NETLIGHT,QL_PINMODE_1);
                        Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  
                        break;
                    }
                    
                    //command-->select QL_PINNAME_NETLIGHT pin function to mode3, it is pwm function
                    p = Ql_strstr(pData,"3");
                    if (p)
                    {
                        QlPinParameter pinparameter;
                        pinparameter.pinconfigversion = QL_PIN_VERSION;
                        pinparameter.pinparameterunion.pwmparameter.pwmsource = QL_PWMSOURCE_32K;
                        pinparameter.pinparameterunion.pwmparameter.pwmclkdiv = QL_PWMSOURCE_DIV8;
                        pinparameter.pinparameterunion.pwmparameter.lowpulesnumber = 4000;
                        pinparameter.pinparameterunion.pwmparameter.highpulesnumber = 4000;
                        iret = Ql_pinSubscribe(QL_PINNAME_NETLIGHT, QL_PINMODE_3, &pinparameter);    
                        Ql_sprintf(buffer, "\r\nSubscribe(%d),pin=%d,mod=%d\r\n",iret,QL_PINNAME_NETLIGHT,QL_PINMODE_3);
                        Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  
                        break;
                    }   
                    
                    //command-->unsubscribe
                    p = Ql_strstr(pData,"u");
                    if (p)
                    {
                        iret = Ql_pinUnSubscribe(QL_PINNAME_NETLIGHT);
                        Ql_sprintf(buffer, "\r\nUnSubscribe(%d),pin=%d\r\n",iret,QL_PINNAME_NETLIGHT);
                        Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  
                        break;
                    } 
                    
                    //command-->control start pwm
                    p = Ql_strstr(pData,"s");
                    if (p)
                    {
                        iret = Ql_pinControl(QL_PINNAME_NETLIGHT, QL_PINCONTROL_START);
                        Ql_sprintf(buffer, "\r\nStart(%d),pin=%d\r\n",iret,QL_PINNAME_NETLIGHT);
                        Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  
                        break;
                    }
                    
                    //command-->control stop pwm
                    p = Ql_strstr(pData,"t");
                    if (p)
                    {
                        iret = Ql_pinControl(QL_PINNAME_NETLIGHT, QL_PINCONTROL_STOP);
                        Ql_sprintf(buffer, "\r\nStop(%d),pin=%d\r\n",iret,QL_PINNAME_NETLIGHT);
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

#endif // __EXAMPLE_PWM__

