#include "ql_type.h"
#include "ql_stdlib.h"
#include "events.h"

#ifndef __FILES_H__
#define __FILES_H__

extern u8* __file_events;
extern u16 __confirmRecive;
extern EventData __bufEventData[];
#define COUNT_EVENTS 10

void WriteEvents(EventData* evdata);
bool CheckFiles(void);
s8 ReadEvents(void);
void ConfirmMsg(void);
void PrintMessage(void);

#endif 


