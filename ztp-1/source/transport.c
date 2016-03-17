#include "global.h"
#include "transport.h"
#include "ql_interface.h"
#include "Ql_tcpip.h"
#include "events.h"
#include "files.h"
#include "ql_timer.h"
#include "Ql_multitask.h"


OpenCpuTcpIp_Callback_t callback_func = 
{
    Cbnetwork_actived,
    Cbnetwork_deactived,
    Cbsocket_connect,
    Cbsocket_close,
    Cbsocket_read,
    Cbsocket_write
};


STATE_TCP nSTATE=STATE_QUERY_NETWORKSTATUS;
STATE_TCP CURRENT=STATE_NONE;
unsigned char _sendInput=0;
s32 _error;
s8 _socketID=-1;
u8  __waitConfirm=0;
u8  __socketBuffer[LEN_SOCKET_BUFFER];
u32 __empty_msg=0;
u32 __repeat_msg=0;
u32 _count_repeat=0;
s32 __simcard=0;
s32 __creg=0;
s32 __cgreg=0;
u8 __rssi=0;
u8 __ber=0;
u8 __local_ip_addr[4];

#define OUTNET(x,...)\
    if(CURRENT!=nSTATE){\
     CURRENT=nSTATE;\
     OUTD((x),__VA_ARGS__);\
    }

//u8 _address[4] = {172,23,0,9};
//u16 _port=10245;


void GprsState(){

   s32 lenRecv=0;
   //OUTD("nState:%d\r\n",nSTATE);
    switch (nSTATE) {
    case STATE_WAIT_CALLBACK:{
        OUTNET("Waiting callback function...",NULL);
        break;
    }
    case STATE_WAIT_GPRS:{
        nSTATE = STATE_SET_APN; 
        OUTNET("GPRS network waiting for attach",NULL);
        break;
    }
    case STATE_QUERY_NETWORKSTATUS:{
        u8 ps_status;
        s32 simcard;
        s32 creg;
        s32 cgreg;
        u8 rssi;
        u8 ber;
       
        Ql_GetDeviceCurrentRunState(&simcard, &creg, &cgreg, &rssi, &ber);
        if(simcard != 1 || creg != 1 || cgreg != 1){
            CheckReboot(FALSE);
           OUTNET("!Device not ready.SIMCARD:%d.CREG:%d.CGREG:%d.Waiting...",simcard,creg,cgreg);
           if (__simcard!=simcard) {
               OUTD("Change SIM card state:%d",simcard);
               __simcard=simcard;
           }
           if (__creg!=creg) {
               OUTD("Change network registion state:%d",creg);
               __creg=creg;
           }
            if (__cgreg!=cgreg) {
               OUTD("Change GPRS Network registion state:%d",cgreg);
               __cgreg=cgreg;
           }
            if (__rssi!=rssi) {
               OUTD("Change signal strength, unit in dBm:%d",rssi);
               __rssi=rssi;
           }
            if (__ber!=ber) {
               OUTD("Change bit error rate:%d",ber);
               __ber=ber;
           }
        }else{
            _error = Ql_GprsNetworkGetState(0, NULL, &ps_status);
              if(ps_status == 1) 
               {
                nSTATE = STATE_SET_APN; 
                OUTNET(">GPRS network state OK.Code:%d",ps_status);
               }
              else OUTNET("!GPRS network state error code:%d",ps_status);
        }
       break;
    }
    case STATE_SET_APN:{
        OUTD(">Try set APN:%s Login:%s Pass:%s",__settings.APN,__settings.User,__settings.Pass);
        _error=Ql_GprsAPNSet(0,(u8*)__settings.APN,(u8*)__settings.User,(u8*)__settings.Pass,CbGprsAPNSet);
        if (_error) {
            nSTATE=STATE_WAIT_GPRS;
            OUTNET("!Error network set APN:%d",_error);
            nSTATE=STATE_WAIT_CALLBACK;
        }
        else 
        {
            nSTATE=STATE_WAIT_CALLBACK;
        }
        break; 
    }
    case STATE_ACTIVE_PDP:{
        nSTATE=STATE_WAIT_CALLBACK;
        _error = Ql_GprsNetworkActive(0);
         break;
    }
    case STATE_SOCKET_CREATE:{
        _socketID = Ql_SocketCreate(0, SOC_TYPE_TCP); 
            if (_socketID>=0) {
                nSTATE=STATE_SOCKET_CONNECT; 
                OUTNET(">Create socket status:OK",NULL);
                   
            }
            else{
                nSTATE=STATE_QUERY_NETWORKSTATUS;
                OUTNET("!Create socket status ERROR:%d",_socketID);
            }
            break;
        }
    case STATE_SOCKET_CONNECT:{
        nSTATE=STATE_WAIT_CALLBACK;
        OUTD(">Try connect to address:%d.%d.%d.%d port:%d",__settings.IP[0],__settings.IP[1],__settings.IP[2],__settings.IP[3],__settings.TCP);
        Ql_SocketConnect(_socketID,__settings.IP,__settings.TCP);
        //nSTATE = STATE_SOCKET_SEND; 
        break;
    }
    case STATE_SOCKET_SEND:{
        
        if (__toSend>0) {

            OUTD("<Send server message:packet num:%d",__numPacket);
            __empty_msg=0;
            Ql_SocketSend(_socketID, &__SendBuffer[0], __toSend);
            nSTATE=STATE_SOCKET_RECIVE;
            return;
          }
        
        else{
            __empty_msg += DEVICE_CYCLE; 
           if (__empty_msg>__settings.TSend*TIME_SECOND) {
               __empty_msg=0;
               TrigerEvent(None,0,&__currentEvData);  
               __toSend=BuildEventMessage(0,&__currentEvData,1);
               OUTD("<No Events.Send to server empty message.",NULL);
            Ql_SocketSend(_socketID, &__SendBuffer[0], __toSend);
            nSTATE=STATE_SOCKET_RECIVE;
            return;
           }
        }
        ReadEvents(); 
        
        break;
    }
    case STATE_SOCKET_RECIVE:{
        CheckReboot(TRUE);
         lenRecv = Ql_SocketRecv(_socketID, (u8 *)&__socketBuffer[0], LEN_SOCKET_BUFFER); 
            if (lenRecv>0) {
                //OUTD("<<--Recive from server packet lenght:%d",lenRecv);
                unreplace(&__socketBuffer[0],lenRecv);
                if (!__waitConfirm){
                    nSTATE = STATE_SOCKET_SEND; 
                    __toSend=0;
                }
                 __countRepeat=0;
                 __repeat_msg=0;
               }
        break;
    }
    }
}

void CheckReboot(bool repeat_send){
    __repeat_msg+=DEVICE_CYCLE;
        if (__repeat_msg>TIME_REBOOT_WAIT) {
             __countRepeat++;
             if (repeat_send) {
                 OUTD("!Timeout recive any bytes.Try send one more.Repeate:%d", __countRepeat); 
                nSTATE = STATE_SOCKET_SEND; 
                //OUTD("__toSend:%d",__toSend);
             }
             __repeat_msg=0;
            if (__countRepeat>2) {
                __countRepeat=0;
                OUTD("!Timeout system recive message. System reboot", NULL);
                    Ql_Sleep(300); 
                    Ql_Reset(0);
            }
        }    
}

void InitNetwork(void){
    Ql_GprsNetworkInitialize(0,0,&callback_func);
}

void CbGprsAPNSet(bool result, s32 error_code){
    if (result) {
        nSTATE=STATE_ACTIVE_PDP;    
        OUTNET(">APN set OK",NULL);
    }
    else
    {
        nSTATE=STATE_QUERY_NETWORKSTATUS;
         OUTNET("!APN set ERROR.Code:%d",error_code); 
         //Ql_Reset(0);
    }
}
void Cbnetwork_actived(u8 contexid){
    s8 ret;
    //u8 ip_addr[4];
    nSTATE=STATE_SOCKET_CREATE;
    ret = Ql_GetLocalIpAddress(contexid, __local_ip_addr);
    OUTNET(">Network activate status:OK. Local IP Address:%d.%d.%d.%d",ret,__local_ip_addr[0],__local_ip_addr[1],__local_ip_addr[2],__local_ip_addr[3]);
    _sendInput=0;
    __numPacket=0;
} 
void Cbnetwork_deactived(u8 contexid,  s32 error_cause, s32 error){
    nSTATE=STATE_QUERY_NETWORKSTATUS;
    OUTNET("!Network deactivated:coause:%d error:%d",error_cause,error);
    CURRENT=STATE_NONE;
}
void Cbsocket_connect(u8 contexid, s8 sock, bool result, s32 error){
    if (result) {
        //Ql_osSendEvent(2,SYSTEM_MESSAGE_TIMER_EMPTY_START,0);
        OUTD(">Socket connected OK result:%d error:%d", result,error); 
        __toSend=BuildInputStateMessage();
        Ql_SocketSend(_socketID, &__SendBuffer[0], __toSend);
        TrigerEvent(None,0,&__currentEvData);  
        __toSend=BuildEventMessage(0,&__currentEvData,1);
        Ql_SocketSend(_socketID, &__SendBuffer[0], __toSend);
        nSTATE=STATE_SOCKET_RECIVE;
        OUTD("<Send server input state message...",NULL);
        //nSTATE=STATE_SOCKET_SEND;
        //
    }
    else{
        nSTATE=STATE_SOCKET_CREATE;
        //Ql_osSendEvent(2,SYSTEM_MESSAGE_TIMER_EMPTY_STOP,0);
        OUTNET("!Error connect.Code:%d",NULL);
       Ql_SocketClose(_socketID); 
    }
    
}
void Cbsocket_close(u8 contexid, s8 sock, bool result, s32 error){
    nSTATE=STATE_SOCKET_CREATE;   
    OUTD("!Soccket closed.Try other connection",NULL);
}
void Cbsocket_read(u8 contexid, s8 sock, bool result, s32 error){
    //OUTD("Soccket read",NULL);
}
void Cbsocket_write(u8 contexid, s8 sock, bool result, s32 error){
    //OUTD("Soccket write",NULL);
}

