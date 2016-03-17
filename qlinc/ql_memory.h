/*==========================================================================
 |               Quectel OpenCPU --  User API
 |
 |              Copyright (c) 2010 Quectel Ltd.
 |
 |--------------------------------------------------------------------------
 | File Description
 | ----------------
 |      Definitions for memory operations.
 |
 |--------------------------------------------------------------------------
 |
 |  Designed by    :    Jay XIN
 |  Coded by       :    Jay XIN
 |  Tested by      :    Stanley YONG
 |
 \=========================================================================*/

#ifndef __QL_MEMORY_H__
#define __QL_MEMORY_H__
#include "ql_type.h"

/*****************************************************************
* Function:     Ql_GetMemory 
* 
* Description:
*               Allocates memory with the specified size in the memory heap.
* Parameters:
*               Size: 
*                   Number of bytes of memory to be allocated.
* Return:        
*               A pointer of void type to the allocated space.
*****************************************************************/
void* Ql_GetMemory(u32 Size);


/*****************************************************************
* Function:     Ql_FreeMemory 
* 
* Description:
*               Free memory 
* Parameters:
*               Ptr: Previously allocated memory block to be freed. 
* Return:        
*               Success, QL_RET_OK
*               Failure, Error Code
*****************************************************************/
s32   Ql_FreeMemory(void *Ptr);


/*****************************************************************
* Function:     Ql_memMaxCanAllocSize 
* 
* Description:
*               Get the max memory free block that is allocable in SRAM
* Parameters:
*               None
* Return:        
*               Number of bytes
*****************************************************************/
u32 Ql_memMaxCanAllocSize(void);


/*****************************************************************
* Function:     Ql_memTotalLeftSize 
* 
* Description:
*               Get the total number of bytes of the free space in SRAM
* Parameters:
*               None
* Return:        
*               Number of bytes
*****************************************************************/
u32 Ql_memTotalLeftSize(void);

#endif  // End-of __QL_MEMORY_H__

