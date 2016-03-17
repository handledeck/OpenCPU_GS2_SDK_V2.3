#ifdef __EXAMPLE_HEADSET__
  
#include "ql_appinit.h"
#include "ql_interface.h"
#include "ql_type.h"
#include "ql_trace.h"
#include "ql_audio.h"
#include "ql_timer.h"
#include "ql_stdlib.h"
#include "ql_pin.h"
#include "ql_fcm.h"


/******************************************************************
* Please refer to how to configure the constant 
* 'Customer_user_qlconfig' in ql_customer_config.c
*******************************************************************/
void ql_entry()
{
    bool           keepGoing = TRUE;
    QlEventBuffer  flSignalBuffer;
    s32 period = 10;
    char buffer[100];
    char *pData, *p;
    QlAudioPlayPath path;

    Ql_SetDebugMode(BASIC_MODE);   
    Ql_DebugTrace("headset: ql_entry\r\n");
    Ql_OpenModemPort(ql_md_port1);       
     
    while(keepGoing)
    {    
        Ql_GetEvent(&flSignalBuffer);
        switch(flSignalBuffer.eventType)
        {
            case EVENT_POWERKEY:
            {
                if(flSignalBuffer.eventData.powerkey_evt.powerkey == POWERKEY_ON)
                {
                    Ql_PowerOnAck();
                    Ql_sprintf(buffer, "\r\nNow, Power on System\r\n");
                    Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  
                    path = Ql_VoiceCallGetCurrentPath();
                    Ql_sprintf(buffer, "\r\nCurrent Voice call path is %d\r\n",path);
                    Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  
                    Ql_VoiceCallChangePath(QL_AUDIO_PATH_LOUDSPEAKER);
                    if(path == QL_AUDIO_PATH_NORMAL)
                    {
                        Ql_sprintf(buffer, "\r\nnow change to QL_AUDIO_PATH_LOUDSPEAKER\r\n");
                        Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  
                    }
                }
                else if(flSignalBuffer.eventData.powerkey_evt.powerkey == POWERKEY_OFF)
                {
                    if( flSignalBuffer.eventData.powerkey_evt.isPressed)
                    {
                        Ql_sprintf(buffer, "\r\nNow, Press PWRKEY\r\n");
                        Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  
                    }
                    else
                    {
                        Ql_sprintf(buffer, "\r\nNow, Release PWRKEY\r\n");
                        Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  
                    }
                }
                break;
            }
                    
            case EVENT_HEADSET:
            {                
                switch(flSignalBuffer.eventData.headset_evt.headsettype)
                {
                    case HEADSET_PLUGOUT:
                        Ql_sprintf(buffer, "\r\nHEADSET_PLUGOUT");
                        Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  
                        path = Ql_VoiceCallGetCurrentPath();
                        Ql_sprintf(buffer, "\r\nCurrent Voice call path is %d\r\n",path);
                        Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  

                    #if 1                                           
                        if(path != QL_AUDIO_PATH_LOUDSPEAKER)
                        {
                            Ql_sprintf(buffer, "\r\nnow change to QL_AUDIO_PATH_LOUDSPEAKER\r\n");
                            Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  
                            Ql_VoiceCallChangePath(QL_AUDIO_PATH_LOUDSPEAKER);
                        }                                            
                    #endif
                    
                        break;
                    case HEADSET_PLUGIN:
                        Ql_sprintf(buffer, "\r\nHEADSET_PLUGIN");
                        Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  
                        path = Ql_VoiceCallGetCurrentPath();
                        Ql_sprintf(buffer, "\r\nCurrent Voice call path is %d\r\n",path);
                        Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));

                    #if 1                                            
                        if(path != QL_AUDIO_PATH_HEADSET)
                        {
                            Ql_sprintf(buffer, "\r\nnow change to QL_AUDIO_PATH_HEADSET\r\n");
                            Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  
                            Ql_VoiceCallChangePath(QL_AUDIO_PATH_HEADSET);
                        }                                            
                    #endif
                    
                        break;
                    case HEADSET_KEYPRESS:
                        Ql_sprintf(buffer, "\r\nHEADSET_KEYPRESS");
                        Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  
                       break;
                    case HEADSET_KEYRELEASE:
                        Ql_sprintf(buffer, "\r\nHEADSET_KEYRELEASE");
                        Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  
                        break;
                    case HEADSET_ADC:
                        Ql_sprintf(buffer, "\r\nHEADSET_ADC:%d",flSignalBuffer.eventData.headset_evt.data1);
                        Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  
                        break;
                }
                 break;    
            }
            
            case EVENT_MODEMDATA:
            {                
                //Ql_DebugTrace("\r\nEVENT_MODEMDATA type=%d \r\n",flSignalBuffer.eventData.modemdata_evt.type);
                //Ql_DebugTrace("\r\nmodemdata =%s\r\n",flSignalBuffer.eventData.uartdata_evt.data);    
                //if (flSignalBuffer.eventData.modemdata_evt.type==DATA_AT)
                Ql_SendToUart(ql_uart_port1,(u8*)flSignalBuffer.eventData.uartdata_evt.data,flSignalBuffer.eventData.uartdata_evt.len);    
                break;
            }
            case EVENT_UARTDATA:
            {            
                //Ql_DebugTrace("\r\nEVENT_UARTDATA PORT=%d\r\n",flSignalBuffer.eventData.uartdata_evt.port);
                //Ql_DebugTrace("\r\nuart data =%s\r\n",flSignalBuffer.eventData.uartdata_evt.data);    
                Ql_SendToModem(ql_md_port1,(u8*)flSignalBuffer.eventData.uartdata_evt.data,flSignalBuffer.eventData.uartdata_evt.len);    
                break;                        
            }
            
            default:
                break;
        }
    }
}

#endif //__EXAMPLE_HEADSET__

