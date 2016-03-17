/*==========================================================================
 |               Quectel OpenCPU --  User API
 |
 |              Copyright (c) 2010 Quectel Ltd.
 |
 |--------------------------------------------------------------------------
 | File Description
 | ----------------
 |      Serial Port Interfaces
 |
 |--------------------------------------------------------------------------
 |
 |  Designed by     :   Willis YNAG, Jay XIN
 |  Coded    by     :   Willis YNAG, Jay XIN
 |  Tested   by     :   Stanley YONG
 |
 |--------------------------------------------------------------------------
 | Revision History
 | ----------------
 |  Sep. 11, 2010       Stanley Yong        Add API 'Ql_SetUartFlowCtrl'.
 |  ------------------------------------------------------------------------
 |  Sep. 25, 2010       Stanley Yong        Delete API 'Ql_SetUart1dataToQL'.
 |  ------------------------------------------------------------------------
 |  Sep. 26, 2010       Stanley Yong        Make the notes clear.
 |
 \=========================================================================*/

#ifndef __QL_FCM_H__
#define __QL_FCM_H__

#include "ql_type.h"
#include "ql_appinit.h"
#include "ql_multitask.h"

typedef enum {
      sb_One = 1,
      sb_Two,
      sb_OnePointFive
} Ql_UART_StopBit;

typedef enum {
      pb_none=0,
      pb_odd,
      pb_even,
      pb_space,
      pb_mark
} Ql_UART_ParityBit;

typedef enum{
    UART_BUF_NONE = 0,
    UART_BUF_RX = 1,
    UART_BUF_TX = 2,
    UART_BUF_END
}QL_UART_BUF_TYPE;

/*****************************************************************
* Function:     Ql_OpenModemPort 
* 
* Description:
*               Open the specified virtual modem serial port
*
* Parameters:
*               port:
*               [in]        virtual modem serial port number
* Return:        
*               TRUE, open successfully
*               FALSE, fail to open modem port
*****************************************************************/
bool Ql_OpenModemPort(QlPort port);


/*****************************************************************
* Function:     Ql_SendToModem 
* 
* Description:
*               Send data to the specified virtual modem serial port
*
* Parameters:
*               port:
*               [in]   virtual modem serial port number
*               data:
*               [in]       Pointer to data to send
*               data_len:
*               [in]       The length of data to send.  data_len <= 1024
* Return:        
*               The number of bytes actually sent;
*               If this function fails to send data, a negative 
*               number will be returned. To get extended information,
*               please see 'Error Code Definition'.
*****************************************************************/
s32  Ql_SendToModem(QlPort port, u8* data, u16 data_len);


/*****************************************************************
* Function:     Ql_SendToUart 
* 
* Description:
*               Send data to the specified UART port.
*               When the number of bytes actually sent is less than
*               that to send, App should stop sending data, and app will
*               receive an event -- EVENT_UARTREADY later. After receiving
*               this Event App can continue to send data, and previously unsent
*               datashould be resend.
* Parameters:
*               port:
*                       UART port
*               data:
*                       Pointer to data to send
*               data_len:
*                       The length of data to send,  in bytes,
*                       cannot be exceed 1024.
* Return:        
*               The number of bytes actually sent;
*               If this function fails to send data, a negative 
*               number will be returned. To get extended information,
*               please see 'Error Code Definition'.
*****************************************************************/
s32  Ql_SendToUart(QlPort port, u8* data, u16 data_len);


/*****************************************************************
* Function:     Ql_SetPortRts 
* 
* Description:
*               Set the RTS of the specified UART port.
*               If rts is set to 0, the virtual modem port or 
*               UART port will stop sending data to Embedded 
*               Application until rts is set to 1 again.
* Parameters:
*               port:
*                       UART port
*               rts:
*                       RTS enable
* Return:        
*               None
*****************************************************************/
void Ql_SetPortRts(QlPort port, bool rts);


/*****************************************************************
* Function:     Ql_SetUartBaudRate 
* 
* Description:
*               Set the baud rate of the specified UART port.
* Parameters:
*               port:
*                       UART port
*               rate:
*                       Baud rate (unit : bit per second)
* Return:        
*               QL_RET_OK, suceess
*               or other error codes, please see 'Error Code Definition'
*****************************************************************/
s32  Ql_SetUartBaudRate(QlPort port, s32 rate);


/*****************************************************************
* Function:     Ql_SetUartDCBConfig 
* 
* Description:
*               Set the baud rate, data bit, stop bit, and 
*               parity bit of the specified UART port.
* Parameters:
*               port:
*                       UART port
*               rate:
*                       Baud rate (unit : bit per second)
*               dataBits:
*                       Data bit
*               stopBits:
*                       Stop bit
*               parity:
*                       Parity bit
* Return:        
*               QL_RET_OK, suceess
*               or other error codes, please see 'Error Code Definition'
*****************************************************************/
s32  Ql_SetUartDCBConfig(QlPort port, s32 rate, s32 dataBits, s32 stopBits, s32 parity);


/*****************************************************************
* Function:     Ql_SetUartFlowCtrl 
* 
* Description:
*               Set flow-contrl mode of the specified Uart port.
*               And this setting just takes effect during run-time
*
* Parameters:
*               port:
*                       UART port
*
*               wrtFlowCtrl:
*                       Write flow control
*
*               rdFlowCtrl:
*                       Read flow control
* Return:        
*               None
*****************************************************************/
void Ql_SetUartFlowCtrl(QlPort port, Ql_FlowCtrlMode wrtFlowCtrl, Ql_FlowCtrlMode rdFlowCtrl);


/*****************************************************************
* Function:     Ql_SetPortOwner 
* 
* Description:
*               Set the owner (task) who will appropriate the specified UART port.
*
* Parameters:
*               port:
*                       UART port
*               id:
*                       Task Id
* Return:        
*               QL_RET_OK, suceess
*               or other error codes, please see 'Error Code Definition'
*****************************************************************/
s32  Ql_SetPortOwner(QlPort port, QlTaskId id);


/*****************************************************************
* Function:     Ql_UartGetBytesAvail 
* 
* Description:
*               Get the number of bytes of data in the receive buffer.
*
* Parameters:
*               port:
*                       UART port
* Return:        
*               The number of bytes of data in the receive buffer
*****************************************************************/
u16  Ql_UartGetBytesAvail(QlPort port);


/*****************************************************************
* Function:     Ql_UartGetTxRoomLeft 
* 
* Description:
*               Get the number of bytes of free space in the send buffer.
*
* Parameters:
*               port:
*                       UART port
* Return:        
*               The number of bytes of free space in the send buffer
*****************************************************************/
u16  Ql_UartGetTxRoomLeft(QlPort port);


/*****************************************************************
* Function:     Ql_UartGetTxRestBytes 
* 
* Description:
*               Gets the number of bytes not sent out in the send buffer.
*
* Parameters:
*               port:
*                       UART port
*
*               totalbufffer_size:
*                       [out] Pointer to the total number of bytes of 
*                       the send buffer. 
*                       This parameter may be set to NULL if don't be cared.
* Return:        
*               The number of bytes not sent out in the send buffer.
*****************************************************************/
u32  Ql_UartGetTxRestBytes(QlPort port, u32* totalbufffer_size);


/*****************************************************************
* Function:     Ql_UartConfigEscape 
* 
* Description:
*               Configure Escape function of specified UART port.
*
* Parameters:
*               port:
*                       UART port
*
*               escapechar:
*                       Escape character
*
*               escguardtime:
*                       Interval between Escape characters (the unit is ms)
* Return:        
*               TRUE  means configuretion successful
*               FALSE means configuretion failed
*****************************************************************/
bool Ql_UartConfigEscape(QlPort port, u8 escapechar, u16 escguardtime);


/*****************************************************************
* Function:     Ql_UartClrTxBuffer 
* 
* Description:
*               Clear send buffer of specified UART port.
*
* Parameters:
*               port:
*                       UART port
* Return:        
*               None
*****************************************************************/
void Ql_UartClrTxBuffer(QlPort port);


/*****************************************************************
* Function:     Ql_UartClrRxBuffer 
* 
* Description:
*               Clear receive buffer of specified UART port.
*
* Parameters:
*               port:
*                       UART port
* Return:        
*               None
*****************************************************************/
void Ql_UartClrRxBuffer(QlPort port);


/*****************************************************************
* Function:     Ql_UartSetGenericThreshold 
* 
* Description:
*               Set the threshold of receive buffer of specified UART port.
*               Now, this function just works for UART2 (Generic uart, not vfifo uart).
* Parameters:
*               port:
*                       UART port, only support UART2
*
*               benable:
*                       Enable flag
*
*               buffer_threshold:
*                       The number of bytes in the internal input buffer 
*                       before a Data Received event occurs;
*                       The maximum value is 2048.
*
*               waittimeout:
*                       If the number of bytes of arrived data has been
*                       less thanthe value of 'buffer_threshold' within
*                       the time of 'waittimeout', a Data Received event
*                       will occur as a result of expiring 'waittimeout'.
* Return:        
*               TRUE indicates suceess; otherwise failure
*****************************************************************/
bool Ql_UartSetGenericThreshold(QlPort port, bool benable, u32 buffer_threshold, u32 waittimeout);


/*****************************************************************
* Function:     Ql_UartGenericClearFEFlag 
* 
* Description:
*               Clear FE flag of specified UART port.
*               Now, this function just works for UART2.
*
*               Note: This function is called only when app receives
*                     the EVENT_UARTFE event, which indicates some
*                     transmission errors, such as frame error, parity
*                     error, happened.
* Parameters:
*               port:
*                       UART port,  only support UART2
* Return:        
*               None
*****************************************************************/
void Ql_UartGenericClearFEFlag(QlPort port);


/*****************************************************************
* Function:     Ql_UartSetVfifoThreshold 
* 
* Description:
*               Set the buffer size and the threshold in input buffer
*               and output buffer of specified VFIFO UART port.
*
*               Now, this function just works for UART1 and UART3.
*
* Parameters:
*               port:
*                       UART port, only support UART1 and UART3
*
*               rx_len:
*                       The size (number of bytes) of the input buffer
*                       of specified UART port
*
*               tx_len:
*                       The size (number of bytes) of the output buffer
*                       of specified UART port
*
*               rx_alert_length:
*                       The size (number of bytes) of the internal  
*                       input buffer before a Data Received event occurs;
*                       The maximum value is 2048.
*
*               tx_alert_length:
*                       The size (number of bytes) of the internal  
*                       output buffer before a Data Received event occurs
*                       The maximum value is 3584.
* Return:        
*               TRUE indicates suceess; otherwise failure.
*****************************************************************/
bool Ql_UartSetVfifoThreshold(QlPort port, u32 rx_len, u32 tx_len, u32 rx_alert_length, u32 tx_alert_length);

/*****************************************************************
* Function:     Ql_VfifoUart_SetDriverBuf 
* 
* Description:
*               Set the buffer size and the threshold in TX buffer and RX buffer of
*               specified VFIFO UART port.
*
*               Now, this function just works for UART1,UART2 and UART3.
*
* Parameters:
*               port:
*                       UART port, just support UART1 ,UART2 and UART3
*
*               buf_type:
*                       Buffer type, RX/TX buffer.please refer to QL_UART_BUF_TYPE.
*                       
*
*               buf_addr:
*                       Pointer to the RX/TX buffer address of specified UART port.
*                       NULL means the default value.
*                       
*
*               buf_len:
*                       The size (number of bytes) of the RX/TX buffer of specified UART port.
*                       0 means the default value,the maximum value is the size of buffer
*                       which is pointed to by the buf_addr.
*
*               ring_len:
*                       The trigger threshold of the RX/TX buffer of specified UART port.
*                       0 means the default value.Recommended value: buf_len *3/4
* Return:        
*               QL_RET_OK indicates suceess
*               QL_RET_ERR_FATAL indicates fatal error. 
*               QL_RET_ERR_INVALID_PARAMETER, indicates the invalid parameter.
*****************************************************************/
s32 Ql_VfifoUart_SetDriverBuf(
                                                    QlPort port,
                                                    QL_UART_BUF_TYPE buf_type,
                                                    u8* buf_addr,   /* NULL  -> default,ignore the latter two parameters*/
                                                    u32 buf_len,
                                                    u32 ring_len
                                                    );

/*****************************************************************
* Function:     Ql_UartGetVfifoThresholdInfo 
* 
* Description:
*               Get the buffer size and the threshold in input buffer
*               and output buffer of specified VFIFO UART port.
*
*               Now, this function just works for UART1 and UART3.
*
* Parameters:
*               port:
*                       UART port, only support UART1 and UART3.
*
*               arg_rx_len:
*                       [out] Pointer to the size (number of bytes) of
*                             the input buffer of specified UART port.
*
*               arg_tx_len:
*                       [out] Pointer to the size (number of bytes) of
*                             the output buffer of specified UART port.
*
*               arg_rx_alert_length:
*                       [out] Pointer to the size (number of bytes) of the internal  
*                             input buffer before a Data Received event occurs.
*
*               arg_tx_alert_length:
*                       [out] Pointer to the size (number of bytes) of the internal  
*                             output buffer before a Data Received event occurs.
* Return:        
*               TRUE indicates suceess; otherwise failure.
*****************************************************************/
bool Ql_UartGetVfifoThresholdInfo(QlPort port, u32 *arg_rx_len, u32 *arg_tx_len, u32 *arg_rx_alert_length, u32 *arg_tx_alert_length);
bool Ql_UartMaxGetVfifoThresholdInfo(QlPort port, u32 *arg_max_rx_len, u32 *arg_max_tx_len, u32 *arg_max_rx_alert_length, u32 *arg_max_tx_alert_length);

bool Ql_UartStartVfifoMonitorRxTimeout(QlPort port, u32 mstime);
bool Ql_UartStopVfifoMonitorRxTimeout(QlPort port);


/*****************************************************************
* Function:     Ql_UartRcvData 
* 
* Description:
*               Read data from specified physical serial port.
*               If the calling origin is in the EVENT_UARTDATA
*               event, caller should not call this function Before
*               handling the data about the EVENT_UARTDATA event.
* Parameters:
*               port:
*                       UART port
*
*               buffer_read:
*                       [out] Pointer to the read data, and the
*                             buffer size must great than or equal
*                             to 1024 bytes.
* Return:        
*               If >= 0, indicates success, and the return value 
*                        is the length of read data
*               If <  0, indicates failure to read
*****************************************************************/
#define Ql_UartDirectnessReadData(QLPORT, DATA_BUFFER, BUFFER_SIZE)  Ql_UartRcvData(QLPORT, DATA_BUFFER) // for compatibility with old name
s32  Ql_UartRcvData(QlPort port, u8* buffer_read);


/*****************************************************************
* Function:     Ql_UartQueryDataMode 
* 
* Description:
*               Query the working mode (Command Mode or Data Mode)
*               of specified virtual serial port.
* Parameters:
*               port:
*                       Virtal modem serial port
* Return:        
*               1 = data mode
*               0 = command mode
*              -1 = invalid port
*****************************************************************/
int  Ql_UartQueryDataMode(QlPort port);


/*****************************************************************
* Function:     Ql_UartForceSendEscape 
* 
* Description:
*               This function notifies the virtual serial port to
*               quit Data Mode, and return back to Command Mode.
* Parameters:
*               port:
*                       Virtal modem serial port
* Return:        
*               TRUE indicates success in calling this function;
*                   however, it doesn't mean the port has quit
*                   Data Mode and returned back to Command Mode.
*               FALSE is returned in the case of:
*                   1. invalid port;
*                   2. the input port is debug port;
*                    
*****************************************************************/
bool Ql_UartForceSendEscape(QlPort port);

#endif  // End-of __QL_FCM_H__

