#ifdef __EXAMPLE_EINTCALLBACK__
/***************************************************************************************************
*   Example:
*       
*           EINTCALLBACK Routine
*
*   Description:
*
*           This example gives an example for eintcallback operation.
*           Through Main Uart port, input the special command, and the response message will be 
*           printed out through MAIN Uart port. The gpio used in this example is QL_PINNAME_DTR.
*           
*   Usage:
*
*           Compile & Run:
*
*                   Use "make eintcallback" to compile, and download bin image to module.
*           
*           Through Uart port:
*
*               If input "1", that will subscribe QL_PINNAME_DTR pin to QL_PINMODE_1,it is DTR function.
*               If input "3", that will subscribe QL_PINNAME_DTR pin to QL_PINMODE_3, it is EINT function.
*               If input "u", that will unsubscribe QL_PINNAME_DTR pin.
*               If input "r", that will read pin level.
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
#include "Ql_multitask.h"

void eint_callback_isr_handle(u8 eintno)
{
    QlPinLevel pinlevel;

    Ql_eintRead(eintno, &pinlevel);
    if(QL_PINLEVEL_LOW == pinlevel)
    {
        Ql_eintSetPolarity(eintno,QL_PINLEVEL_HIGH);
    }
    else
    {       
        Ql_eintSetPolarity(eintno,QL_PINLEVEL_LOW);  
    }
    Ql_osSendEvent(ql_main_task, pinlevel, 0); //send EVENT_MSG to ql_main_task
    Ql_eintMask(eintno, FALSE);   // unmask
}

void ql_entry()
{
    bool           keepGoing = TRUE;
    QlEventBuffer  flSignalBuffer;
    s32 period = 10;
    char buffer[100];
    char *pData, *p;

    Ql_SetDebugMode(BASIC_MODE);   
    Ql_DebugTrace("eintcallback: ql_entry\r\n");
     
    while(keepGoing)
    {    
        Ql_GetEvent(&flSignalBuffer);
        switch(flSignalBuffer.eventType)
        {
            case EVENT_INTR:
            {
                //because you use Eint call back, so the EVENT_INTR will not report
                break;
            }

            case EVENT_MSG:
            {
                Ql_sprintf(buffer, "\r\nEINT give level=%d\r\n",flSignalBuffer.eventData.msg_evt.data1);
                Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  
                break;
            }
            
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
                        pinparameter.pinparameterunion.eintcallbackparameter.einttriggertype = QL_EINTTRIGGERTYPE_CALLBACK;
                        pinparameter.pinparameterunion.eintcallbackparameter.eintsensitivetype = QL_EINTSENSITIVETYPE_LEVEL;
                        pinparameter.pinparameterunion.eintcallbackparameter.hardware_de_bounce = 10; //10 count 32Khz clock
                        pinparameter.pinparameterunion.eintcallbackparameter.software_de_bounce = 0; // unit is ms , max is 2559
                        pinparameter.pinparameterunion.eintcallbackparameter.automask = TRUE; // auto mask
                        pinparameter.pinparameterunion.eintcallbackparameter.callbak_fun = eint_callback_isr_handle;
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
                    
                    //command-->read gpio level
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

#endif // __EXAMPLE_EINTCALLBACK__

