
#ifndef __GPIO_H__
#define __GPIO_H__

#include "ql_type.h"
#include "ql_pin.h"

extern QlPinName __pinLow[];
extern unsigned char __pins;
void SetModeGpio(void);
void ReadStateGpio(void);
void CheckStateGpio(void);
char* GetTextStateGpio(void);
void GetInputDescripon(unsigned char pin,char level,unsigned char* state);
#endif 
