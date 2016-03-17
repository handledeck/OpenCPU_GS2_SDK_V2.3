/***************************************************************************
 *
 *            Quectel Open CPU
 *
 *           Copyright (c) 2009 The Quectel Ltd.
 *
 ***************************************************************************
 *
 *  Designed by    :  Jay xin
 *  Coded by       :   Jay xin
 *  Tested by      :
 *
 ***************************************************************************
 *
 * File Description
 * ----------------
 *
 *
 ***************************************************************************
 *
 ***************************************************************************/


/****************************************************************************
 * Nested Include Files
 ***************************************************************************/
#include "Ql_type.h"
#include "ql_appinit.h"


/*=========================================================================
 * Heap Configuration for Application
 *=========================================================================*/
/*
 * Heap size for application:
 * min value is 1, and the default value is 10K
 *----------------------------------------------------*/
#define QL_MEMORY_HEAP_SIZE (10*1024)
const u32 qlOpenCPUHeapMemSize = QL_MEMORY_HEAP_SIZE;
u8 qlOpenCPUHeapMem[QL_MEMORY_HEAP_SIZE];


/*=========================================================================
 * Configurations for Main Task
 *=========================================================================*/
/*
 * Stack size for main task:
 * range from 1K to 10K, and the default value is 4K.
 *----------------------------------------------------*/
#define QL_TASK_STACK_SIZE (4*1024)
const u32 qlMainTaskStackSize = QL_TASK_STACK_SIZE;
const u32 qlMainTaskPriority = 200;  // 200-255, main task priority. the smaller, the greater priority
const u32 qlMainTaskExtqsize = 10;   // 10-30, the length of message queue of main task 



/*=========================================================================
 * <<< Begin: Configurations for Subtasks
 *=========================================================================*/
//extern void ztp_subtask_1(s32 TaskId);
//extern void ztp_subtask_2(s32 TaskId);
//extern void tcp_subtask2_entry(s32 TaskId);
//extern void tcp_subtask3_entry(s32 TaskId);



/*
*--------------------------
*  Subtasks definition
*--------------------------
*   TaskStackSize:
*           The stack size of subtask. Range from 1K to 10K.
*           If there are any file operations to do in subtask, the stack size for this subtask must be set to at least 4K.
*   TaskPriority:
*           The subtask priority. Range from 200 to 255, the smaller, the greater priority.
*   TaskExtqsize:
*           The length of message queue of subtask. Range from 10 to 30.
*/
QlMutitask SubMutitaskArray[] =  
{
/*    {ztp_subtask_1, 1024, 200, 10},
    {ztp_subtask_2, 1024, 200, 10},*/
    {NULL, 0, 0, 0}, // KEEP THIS LINE, DON'T DO MODIFICATION

};

/*=========================================================================
 * End-of: Configurations for Subtasks >>>
 *=========================================================================*/

