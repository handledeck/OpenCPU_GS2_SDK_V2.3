#include "ql_trace.h"
#include "ql_type.h"
#include "ql_timer.h"
#include "ql_stdlib.h"
#include "ql_fcm.h"
#include "files.h"

        

#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#define DEVICE_CYCLE 100
#define DEVICE_CONTROL_TRAFFIC 90000
#define DEVICE_CONTROL_EMPTY 30000
#define SYSTEM_MESSAGE_OK 500
#define SYSTEM_MESSAGE_EMPTY 501
#define SYSTEM_MESSAGE_TIMER_EMPTY_START 502
#define SYSTEM_MESSAGE_TIMER_EMPTY_STOP 503
#define SYSTEM_MESSAGE_SEND_STATE 504
#define SYSTEM_MESSAGE_TIMER_EMPTY_RESET 505
#define SYSTEM_MESSAGE_FILE_READ_SETTINGS 506
#define LEN_DEBUG_BUFFER 100
#define TIME_SYNC 1
extern char __debug_buffer[];
extern char __message_log[];

//extern QlTimer __tmFull;
//extern u32 __idTmFull;

extern bool __debug;
extern bool __log;
extern char __version[];

typedef struct {
    char Pwd[20];
    unsigned short Num;
    char APN[20];
    char User[20];
    char Pass[20];
    u32  TSend;
    u8 IP[4];
    u16 TCP;
    char Debug[6];
    u32 Deboung;
    char Log[6];
    char WUser[30];
    char WPass[30];
}settings;

extern settings __settings;
extern u8 __countRepeat;

void GetTextTime(void);
void GetFullTime(void);

//extern settings __settings;
//#define LOG(format,...)\
  //  if(__log){\
    //   Ql_sprintf(&__message_log[16],frm,__VA_ARGS__);\
      // WriteLog(__message_log);}\


#define OUTD(frm,...)\
    if (__debug) {\
    Ql_memset(&__debug_buffer[0],0,LEN_DEBUG_BUFFER);\
    GetTextTime();\
    Ql_sprintf(&__debug_buffer[10],frm,__VA_ARGS__);\
    Ql_SendToUart(ql_uart_port1,(u8*)__debug_buffer,Ql_strlen(__debug_buffer));\
    Ql_SendToUart(ql_uart_port1,(u8*)"\r\n",2);}

#define LOG(frm,...)\
    if(__log){\
    Ql_memset(&__message_log[0],0,COUNT_LINE_LOG);\
    GetFullTime();\
    Ql_sprintf(&__message_log[16],frm,__VA_ARGS__);\
    WriteLog(__message_log);}

 

/*#define CONSOLE(frm,...)\
    if(__debug){\
    Ql_memset(&__debug_buffer[0],0,LEN_DEBUG_BUFFER);\
    GetTextTime();\
    Ql_sprintf(&__debug_buffer[10],frm,__VA_ARGS__);\
    Ql_SendToUart(ql_uart_port1,(u8*)__debug_buffer,Ql_strlen(__debug_buffer));\
    Ql_SendToUart(ql_uart_port1,(u8*)"\r\n",2);}\*/
   

/*#define OUTER(frm,...)\  
    Ql_memset(&__debug_buffer[0],0,LEN_DEBUG_BUFFER);\
    GetTextTime();\
    Ql_sprintf(&__debug_buffer[10],frm,__VA_ARGS__);\
    Ql_SendToUart(ql_uart_port1,(u8*)__debug_buffer,Ql_strlen(__debug_buffer));\
    Ql_SendToUart(ql_uart_port1,(u8*)"\r\n",2);*/
   
   
#endif 

   


/* 
 //Ql_DebugTrace(__debug_buffer);\
  */
