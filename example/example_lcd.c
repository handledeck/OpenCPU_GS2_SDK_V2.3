#ifdef __EXAMPLE_LCD__
/***************************************************************************************************
*   Example:
*       
*           LCD Routine
*
*   Description:
*
*           This example gives an example for LCD setting.
*           Through Uart port, input the special command, there will be given the response about LCD operation.
*           
*
*   Usage:
*
*           Precondition:
*
*                   Use "make/make128 LCD" to compile, and download bin image to module.
*           
*           Through Uart port:
*
*               If input "s=bitshift,bitsize", that will subscribe bus function.
*               If input "u", that will unsubscribe bus function.
*               If input "c", that will write CMD to lcd.
*               If input "d",  that will write data to lcd.
*               If input "0", that will clear_reset lcd.
*               If input "1", that will set_reset lcd.
*           
****************************************************************************************************/ 
#include "ql_appinit.h"
#include "ql_interface.h"
#include "ql_type.h"
#include "ql_trace.h"
#include "ql_audio.h"
#include "ql_timer.h"
#include "ql_stdlib.h"
#include "ql_pin.h"
#include "ql_bus.h"
#include "ql_fcm.h"


QL_BUS_HANDLE busLcd = (QL_BUS_HANDLE)-1;

void ql_entry()
{
    bool           keepGoing = TRUE;
    QlEventBuffer  flSignalBuffer;
    s32 period = 10;
    char buffer[100];
    char *pData, *p;

    Ql_SetDebugMode(BASIC_MODE);   
    Ql_DebugTrace("lcd: ql_entry\r\n");
    Ql_OpenModemPort(ql_md_port1);       

     
    while(keepGoing)
    {
        Ql_GetEvent(&flSignalBuffer);
        switch(flSignalBuffer.eventType)
        {
            case EVENT_UARTDATA:
            {
                if (flSignalBuffer.eventData.uartdata_evt.len>0)
                {
                    s32 pin;
                    s32 mod = 0;
                    s32 pinpullenable = 0;
                    s32 pindirection = 0;
                    s32 pinlevel = 0;
                    s32 iret;

                    pData = (char*)flSignalBuffer.eventData.uartdata_evt.data;
                    p = Ql_strstr(pData, "at+");
                    if(p)
                    {
                        Ql_SendToModem(ql_md_port1, (u8*)pData, flSignalBuffer.eventData.uartdata_evt.len);
                        break;
                    }
                    
                    p = Ql_strstr(pData,"s=");
                    if (p)
                    {
                        QlBusParameter busparameter;
                        u8 bit_shift = 6, bit_size = QL_SERIAL_LCD_BITSIZE_8;
                        char *pComma = NULL;
                        busparameter.busconfigversion = QL_BUS_VERSION;
                        p += 2;
                        pComma = Ql_strstr(p, ",");
                        if(pComma)
                        {
                            *pComma = '\0';
                            pComma ++;
                            bit_shift = Ql_atoi(p);
                            bit_shift %= 9;
                            bit_size = Ql_atoi(pComma);
                        }
                        
                        #if 0
                        busparameter.busparameterunion.lcdparameter.brequire_cs = 1;
                        busparameter.busparameterunion.lcdparameter.brequire_resetpin = 1;
                        /*following selcet 3.25M clock, cs low valid, clock rising edge  sampling  data*/
                        busparameter.busparameterunion.lcdparameter.lcd_serial_config = 
                            QL_LCD_SERIAL_CONFIG_13MHZ_CLK | QL_LCD_SERIAL_CONFIG_CLOCK_DIVIDE_3
                            /*| QL_LCD_SERIAL_CONFIG_CS_POLARITY*/
                            | QL_LCD_SERIAL_CONFIG_8_BIT_MODE;
                            /*QL_LCD_SERIAL_CONFIG_CLOCK_PHASE | QL_LCD_SERIAL_CONFIG_CLOCK_POLARITY;*/
                        #else
                        busparameter.busparameterunion.lcd_serial_param_ex.rd_timing = 
                            busparameter.busparameterunion.lcd_serial_param_ex.wr_timing = 0x1<<bit_shift | ((0x1 << bit_shift) << 16);
                        busparameter.busparameterunion.lcd_serial_param_ex.bit_size = bit_size; //QL_SERIAL_LCD_BITSIZE_8;
                        busparameter.busparameterunion.lcd_serial_param_ex.pins[QL_LSCK] = QL_PINNAME_LSCK;
                        busparameter.busparameterunion.lcd_serial_param_ex.pins[QL_LSDA] = QL_PINNAME_LSDA;
                        busparameter.busparameterunion.lcd_serial_param_ex.pins[QL_LSA0] = QL_PINNAME_LSA0;
                        busparameter.busparameterunion.lcd_serial_param_ex.pins[QL_LSCS] = QL_PINNAME_LSCS;
                        busparameter.busparameterunion.lcd_serial_param_ex.pins[QL_LSRST] = QL_PINNAME_MAX;
                        #endif

                        if(busLcd > 0)
                        {
                            Ql_sprintf(buffer, "\r\nAlready Subscribe LCD, %x\r\n",busLcd);
                            Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  
                            break;
                        }
                        
                        busLcd = Ql_busSubscribe(QL_BUSTYPE_LCD, &busparameter);
                        if(busLcd < 0)                                    
                        {
                            Ql_sprintf(buffer, "\r\nSubscribe LCD Failed=%d\r\n",busLcd);
                            Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  
                            break;
                        }
                
                        Ql_sprintf(buffer, "\r\nSubscribe LCD succcess busLcd =%x\r\n", busLcd );
                        Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  
                        break;
                    }

                    p = Ql_strstr(pData,"u");
                    if (p)
                    {
                        iret = Ql_busUnSubscribe(busLcd);
                        busLcd = (QL_BUS_HANDLE)-1;
                        Ql_sprintf(buffer, "\r\nUnSubscribe(%d)\r\n",iret);
                        Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  
                        break;
                    }                        
                    p = Ql_strstr(pData,"c");
                    if (p)
                    {
                        QlBusAccess access;
                        u8 data[3] = {0x11,0x22,0xaa};
                        s32 cnt = 1000;
                        access.opcode = QL_BUSACCESSOPCODE_LCD_CMDWRITE;
                        while(cnt--)
                        {
                            iret = Ql_busWrite(busLcd, &access, data, 3);
                        }
                        Ql_sprintf(buffer, "\r\nLcd Write Cmd, ret=%d,busLcd=%x\r\n",iret,busLcd);
                        Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));
                        break;
                    }  
                    p = Ql_strstr(pData,"d");
                    if (p)
                    {
                        QlBusAccess access;
                        u8 data[3] = {0x11,0x22,0xaa};
                        s32 cnt = 1000;
                        access.opcode = QL_BUSACCESSOPCODE_LCD_DATAWRITE;
                        while(cnt--)
                        {
                            iret = Ql_busWrite(busLcd, &access, data, 3);
                        }
                        Ql_sprintf(buffer, "\r\nLcd Write Data, ret=%d,busLcd=%x\r\n",iret,busLcd);
                        Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  
                        break;
                    }  
                    p = Ql_strstr(pData,"t");
                    if (p)
                    {
                        QlBusAccess access;
                        u8 data[3] = {0x11,0x22,0xaa};
                        s32 cnt = 1000;
                        access.opcode = QL_BUSACCESSOPCODE_LCD_DATAWRITE;
                        while(cnt--)
                        {
                            access.opcode = access.opcode == QL_BUSACCESSOPCODE_LCD_CMDWRITE ?  QL_BUSACCESSOPCODE_LCD_DATAWRITE : QL_BUSACCESSOPCODE_LCD_CMDWRITE; 
                            iret = Ql_busWrite(busLcd, &access, data, 3);
                        }
                        Ql_sprintf(buffer, "\r\nLcd Write Cmd, ret=%d,busLcd=%x\r\n",iret,busLcd);
                        Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));
                        break;
                    }  
                    p = Ql_strstr(pData,"0");
                    if (p)
                    {
                        QlBusAccess access;
                        u8 data[3] = {0x11,0x22,0xaa};
                        access.opcode = QL_BUSACCESSOPCODE_LCD_CLEAR_RESET;
                        iret = Ql_busWrite(busLcd, &access, NULL, 0);
                        Ql_sprintf(buffer, "\r\nLcd Clean Reset, ret=%d,busLcd=%x\r\n",iret,busLcd);
                        Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  
                        break;
                    }  
                    p = Ql_strstr(pData,"1");
                    if (p)
                    {
                        QlBusAccess access;
                        u8 data[3] = {0x11,0x22,0xaa};
                        access.opcode = QL_BUSACCESSOPCODE_LCD_SET_RESET;
                        iret = Ql_busWrite(busLcd, &access, NULL, 0);
                        Ql_sprintf(buffer, "\r\nLcd Set Reset, ret=%d,busLcd=%x\r\n",iret,busLcd);
                        Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  
                        break;
                    }                    
                }
                break;
            }
            case EVENT_MODEMDATA:
                Ql_SendToUart(ql_uart_port1, flSignalBuffer.eventData.modemdata_evt.data,
                    flSignalBuffer.eventData.modemdata_evt.len);
                break;
            default:
                break;
        }
    }
}

#endif // __EXAMPLE_LCD__

