#ifdef __EXAMPLE_MULTITASK__
/***************************************************************************************************
*   Example:
*       
*           MULTITASK Routine
*
*   Description:
*
*           This example demonstrates how to use multitask function with APIs in OpenCPU.
*           Through MAIN Uart port, input the specified command, and the response message will be 
*           printed out through DEBUG port or main port.
*
*   Usage:
*
*           Compile & Run:
*
*               Use "make MULTITASK" to compile, and download bin image to module to run.
*           
*           Operation: (Through MAIN port)
*
*               If input "Ql_osSendEvent=", that will send message to others tasks.
*               If input "TestMutex", that will learn how to use Mutex.
*               If input "TestSemaphore", that will learn how to use Semaphore.
*               If input "Ql_SetLastErrorCode=", that will learn how to set errorcode.
*               If input "Ql_GetLastErrorCode", that will get the current errorcode.
*               If input "Ql_osGetCurrentTaskPriority", that will get the current priority of all subtask.
*               If input "Ql_osGetCurrenTaskRemainStackSize", that will get the left number of bytes in the current task stack.
*               If input "Ql_osChangeTaskPriority=", that will change the priority of the specified task.
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

#define MAX_TASK_NUM  ql_sub_task10

static char textBuf[128];  
static int  s_iMutexId = 0;
static int  s_iSemaphoreId = 0;
static int  s_iSemMutex = 0;
static int  s_iTestSemNum = 2;
static int  s_iPassTask = 1;
    
#define MY_DEBUG_OUT(y,x,...) \
    Ql_memset((char *)(x), 0, sizeof(x)); \
    Ql_sprintf((char *)(x), __VA_ARGS__); \
    Ql_SendToUart((QlPort)(y), (u8*)(x), Ql_strlen((const char *) (x)));


void MutextTest(int iTaskId)  //Two task Run this function at the same time
{

    MY_DEBUG_OUT(ql_uart_port2, textBuf, "I am Task %d\r\n", iTaskId);
    Ql_osTakeMutex(s_iMutexId);
    MY_DEBUG_OUT(ql_uart_port2, textBuf, "---->I am Task %d\r\n", iTaskId);   //Another Caller prints this sentence after 5 seconds
    Ql_Sleep(3000);                                                             
    MY_DEBUG_OUT(ql_uart_port2, textBuf, "(TaskId=%d)Do not reboot with calling Ql_sleep\r\n", iTaskId);
    Ql_osGiveMutex(s_iMutexId);
}

void SemaphoreTest(int iTaskId)
{
    int iRet = -1;
    
    Ql_DebugTrace("\r\nI am Task %d", iTaskId);
    
    iRet = Ql_osTakeSemaphore(s_iSemaphoreId, TRUE);//TRUE or FLASE indicate the task should wait infinitely or return immediately.
    Ql_DebugTrace("\r\n=========Ql_osTakeSemaphore(%d, TRUE) = %d", s_iSemaphoreId, iRet);   //Flag Sentence

    Ql_osTakeMutex(s_iSemMutex);
    s_iTestSemNum--;
    Ql_DebugTrace("\r\n=========Task[%d]: s_iTestSemNum = %d", iTaskId, s_iTestSemNum);
    Ql_osGiveMutex(s_iSemMutex);
    
    s_iTestSemNum++;
    Ql_Sleep(3000);     //The last task  print Flag Sentence delaying 5 seconds;  
    Ql_osGiveSemaphore(s_iSemaphoreId);
}

void SemaphoreTest1(int iTaskId)
{
    int iRet = -1;
    
    Ql_DebugTrace("\r\nI am Task %d", iTaskId);

    while(1)
    {
        iRet = Ql_osTakeSemaphore(s_iSemaphoreId, FALSE);
        if(OS_SUCCESS == iRet)
        {
            Ql_DebugTrace("\r\n=========Task[%d]: take semaphore OK!", iTaskId);
            break;
        }
        else
        {

            Ql_DebugTrace("\r\n*****Task[%d]: take semaphore FAILED(iRet = %d)!", iTaskId, iRet);
            Ql_Sleep(300);
        }
    }

    Ql_osTakeMutex(s_iSemMutex);
    s_iTestSemNum--;
    Ql_DebugTrace("\r\nTask[%d]: s_iTestSemNum = %d", iTaskId, s_iTestSemNum);
    Ql_osGiveMutex(s_iSemMutex);
    
    s_iTestSemNum++;
    Ql_Sleep(5000);     //The last task  print Flag Sentence delaying 5 seconds;  
    Ql_osGiveSemaphore(s_iSemaphoreId);
}

void GetCurrentTaskPriority(int iTaskId)
{
    int iRet = 0;
    iRet = Ql_osGetCurrentTaskPriority();
    Ql_DebugTrace("\r\nTask[%d]: priority=%d", iTaskId, iRet);
}

void GetCurrenTaskRemainStackSize(int iTaskId)
{
    int iRet = 0;
    iRet = Ql_osGetCurrenTaskRemainStackSize();
    Ql_DebugTrace("\r\nTask[%d]: Task Remain Stack Size -> %d", iTaskId, iRet);
}

void SendEvent2AllSubTask(int iData1, int iData2)
{
    int iTmp = 0;
    int iRet = 0;
    
    for(iTmp=ql_sub_task1; iTmp<MAX_TASK_NUM+1; iTmp++)
    {

        iRet = Ql_osSendEvent(iTmp, iData1, iData2);
        MY_DEBUG_OUT(ql_uart_port1, textBuf, "Ql_osSendEvent(%d, %d, %d)=%d\r\n", iTmp, iData1, iData2, iRet);
    }
}
/**************************************************************
* main task
***************************************************************/

QlEventBuffer    MyEvent; // Set flSignalBuffer to global variables  may as well, otherwise it will occupy stack space
void ql_entry()
{
    bool keepGoing = TRUE;
    char *pData = NULL;
    char *p = NULL;
    char *q = NULL;

    int iRet = 0;
    int iNum = 0;
    int iTmp = 0;

    Ql_SetDebugMode(BASIC_MODE);   
    Ql_DebugTrace("multitask: ql_entry\r\n");

    while(keepGoing)
    {    
        Ql_GetEvent(&MyEvent);
        switch(MyEvent.eventType)
        {
            case EVENT_UARTDATA:
            {   
                if(MyEvent.eventData.uartdata_evt.len > 0)
                {
                    pData = (char *)MyEvent.eventData.uartdata_evt.data;
                                                          
                    if(NULL != (p = Ql_strstr(pData, "Ql_osSendEvent")))
                    {                                          //Ql_osSendEvent=1,2
                        p = Ql_strstr(p, "=");
                        q = Ql_strstr(p, ",");
                        if((p == NULL) || (q == NULL))
                        {
                            MY_DEBUG_OUT(ql_uart_port1, textBuf, "\r\nQl_osSendEvent: Error parameter!\r\n");
                            break;
                        }
                        p++; (*q) = 0; q++;

                        iNum = Ql_atoi(p);
                        iTmp = Ql_atoi(q);

                        s_iPassTask = 1;
                        iRet = Ql_osSendEvent(1, iNum, iTmp);
                        MY_DEBUG_OUT(ql_uart_port1, textBuf, "\r\nQl_osSendEvent(1, %d, %d)=%d\r\n", iNum, iTmp, iRet);
                    }
                    else if(NULL != (p = Ql_strstr(pData, "TestMutex")))
                    {
                        s_iMutexId = Ql_osCreateMutex("MyMutex");
                        MY_DEBUG_OUT(ql_uart_port1, textBuf, "Ql_osCreateMutex(\"MyMutex\")=%d\r\n", s_iMutexId);
                        SendEvent2AllSubTask(789, 0);
                    }
                    else if(NULL != (p = Ql_strstr(pData, "TestSemaphore")))
                    {
                        s_iSemaphoreId = Ql_osCreateSemaphore("MySemaphore", s_iTestSemNum);
                        MY_DEBUG_OUT(ql_uart_port1, textBuf, "Ql_osCreateSemaphore(\"MySemaphore\", %d)=%d\r\n", s_iTestSemNum, s_iSemaphoreId);

                        s_iSemMutex= Ql_osCreateMutex("SemMutex");
                        MY_DEBUG_OUT(ql_uart_port1, textBuf, "Ql_osCreateMutex(\"SemMutex\")=%d\r\n", s_iSemMutex);

                        SendEvent2AllSubTask(999, 0);
                    }
                    else if(NULL != (p = Ql_strstr(pData, "Ql_SetLastErrorCode")))
                    {                                    //Ql_SetLastErrorCode=ErrorCode
                        p = Ql_strstr(p, "=");
                        if(p == NULL)
                        {
                            MY_DEBUG_OUT(ql_uart_port1, textBuf, "\r\nQl_SetLastErrorCode: parameter ERROR!\r\n");
                            break;
                        }

                        p++;
                        iNum = Ql_atoi(p);
                        iRet = Ql_SetLastErrorCode(iNum);
                        MY_DEBUG_OUT(ql_uart_port1, textBuf, "\r\nQl_SetLastErrorCode(%d)=%d\r\n", iNum, iRet);
                    }
                    else if(NULL != (p = Ql_strstr(pData, "Ql_GetLastErrorCode")))
                    {
                        iRet = Ql_GetLastErrorCode(&iNum);
                        MY_DEBUG_OUT(ql_uart_port1, textBuf, "\r\nQl_GetLastErrorCode: iRet = %d, ErrorCode=%d\r\n", iRet, iNum);
                    }
                    else if(NULL != (p = Ql_strstr(pData, "Ql_osGetCurrentTaskPriority")))
                    {
                        SendEvent2AllSubTask(777, 0);
                    }
                    else if(NULL != (p = Ql_strstr(pData, "Ql_osGetCurrenTaskRemainStackSize")))
                    {
                        SendEvent2AllSubTask(888, 0);
                    }
                    else if(NULL != (p = Ql_strstr(pData, "Ql_osChangeTaskPriority")))
                    {                                                 //Ql_osChangeTaskPriority=1,201
                        p = Ql_strstr(p, "=");
                        q = Ql_strstr(p, ",");

                        if((NULL == p)||(NULL == q))
                        {
                            MY_DEBUG_OUT(ql_uart_port1, textBuf, "\r\nQl_osChangeTaskPriority: Parameter format ERROR!\r\n");
                            break;
                        }

                        p++; (*q)=0; q++;
                        iNum = Ql_atoi(p);
                        iTmp = Ql_atoi(q);

                        if(iNum > MAX_TASK_NUM || iNum < ql_sub_task1 || iTmp > 255 || iTmp < 200)
                        {
                            MY_DEBUG_OUT(ql_uart_port1, textBuf, "\r\nQl_osChangeTaskPriority: Parameter value ERROR!\r\n");
                            break;
                        }

                        iRet = Ql_osChangeTaskPriority(iNum, iTmp);
                        Ql_DebugTrace("\r\----->Ql_osChangeTaskPriority(%d, %d)=%d\r\n", iNum, iTmp, iRet);

                    }
                    else
                    {
                        Ql_SendToModem(ql_md_port1, MyEvent.eventData.uartdata_evt.data, MyEvent.eventData.uartdata_evt.len);
                    }
                }
                break;
            }
            case EVENT_MSG:
            {
                Ql_DebugTrace("\r\nMain task recv MSG: SrcId=%d, Data1=%d, Data2=%d\r\n", MyEvent.eventData.msg_evt.src_taskid,\
                    MyEvent.eventData.msg_evt.data1, MyEvent.eventData.msg_evt.data2);
                break;
            }
            case EVENT_MODEMDATA:
            {
                Ql_SendToUart(ql_uart_port1, MyEvent.eventData.uartdata_evt.data, MyEvent.eventData.uartdata_evt.len);
                break;
            }
            default:
                break;
        }
    }
}


/**************************************************************
* the 1st sub task
***************************************************************/
QlEventBuffer    MyEvent_subtask1;  //  Set flSignalBuffer_subtask1 to global variables  may as well, otherwise it will occupy stack space
void example_subtask1_entry(s32 TaskId)
{
    bool keepGoing = TRUE;
    
    Ql_DebugTrace("multitask: example_task1_entry\r\n");

    while(keepGoing)
    {    
        Ql_GetEvent(&MyEvent_subtask1);
        switch(MyEvent_subtask1.eventType)
        {
            case EVENT_MSG:
            {
                if(MyEvent_subtask1.eventData.msg_evt.data1 == 789)
                {
                    MutextTest(1);
                }
                else if(MyEvent_subtask1.eventData.msg_evt.data1 == 999)
                {
                    SemaphoreTest(1);  
                }
                else if(MyEvent_subtask1.eventData.msg_evt.data1 == 777)
                {
                    GetCurrentTaskPriority(1);
                }
                else if(MyEvent_subtask1.eventData.msg_evt.data1 == 888)
                {
                    GetCurrenTaskRemainStackSize(ql_sub_task1);
                }
                else
                {
                    Ql_DebugTrace("\r\nSub task 1 recv MSG: SrcId=%d, Data1=%d, Data2=%d\r\n", \
                        MyEvent_subtask1.eventData.msg_evt.src_taskid, \
                        MyEvent_subtask1.eventData.msg_evt.data1, \
                        MyEvent_subtask1.eventData.msg_evt.data2);
                    if(s_iPassTask == MAX_TASK_NUM)
                    {
                        s_iPassTask = ql_main_task;
                    }
                    else
                    {
                        s_iPassTask++;
                    }
                    Ql_osSendEvent(s_iPassTask, MyEvent_subtask1.eventData.msg_evt.data1, MyEvent_subtask1.eventData.msg_evt.data2);
                }
                break;
            }

            default:
                break;
        }
    }    
}


/**************************************************************
* the 2nd sub task
***************************************************************/
QlEventBuffer    MyEvent_subtask2;  //  Set flSignalBuffer_subtask2 to global variables  may as well, otherwise it will occupy stack space
void example_subtask2_entry(s32 TaskId)
{
    bool keepGoing = TRUE;

    Ql_DebugTrace("multitask: example_task2_entry\r\n");

    while(keepGoing)
    {    
        Ql_GetEvent(&MyEvent_subtask2);
        switch(MyEvent_subtask2.eventType)
        {
            case EVENT_MSG:
            {
                 if(MyEvent_subtask2.eventData.msg_evt.data1 == 789)
                 {
                    MutextTest(2);
                 }
                 else if(MyEvent_subtask2.eventData.msg_evt.data1 == 999)
                 {
                    SemaphoreTest(2);  
                 }
                 else if(MyEvent_subtask2.eventData.msg_evt.data1 == 777)
                 {
                    GetCurrentTaskPriority(2);
                 }
                 else if(MyEvent_subtask2.eventData.msg_evt.data1 == 888)
                 {
                    GetCurrenTaskRemainStackSize(ql_sub_task2);
                 }
                 else
                 {
                    Ql_DebugTrace("\r\nSub task 2 recv MSG: SrcId=%d, Data1=%d, Data2=%d\r\n", \
                        MyEvent_subtask2.eventData.msg_evt.src_taskid, \
                        MyEvent_subtask2.eventData.msg_evt.data1, \
                        MyEvent_subtask2.eventData.msg_evt.data2);
                    
                    if(s_iPassTask == MAX_TASK_NUM)
                    {
                        s_iPassTask = ql_main_task;
                    }
                    else
                    {
                        s_iPassTask++;
                    }
                    Ql_osSendEvent(s_iPassTask, MyEvent_subtask2.eventData.msg_evt.data1, MyEvent_subtask2.eventData.msg_evt.data2);
                 }
                 break;
            }

            default:
                break;
        }
    }
}


/**************************************************************
* the 3nd sub task
***************************************************************/
QlEventBuffer    MyEvent_subtask3;  //  Set flSignalBuffer_subtask2 to global variables  may as well, otherwise it will occupy stack space
void example_subtask3_entry(s32 TaskId)
{
    bool keepGoing = TRUE;

    Ql_DebugTrace("multitask: example_task3_entry\r\n");

    while(keepGoing)
    {    
        Ql_GetEvent(&MyEvent_subtask3);
        switch(MyEvent_subtask3.eventType)
        {
            case EVENT_MSG:
            {
                 if(MyEvent_subtask3.eventData.msg_evt.data1 == 789)
                 {
                    MutextTest(3);
                 }
                 else if(MyEvent_subtask3.eventData.msg_evt.data1 == 999)
                 {
                    SemaphoreTest(3);  
                 }
                 else if(MyEvent_subtask3.eventData.msg_evt.data1 == 777)
                 {
                    GetCurrentTaskPriority(3);
                 }
                 else if(MyEvent_subtask3.eventData.msg_evt.data1 == 888)
                 {
                    GetCurrenTaskRemainStackSize(ql_sub_task3);
                 }
                 else
                 {
                    Ql_DebugTrace("\r\nSub task 3 recv MSG: SrcId=%d, Data1=%d, Data2=%d\r\n", \
                        MyEvent_subtask3.eventData.msg_evt.src_taskid, \
                        MyEvent_subtask3.eventData.msg_evt.data1, \
                        MyEvent_subtask3.eventData.msg_evt.data2);
                    
                    if(s_iPassTask == MAX_TASK_NUM)
                    {
                        s_iPassTask = ql_main_task;
                    }
                    else
                    {
                        s_iPassTask++;
                    }
                    Ql_osSendEvent(s_iPassTask, MyEvent_subtask3.eventData.msg_evt.data1, MyEvent_subtask3.eventData.msg_evt.data2);
                 }
                 break;
            }

            default:
                break;
        }
    }
}

/**************************************************************
* the 4nd sub task
***************************************************************/
QlEventBuffer    MyEvent_subtask4;  //  Set flSignalBuffer_subtask2 to global variables  may as well, otherwise it will occupy stack space
void example_subtask4_entry(s32 TaskId)
{
    bool keepGoing = TRUE;

    Ql_DebugTrace("multitask: example_task4_entry\r\n");

    while(keepGoing)
    {    
        Ql_GetEvent(&MyEvent_subtask4);
        switch(MyEvent_subtask4.eventType)
        {
            case EVENT_MSG:
            {
                 if(MyEvent_subtask4.eventData.msg_evt.data1 == 789)
                 {
                     MutextTest(4);
                 }
                 else if(MyEvent_subtask4.eventData.msg_evt.data1 == 999)
                 {
                    SemaphoreTest(4);  
                 }
                 else if(MyEvent_subtask4.eventData.msg_evt.data1 == 777)
                 {
                    GetCurrentTaskPriority(4);
                 }
                 else if(MyEvent_subtask4.eventData.msg_evt.data1 == 888)
                 {
                    GetCurrenTaskRemainStackSize(ql_sub_task4);
                 }
                 else
                 {
                    Ql_DebugTrace("\r\nSub task 4 recv MSG: SrcId=%d, Data1=%d, Data2=%d\r\n", \
                        MyEvent_subtask4.eventData.msg_evt.src_taskid, \
                        MyEvent_subtask4.eventData.msg_evt.data1, \
                        MyEvent_subtask4.eventData.msg_evt.data2);
                    
                    if(s_iPassTask == MAX_TASK_NUM)
                    {
                        s_iPassTask = ql_main_task;
                    }
                    else
                    {
                        s_iPassTask++;
                    }
                    Ql_osSendEvent(s_iPassTask, MyEvent_subtask4.eventData.msg_evt.data1, MyEvent_subtask4.eventData.msg_evt.data2);
                 }
                 break;
            }

            default:
                break;
        }
    }
}

/**************************************************************
* the 5nd sub task
***************************************************************/
QlEventBuffer    MyEvent_subtask5;  //  Set flSignalBuffer_subtask2 to global variables  may as well, otherwise it will occupy stack space
void example_subtask5_entry(s32 TaskId)
{
    bool keepGoing = TRUE;

    Ql_DebugTrace("multitask: example_task5_entry\r\n");

    while(keepGoing)
    {    
        Ql_GetEvent(&MyEvent_subtask5);
        switch(MyEvent_subtask5.eventType)
        {
            case EVENT_MSG:
            {
                 if(MyEvent_subtask5.eventData.msg_evt.data1 == 789)
                 {
                    MutextTest(5);
                 }
                 else if(MyEvent_subtask5.eventData.msg_evt.data1 == 999)
                 {
                    SemaphoreTest(5);  
                 }
                 else if(MyEvent_subtask5.eventData.msg_evt.data1 == 777)
                 {
                    GetCurrentTaskPriority(5);
                 }
                 else if(MyEvent_subtask5.eventData.msg_evt.data1 == 888)
                 {
                    GetCurrenTaskRemainStackSize(ql_sub_task5);
                 }
                 else
                 {
                    Ql_DebugTrace("\r\nSub task 5 recv MSG: SrcId=%d, Data1=%d, Data2=%d\r\n", \
                        MyEvent_subtask5.eventData.msg_evt.src_taskid, \
                        MyEvent_subtask5.eventData.msg_evt.data1, \
                        MyEvent_subtask5.eventData.msg_evt.data2);
                    
                    if(s_iPassTask == MAX_TASK_NUM)
                    {
                        s_iPassTask = ql_main_task;
                    }
                    else
                    {
                        s_iPassTask++;
                    }
                    Ql_osSendEvent(s_iPassTask, MyEvent_subtask5.eventData.msg_evt.data1, MyEvent_subtask5.eventData.msg_evt.data2);
                 }
                 break;
            }

            default:
                break;
        }
    }
}

/**************************************************************
* the 6nd sub task
***************************************************************/
QlEventBuffer    MyEvent_subtask6;  //  Set flSignalBuffer_subtask2 to global variables  may as well, otherwise it will occupy stack space
void example_subtask6_entry(s32 TaskId)
{
    bool keepGoing = TRUE;

    Ql_DebugTrace("multitask: example_task6_entry\r\n");

    while(keepGoing)
    {    
        Ql_GetEvent(&MyEvent_subtask6);
        switch(MyEvent_subtask6.eventType)
        {
            case EVENT_MSG:
            {
                 if(MyEvent_subtask6.eventData.msg_evt.data1 == 789)
                 {
                    MutextTest(6);
                 }
                 else if(MyEvent_subtask6.eventData.msg_evt.data1 == 999)
                 {
                    SemaphoreTest(6);  
                 }
                 else if(MyEvent_subtask6.eventData.msg_evt.data1 == 777)
                 {
                    GetCurrentTaskPriority(6);
                 }
                 else if(MyEvent_subtask6.eventData.msg_evt.data1 == 888)
                 {
                    GetCurrenTaskRemainStackSize(ql_sub_task6);
                 }
                 else
                 {
                    Ql_DebugTrace("\r\nSub task 6 recv MSG: SrcId=%d, Data1=%d, Data2=%d\r\n", \
                        MyEvent_subtask6.eventData.msg_evt.src_taskid, \
                        MyEvent_subtask6.eventData.msg_evt.data1, \
                        MyEvent_subtask6.eventData.msg_evt.data2);
                    
                    if(s_iPassTask == MAX_TASK_NUM)
                    {
                        s_iPassTask = ql_main_task;
                    }
                    else
                    {
                        s_iPassTask++;
                    }
                    Ql_osSendEvent(s_iPassTask, MyEvent_subtask6.eventData.msg_evt.data1, MyEvent_subtask6.eventData.msg_evt.data2);
                 }
                 break;
            }

            default:
                break;
        }
    }
}

/**************************************************************
* the 7nd sub task
***************************************************************/
QlEventBuffer    MyEvent_subtask7;  //  Set flSignalBuffer_subtask2 to global variables  may as well, otherwise it will occupy stack space
void example_subtask7_entry(s32 TaskId)
{
    bool keepGoing = TRUE;

    Ql_DebugTrace("multitask: example_task7_entry\r\n");

    while(keepGoing)
    {    
        Ql_GetEvent(&MyEvent_subtask7);
        switch(MyEvent_subtask7.eventType)
        {
            case EVENT_MSG:
            {
                 if(MyEvent_subtask7.eventData.msg_evt.data1 == 789)
                 {
                    MutextTest(7);
                 }
                 else if(MyEvent_subtask7.eventData.msg_evt.data1 == 999)
                 {
                    SemaphoreTest(7);  
                 }
                 else if(MyEvent_subtask7.eventData.msg_evt.data1 == 777)
                 {
                    GetCurrentTaskPriority(7);
                 }
                 else if(MyEvent_subtask7.eventData.msg_evt.data1 == 888)
                 {
                    GetCurrenTaskRemainStackSize(ql_sub_task7);
                 }
                 else
                 {
                    Ql_DebugTrace("\r\nSub task 7 recv MSG: SrcId=%d, Data1=%d, Data2=%d\r\n", \
                        MyEvent_subtask7.eventData.msg_evt.src_taskid, \
                        MyEvent_subtask7.eventData.msg_evt.data1, \
                        MyEvent_subtask7.eventData.msg_evt.data2);
                    
                    if(s_iPassTask == MAX_TASK_NUM)
                    {
                        s_iPassTask = ql_main_task;
                    }
                    else
                    {
                        s_iPassTask++;
                    }
                    Ql_osSendEvent(s_iPassTask, MyEvent_subtask7.eventData.msg_evt.data1, MyEvent_subtask7.eventData.msg_evt.data2);
                 }
                 break;
            }

            default:
                break;
        }
    }
}

/**************************************************************
* the 8nd sub task
***************************************************************/
QlEventBuffer    MyEvent_subtask8;  //  Set flSignalBuffer_subtask2 to global variables  may as well, otherwise it will occupy stack space
void example_subtask8_entry(s32 TaskId)
{
    bool keepGoing = TRUE;

    Ql_DebugTrace("multitask: example_task8_entry\r\n");

    while(keepGoing)
    {    
        Ql_GetEvent(&MyEvent_subtask8);
        switch(MyEvent_subtask8.eventType)
        {
            case EVENT_MSG:
            {
                 if(MyEvent_subtask8.eventData.msg_evt.data1 == 789)
                 {
                    MutextTest(8);
                 }
                 else if(MyEvent_subtask8.eventData.msg_evt.data1 == 999)
                 {
                    SemaphoreTest(8);  
                 }
                 else if(MyEvent_subtask8.eventData.msg_evt.data1 == 777)
                 {
                    GetCurrentTaskPriority(8);
                 }
                 else if(MyEvent_subtask8.eventData.msg_evt.data1 == 888)
                 {
                    GetCurrenTaskRemainStackSize(ql_sub_task8);
                 }
                 else
                 {
                    Ql_DebugTrace("\r\nSub task 8 recv MSG: SrcId=%d, Data1=%d, Data2=%d\r\n", \
                        MyEvent_subtask8.eventData.msg_evt.src_taskid, \
                        MyEvent_subtask8.eventData.msg_evt.data1, \
                        MyEvent_subtask8.eventData.msg_evt.data2);
                    
                    if(s_iPassTask == MAX_TASK_NUM)
                    {
                        s_iPassTask = ql_main_task;
                    }
                    else
                    {
                        s_iPassTask++;
                    }
                    Ql_osSendEvent(s_iPassTask, MyEvent_subtask8.eventData.msg_evt.data1, MyEvent_subtask8.eventData.msg_evt.data2);
                 }
                 break;
            }

            default:
                break;
        }
    }
}

/**************************************************************
* the 9nd sub task
***************************************************************/
QlEventBuffer    MyEvent_subtask9;  //  Set flSignalBuffer_subtask2 to global variables  may as well, otherwise it will occupy stack space
void example_subtask9_entry(s32 TaskId)
{
    bool keepGoing = TRUE;

    Ql_DebugTrace("multitask: example_task9_entry\r\n");

    while(keepGoing)
    {    
        Ql_GetEvent(&MyEvent_subtask9);
        switch(MyEvent_subtask9.eventType)
        {
            case EVENT_MSG:
            {
                 if(MyEvent_subtask9.eventData.msg_evt.data1 == 789)
                 {
                    MutextTest(9);
                 }
                 else if(MyEvent_subtask9.eventData.msg_evt.data1 == 999)
                 {
                    SemaphoreTest(9);  
                 }
                 else if(MyEvent_subtask9.eventData.msg_evt.data1 == 777)
                 {
                    GetCurrentTaskPriority(9);
                 }
                 else if(MyEvent_subtask9.eventData.msg_evt.data1 == 888)
                 {
                    GetCurrenTaskRemainStackSize(ql_sub_task9);
                 }
                 else
                 {
                    Ql_DebugTrace("\r\nSub task 9 recv MSG: SrcId=%d, Data1=%d, Data2=%d\r\n", \
                        MyEvent_subtask9.eventData.msg_evt.src_taskid, \
                        MyEvent_subtask9.eventData.msg_evt.data1, \
                        MyEvent_subtask9.eventData.msg_evt.data2);
                    
                    if(s_iPassTask == MAX_TASK_NUM)
                    {
                        s_iPassTask = ql_main_task;
                    }
                    else
                    {
                        s_iPassTask++;
                    }
                    Ql_osSendEvent(s_iPassTask, MyEvent_subtask9.eventData.msg_evt.data1, MyEvent_subtask9.eventData.msg_evt.data2);
                 }
                 break;
            }

            default:
                break;
        }
    }
}

/**************************************************************
* the 10nd sub task
***************************************************************/
QlEventBuffer    MyEvent_subtask10;  //  Set flSignalBuffer_subtask2 to global variables  may as well, otherwise it will occupy stack space
void example_subtask10_entry(s32 TaskId)
{
    bool keepGoing = TRUE;
    char buf[512];   //for test Ql_osGetCurrentTaskRemainSize
                    //buffer size must be proper,otherwise the module will reboot incessantly

    Ql_DebugTrace("multitask: example_task10_entry\r\n");

    Ql_memset(buf, 0, sizeof(buf));
    while(keepGoing)
    {    
        Ql_GetEvent(&MyEvent_subtask10);
        switch(MyEvent_subtask10.eventType)
        {
            case EVENT_MSG:
            {
                 if(MyEvent_subtask10.eventData.msg_evt.data1 == 789)
                 {
                    MutextTest(10);
                 }
                 else if(MyEvent_subtask10.eventData.msg_evt.data1 == 999)
                 {
                    SemaphoreTest(10);  
                 }
                 else if(MyEvent_subtask10.eventData.msg_evt.data1 == 777)
                 {
                    GetCurrentTaskPriority(10);
                 }
                 else if(MyEvent_subtask10.eventData.msg_evt.data1 == 888)
                 {
                    GetCurrenTaskRemainStackSize(ql_sub_task10);
                 }
                 else
                 {
                    Ql_DebugTrace("\r\nSub task 10 recv MSG: SrcId=%d, Data1=%d, Data2=%d\r\n", \
                        MyEvent_subtask10.eventData.msg_evt.src_taskid, \
                        MyEvent_subtask10.eventData.msg_evt.data1, \
                        MyEvent_subtask10.eventData.msg_evt.data2);
                    
                    if(s_iPassTask == MAX_TASK_NUM)
                    {
                        s_iPassTask = ql_main_task;
                    }
                    else
                    {
                        s_iPassTask++;
                    }
                    Ql_osSendEvent(s_iPassTask, MyEvent_subtask10.eventData.msg_evt.data1, MyEvent_subtask10.eventData.msg_evt.data2);
                 }
                 break;
            }
            default:
                break;
        }
    }
}


#endif // __EXAMPLE_MULTITASK__

