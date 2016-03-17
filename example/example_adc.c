#ifdef __EXAMPLE_ADC__
#include "ql_appinit.h"
#include "ql_interface.h"
#include "ql_type.h"
#include "ql_trace.h"
#include "ql_audio.h"
#include "ql_timer.h"
#include "ql_stdlib.h"
#include "ql_pin.h"
#include "ql_fcm.h"


void Callback_Read_ADC(QlADCPin adc_pin, u8 status, u16 adc_val);

QlEventBuffer  g_event;
void ql_entry()
{
    bool keepGoing = TRUE;
    char buffer[100];
    char *pData, *p;
    s32 ret;

    Ql_SetDebugMode(BASIC_MODE);   
    Ql_DebugTrace("OpenCPU: ADC\r\n");
     
    while(keepGoing)
    {    
        Ql_GetEvent(&g_event);
        switch(g_event.eventType)
        {
            case EVENT_UARTDATA:
            {
                PortData_Event* pPortEvt = (PortData_Event*)&g_event.eventData.uartdata_evt;
                QlPinName pin_name = QL_PINNAME_MAX;
                s32 iret;
                
                pData = (char*)pPortEvt->data;    

                // Read ADC (feed content: adc<0/1>?)
                p = Ql_strstr(pData, "adc<");
                if (p)
                {
                    QlADCPin adcPin;
                    Ql_DebugTrace(pData);
                    if (Ql_strcmp(pData, "adc<0>?\r\n") == 0)
                    {
                        adcPin = QL_PIN_ADC0;
                    }
                    else if (Ql_strcmp(pData, "adc<1>?\r\n") == 0)
                    {
                        adcPin = QL_PIN_ADC1;
                    }
                    else
                    {
                        Ql_DebugTrace("No adc pin matches!\r\n");
                        break;
                    }
                    ret = Ql_ReadADC(adcPin, Callback_Read_ADC);
                    Ql_DebugTrace("Ql_ReadADC(adcpin=%d,...) = %d\r\n", (u8)adcPin, ret);
                    break;
                }
            }
                
            default:
                break;
        }
    }
}

void Callback_Read_ADC(QlADCPin adc_pin, u8 status, u16 adc_val)
{
    Ql_DebugTrace("adc: %d, status: %d, adcValue: %d\r\n", adc_pin, status, adc_val);
}

#endif // __EXAMPLE_ADC__

