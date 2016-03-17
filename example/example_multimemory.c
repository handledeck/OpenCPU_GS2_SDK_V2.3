#ifdef __EXAMPLE_MULTIMEMORY__
/***************************************************************************************************
*   Example:
*       
*           MULTIMEMORY Routine
*
*   Description:
*
*           This example gives an example for MULTIMEMORY operation.
*           Run the code,that the tasks will apply for memory ,one by one .
*
*   Usage:
*
*           Precondition:
*
*                   Use "make/make128 MULTIMEMORY" to compile, and download bin image to module.
*           
*           Through Uart port:
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
#include "Ql_multitask.h"
#include "Ql_tcpip.h"
#include "Ql_fcm.h"


#define ALLOC_SIZE 2049

struct memponter_tag{
    void *p1;
    void *p2;
};


#define OUT_DEBUG(x,...)  \
    Ql_memset((void*)(x),0,100);  \
    Ql_sprintf((char*)(x),__VA_ARGS__);   \
    Ql_SendToUart(ql_uart_port1,(u8*)(x),Ql_strlen((const char*)(x)));


/**************************************************************
*  main task
***************************************************************/

struct memponter_tag  memponter_main = {NULL,NULL};
struct memponter_tag  memponter_sub1 = {NULL,NULL};
struct memponter_tag  memponter_sub2 = {NULL,NULL};
struct memponter_tag  memponter_sub3 = {NULL,NULL};
QlEventBuffer    flSignalBuffer; // Set flSignalBuffer to global variables  may as well, otherwise it will occupy stack space
void ql_entry()
{
    bool    keepGoing = TRUE;
    QlTimer timer;

    Ql_SetDebugMode(BASIC_MODE);   

    Ql_DebugTrace("multimemory: ql_entry\r\n");

    timer.timeoutPeriod = 300;
    Ql_StartTimer(&timer);
    while(keepGoing)
    {
        Ql_GetEvent(&flSignalBuffer);
        switch(flSignalBuffer.eventType)
        {
            case EVENT_TIMER:
            {
                if( timer.timerId  == flSignalBuffer.eventData.timer_evt.timer_id)
                {
                    if(memponter_main.p1)
                    {
                        if(*((u8*)(memponter_main.p1)) != 0xFF)
                            Ql_DebugTrace("Warning,Warning,Warning,Warning ql_entry value change");
                        Ql_FreeMemory(memponter_main.p1);
                        memponter_main.p1 = NULL;
                    }
                    if(memponter_main.p2)
                    {
                        if(*((u8*)(memponter_main.p2)) != 0xFF)
                            Ql_DebugTrace("Warning,Warning,Warning,Warning ql_entry value change");
                        Ql_FreeMemory(memponter_main.p2);
                        memponter_main.p2 = NULL;
                    }

                    if(memponter_sub1.p1==NULL)
                    {
                        memponter_sub1.p1 = Ql_GetMemory(ALLOC_SIZE);
                        if(memponter_sub1.p1)
                        {
                            Ql_memset(memponter_sub1.p1, 1, ALLOC_SIZE);
							Ql_DebugTrace("memponter_sub1.p1:get memory ok!");
                        }
                        else
                        {
                            Ql_DebugTrace("ql_entry: fail to Ql_GetMemory() for memponter_sub1.p1 \r\n");
                        }
                    }

                    if(memponter_sub1.p2==NULL)
                    {
                        memponter_sub1.p2 = Ql_GetMemory(ALLOC_SIZE);
                        if(memponter_sub1.p2)
                        {
                            Ql_memset(memponter_sub1.p2, 1, ALLOC_SIZE);
							Ql_DebugTrace("memponter_sub1.p2:get memory ok!");
                        }
                        else
                        {
                            Ql_DebugTrace("ql_entry: fail to Ql_GetMemory() for memponter_sub1.p2 \r\n");
                        }
                    }
                    Ql_DebugTrace("ql_entry 5555555555");
                    timer.timeoutPeriod = 500;
                    Ql_StartTimer(&timer);
                }
                break;
            }
            
            default:
                break;
        }
    }
}


/**************************************************************
*  1st sub task
***************************************************************/
QlEventBuffer    flSignalBuffer_subtask1;  //  Set flSignalBuffer_subtask1 to global variables  may as well, otherwise it will occupy stack space
void example_subtask1_entry(s32 TaskId)
{
    bool    keepGoing = TRUE;
    QlTimer timer1;

    Ql_DebugTrace("multimemory: example_task1_entry\r\n");

    timer1.timeoutPeriod = 400;
    Ql_StartTimer(&timer1);

    while(keepGoing)
    {
        Ql_GetEvent(&flSignalBuffer_subtask1);
        switch(flSignalBuffer_subtask1.eventType)
        {
            case EVENT_TIMER:
            {
               if( timer1.timerId  == flSignalBuffer_subtask1.eventData.timer_evt.timer_id)
                {
                    if(memponter_sub1.p1)
                    {
                        if(*((u8*)(memponter_sub1.p1)) != 0x1)
                            Ql_DebugTrace("Warning,Warning,Warning,Warning example_subtask1_entry value change");
                        Ql_FreeMemory(memponter_sub1.p1);
                        memponter_sub1.p1 = NULL;
                    }
                    if(memponter_sub1.p2)
                    {
                        if(*((u8*)(memponter_sub1.p2)) != 0x1)
                            Ql_DebugTrace("Warning,Warning,Warning,Warning example_subtask1_entry value change");
                        Ql_FreeMemory(memponter_sub1.p2);
                        memponter_sub1.p2 = NULL;
                    }
                    if(memponter_sub2.p1==NULL)
                    {
                        memponter_sub2.p1 = Ql_GetMemory(ALLOC_SIZE);
                        if(memponter_sub2.p1)
                        {
                            Ql_memset(memponter_sub2.p1, 2, ALLOC_SIZE);
							Ql_DebugTrace("memponter_sub2.p1:get memory ok!");
                        }
                        else
                        {
                            Ql_DebugTrace("ql_entry: fail to Ql_GetMemory() for memponter_sub2.p1 \r\n");
                        }
                    }
                    if(memponter_sub2.p2==NULL)
                    {
                        memponter_sub2.p2 = Ql_GetMemory(ALLOC_SIZE);
                        if(memponter_sub2.p2)
                        {
                            Ql_memset(memponter_sub2.p2, 2, ALLOC_SIZE);
							Ql_DebugTrace("memponter_sub2.p2:get memory ok!");
                        }
                        else
                        {
                            Ql_DebugTrace("ql_entry: fail to Ql_GetMemory() for memponter_sub2.p2 \r\n");
                        }
                    }
					Ql_DebugTrace("ql_entry 6666666666");
                    timer1.timeoutPeriod = 503;
                    Ql_StartTimer(&timer1);
                }
            }
                break;
            default:
                break;
        }
    }
    
}

/**************************************************************
* 2nd sub task
***************************************************************/
QlEventBuffer    flSignalBuffer_subtask2;  //  Set flSignalBuffer_subtask1 to global variables  may as well, otherwise it will occupy stack space
void example_subtask2_entry(s32 TaskId)
{
    bool    keepGoing = TRUE;
    QlTimer timer2;

    Ql_DebugTrace("multimemory: example_task2_entry\r\n");

    timer2.timeoutPeriod = 500;
    Ql_StartTimer(&timer2);

    while(keepGoing)
    {
        Ql_GetEvent(&flSignalBuffer_subtask2);
        switch(flSignalBuffer_subtask2.eventType)
        {
            case EVENT_TIMER:
            {
                if( timer2.timerId  == flSignalBuffer_subtask2.eventData.timer_evt.timer_id)
                {
                    if(memponter_sub2.p1)
                    {
                        if(*((u8*)(memponter_sub2.p1)) != 0x2)
                            Ql_DebugTrace("Warning,Warning,Warning,Warning example_subtask2_entry value change");
                        Ql_FreeMemory(memponter_sub2.p1);
                        memponter_sub2.p1 = NULL;
                    }
                    if(memponter_sub2.p2)
                    {
                        if(*((u8*)(memponter_sub2.p2)) != 0x2)
                            Ql_DebugTrace("Warning,Warning,Warning,Warning example_subtask2_entry value change");
                        Ql_FreeMemory(memponter_sub2.p2);
                        memponter_sub2.p2 = NULL;
                    }
                    
                    if(memponter_sub3.p1==NULL)
                    {
                        memponter_sub3.p1 = Ql_GetMemory(ALLOC_SIZE);
                        if(memponter_sub3.p1)
                        {
                            Ql_memset(memponter_sub3.p1, 3, ALLOC_SIZE);
							Ql_DebugTrace("memponter_sub3.p1:get memory ok!");
                        }
                        else
                        {
                            Ql_DebugTrace("ql_entry: fail to Ql_GetMemory() for memponter_sub3.p1 \r\n");
                        }
                    }
                    if(memponter_sub3.p2==NULL)
                    {
                        memponter_sub3.p2 = Ql_GetMemory(ALLOC_SIZE);
                        if(memponter_sub3.p2)
                        {
                            Ql_memset(memponter_sub3.p2, 3, ALLOC_SIZE);
							Ql_DebugTrace("memponter_sub3.p2:get memory ok!");
                        }
                        else
                        {
                            Ql_DebugTrace("ql_entry: fail to Ql_GetMemory() for memponter_sub3.p2 \r\n");
                        }
                    }
                    Ql_DebugTrace("ql_entry 7777777777");
                    timer2.timeoutPeriod = 505;
                    Ql_StartTimer(&timer2);
                }
            }
                break;
            default:
                break;
        }
    }
    
}


/**************************************************************
* 3rd sub task
***************************************************************/
QlEventBuffer    flSignalBuffer_subtask3;  //  Set flSignalBuffer_subtask1 to global variables  may as well, otherwise it will occupy stack space
void example_subtask3_entry(s32 TaskId)
{
    bool    keepGoing = TRUE;
    QlTimer timer3;

    Ql_DebugTrace("multimemory: example_task3_entry\r\n");

    timer3.timeoutPeriod = 600;
    Ql_StartTimer(&timer3);
    while(keepGoing)
    {
        Ql_GetEvent(&flSignalBuffer_subtask3);
        switch(flSignalBuffer_subtask3.eventType)
        {
            case EVENT_TIMER:
            {
               if( timer3.timerId  == flSignalBuffer_subtask3.eventData.timer_evt.timer_id)
                {
                    if(memponter_sub3.p1)
                    {
                        if(*((u8*)(memponter_sub3.p1)) != 0x3)
                            Ql_DebugTrace("Warning,Warning,Warning,Warning example_subtask3_entry value change");
                         Ql_FreeMemory(memponter_sub3.p1);
                        memponter_sub3.p1 = NULL;
                    }
                    if(memponter_sub3.p2)
                    {
                        if(*((u8*)(memponter_sub3.p2)) != 0x3)
                            Ql_DebugTrace("Warning,Warning,Warning,Warning example_subtask3_entry value change");
                        Ql_FreeMemory(memponter_sub3.p2);
                        memponter_sub3.p2 = NULL;
                    }
                    if(memponter_main.p1==NULL)
                    {
                        memponter_main.p1 = Ql_GetMemory(ALLOC_SIZE);
                        if(memponter_main.p1)
                        {
                            Ql_memset(memponter_main.p1, 0xFF, ALLOC_SIZE);
							Ql_DebugTrace("memponter_main.p1:get memory ok!");
                        }
                    }
                    if(memponter_main.p2==NULL)
                    {
                        memponter_main.p2 = Ql_GetMemory(ALLOC_SIZE);
                        if(memponter_main.p2)
                        {
                            Ql_memset(memponter_main.p2, 0xFF, ALLOC_SIZE);
							Ql_DebugTrace("memponter_main.p2:get memory ok!");
                        }
                    }
					Ql_DebugTrace("ql_entry 8888888888");
                    timer3.timeoutPeriod = 507;
                    Ql_StartTimer(&timer3);
                }
                break;
            }
            
            default:
                break;
        }
    }
}

#endif // __EXAMPLE_MULTIMEMORY__

