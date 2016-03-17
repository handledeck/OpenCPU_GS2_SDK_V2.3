/*==========================================================================
 |               Quectel OpenCPU --  User API
 |
 |              Copyright (c) 2010 Quectel Ltd.
 |
 |--------------------------------------------------------------------------
 | File Description
 | ----------------
 |      Definitions for multitask parameters & APIs.
 |
 |--------------------------------------------------------------------------
 |
 |  Designed by    :    Jay XIN
 |  Coded by       :    Jay XIN
 |  Tested by      :    Stanley YONG
 |
 \=========================================================================*/

#ifndef __QL_MULTITASK_H__
#define __QL_MULTITASK_H__

//#define CUSTOMER_AT_SUPPORT

/**************************************************************
 * Error Code Definition
 **************************************************************/
typedef enum {
    OS_SUCCESS,
    OS_ERROR,
    OS_Q_FULL,
    OS_Q_EMPTY,
    OS_SEM_NOT_AVAILABLE,
    OS_WOULD_BLOCK,
    OS_MESSAGE_TOO_BIG,
    OS_INVALID_ID,
    OS_NOT_INITIALIZED,
    OS_INVALID_LENGHT,
    OS_NULL_ADDRESS,
    OS_NOT_RECEIVE,
    OS_NOT_SEND,
    OS_MEMORY_NOT_VALID,
    OS_NOT_PRESENT,
    OS_MEMORY_NOT_RELEASE
} ql_os_error_enum;


/**************************************************************
 * Task ID Definition
 **************************************************************/
typedef enum QlTaskIDtag{
    ql_main_task,
#ifdef CUSTOMER_AT_SUPPORT
    ql_custom_AT_task,
#endif
    ql_sub_task1,
    ql_sub_task2,
    ql_sub_task3,
    ql_sub_task4,    
    ql_sub_task5,
    ql_sub_task6,
    ql_sub_task7,
    ql_sub_task8,    
    ql_sub_task9,
#ifndef CUSTOMER_AT_SUPPORT
    ql_sub_task10,
#endif
    ql_max_task
} QlTaskId;


/*****************************************************************
* Function:     Ql_osSendEvent 
* 
* Description:
*               Send event message between tasks.

* Parameters:
*               desttaskid: (0 - 10), 0 is for maintask id, 1-10 is for subtask id
*               data1:
*               data2:
* Return:        
*               OS_SUCCESS
*               OS_INVALID_ID
*               OS_MEMORY_NOT_VALID
*               OS_Q_FULL
*****************************************************************/
s32 Ql_osSendEvent(s32 desttaskid, u32 data1, u32 data2);


/*****************************************************************
* Function:     Ql_osCreateMutex 
* 
* Description:
*               Create a mutext with name.

* Parameters:
*               mutexname: Mutext Name
* Return:        
*               Mutex Id
*****************************************************************/
u32 Ql_osCreateMutex(char *mutexname);


/*****************************************************************
* Function:     Ql_osTakeMutex 
* 
* Description:
*               Obtain an instance of the specified MUTEX.

* Parameters:
*               mutexid: Mutext Id
* Return:        
*               None
*****************************************************************/
void Ql_osTakeMutex(u32 mutexid);


/*****************************************************************
* Function:     Ql_osGiveMutex 
* 
* Description:
*               Release the instance of the specified MUTEX.

* Parameters:
*               mutexid: Mutext Id
* Return:        
*               None
*****************************************************************/
void Ql_osGiveMutex(u32 mutexid);


/*****************************************************************
* Function:     Ql_osCreateSemaphore 
* 
* Description:
*               Creates a counting semaphore.

* Parameters:
*               semname: Name of semaphore
*               initial_count: Initial value of semaphore
* Return:        
*               Value of created semaphore
*****************************************************************/
u32 Ql_osCreateSemaphore(char *semname, u32 initial_count);


/*****************************************************************
* Function:     Ql_osTakeSemaphore 
* 
* Description:
*               Obtain an instance of the specified semaphore.

* Parameters:
*               semid: Name of semaphore
*               wait: 
*                   Indicate the task should wait infinitely or return immediately.
* Return:        
*               OS_SUCCESS if this function succeeds
*               OS_SEM_NOT_AVAILABLE  if this semaphore not available.
*****************************************************************/
u32 Ql_osTakeSemaphore(u32 semid, bool wait);


/*****************************************************************
* Function:     Ql_osGiveSemaphore 
* 
* Description:
*               Release the instance of the specified semaphore.

* Parameters:
*               semid: Name of semaphore
* Return:        
*               None
*****************************************************************/
void Ql_osGiveSemaphore(u32 semid);


/*****************************************************************
* Function:     Ql_SetLastErrorCode 
* 
* Description:
*               Set error code

* Parameters:
*               errorcode: Error code
* Return:        
*              True indicates success or falure indicates failure. 
*****************************************************************/
bool Ql_SetLastErrorCode(s32 errorcode);


/*****************************************************************
* Function:     Ql_GetLastErrorCode 
* 
* Description:
*               Get error code

* Parameters:
*               errorcode: Error code
* Return:        
*               True indicates success or falure indicates failure. 
*****************************************************************/
bool Ql_GetLastErrorCode(s32 *errorcode);


/*****************************************************************
* Function:     Ql_osGetCurrentTaskPriority 
* 
* Description:
*               Get the priority of the current task

* Parameters:
*               None
* Return:        
*               Task priority
*****************************************************************/
u32 Ql_osGetCurrentTaskPriority(void);


/*****************************************************************
* Function:     Ql_osGetCurrenTaskRemainStackSize 
* 
* Description:
*               Get the left number of bytes in the current task stack
* Parameters:
*               None
* Return:        
*               Number of bytes
*****************************************************************/
u32 Ql_osGetCurrenTaskRemainStackSize(void);


/*****************************************************************
* Function:     Ql_osChangeTaskPriority 
* 
* Description:
*               Change the priority of the specified task.
*               Developers can call this function in program run-time.
*               But caller must be very careful to avoid deadlock!
* Parameters:
*               desttaskid:  Task id
*               newpriority: Task priority, ranges from 200 to 255
* Return:        
*               If sucess, return privious priority
*               If fail,   return (u32)(-1)
*                           i.e 0xFFFFFFFF
*****************************************************************/
u32 Ql_osChangeTaskPriority(s32 desttaskid, u32 newpriority);

#endif  // End-of __QL_MULTITASK_H__

