#ifdef __EXAMPLE_FONT__
#include "ql_appinit.h"
#include "ql_interface.h"
#include "ql_type.h"
#include "ql_timer.h"
#include "ql_stdlib.h"
#include "ql_fcm.h"
#include "ql_trace.h"
#include "ql_font.h"


QlEventBuffer g_event; // Keep this variable a global variable due to its big size
char notes[100];

void ShowBitmap(char wd[2])
{
    s32 ret;
    s32 i, k, x =0;
    u8 fontBitmapData[50] = {0x0};
    u8 ch;

    ret = Ql_GetFontData(wd, 2, fontBitmapData);
    Ql_DebugTrace("Font length: %d\r\n", ret);
    if (ret > 0)
    {
        for (i = 0; i < ret; i++)
    	{
    		for (k = 0; k < 8; k++)
    		{
    			ch = (fontBitmapData[i] >> k) & 0x1;
    			if (ch)
    			{
    				Ql_DebugTrace("Q");
                    Ql_SendToUart(ql_uart_port1, (u8*)"Q", 1);
    			} else {
    				Ql_DebugTrace("-");
                    Ql_SendToUart(ql_uart_port1, (u8*)"-", 1);
    			}
    			x++;
    			if (x >= WORD_WIDTH)
    			{
    				Ql_DebugTrace("\r\n");
                    Ql_SendToUart(ql_uart_port1, (u8*)"\r\n", 2);
    				x = 0;
    			}
    		}
    	}
    	Ql_DebugTrace("\r\n");
        Ql_SendToUart(ql_uart_port1, (u8*)"\r\n", 2);
    }
}

void ql_entry(void)
{
    s32 ret;
    bool keepGoing = TRUE;

    Ql_SetDebugMode(BASIC_MODE);    /* Only after the device reboots, 
                                     *  the set debug-mode takes effect. 
                                     */
    Ql_DebugTrace("OpenCPU: Hello Font !\r\n");    /* Print out message through DEBUG port */
    Ql_OpenModemPort(ql_md_port1);  /* or ql_md_port2, two virtual modem ports are available.*/

    while(keepGoing)
    {
        Ql_GetEvent(&g_event);
        switch(g_event.eventType)
        {
            case EVENT_UARTDATA:
            {
                //TODO: receive and handle data from UART
                u8 temp;
                char* pChar;

                PortData_Event* pDataEvt = (PortData_Event*)&g_event.eventData.uartdata_evt;
                ShowBitmap((char*)pDataEvt->data);
            }
        }
    }
}

#endif // __EXAMPLE_HELLWORLD__

