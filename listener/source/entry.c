
#include "ql_appinit.h"
#include "ql_interface.h"
#include "ql_type.h"
#include "ql_trace.h"
#include "ql_audio.h"
#include "ql_timer.h"
#include "ql_stdlib.h"
#include "ql_pin.h"
#include "ql_memory.h"
#include "ql_fcm.h"


#pragma arm section zidata = "NVZI",  rwdata = "NVRW" 
int nv1;
int nv2;
#pragma arm section zidata, rwdata
static u32 k=11;

void ql_entry()
{
    bool           keepGoing = TRUE;
    QlEventBuffer  flSignalBuffer;
    s32 period = 10;
    char buffer[100];
    char *pData, *p;
    void *pointer = 0;

    Ql_SetDebugMode(BASIC_MODE);   
    Ql_DebugTrace("memory: ql_entry\r\n");
           

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

                    //command-->allocate memory, size is 1k bytes
                    p = Ql_strstr(pData,"a");
                    if (p)
                    {
                        if (pointer != NULL)
                        {
                            Ql_FreeMemory(pointer);
                            pointer = NULL;
                        }
                        pointer = Ql_GetMemory(1024);
                        Ql_sprintf(buffer, "\r\nALLOC,pointer=%x\r\n",pointer);
                        Ql_DebugTrace("%s",buffer);  
                        break;
                    }
                    
                    //command-->free memory
                    p = Ql_strstr(pData,"f");
                    if (p)
                    {
                        Ql_sprintf(buffer, "\r\nFREE,pointer=%x\r\n",pointer);
                        Ql_DebugTrace("%s",buffer);  
                        Ql_FreeMemory(pointer); 
                        pointer = NULL;
                        break;
                    }                        
                    p = Ql_strstr(pData,"w");
                    if (p)
                    {
                        nv1 = 43;
                        nv2 = 27;
                        break;
                    }                                 
                    p = Ql_strstr(pData,"r");
                    if (p)
                    {
                        Ql_sprintf(buffer, "nvdata1=%d nvdata2=%d k=%d\r\n",nv1,nv2,k);                                
                        Ql_SendToUart(ql_uart_port1, (u8*)buffer,Ql_strlen(buffer));  
                        break;
                    }                            
                }
                break;
            }
            
            default:
                break;
        }
    }
    if (pointer != NULL)
    {
        Ql_FreeMemory(pointer);
        pointer = NULL;
    }
}




