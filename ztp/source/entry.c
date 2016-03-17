
#include "ql_appinit.h"
#include "ql_interface.h"
#include "ql_type.h"
#include "ql_stdlib.h"
#include "ql_trace.h"
#include "ql_timer.h"
#include "ql_fcm.h"
#include "global.h"
#include "transport.h"
#include "stdio.h"
#include "utils.h"
#include "events.h" 
#include "mgpio.h"
#include "files.h"
#include "utils.h"

bool __debug =TRUE;
u32 __toSend=10;
QlEventBuffer ebuf;
u16 __numPacket=0;
s32 __timeOutEvents=EVENT_SEND_TIMEOUT;
EventData __currentEvData;
settings __settings;


void ql_entry(void)
{
     
   QlTimer tmgprs;
   u32 idTm;
   tmgprs.timeoutPeriod=Ql_MillisecondToTicks(DEVICE_CYCLE);
  
   SetModeGpio();   
   InitNetwork();
   
   OUTD("/***********Enter main task************/",NULL);
   SetDefaultSettins();
   PrintSettings();
   Ql_osSendEvent(2,SYSTEM_MESSAGE_FILE_READ_SETTINGS,0);
   if (Ql_strstr(__settings.Debug, "TRUE"))
       __debug=TRUE;
   else __debug=FALSE;
   idTm=Ql_StartTimer(&tmgprs);
    
    while (TRUE) {
        Ql_GetEvent(&ebuf);
        switch (ebuf.eventType) {
        case EVENT_UARTDATA:{
            char* pData;
            pData = (char*)ebuf.eventData.uartdata_evt.data;
            if (*pData==13 || *pData==10){
               //Ql_StopTimer(&tmgprs);
               commandParce();
               //idTm=Ql_StartTimer(&tmgprs); 
            }
                else 
                fillBuffer(pData);
            
            
            //char* pData;
            //char* p;
            //u8 ip[]={172,23,0,9};
            
            //pData = (char*)ebuf.eventData.uartdata_evt.data;
            //TODO Парсер команд
            //ParseCommands(pData);
            //p = Ql_strstr(pData,"W");
            //if(p){
                 /*--DELETE AFTER ENDED WORK--*/
              //  __settings.APN="vpn2.mts.by";
                //__settings.Login="vpn";
                //Ql_memcpy(__settings.IP,ip,4);
                //__settings.TCP=10245;
                //__settings.Pass="gsd9drekj5";
                /*---------------------------*/  
    
                
                //OUTD("Try APN:%s Login:%s Pass:%s",__settings.APN,__settings.Login,__settings.Pass);
                //WriteSettings();
           // }
            /*p = Ql_strstr(pData,"c");
            if(p){
               //ConfirmMsg();
                QlPinLevel pinlevel;
                        Ql_pinRead(QL_PINNAME_NETLIGHT, &pinlevel);
                        OUTD("QL_PINNAME_NETLIGHT:%d\r\n",pinlevel);
            }*/
            
           /* p = Ql_strstr(pData,"p");
            if(p){
               PrintMessage();
            }
            p = Ql_strstr(pData,"R");
            if(p){
               GetTextStateGpio();
            }
            p = Ql_strstr(pData,"S");
            if(p){
                ReadEvents();
                //TrigerEvent(None,0,&__currentEvData);
                //__toSend=BuildEventMessage(__numPacket,0,&__currentEvData,1);
                //nSTATE=STATE_SOCKET_SEND;
                //__toSend=BuildInputStateMessage();
            }
             p = Ql_strstr(pData,"D");
            if(p){
               CheckFiles();
            }*/
            break;
        }
        case EVENT_TIMER:{
            if (ebuf.eventData.timer_evt.timer_id==idTm) {

                CheckStateGpio();
                GprsState();
                
                idTm=Ql_StartTimer(&tmgprs); 
            }
            break;
        }
        }
        
   }
    
}



QlEventBuffer esub1;

void ztp_subtask_1(){
    
    //OUTD("Enter to sub task 1",NULL);
    u32 IdsystemTimer;
    QlTimer systemTimer;
     systemTimer.timeoutPeriod=Ql_MillisecondToTicks(__settings.TSend*1000*3);
    IdsystemTimer=Ql_StartTimer(&systemTimer);
    while (TRUE) {
        Ql_GetEvent(&esub1);
        switch (esub1.eventType) {
        case EVENT_TIMER:{
            if (esub1.eventData.timer_evt.timer_id==IdsystemTimer) {
                
                if (__countRepeat<2) {
                    OUTD("Timeout traffic.Try send one more time.Count Repeate:%d", __countRepeat);
                    __countRepeat++;
                    if (__toSend==0) {
                        TrigerEvent(None,0,&__currentEvData);  
                     __toSend=BuildEventMessage(0,&__currentEvData,1);
                    }
                    nSTATE = STATE_SOCKET_SEND; 
                    IdsystemTimer=Ql_StartTimer(&systemTimer);    

                }
                else{
                    OUTD("Timeout system recive message. System reboot.", NULL);
                    Ql_Sleep(300); 
                    Ql_Reset(0);
                }
                
           }
            break;
       }
        case EVENT_MSG:{
            if (esub1.eventData.msg_evt.data1==SYSTEM_MESSAGE_OK) {
                OUTD("System traffic GOOD.Continue device work.",NULL);
                Ql_StopTimer(&systemTimer);
                IdsystemTimer=Ql_StartTimer(&systemTimer);    
            }
            if (esub1.eventData.msg_evt.data1==SYSTEM_MESSAGE_FILE_READ_SETTINGS) {
                OUTD("System traffic GOOD.Continue device work.",NULL);
                IdsystemTimer=Ql_StartTimer(&systemTimer);    
            }
            break; 
        }
        }
    }
}

QlEventBuffer esub2;

void ztp_subtask_2(){
    u32 IdTimerEmpty=0;
    QlTimer timerEmptyMsg;
    //OUTD("Enter to sub task 2.timer->%d",__settings.TSend*1000);
    
    timerEmptyMsg.timeoutPeriod=Ql_MillisecondToTicks(__settings.TSend*1000);
    while (TRUE) {
        Ql_GetEvent(&esub2);
        switch (esub2.eventType) {
        case EVENT_TIMER:{
            if (esub2.eventData.timer_evt.timer_id==IdTimerEmpty) {
                if (!__waitConfirm) {
                    OUTD("Time for send empty message", NULL); 
                      
                    TrigerEvent(None,0,&__currentEvData);  
                     nSTATE=STATE_SOCKET_SEND; 
                     __toSend=BuildEventMessage(0,&__currentEvData,1);
                     IdTimerEmpty = Ql_StartTimer(&timerEmptyMsg);
                }
           }
            break;
       }
        case EVENT_MSG:{
            if (esub2.eventData.msg_evt.data1==SYSTEM_MESSAGE_TIMER_EMPTY_START) {
                OUTD("Reset timer send empty message start",NULL);
                __toSend=BuildInputStateMessage();
                Ql_StopTimer(&timerEmptyMsg);
                IdTimerEmpty=Ql_StartTimer(&timerEmptyMsg);        
            }
            if (esub2.eventData.msg_evt.data1==SYSTEM_MESSAGE_TIMER_EMPTY_STOP) {
                OUTD("Reset timer send empty message stop",NULL);
                Ql_StopTimer(&timerEmptyMsg);
            }
            if (esub2.eventData.msg_evt.data1==SYSTEM_MESSAGE_TIMER_EMPTY_RESET) {
                OUTD("Reset timer send empty message",NULL);
                Ql_StopTimer(&timerEmptyMsg);
                IdTimerEmpty=Ql_StartTimer(&timerEmptyMsg);        
            }
            if (esub2.eventData.msg_evt.data1==SYSTEM_MESSAGE_SEND_STATE) {
                //OUTD("Reset timer stop",NULL);
                //Ql_StopTimer(&timerEmptyMsg);
            }
            if (esub1.eventData.msg_evt.data1==SYSTEM_MESSAGE_FILE_READ_SETTINGS) {
                OUTD("Set timer for sending empty message:%d sec",__settings.TSend*1000);
                timerEmptyMsg.timeoutPeriod=Ql_MillisecondToTicks(__settings.TSend*1000);
                IdTimerEmpty=Ql_StartTimer(&timerEmptyMsg);    
            }
            break; 
        }
        }
    }
}


