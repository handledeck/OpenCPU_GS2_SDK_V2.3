#include "ql_type.h"
#include "ql_stdlib.h"
#include "events.h"


#ifndef __FILES_H__
#define __FILES_H__



extern u8* __file_events;
extern u8* __file_logg;
extern u16 __confirmRecive;
extern EventData __bufEventData[];
extern u8 __buf_log[];

#define COUNT_EVENTS 10
#define COUNT_LINE_LOG 50
#define COUNT_LINES_LOG 140

typedef struct{
	char Message[COUNT_LINE_LOG];
}MessageLog;

extern MessageLog __MessageLog[];

void ReadLogFile(u16* len);
void WriteLog(char*message);
void WriteEvents(EventData* evdata);
bool CheckFiles(void);
s8 ReadEvents(void);
void ConfirmMsg(void);
void PrintMessage(void);

#endif 


