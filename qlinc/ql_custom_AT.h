/***************************************************************************
 *
 *            Quectel OpenCPU
 *
 *           Copyright (c) 2009 The Quectel Ltd.
 *
*
 ***************************************************************************/
#ifndef __QL_CUSTOMAT_H__
#define __QL_CUSTOMAT_H__

#include "Ql_type.h"

typedef struct at_cmd_struct{
	u16 index;
	u16 src_id;  
	u8 cmd_mode; /*read £¬test £¬active£¬set*/
	u8 *string_ptr;
	u16 string_len;
}st_atcmd;


typedef void(*func_hdlr)( st_atcmd* strPtr);

typedef struct {
	u8*cmd;
	func_hdlr func;
}ST_CmdTbl;

typedef struct {
	u32 at_task;
	u32 confirm;
	const ST_CmdTbl * cmdTbl;
}ST_CustomAT;



void  OCPU_AT_RESULT_OK(u16 src);


void  OCPU_AT_RESULT_ERROR(u16 src, u16 err_code);


u16   OCPU_AT_RESULT_CONTINUE(u16 src, u8* buffer, u16 len);


void customer_AT_handler( st_atcmd* strPtr);
void customer_AT_handler2( st_atcmd* strPtr);
void customer_AT_Print( st_atcmd* strPtr);


#endif
