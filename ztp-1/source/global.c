#include "ql_trace.h"
#include "ql_type.h"
#include "ql_timer.h"
#include "ql_stdlib.h"
#include "global.h"

char __debug_buffer[LEN_DEBUG_BUFFER];

void GetTextTime(){
    QlSysTimer tm;
    Ql_GetLocalTime(&tm);
    Ql_sprintf(&__debug_buffer[0],"%.2d:%.2d:%.2d %s",tm.hour,tm.minute,tm.second,"  ");
     
}

