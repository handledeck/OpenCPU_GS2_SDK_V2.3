
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
#include "events.h" 
#include "mgpio.h"
#include "files.h"
#include "utils.h"
#include "socket.h"
#include "calendar.h"
#include "scheduler.h"

bool            __debug =FALSE;
bool            __log=FALSE;
u32             __toSend=0;
QlEventBuffer   __ebuf;
u16             __numPacket=0;
s32             __timeOutEvents=EVENT_SEND_TIMEOUT;
EventData       __currentEvData;
settings        __settings;
bool            __heap;
u64             __start_reg_net=0;
u32             __regnet_timeout=120*1000;
char            __version[10]="3.1\0";

bool RegisterNetwork(void);
void GetDateTimeGSM(char* data);

void ql_entry(void)
{
    
  //Ql_FileDelete(__file_logg);  
   QlTimer tmgprs;
   u32 idTm;
   tmgprs.timeoutPeriod=Ql_MillisecondToTicks(50);
   __start_reg_net=Ql_GetRelativeTime();
   SetModeGpio();   
   InitNetwork();
   Ql_SetUartDCBConfig(ql_uart_port3,9600,8,1,0);
   Ql_SetUartFlowCtrl(ql_uart_port3,FC_None,FC_None);
   OUTD("****Start system ver %s******",__version);
   
   SetDefaultSettins();
   PrintSettings();
   //Ql_osSendEvent(2,SYSTEM_MESSAGE_FILE_READ_SETTINGS,0);
   if (Ql_strstr(__settings.Debug, "TRUE"))
       __debug=TRUE;
   else __debug=FALSE;
    if (Ql_strstr(__settings.Log, "TRUE"))
       __log=TRUE;
   else __log=FALSE;
   LOG("****Start system ver %s******",__version);
   idTm=Ql_StartTimer(&tmgprs);
   //__heap=TRUE;
   Ql_StartWatchdog(500,90,0); 
   Ql_OpenModemPort(ql_md_port1);
   // out core version and sdk version
   u8 ssb[50];
   Ql_memset(&ssb[0],0,50);
   Ql_GetCoreVer(&ssb[0],50);
   OUTD("Core Ver:%s",&ssb[0]);
   Ql_memset(&ssb[0],0,50);
   Ql_GetSDKVer(&ssb[0],50);
   OUTD("SDK Ver:%s",&ssb[0]);
    while (TRUE) {
        Ql_GetEvent(&__ebuf);
        switch (__ebuf.eventType) {
        case EVENT_UARTDATA:{
            u8* pData;
            pData = (u8*)__ebuf.eventData.uartdata_evt.data;
            switch (__ebuf.eventData.uartdata_evt.port) 
            {
               case ql_uart_port1:
                 {
                     if (__ebuf.eventData.uartdata_evt.len>0)
                       {
                         char* cmd=Ql_strstr(pData, "$");
                         if (cmd)
                          {
                             OUTD("Try parse schedulers", NULL);
                             struct schedule_config sch_cfg;
                             s8 checker=read_schedule((u8*)cmd,&sch_cfg);
                             if (checker!=-1)
                             {
                                 //запись в файл расписаний
                                 OUTD("good",NULL);
                              }
                              else
                                   OUTD("error",NULL);
                            }
                        }
                        else
                        {
                            if (*pData==13 || *pData==10){
                                commandParce();
                            }
                            else 
                                fillBuffer(pData); 
                        }
                        break; 
                 }
                case ql_uart_port3:
                {
                    send_all_stream((u8*)pData,__ebuf.eventData.uartdata_evt.len);
                    break;
                }
            }
            break;
        }
        case EVENT_TIMER:{
            CheckStateGpio();
            GprsState();
			if (nSTATE==STATE_QUERY_NETWORKSTATUS) {
				if (RegisterNetwork()) {
                    Ql_Reset(0);
				}
			}
            
            idTm = Ql_StartTimer(&tmgprs); 
            break;
        }
        case EVENT_MODEMDATA:{
            s8* mdata=(s8*)__ebuf.eventData.modemdata_evt.data;
            if (Ql_strstr(mdata,"Call Ready")) {
                OUTD("Modem is ready. Try sync date time", NULL);
                Ql_SendToModem(ql_md_port1,"AT+QLTS\n",8);
            }
            else if (Ql_strstr(mdata,"AT+QLTS") && __ebuf.eventData.modemdata_evt.len>22) {
                char* pat=Ql_strstr(mdata,"+QLTS:");
                pat+=8;
                u8 dtl[22];
                Ql_memcpy(&dtl[0],pat,22);
                GetDateTimeGSM(dtl);
             }
            else
                OUTD("-->>%s", mdata);
           
            break; 
        }
       
       }
     Ql_FeedWatchdog();
    }
   }

void GetDateTimeGSM(char* data){
    QlSysTimer st;
    OUTD("%s", data);
    st.year=Ql_atoi(data);
    data+=3;
    st.month=Ql_atoi(data);
    data+=3;
    st.day=Ql_atoi(data);
    data+=3;
    st.hour=Ql_atoi(data);
    data+=3;
    st.minute=Ql_atoi(data);
    data+=3;
    st.second=Ql_atoi(data);
    data+=2;
    s32 zone_hour;
    s32 zone_min;
    char mark;
    Ql_memcpy(&mark,data,1);
    data++;
    zone_hour=Ql_atoi(data)/4;
    zone_min=Ql_atoi(data)%4;
    if (mark=='-') 
        zone_hour*=-1;
    calendar_date_to_tz(&st,zone_hour,zone_min);
     Ql_SetLocalTime(&st);
     st.minute+=1;
     //Ql_Alarm_StartUp(&st,0);
    
    
    //OUTD("%d.%d.%d %d:%d:%d zone:%d", year, month, date, hour, minute, sec,zone); 
}


bool RegisterNetwork(void){
	if (Ql_GetRelativeTime()-__start_reg_net>__regnet_timeout) {
        return TRUE;
	}
    else return FALSE;
}


/*QlEventBuffer esub1;

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
} */


