/*==========================================================================
 |               Quectel OpenCPU --  User API
 |
 |              Copyright (c) 2010 Quectel Ltd.
 |
 |--------------------------------------------------------------------------
 | File Description
 | ----------------
 |      Debug interface definition
 |
 |--------------------------------------------------------------------------
 |
 |  Designed by     :   
 |  Coded    by     :   
 |  Tested   by     :   Stanley YONG
 |
 \=========================================================================*/

#ifndef __QL_TRACE_H__
#define __QL_TRACE_H__

#include "ql_type.h"

typedef enum QlDebugModeTag
{
    BASIC_MODE,  /* In basic mode, debug messages from application will be 
                     output to debug serial port as text.
                     */
    ADVANCE_MODE /* Default mode.
                     In advance mode, debug messages from System and application
                     will be output to debug serial port in special format. Only
                     the Catcher Tool can capture and display these debug messages
                     legibly.
                     */
} QlDebugMode;


/*****************************************************************
* Function:     Ql_SetDebugMode 
* 
* Description:
*               Set debug mode.
*
* Parameters:
*               mode:
*                       Debug mode, please see 'QlDebugMode'
* Return:        
*               None
*****************************************************************/
void Ql_SetDebugMode(QlDebugMode mode);


/*****************************************************************
* Function:     Ql_DebugTrace 
* 
* Description:
*               This function prints formatted output to 
*               debug serial port. Its function is same to 'sprintf'.
*
* Parameters:
*               fmt:
*                   Pointer to a null-terminated multibyte string 
*                   specifying how to interpret the data.
*                   The maximum string length is 512 bytes.
* Return:        
*               Number of characters printed
*****************************************************************/
extern s32  (*Ql_DebugTrace)(char* fmt, ... );

#endif  // #nd-of __QL_TRACE_H__

