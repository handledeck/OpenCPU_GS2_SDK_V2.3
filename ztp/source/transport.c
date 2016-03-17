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
u8 __waitConfirm=0;
u8 __socketBuffer[LEN_SOCKET_BUFFER];

#define OUTNET(x,...)\
    if(CURRENT!=nSTATE){\
     CURRENT=nSTATE;\
     OUTD((x),__VA_ARGS__);\
    }

s32 _error;
s8 _socketID=-1;
unsigned char _sendInput=0;



u8 _address[4] = {172,23,0,9};
u16 _port=10245;
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
            /*if (simcard>1) {
                OUTD("Sim card not ready.Code:%d.Device will be reboot",simcard);
                Ql_Sleep(300);
                Ql_Reset(0);
            }*/
           OUTNET("Device not ready.SIMCARD:%d.CREG:%d.CGREG:%d.Waiting...",simcard,creg,cgreg);
        }else{
            _error = Ql_GprsNetworkGetState(0, NULL, &ps_status);
              if(ps_status == 1) 
               {
                nSTATE = STATE_SET_APN; 
                OUTNET("GPRS network state OK.Code:%d",ps_status);
               }
              else OUTNET("GPRS network state error code:%d",ps_status);
        }
       break;
    }
    case STATE_SET_APN:{
        OUTD("Try set APN:%s Login:%s Pass:%s",__settings.APN,__settings.User,__settings.Pass);
        _error=Ql_GprsAPNSet(0,(u8*)__settings.APN,(u8*)__settings.User,(u8*)__settings.Pass,CbGprsAPNSet);
        if (_error) {
            nSTATE=STATE_WAIT_GPRS;
            OUTNET("!!!!!!!Error network set APN:%d",_error);
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
                OUTNET("-->Create socket status:OK",NULL);
                   
            }
            else{
                nSTATE=STATE_QUERY_NETWORKSTATUS;
                OUTNET("!!!!!!!Create socket status ERROR:%d",_socketID);
            }
            break;
        }
    case STATE_SOCKET_CONNECT:{
        nSTATE=STATE_WAIT_CALLBACK;
        OUTD("Try connect to address:%d.%d.%d.%d port:%d",__settings.IP[0],__settings.IP[1],__settings.IP[2],__settings.IP[3],__settings.TCP);
        Ql_SocketConnect(_socketID,__settings.IP,__settings.TCP);
        nSTATE = STATE_SOCKET_SEND; 
        break;
    }
    case STATE_SOCKET_SEND:{
        /*if (!_sendInput) {
            __toSend=BuildInputStateMessage();
            Ql_SocketSend(_socketID, &__SendBuffer[0], __toSend); 
            //__timeOutEvents=EVENT_SEND_TIMEOUT; 
            __toSend=0;
            _sendInput=1;
            nSTATE=STATE_SOCKET_RECIVE; 
            
            break;
        }*/
       
       
        if (__toSend>0) {
            
            OUTD("have message for sending.size:%d number packet:%d",__toSend,__numPacket);
            Ql_SocketSend(_socketID, &__SendBuffer[0], __toSend);
            Ql_osSendEvent(2,SYSTEM_MESSAGE_TIMER_EMPTY_RESET,0);
            
            nSTATE=STATE_SOCKET_RECIVE;
          }
       else  ReadEvents(); 
        //nSTATE=STATE_SOCKET_RECIVE;
        /*else if (__timeOutEvents<0) {
            //OUTD("Don't heve events.Send event NONE.Number packet:%d\r\n",__numPacket);
            TrigerEvent(None,0,&__currentEvData);            
            __toSend=BuildEventMessage(0,&__currentEvData,1);
            Ql_SocketSend(_socketID, &__SendBuffer[0], __toSend); 
            __timeOutEvents=EVENT_SEND_TIMEOUT; 
            __toSend=0;
            nSTATE=STATE_SOCKET_RECIVE;     
         }
        else
           //__timeOutEvents-=(DEVICE_CYCLE*10);
        //OUTD("Timeout value:%d\r\n",__timeOutEvents);*/

        break;
    }
    case STATE_SOCKET_RECIVE:{
        //nSTATE=STATE_SOCKET_RECIVE;
        lenRecv=Ql_SocketRecv(_socketID,(u8*)&__socketBuffer[0],LEN_SOCKET_BUFFER);
        
           if (lenRecv>0) {
               OUTD("Recive from server packet lenght:%d",lenRecv);
               unreplace(&__socketBuffer[0],lenRecv);
               //__toSend=0;
               //nSTATE=STATE_SOCKET_RECIVE;
               if (!__waitConfirm){
                   nSTATE = STATE_SOCKET_SEND; 
                   __toSend=0;
                   Ql_osSendEvent(1,SYSTEM_MESSAGE_OK,0);
               }
           }
           //else nSTATE=STATE_SOCKET_SEND;
       }
    }
}

void InitNetwork(void){
    Ql_GprsNetworkInitialize(0,0,&callback_func);
}

void CbGprsAPNSet(bool result, s32 error_code){
    if (result) {
        nSTATE=STATE_ACTIVE_PDP;    
        OUTNET("APN set OK",NULL);
    }
    else
    {
        nSTATE=STATE_QUERY_NETWORKSTATUS;
         OUTNET("!!!!!!APN set ERROR.Code:%d",error_code); 
         //Ql_Reset(0);
    }
}
void Cbnetwork_actived(u8 contexid){
    nSTATE=STATE_SOCKET_CREATE;
    OUTNET("Network activate status:OK",NULL);
    _sendInput=0;
    __numPacket=0;
} 
void Cbnetwork_deactived(u8 contexid,  s32 error_cause, s32 error){
    nSTATE=STATE_QUERY_NETWORKSTATUS;
    OUTNET("Network deactivated:coause:%d error:%d",error_cause,error);
    CURRENT=STATE_NONE;
}
void Cbsocket_connect(u8 contexid, s8 sock, bool result, s32 error){
    if (result) {
        
        Ql_osSendEvent(2,SYSTEM_MESSAGE_TIMER_EMPTY_START,0);
        
        nSTATE=STATE_SOCKET_SEND;
        OUTNET("Soccket connected OK result:%d error:%d", result,error); 
    }
    else{
        nSTATE=STATE_SOCKET_CREATE;
        Ql_osSendEvent(2,SYSTEM_MESSAGE_TIMER_EMPTY_STOP,0);
        OUTNET("Error connect.Code:%d",NULL);
       Ql_SocketClose(_socketID); 
    }
    
}
void Cbsocket_close(u8 contexid, s8 sock, bool result, s32 error){
    nSTATE=STATE_SOCKET_CREATE;   
    OUTD("Soccket closed.Try other connection",NULL);
}
void Cbsocket_read(u8 contexid, s8 sock, bool result, s32 error){
    //OUTD("Soccket read\r\n",NULL);
}
void Cbsocket_write(u8 contexid, s8 sock, bool result, s32 error){
    //OUTD("Soccket write\r\n",NULL);
}

