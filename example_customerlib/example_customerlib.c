#include "ql_appinit.h"
#include "ql_interface.h"
#include "ql_type.h"
#include "ql_trace.h"
#include "ql_audio.h"
#include "ql_timer.h"
#include "ql_stdlib.h"


void Customer_Lib_Interface1(void* dest, u8 value, u32 size)
{
	Ql_memset(dest, value, size);
}


u32 Customer_Lib_Interface2(void)
{
	u32 i = 0;
	return i;
}





