#ifdef __EXAMPLE_MULTITASK_PORT__
/***************************************************************************************************
*   Example:
*       
*           MULTITASK_PORT Routine
*
*   Description:
*
*           This example demonstrates how to use multitask_port function with APIs in OpenCPU.
*
*   Usage:
*
*           Compile & Run:
*
*               Use "make MULTITASK_PORT" to compile, and download bin image to module to run.
*           
*           Operation: 
*
*           Through MAIN/DEBUG/UART3  port, input data and the response message will be 
*           printed out through input port.
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
#include "ql_fcm.h"
#include "Ql_multitask.h"

s32 gvar1 = 3;
s32 gvar2 = 16;


/**************************************************************
* the main task
***************************************************************/
QlEventBuffer    flSignalBuffer; // Set flSignalBuffer to global variables  may as well, otherwise it will occupy stack space
void ql_entry()
{
    bool keepGoing = TRUE;
    s32 ret;

    Ql_SetDebugMode(BASIC_MODE);

    while(keepGoing)
    {    
        Ql_GetEvent(&flSignalBuffer);
        #if 0
        switch(flSignalBuffer.eventType)
        {    
            case EVENT_MODEMDATA:
                {                
                     Ql_DebugTrace("\r\n main task modem[%d] data =%s\r\n",flSignalBuffer.eventData.modemdata_evt.port,flSignalBuffer.eventData.modemdata_evt.data);        
                }
                break;
            case EVENT_UARTDATA:
                {        
                     Ql_DebugTrace("\r\n main task uart[%d] length=%d data =%s \r\n",flSignalBuffer.eventData.uartdata_evt.port-2,flSignalBuffer.eventData.uartdata_evt.len,flSignalBuffer.eventData.uartdata_evt.data);                            
                }
                break;                    
            default:
                break;
        }
            #endif
    }
}


u8 atbuffer[100];
QlEventBuffer    flSignalBuffer_subtask1;  //  Set flSignalBuffer_subtask1 to global variables  may as well, otherwise it will occupy stack space
void example_subtask1_entry(s32 TaskId)
{
    bool keepGoing = TRUE;
    s32 period = 10;
    char buffer[100];
    char *pData, *p;

    Ql_SetPortOwner(ql_uart_port1,ql_sub_task1);
    Ql_SetUartDCBConfig(ql_uart_port1, 115200, 8, 1, 0);
    Ql_UartConfigEscape(ql_uart_port1, '+', 1000); // to EVENT_UARTESCAPE

    while(keepGoing)
    {    
        Ql_GetEvent(&flSignalBuffer_subtask1);
        switch(flSignalBuffer_subtask1.eventType)
        {
            case EVENT_UARTDATA:
            {        
                                //
                 Ql_SendToUart(ql_uart_port1, (u8*)"\r\nECHO:\r\n", 9);        
                 Ql_SendToUart(ql_uart_port1, (u8*)flSignalBuffer_subtask1.eventData.uartdata_evt.data, flSignalBuffer_subtask1.eventData.uartdata_evt.len);        
                break;    
            }
            
            case EVENT_UARTESCAPE:
            {        
                //
                //notify  '+' will not report 3 EVENT_UARTDATA, because this is uart1
                Ql_SendToUart(ql_uart_port1, (u8*)"\r\nEVENT_UARTESCAPE uart1\r\n", 26);        
                break;    
            }
            
            default:
                break;
        }
    }
}


QlEventBuffer    flSignalBuffer_subtask2;  //  Set flSignalBuffer_subtask2 to global variables  may as well, otherwise it will occupy stack space
void example_subtask2_entry(s32 TaskId)
{
    bool keepGoing = TRUE;
    s32 period = 10;
    char buffer[100];
    char *pData, *p;
    bool bset;

    Ql_DebugTrace("multitask_port: subtask2\r\n");
    Ql_SetPortOwner(ql_uart_port2,ql_sub_task2);
    //Ql_SetUartDCBConfig(ql_uart_port2, 1200, 8, 1, 0);
    //Ql_UartConfigEscape(ql_uart_port2, '+', 1000); // to EVENT_UARTESCAPE

    //bset = Ql_UartSetGenericThreshold(ql_uart_port2, TRUE, 50, 50); 

    while(keepGoing)
    {    
        Ql_GetEvent(&flSignalBuffer_subtask2);
        switch(flSignalBuffer_subtask2.eventType)
        {
            case EVENT_UARTDATA:
            {
                Ql_SendToUart(ql_uart_port2, (u8*)"\r\nql_uart_port2 ECHO:\r\n", 23);        
                Ql_SendToUart(ql_uart_port2, (u8*)flSignalBuffer_subtask2.eventData.uartdata_evt.data, flSignalBuffer_subtask2.eventData.uartdata_evt.len);        
                break;                
            }

            case EVENT_UARTESCAPE:
            {
                //notify  '+' will report 3 EVENT_UARTDATA, because this is uart2
                Ql_SendToUart(ql_uart_port2, (u8*)"\r\nEVENT_UARTESCAPE uart2\r\n", 26);        
                break;    
            }
            
            case EVENT_UARTFE:
            {
                //notify uart error
                Ql_SendToUart(ql_uart_port2, (u8*)"\r\nEVENT_UARTFE uart2\r\n", 22);    
                Ql_sprintf(buffer, "\r\nport=%d,lsr=%x\r\n", flSignalBuffer_subtask2.eventData.uartfe_evt.port,flSignalBuffer_subtask2.eventData.uartfe_evt.data1);
                Ql_SendToUart(ql_uart_port2, (u8*)buffer, Ql_strlen(buffer));

                /*you must call Ql_UartGenericClearFEFlag to clear flag*/
                Ql_UartGenericClearFEFlag(ql_uart_port2);
                break;    
            }
            
            case EVENT_MODEMDATA:
            {
                //TODO: receive and hanle data from CORE through virtual modem port
                PortData_Event* pPortEvt = (PortData_Event*)&flSignalBuffer_subtask2.eventData.modemdata_evt;
                //Ql_DebugTrace("Modem Data at vPort [%d]: %s\r\n", pPortEvt->port, pPortEvt->data);
                Ql_DebugTrace("%s", pPortEvt->data);
                break;
            }
            
            default:
                break;
        }
    }
}


QlEventBuffer    flSignalBuffer_subtask3;  //  Set flSignalBuffer_subtask2 to global variables  may as well, otherwise it will occupy stack space
void example_subtask3_entry(s32 TaskId)
{
    bool           keepGoing = TRUE;
    s32 period = 10;
    char buffer[100];
    char *pData, *p;
    u32 arg_max_rx_len;
    u32 arg_max_tx_len;
    u32 arg_max_rx_alert_length;
    u32 arg_max_tx_alert_length;

    
    Ql_SetPortOwner(ql_uart_port3,ql_sub_task3);
    Ql_SetUartDCBConfig(ql_uart_port3, 1200, 8, 1, 0);
    Ql_UartConfigEscape(ql_uart_port3, '+', 1000);
    
    //Ql_UartMaxGetVfifoThresholdInfo(ql_uart_port3, &arg_max_rx_len, &arg_max_tx_len, &arg_max_rx_alert_length, &arg_max_tx_alert_length);
    //Ql_UartGetVfifoThresholdInfo(ql_uart_port3, &arg_max_rx_len, &arg_max_tx_len, &arg_max_rx_alert_length, &arg_max_tx_alert_length);
    //Ql_UartSetVfifoThreshold(ql_uart_port3, 2048, 3056, 10, 200);
    //Ql_UartGetVfifoThresholdInfo(ql_uart_port3, &arg_max_rx_len, &arg_max_tx_len, &arg_max_rx_alert_length, &arg_max_tx_alert_length);

    Ql_UartSetVfifoThreshold(ql_uart_port3, 2048, 3056, 30, 200);
   
    while(keepGoing)
    {    
        Ql_GetEvent(&flSignalBuffer_subtask3);
        switch(flSignalBuffer_subtask3.eventType)
        {
            case EVENT_MODEMDATA:
            {
                // BB return data, send to UART3
                //Ql_SendToUart(ql_uart_port3, flSignalBuffer_subtask3.eventData.uartdata_evt.data, flSignalBuffer_subtask3.eventData.uartdata_evt.len);        
                break;
            }
            
            case EVENT_UARTDATA:
            {
                //send to BB, goto run 
                //Ql_SendToModem(ql_md_port1, flSignalBuffer_subtask3.eventData.modemdata_evt.data, flSignalBuffer_subtask3.eventData.modemdata_evt.len);                            
                Ql_SendToUart(ql_uart_port3, (u8*)"\r\nql_uart_port3 ECHO:\r\n", 23);        
                Ql_SendToUart(ql_uart_port3, (u8*)flSignalBuffer_subtask3.eventData.uartdata_evt.data, flSignalBuffer_subtask3.eventData.uartdata_evt.len);        
                break;                
            }
            
            case EVENT_UARTESCAPE:
            {
                //notify  '+' will not report 3 EVENT_UARTDATA, because this is uart3
                 Ql_SendToUart(ql_uart_port3, (u8*)"\r\nEVENT_UARTESCAPE uart3\r\n", 26);        
                break;    
            }

            default:
                break;
        }
    }
    
}


QlEventBuffer    flSignalBuffer_subtask4; 
s8 Readbuffer[1024];
void example_subtask4_entry(s32 TaskId) // for VCOM1 test
{
    bool keepGoing = TRUE;
    s32 period = 10;
    char buffer[100];
    char *pData, *p;
    
    Ql_SetPortOwner(ql_usb_com_port1,ql_sub_task4);
    Ql_SetUartDCBConfig(ql_usb_com_port1, 115200, 8, 1, 0);
    Ql_UartConfigEscape(ql_usb_com_port1, '+', 1000); // to EVENT_UARTESCAPE

    Ql_SendToUart(ql_usb_com_port1, (u8*)"\r\nmultitask_port: subtask4:\r\n", 29);  
    while(keepGoing)
    {    
        Ql_GetEvent(&flSignalBuffer_subtask4);
        switch(flSignalBuffer_subtask4.eventType)
        {
            case EVENT_UARTDATA:
            {        
                s32 readedlen = 0;
                Ql_memset(Readbuffer, 0x00, EVENT_MAX_DATA);
                readedlen = Ql_UartDirectnessReadData(ql_usb_com_port1,Readbuffer,EVENT_MAX_DATA);
                Ql_SendToUart(ql_usb_com_port1, (u8*)"\r\nql_usb_com_port1 ECHO:\r\n", 26);        
                Ql_SendToUart(ql_usb_com_port1, Readbuffer, readedlen);      
                break;     
            }
            
            case EVENT_UARTESCAPE:
            {        
                //
                //notify  '+' will not report 3 EVENT_UARTDATA, because this is uart1
                Ql_SendToUart(ql_usb_com_port1, (u8*)"\r\nEVENT_UARTESCAPE VCOM1\r\n", 26);        
                break;    
            }
            
            default:
                break;
        }
    }
}

QlEventBuffer    flSignalBuffer_subtask5; 
void example_subtask5_entry(s32 TaskId) // for VCOM2 test
{
    bool keepGoing = TRUE;
    s32 period = 10;
    char buffer[100];
    char *pData, *p;
    
    Ql_SetPortOwner(ql_usb_com_port2,ql_sub_task5);
    Ql_SetUartDCBConfig(ql_usb_com_port2, 115200, 8, 1, 0);
    Ql_UartConfigEscape(ql_usb_com_port2, '+', 1000); // to EVENT_UARTESCAPE

    Ql_SendToUart(ql_usb_com_port2, (u8*)"\r\nmultitask_port: subtask5:\r\n", 29);  
    while(keepGoing)
    {    
        Ql_GetEvent(&flSignalBuffer_subtask5);
        switch(flSignalBuffer_subtask5.eventType)
        {
            case EVENT_UARTDATA:
            {        
                s32 readedlen = 0;
                Ql_memset(Readbuffer, 0x00, EVENT_MAX_DATA);
                readedlen = Ql_UartDirectnessReadData(ql_usb_com_port2,Readbuffer,EVENT_MAX_DATA);
                Ql_SendToUart(ql_usb_com_port2, (u8*)"\r\nql_usb_com_port2 ECHO:\r\n", 26);        
                Ql_SendToUart(ql_usb_com_port2, Readbuffer, readedlen);      
                break;     
            }
            
            case EVENT_UARTESCAPE:
            {        
                //
                //notify  '+' will not report 3 EVENT_UARTDATA, because this is uart1
                Ql_SendToUart(ql_usb_com_port2, (u8*)"\r\nEVENT_UARTESCAPE VCOM2\r\n", 26);        
                break;    
            }
            
            default:
                break;
        }
    }
}


#endif // __EXAMPLE_MULTITASK_PORT__

