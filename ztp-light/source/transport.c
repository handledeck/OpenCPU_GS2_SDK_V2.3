#include "Ql_stdlib.h"
#include "global.h"
#include "transport.h"
#include "ql_interface.h"
#include "Ql_tcpip.h"
#include "events.h"
#include "files.h"
#include "ql_timer.h"
#include "Ql_multitask.h"
#include "mgpio.h"
#include "websrv.h"
#include "socket.h"
#include "ql_fota.h"


OpenCpuTcpIp_Callback_t callback_func = 
{
    CallBack_network_actived,
    CallBack_network_deactived,
    CallBack_socket_connect,
    CallBack_socket_close,
    CallBack_socket_accept,
    CallBack_socket_read,
    CallBack_socket_write
};



STATE_TCP nSTATE=STATE_QUERY_NETWORKSTATUS;
STATE_TCP CURRENT=STATE_NONE;
unsigned char _sendInput=0;
s32 _error;
s8 __http_socketID=-1;
s8 __est_connector_socket=-1;
u8  __waitConfirm=0;
u8  __socketBuffer[LEN_SOCKET_BUFFER];
u32 _count_repeat=0;
s32 __simcard=0;
s32 __creg=0;
s32 __cgreg=0;
u8 __rssi=0;
u8 __ber=0;
u8 __local_ip_addr[4];
char __str_loc_ip_addr[15];
u32 __since_time=0;
u16 __tmr_array[6]={500,1500,3000,6000,15000};
u8  __tmr_index=0;
u64 __start_empty_tmr=0;
bool __est_connection=FALSE;

#define OUTNET(x,...)\
    if(CURRENT!=nSTATE){\
     CURRENT=nSTATE;\
     OUTD((x),__VA_ARGS__);\
     if(__log){LOG((x), __VA_ARGS__);}}\
     


//u8 _address[4] = {172,23,0,9};
//u16 _port=10245;


void GprsState(){

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
           //LOG("!SIMCARD:%d.CREG:%d.CGREG:%d.",simcard,creg,cgreg); 
           OUTNET("!SIMCARD:%d.CREG:%d.CGREG:%d.",simcard,creg,cgreg);
           if (__simcard!=simcard) {
               OUTD("Change SIM card state:%d",simcard);
               LOG("Change SIM card state:%d",simcard);
               __simcard=simcard;
           }
           if (__creg!=creg) {
               OUTD("Change network registion:%d",creg);
               LOG("Change network registion:%d",creg);
               __creg=creg;
           }
            if (__cgreg!=cgreg) {
               OUTD("Change GPRS Network registion:%d",cgreg);
               LOG("Change GPRS registion:%d",cgreg);
               __cgreg=cgreg;
           }
            if (__rssi!=rssi) {
               OUTD("Change signal strength, unit in dBm:%d",rssi);
               LOG("Change signal unit dBm:%d",rssi);
               __rssi=rssi;
           }
            if (__ber!=ber) {
               OUTD("Change bit error rate:%d",ber);
               LOG("Change bit error:%d",ber);
               __ber=ber;
           }
        }else{
            _error = Ql_GprsNetworkGetState(0, NULL, &ps_status);
              if(ps_status == 1) 
               {
                nSTATE = STATE_SET_APN; 
                OUTNET(">GPRS network OK.Code:%d",ps_status);
                //LOG(">GPRS network OK.Code:%d",ps_status);
               }
              else{
                   OUTNET("!GPRS network error code:%d",ps_status);
                   //LOG("!GPRS network error code:%d",ps_status);
               }
        }
       break;
    }
    case STATE_TIME_SYNC:{

        break;
    }
    case STATE_SET_APN:{
        OUTD(">Try set APN:%s Login:%s Pass:%s",__settings.APN,__settings.User,__settings.Pass);
        LOG(">Try set APN:",NULL);
        _error=Ql_GprsAPNSet(0,(u8*)__settings.APN,(u8*)__settings.User,(u8*)__settings.Pass,CallBack_GprsAPNSet);
        if (_error) {
            nSTATE=STATE_WAIT_GPRS;
            //OUTNET("!Error network set APN:%d",_error);
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
    case STATE_EST_SOCKET_CREATE:{
        if (!__est_connection) {
        __est_connector_socket = Ql_SocketCreate(0, SOC_TYPE_TCP); 
        if (__est_connector_socket>=0) {
                nSTATE=STATE_SOCKET_CONNECT;
                OUTD(">EST socket:OK",NULL);
                //LOG(">EST socket:OK",NULL);
            }
            else{
                nSTATE=STATE_EST_SOCKET_CREATE;
                OUTD("!EST socket error:%d",__est_connector_socket);
                LOG("!EST socket error:%d",__est_connector_socket);
            }
        }
        break; 
    }
    case STATE_SOCKET_CREATE:{
        
        __http_socketID = Ql_SocketCreate(0, SOC_TYPE_TCP); 
            if (__http_socketID>=0) {
                nSTATE=STATE_LISTEN; 
                OUTNET(">EST socket:OK",NULL);
                   //LOG(">EST socket:OK",NULL);
            }
            else{
                nSTATE=STATE_QUERY_NETWORKSTATUS;
                OUTNET("!EST socket error:%d",__http_socketID);
                //LOG("!EST socket error:%d",__http_socketID);
            }
            break;
        }
    case STATE_LISTEN:{
        s32 ret;
        
        ret=Ql_SocketListen(__http_socketID,__local_ip_addr,80,10);
		if (ret<0) {
            OUTD("Error listen port:%d",ret);
            LOG("Http port error:%d",ret);
            nSTATE=STATE_LISTEN;
		}
        else{
            OUTD("Listen port OK:",NULL);
           LOG("Http port OK:",NULL);
              //nSTATE=STATE_ING;  
            nSTATE=STATE_EST_SOCKET_CREATE;
            client_socketId_init();
            //OUTD("client_socketId_init OK:",NULL);
        }
		break; 
    }
    case STATE_SOCKET_CONNECT:{
        //QlSysTimer tmr;
       /* u16 a_port=0;
        s8 loc_socket=-1;*/
        s32 err;
        nSTATE=STATE_ING;
            err=Ql_SocketConnect(__est_connector_socket,__settings.IP,__settings.TCP);
			//if (err!=0) {
                //OUTD("Error EST connect:%d",err);
                //LOG("Error EST connect:%d",err);
			//}
			break; 
    }
    case STATE_SOCKET_SEND:{
        if (__toSend>0) {

            OUTD("<Send message:num:%d",__numPacket);
            LOG("<Send message:num:%d",__numPacket);
            //__empty_msg=0;
            Ql_SocketSend(__est_connector_socket, &__SendBuffer[0], __toSend);
            //nSTATE=STATE_SOCKET_RECIVE;
            nSTATE=STATE_ING;
           Ql_StartGPTimer(TIME_WAIT_CONFIRM_MSG,CheckAnyMessage,NULL);
            return;
          }
        
        else{
            //__empty_msg += DEVICE_CYCLE; 
           if (CheckSendEmptyMessage())//(__empty_msg>__settings.TSend*TIME_SECOND) 
           {
              // __empty_msg=0;
               TrigerEvent(None,0,&__currentEvData);  
               __toSend=BuildEventMessage(0,&__currentEvData,1);
               OUTD("<Time send empty message",NULL);
               //LOG("<Time send empty msg",NULL);
            Ql_SocketSend(__est_connector_socket, &__SendBuffer[0], __toSend);
            Ql_StartGPTimer(TIME_WAIT_CONFIRM_MSG,CheckAnyMessage,NULL);
            //nSTATE=STATE_SOCKET_RECIVE;
            nSTATE=STATE_ING;
            return;
           }
        }
        ReadEvents(); 
        break;
    }
    case STATE_EST_SOCKET_PAUSE:{
        
        break;
    }
    case STATE_EST_SOCKET_CLOSE:{
        OUTD("Close Est Tools connection",NULL);
        LOG("Close Est connect",NULL);
         nSTATE=STATE_EST_SOCKET_CREATE;
        Ql_SocketClose(__est_connector_socket);
        __est_connection=FALSE;
        break;
    }
    case STATE_SOCKET_RECIVE:{
        
        break;
    }
    }
}

bool StopEstConnection(void){
        s32 clo=Ql_SocketClose(__est_connector_socket);
        __est_connection=FALSE;
        Ql_StopGPTimer();
        nSTATE=STATE_ING;
        return TRUE;
}

bool CheckSendEmptyMessage(void){
	if ((Ql_GetRelativeTime()-__start_empty_tmr)/1000>__settings.TSend) {
        return TRUE;
	}
    else return FALSE;
}


void CheckAnyMessage(void* param){
     if (__countRepeat>2) {
         __countRepeat=0;
         OUTD("!Timeout system recive", NULL);
         LOG("!Timeout system recive", NULL);
		 nSTATE=STATE_EST_SOCKET_CLOSE;
         
      }
      else{
          __countRepeat++;     
          OUTD("!Timeout recive.Repeate:%d", __countRepeat); 
          LOG("!Timeout recive.Repeate:%d", __countRepeat); 
          nSTATE = STATE_SOCKET_SEND; 
      }
}

void hwTimer_handler(void* param){
    //Ql_StartGPTimer(500,hwTimer_handler,NULL);
    
    //OUTD("Timer chick...",NULL);
    nSTATE=STATE_EST_SOCKET_CREATE;
    Ql_StopGPTimer();
    // __est_connector_socket=Ql_SocketCreate(0,SOC_TYPE_TCP);
    
}

void CallBack_socket_accept(u8 contexid, s8 sock, bool result, s32 error){
    //OUTD("accept context:%d socket:%d result:%d error:%d",contexid, sock,result,error);
    u16 pp;
    u8 i;
    u8 cli_address[4];
    s8 accept_socket=Ql_SocketAccept(__http_socketID,cli_address,&pp);
    if(accept_socket >= 0)
        {
            OUTD("accept_socket=%d, ip=%d.%d.%d.%d, port=%d", accept_socket,cli_address[0],cli_address[1],cli_address[2],cli_address[3], pp);
            for(i = 0; i < MAXCLIENT_NUM; i++)
                {
                    if(__client[i].socketId == 0x7F)
                        {
                            OUTD("Socket accept success %d", accept_socket);
                            client_init(i,accept_socket);
                            __client[i].send_handle_callback = func_send_handle_callback;
                            __client[i].send_handle = func_send_handle;
                            __client[i].read_handle_callback=func_read_handle_callback;    
                            __client[i].protocol=NONE;
                              break;
                         }
                  }
            //accept_count = 0; //use to send connection success tips
            //OUTD("nSTATE_TCPLONG = STATE_TRANSFERS_TCPDATA \r\n");						
            //nSTATE_TCPLONG = STATE_TRANSFERS_TCPDATA;
         }

}


void InitNetwork(void){
    Ql_GprsNetworkInitialize(0,0,&callback_func);
}

void CallBack_GprsAPNSet(bool result, s32 error_code){
    if (result) {
        nSTATE=STATE_ACTIVE_PDP;    
        OUTNET(">APN set OK",NULL);
        //LOG(">APN set OK",NULL);
    }
    else
    {
        nSTATE=STATE_QUERY_NETWORKSTATUS;
         OUTNET("!APN set ERROR.Code:%d",error_code); 
         //LOG("!APN set ERROR.Code:%d",error_code); 
         //Ql_Reset(0);
    }
}
void CallBack_network_actived(u8 contexid){
    s8 ret;
    nSTATE=STATE_SOCKET_CREATE;
    ret = Ql_GetLocalIpAddress(contexid, __local_ip_addr);
    Ql_sprintf(__str_loc_ip_addr,"%d.%d.%d.%d",__local_ip_addr[0],__local_ip_addr[1],__local_ip_addr[2],__local_ip_addr[3]);
    OUTNET(">Network:OK. IP:%d.%d.%d.%d",__local_ip_addr[0],__local_ip_addr[1],__local_ip_addr[2],__local_ip_addr[3]);
    //LOG(">Network:OK. IP:%d.%d.%d.%d",__local_ip_addr[0],__local_ip_addr[1],__local_ip_addr[2],__local_ip_addr[3]);
} 
void CallBack_network_deactived(u8 contexid,  s32 error_cause, s32 error){
    nSTATE=STATE_QUERY_NETWORKSTATUS;
    OUTNET("!Network deactivated:coause:%d",error_cause);
    //LOG("!Network deactivated:%d",error_cause);
    CURRENT=STATE_NONE;
}



void CallBack_socket_connect(u8 contexid, s8 sock, bool result, s32 error){
	if (!result) {
            Ql_SocketClose(sock);
        
		if (__tmr_index>3) {
            __tmr_index=0;//__tmr_index;
            Ql_Reset(0);
		}
        else
          __tmr_index++;
        Ql_StartGPTimer(__tmr_array[__tmr_index],hwTimer_handler,NULL);    
        OUTD("!Error connect. Try over %d second.",__tmr_array[__tmr_index]/100);
        LOG("!Error connect. Try over %d second.",__tmr_array[__tmr_index]/100);
	}
    else{
        __tmr_index=0;
         Ql_StopGPTimer();
        __est_connection=TRUE;
        OUTD(">Connected OK",NULL); 
        LOG(">Est connect OK",NULL); 
        __toSend=BuildInputStateMessage();
        Ql_SocketSend(__est_connector_socket, &__SendBuffer[0], __toSend);
        TrigerEvent(None,0,&__currentEvData);  
        __toSend=BuildEventMessage(0,&__currentEvData,1);
        Ql_SocketSend(__est_connector_socket, &__SendBuffer[0], __toSend);
        //OUTD("<Send input message",NULL);
          LOG("<Send input message",NULL);
		 Ql_StartGPTimer(TIME_WAIT_CONFIRM_MSG,CheckAnyMessage,NULL);  

    }	
}


void CallBack_socket_close(u8 contexid, s8 sock, bool result, s32 error){
    s32 index;
    if (sock==__est_connector_socket) {
        nSTATE = STATE_EST_SOCKET_CREATE;
        __est_connection=FALSE;   
	}
    else{
        
        index = findClientBySockid(sock);
        if(index >= 0)
         {
            __client[index].socketId = 0x7F;
            __client[index].protocol=HTTP;
         }
        Ql_SocketClose(sock);
    }
    OUTD("!Soccket closed:%d",sock);
    //LOG("!Soccket closed:%d",sock);
}



void CallBack_socket_read(u8 contexid, s8 sock, bool result, s32 error){
    u8 len=0;
    s32 index=0;
    s32 ret=0;
    //OUTLOG("resive from socket:%d",sock);
    //Ql_memset(__socketBuffer,0,LEN_SOCKET_BUFFER);
    //len=Ql_SocketRecv(sock,(u8*)&__socketBuffer[0],LEN_SOCKET_BUFFER);
	//if (len>0) {
		if (sock==__est_connector_socket) {
            Ql_memset(__socketBuffer,0,LEN_SOCKET_BUFFER);
            len=Ql_SocketRecv(sock,(u8*)&__socketBuffer[0],LEN_SOCKET_BUFFER);
            if (len>0) {
                unreplace(&__socketBuffer[0],len);
                if (!__waitConfirm){
                    nSTATE = STATE_SOCKET_SEND; 
                    __toSend=0;
                  Ql_StopGPTimer();
                }
                 __countRepeat=0;
                 __start_empty_tmr=Ql_GetRelativeTime();
            }
         }
        else{
           // __socketBuffer[len] = '\0'; 
           //read_request((char*)__socketBuffer, sock); 
            index=findClientBySockid(sock);

            ret=__client[index].read_handle_callback(contexid,sock,result,error,index);
            //OUTD("size recive:%d",ret);
            if (ret>0) {
                  if (__client[index].protocol==HTTP) {
                    if (parse_header((char*)__client[index].recvBuffer,(u32)__client[index].pRecvCurrentPos)) {
                        //OUTD("header:%s",__client[index].recvBuffer);
                        if (__header.method==GET) {
                           if (!__header.authorization) {
                                    Ql_strcpy(__header.action,"autorize");
                           }
                           page_response(); 
                            __client[index].send_handle(sock,__s_buf);
                        }
                        else if (__header.method==POST) {
                                OUTD("len:%d",__header.length);
                          }
                      }
                   }
                   else if (__client[index].protocol==STREAM) {
                       //OUTD("stream",NULL);
                         Ql_pinWrite(QL_PINNAME_SD_CMD, QL_PINLEVEL_HIGH);
                         s32 snd=Ql_SendToUart(ql_uart_port3,&__client[index].recvBuffer[0],ret);
                         Ql_Sleep(25);
                         Ql_pinWrite(QL_PINNAME_SD_CMD, QL_PINLEVEL_LOW);
                    }
                   else if ( __client[index].protocol==UPGRADE) {
                        s32 init=0;
                        int pp=(u16)(__client[index].pRecvCurrentPos);
                        if (Ql_strstr((char*)&__client[index].recvBuffer[pp-3],"END")) {
                            OUTD("end file is:%s",&__client[index].recvBuffer[pp]);
                            init = Ql_Fota_App_Write_Data(ret-3, (s8 *)__client[index].recvBuffer);
                            OUTD("write fota:%d", init); 
                            init=Ql_Fota_App_Finish();
                            __client[index].send_handle(sock,init==0 ? "0":"1");
                            OUTD("finish write:%d", init); 
                            if (init==0) {
                                init=Ql_Fota_Update();
                            }
                        }
                        else{
                            init = Ql_Fota_App_Write_Data(ret, (s8 *)__client[index].recvBuffer);
                            __client[index].send_handle(sock,init==0 ? "0":"1");
                            OUTD("fota write:%d", init); 
                        }
                }
            }
        }
}


void CallBack_socket_write(u8 contexid, s8 sock, bool result, s32 error){
    
   s32 index;
   s32 ret;
	//OUTD("CallBack_socket_write(contexid=%d,sock=%d,result=%d,error=%d)\r\n",contexid,sock,result,error);
   
	index = findClientBySockid(sock);
	if(index >= 0)
	{
        
        //for (int i=0;i<10;i++) {
            ret=__client[index].send_handle_callback(contexid, sock, result, error, index); 
           OUTD("socket nuber:%d send:%d send-rem:%d pos:%d",sock,ret,__client[index].sendRemain_len,__client[index].pSendCurrentPos);
            //if((ret < 0) && (ret == QL_SOC_WOULDBLOCK)){
              //  continue;
           // }
            //else{
              //   break;
            //}
           // }
           //
        }
      
}

