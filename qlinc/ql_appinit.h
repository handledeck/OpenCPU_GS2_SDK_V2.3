/*==========================================================================
 |               Quectel OpenCPU --  User API
 |
 |              Copyright (c) 2010 Quectel Ltd.
 |
 |--------------------------------------------------------------------------
 |
 | File Description
 | ----------------
 |      Definitions for application initialization parameters
 |
 |--------------------------------------------------------------------------
 |
 |  Designed by    :    Jay XIN
 |  Coded by       :    Jay XIN
 |  Tested by      :    Stanley YONG
 |
 \=========================================================================*/

#ifndef __QL_APPINIT_H__
#define __QL_APPINIT_H__

#include "ql_type.h"
#include "ql_pin.h"

#define MAX_TIMER_COUNT	10
#define EVENT_MAX_DATA		1024


typedef enum QlPortTag
{
	ql_md_port1,
	ql_md_port2,
	ql_uart_port1,
	ql_uart_port2,
	ql_uart_port3,
    ql_usb_com_port1,
	ql_usb_com_port2,
	ql_max_port
}QlPort;

typedef enum QlEventTypeTag           
{
	EVENT_NULL = 0,
	EVENT_INTR,
	EVENT_KEY,
	EVENT_UARTREADY,
	EVENT_UARTDATA,
	EVENT_MODEMDATA,
	EVENT_TIMER,
	EVENT_SERIALSTATUS,
	EVENT_MSG,
	EVENT_POWERKEY,
	EVENT_HEADSET,
	EVENT_UARTESCAPE,
	EVENT_UARTFE,           /* only support Generic uart, uart2 */
	EVENT_MEDIA_FINISH,
	EVENT_MAX = 0xFF
}QlEventType;

typedef struct Timer_EventTag          
{
   u32  timer_id;    
   s32  interval;    
}Timer_Event;

typedef struct Key_EventTag          
{
   s16       key_val;      
   bool   isPressed;    
}Key_Event;

typedef enum DATA_TYPETAG
{
    DATA_AT = 0,
    DATA_PPP,
    DATA_CSD,
    DATA_TCP_T,
    DATA_MAX
}QlDataType;

typedef struct PortData_EventTag          
{
   s16          len;        
   s8           data[EVENT_MAX_DATA];    
   QlPort       port;
   QlDataType   type; 
}PortData_Event;

typedef struct PortReady_EventTag
{
	QlPort port;
	u32    freeBuf;
}PortReady_Event;

typedef enum QlLineTypeTag
{
	DCD,
	CTS
}QlLineType;

typedef struct PortStatus_EventTag{
	u8 		   val;
	QlLineType type;
	QlPort 	   port;
}PortStatus_Event;

typedef struct Intr_EventTag          
{
    QlPinName       pinName;      
    bool      		pinState;
}Intr_Event;

typedef struct Msg_EventTag          
{
    s32     src_taskid;
    u32     data1;      
    u32     data2;
}Msg_Event;

typedef enum QlPowerKeyTypeTag
{
	POWERKEY_ON,
	POWERKEY_OFF
}QlPowerKeyType;

typedef struct Powerkey_EventTag          
{
    QlPowerKeyType  powerkey;
    bool            isPressed;    
}Powerkey_Event;

typedef enum QlHeadsetTypeTag
{
	HEADSET_PLUGOUT,
	HEADSET_PLUGIN,
	HEADSET_KEYPRESS,
	HEADSET_KEYRELEASE,
	HEADSET_ADC
}QlHeadsetType;

typedef struct Headset_EventTag          
{
    QlHeadsetType   headsettype;
    u32             data1;
}Headset_Event;

typedef struct UartEscape_EventTag          
{
    QlPort 	   port;
}UartEscape_Event;

typedef struct UartFE_EventTag          
{
    QlPort  port;
    u32     data1;
    u32     data2;
}UartFE_Event;

typedef enum tag_QlMediaType
{
    QlMedia_Play = 0,
    QlMedia_Record,
    QlMedia_Total
}QlMediaType;

typedef enum tag_QlMediaFinishReason
{
    QlMedia_Res_OK = 0,
    QlMedia_Res_Disc_Full,
    QlMedia_Res_No_Disc,
    QlMedia_Res_End_Of_File,
    QlMedia_Res_Decoder_Not_Support,
    QlMedia_Res_Terminated,
    QlMedia_Res_Error
}QlMediaFinishReason;

typedef struct MediaFinish_EventTag
{
    QlMediaType             media_type;
    QlMediaFinishReason     reason;
}MediaFinish_Event;

typedef union QlEventDataTag
{
    Timer_Event         timer_evt;   
    Key_Event           key_evt;
	PortReady_Event     uartReady_evt;
    PortData_Event      uartdata_evt;
    PortData_Event      modemdata_evt;	
    Intr_Event          intr_evt;
    PortStatus_Event    portstatus_evt;	
    Msg_Event           msg_evt;	
    Powerkey_Event      powerkey_evt;
    Headset_Event       headset_evt;
    UartEscape_Event    uartescape_evt;
    UartFE_Event        uartfe_evt;         /* only support Generic uart, uart2 */
    MediaFinish_Event   mediafinish_evt;
}QlEventData;

typedef struct FQlEventBufferTag
{
    QlEventType    eventType;
    QlEventData    eventData;
}
QlEventBuffer;

typedef struct QlMutitaskTag          
{
    void (*MultiTaskEntry)(s32 TaskId);  // Subtask Entrance Procedure
    u32  TaskStackSize;     // The stack size of subtask. Range from 1K to 10K. 
    u32  TaskPriority;      // The subtask priority. Range from 200 to 255, the smaller, the greater priority.
    u32  TaskExtqsize;      // The length of message queue of subtask. Range from 10 to 30.
}QlMutitask;

typedef struct QlRunInfoTag          
{
    bool debug_port_enable;
    s32 (*Fun_Ql_DebugTrace) (char *fmt, ... );
    void* vFunc;
}QlRunInfo;


/* Entry Point */
typedef struct qlEntryTag 
{
    void    (*qlmain)(QlRunInfo *p_runinfo);
    void    (*qlreginit)(void);
    u32     *OpenCPUHeapMemSize;
    u8      *OpenCPUHeapMem;
    u32     *MainTaskStackSize;
    u32     *MainTaskPriority;
    u32     *MainTaskExtqsize;
    void    *customerUserQlconfig;
    void    *customerPinConfigTable;    /* pointer to QlPinConfigTable_t */
    QlMutitask *QlMutitaskArray;
    u8      *OpenCPUFlag;
}qlEntry;


/****************************************************************************
 * Flow control mode
 ***************************************************************************/
typedef enum
{
    FC_None,
    FC_XonXoff,	// Software Flow Control
    FC_RTS			// Hardware Flow Control
} Ql_FlowCtrlMode;

typedef enum 
{
	Ql_API_None = 0,
	Ql_API_SendToUart,		// With Cache
	Ql_API_SendToUart_2,	// No Cache
}Ql_API_Send_Uart;

#endif  // End-of __QL_APPINIT_H__
