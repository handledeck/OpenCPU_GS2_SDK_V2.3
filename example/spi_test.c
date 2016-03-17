#ifdef __SPI_TEST__
 
#include "ql_appinit.h"
#include "ql_interface.h"
#include "ql_type.h"
#include "ql_trace.h"
#include "ql_stdlib.h"
#include "ql_timer.h"
#include "sdconfig.h"

QlEventBuffer g_event; // Keep this variable a global variable due to its big size
char notes[100];

void ql_spi_init(void)
{
    // clock
    SPI_SCK_GPIO();
    SPI_SCK_OUT();
    SPI_SCK_CLR();

    // MISO
    SPI_MISO_GPIO();
    SPI_MISO_IN();
    SPI_MISO_CLR();

    // MOSI
    SPI_MOSI_GPIO();
    SPI_MOSI_OUT();
    SPI_MOSI_CLR();

    // CS
    SPI_CS_GPIO();
    SPI_CS_OUT();
    SPI_CS_SET();
}

void ql_entry(void)
{
    s32 ret;
    bool keepGoing = TRUE;
    QlTimer tm;
    u32 cnt = 0;
    bool bClk = FALSE;
    u16 sdi;
    u8 sdo;
    u8 sdi_val;
    u16 i = 0;
    u8 rcvCnt = 0;
    u8 sndCnt = 0;
    char chSend[1];
    u8 arrMO[] = //{0,1,0,1,0,0,1};
    //{1,0,1,0,0,0,0,0,1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,1,0,0,1,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    /*A7B4*/{1,0,1,0,0,1,1,1,1,0,1,1,0,1,0,0};

    Ql_SetDebugMode(BASIC_MODE);    /* Only after the device reboots, 
                                                                         *  the set debug-mode takes effect. 
                                                                         */
    Ql_DebugTrace("OpenCPU: Hello SPI !\r\n\r\n");    /* Print out message through DEBUG port */
    Ql_OpenModemPort(ql_md_port1);  /* or ql_md_port2, two virtual modem ports are available.*/

    ql_spi_init();
    SPI_CS_CLR();
    
    // Start a timer
    tm.timeoutPeriod = Ql_MillisecondToTicks(10); /* Set the interval of timer */
    Ql_StartTimer(&tm);

#if 0    
    while (1)
    {
        // 1. Clock 拉高 & 写
        SPI_SCK_SET();
        for (i = 0; i < sizeof(arrMO); i++)
        {
            if (1 == arrMO[i])
            {
                sdo = SPI_MOSI_SET();
            }
            else
            {
                sdo = SPI_MOSI_CLR();
            }
            //i = (i < (sizeof(arrMO) - 1)) ? (i + 1) : 0;
            SPI_SCK_CLR();
            SPI_SCK_SET();
        }

        // 2. Clock 再拉低 & 读
        SPI_SCK_CLR();
        
        // 读SDI
        sdi = (u8)SPI_MISO_READ();
        //Ql_DebugTrace("%d", sdi);
        sdi_val = (sdi_val << 1) | sdi;
        rcvCnt++;
        if (8 == rcvCnt)
        {
            Ql_DebugTrace("%s", (char*)(&sdi_val));
            rcvCnt = 0;
        }
    }
#endif


    while(keepGoing)
    {
        Ql_GetEvent(&g_event);
        switch(g_event.eventType)
        {
            case EVENT_TIMER:
            {
                if (bClk)   // Clock现在是'高'，则拉'低'
                {
                    SPI_SCK_CLR();

                    // 读SDI
                    sdi = (u8)SPI_MISO_READ();
                    Ql_DebugTrace("%d", sdi);
                    sdi_val = (sdi_val << 1) | sdi;
                    rcvCnt++;
                    if (8 == rcvCnt)
                    {
                        Ql_DebugTrace("%s", (char*)(&sdi_val));
                        rcvCnt = 0;
                    }
                    SPI_MISO_CLR();
                }
                else    // Clock现在是'低'，则拉'高'
                {
                    u8 temp;
                    SPI_SCK_SET();

                    // 根据输出序列写SDO
                    if (1 == arrMO[i])
                    {
                        sdo = SPI_MOSI_SET();
                    }
                    else
                    {
                        sdo = SPI_MOSI_CLR();
                    }
                    temp = arrMO[i];
                    //Ql_memset(notes, 0x0, sizeof(notes));
                    //Ql_sprintf(notes, "%d ", temp);
                    //notes[2] = 0x0;
                    //Ql_SendToUart_2(ql_uart_port1, (u8*)notes, 2);
                    Ql_sprintf(chSend, "%d", temp);
                    Ql_SendToUart(ql_uart_port1, (u8*)chSend, 1);
                    i = (i < (sizeof(arrMO) - 1)) ? (i + 1) : 0;
                    #if 1
                    sndCnt++;
                    if (8 == sndCnt)
                    {
                        sndCnt = 0;
                        SPI_CS_SET();
                        SPI_SCK_CLR();
                        Ql_Sleep(20);
                        
                        SPI_CS_CLR();
                        SPI_SCK_SET();
                    }
                    #endif
                }
                bClk = !bClk;
                
                // Start the timer again
                Ql_StartTimer(&tm);
                break;
            }
#if 0            
            case EVENT_UARTDATA:
            {
                //TODO: receive and handle data from UART

                /* The following code segment is written to demonstrate the usage of 'AT+QLOCPU'.
                *   If send 'AT+QLOCPU=0' through serial port tool, and reboot the device, only Core 
                *   System software is booted (the application will not be booted).
                */
                PortData_Event* pDataEvt = (PortData_Event*)&g_event.eventData.uartdata_evt;
                Ql_sprintf(notes, "AT+QLOCPU=0\r\n");
                ret = Ql_strncmp((char*)pDataEvt->data, notes, Ql_strlen(notes));
                if (0 == ret)
                {
                    Ql_SendToModem(ql_md_port1, "AT+QLOCPU=0\n", Ql_strlen(notes) + 1);
                    break;
                }
                break;
            }
#endif            
            default:
                break;
        }
    }
}

#endif // __SPI_TEST__

