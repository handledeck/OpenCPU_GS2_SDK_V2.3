#ifdef __EXAMPLE_TCPSERVER__

#include "ql_appinit.h"
#include "ql_interface.h"
#include "ql_type.h"
#include "ql_trace.h"
#include "ql_audio.h"
#include "ql_timer.h"
#include "ql_stdlib.h"
#include "ql_pin.h"
#include "Ql_multitask.h"
#include "Ql_tcpip.h"


/**************************************************************************************************
This is a TCPserver example.It has three functions.
  1.Server will automatically send welcome message when the client connects to the server.
  2.Every 1 minutes(800*50+20000) automatically send broadcast messages to the client.
  3.The information sent from the client will echo back to the client.
**************************************************************************************************/


/*************************************************************
* config network param
**************************************************************/
#define HOST_NAME "www.quectel.com"

#define APN_NAME  "CMNET\0"

#define NORMAL_TIMER  800 // 800ms
#define SENDDATA_TIMER  20000 // 20000ms

#define STATE_QUERY_DNS_MAX_NUMBER 10
#define QUERY_NDS_CALLBACK_TIME 60000
#define DATA_LEN 1024
#define MAX_CONNECT 5 
#define MAXCLIENT_NUM 5

u8 ipaddress[4]; //HOST_NAME ip
s8 tcpsocket = -1;//listen socket
s8 accept_socket = -1; //accept socket
s32 accept_count = 0;
s32 index_ack;

u8   server_address[4];//server ip
u16 port = 5115;   // server port

u8   cli_address[4];//client ip
u16 cli_port; //client port


/*************************************************************
* implement
**************************************************************/
#define OUT_DEBUG(x,...)  \
    Ql_memset((x),0,100);  \
    Ql_sprintf((x),__VA_ARGS__);   \
    Ql_SendToUart(ql_uart_port1,(x),Ql_strlen(x));

void CallBack_GprsAPNSet(bool result, s32 error_code);
void CallBack_GprsAPNGet(u8 profileid, bool result, s32 error_code, u8 *apn, u8 *userId, u8 *password);
void CallBack_getipbyname(u8 contexid, bool result, s32 error, u8 num_entry, u8 *entry_address);
void CallBack_network_actived(u8 contexid);
void CallBack_network_deactived(u8 contexid,  s32 error_cause, s32 error);
void CallBack_socket_connect(u8 contexid, s8 sock, bool result, s32 error);
void CallBack_socket_close(u8 contexid, s8 sock, bool result, s32 error);   
void CallBack_socket_accept(u8 contexid, s8 sock, bool result, s32 error);
void CallBack_socket_read(u8 contexid, s8 sock, bool result, s32 error);
void CallBack_socket_write(u8 contexid, s8 sock, bool result, s32 error);

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




typedef enum{
    STATE_QUERY_NETWORKSTATUS,
    STATE_GET_APN,
    STATE_GET_APN_ING,
    STATE_SET_APN,
    STATE_SET_APN_ING,
    STATE_ACTIVE_PDP,
    STATE_ACTIVE_PDP_ING,
    STATE_QUERY_DNS,
    STATE_QUERY_DNS_ING, 
    STATE_CREATE_SOCKET,
    STATE_CREATE_SOCKET_ING,
    STATE_BIND,
    STATE_BIND_ING,
    STATE_LISTEN,
    STATE_LISTEN_ING,
    STATE_ACCEPT,
    STATE_ACCEPT_ING,
    STATE_TRANSFERS_TCPDATA,
    STATE_TRANSFERS_TCPDATA_ING,
    STATE_TRANSFERS_TCPDATA_CHECKACK,
    STATE_DEACTIVE_PDP,
    STATE_TOTAL_NUM
}STATE_TCPLONG;

STATE_TCPLONG  nSTATE_TCPLONG = STATE_QUERY_NETWORKSTATUS;

//////////////////////////////////////////////////////////////////////
//Debug
char textBuf[500];
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
//send data

u32  serialnum[MAXCLIENT_NUM] = {0}; 
u64 ackedNum[MAXCLIENT_NUM] ={0};


u32 query_dns_number = 0;
QlTimer timer_querydns;
QlTimer timer;
typedef struct
{
    s8 socketId;
    s8 userId;
    char sendBuffer[DATA_LEN];
    char recvBuffer[DATA_LEN];
    s32   sendRemain_len;
    s32   recvRemain_len;
    void *pSendCurrentPos;
    void *pRecvCurrentPos;
    s32 (*read_handle_callback)(u8 contexid, s8 sock, bool result, s32 error, s32 index);
    s32 (*send_handle_callback)(u8 contexid, s8 sock, bool result, s32 error, s32 index);
    s32 (*send_handle)(s8 sock, char *PData);
}QlClient;

QlClient client[5];


s32 func_read_handle_callback(u8 contexid, s8 sock, bool result, s32 error, s32 index);
s32 func_send_handle_callback(u8 contexid, s8 sock, bool result, s32 error, s32 index);
 
s32 func_send_handle(s8 sock, char *PDtata);  
s32 findClientBySockid(s8 sock);  
void client_init(s32 index);
void client_uninit(s32 index);
void client_socketId_init();

s32 findClientBySockid(s8 sock)
{
    s32 i;
    s32 index = -1;
    for( i = 0; i < MAXCLIENT_NUM; i++)
    {
        if(sock == client[i].socketId)
        {
            index = i;
            break;
        }
    }

    return index;	
}

void client_init(s32 index)
{
    client[index].socketId = accept_socket;
    client[index].userId = accept_socket;

    client[index].read_handle_callback = func_read_handle_callback;
 
    Ql_memset(client[index].recvBuffer, '\0', DATA_LEN); 
    client[index].recvRemain_len = DATA_LEN; 
    client[index].pRecvCurrentPos = client[index].recvBuffer;
    serialnum[index] = 0;  
}

void client_uninit(s32 index)
{
    client[index].socketId = 0x7F;
    client[index].userId = 0x7F;
	
    client[index].read_handle_callback = NULL; 
    Ql_memset(client[index].recvBuffer, '\0', DATA_LEN);
    client[index].recvRemain_len = 0;
    client[index].pRecvCurrentPos = NULL; 
}

void client_socketId_init()
{
    s32 i;
    for(i = 0; i < MAXCLIENT_NUM; i++)
    {
        client[i].socketId = 0x7F;
        client[i].userId = 0x7F;
    }
}


void ql_entry()
{
    bool keepGoing = TRUE;
    s32 ret;

    s32 j;
    QlEventBuffer    flSignalBuffer; 
    
    Ql_SetDebugMode(ADVANCE_MODE);   
    Ql_DebugTrace("tcpserver: ql_entry\r\n");
    Ql_OpenModemPort(ql_md_port1);
    client_socketId_init();
   
    Ql_GprsNetworkInitialize(0,  0, &callback_func);

    timer.timeoutPeriod = Ql_MillisecondToTicks(NORMAL_TIMER); //800 millisecond
    Ql_StartTimer(&timer);
    
    while(keepGoing)
    {    
        Ql_GetEvent(&flSignalBuffer);
        switch(flSignalBuffer.eventType)
        {
            case EVENT_TIMER:
            {
                
                if( timer_querydns.timerId  == flSignalBuffer.eventData.timer_evt.timer_id)
                {
                    nSTATE_TCPLONG = STATE_DEACTIVE_PDP;
                    OUT_DEBUG(textBuf,"STATE_QUERY_DNS timerout, will Deactive\r\n");
                    Ql_GprsNetworkDeactive(0);
                    break;
                    
                }
                else if( timer.timerId  == flSignalBuffer.eventData.timer_evt.timer_id)
                {
                    switch(nSTATE_TCPLONG)
                    {
                        case  STATE_QUERY_NETWORKSTATUS:
                        {
                            u8 ps_status;
                            s32 simcard;
                            s32 creg;
                            s32 cgreg;
                            u8 rssi;
                            u8 ber;
                            /*the Ql_GetDeviceCurrentRunState function can replace these AT Command:  AT+CPIN? AT+CREG? AT+CGREG? AT+CSQ*/
                            Ql_GetDeviceCurrentRunState(&simcard, &creg, &cgreg, &rssi, &ber);
                            OUT_DEBUG(textBuf,"Ql_GetDeviceCurrentRunState(simcard=%d, creg=%d, cgreg=%d, rssi=%d, ber=%d)\r\n", simcard, creg, cgreg, rssi, ber);
                            if(simcard != 1)
                            {
                                OUT_DEBUG(textBuf,"Warning SIM card error, please check\r\n");
                                /*Warning,Warning,Warning,Warning,Warning!!!!!!!!!!!!!!!!!! Please check or reset your system when timeout*/
                            }
                            else if(creg != 1)
                            {
                                OUT_DEBUG(textBuf,"Warning GSM network is Registing, Please waiting\r\n");
                                /*Warning,Warning,Warning,Warning,Warning!!!!!!!!!!!!!!!!!! Please reset your system when timeout*/
                            }
                            else if(cgreg != 1)
                            {
                                OUT_DEBUG(textBuf,"Warning GPRS network is Registing, Please waiting\r\n");
                                /*Warning,Warning,Warning,Warning,Warning!!!!!!!!!!!!!!!!!! Please reset your system when timeout*/
                            }
                            else
                            {
                                ret = Ql_GprsNetworkGetState(0, NULL, &ps_status);
                                //if(QL_SOC_SUCCESS != ret) failed
                                OUT_DEBUG(textBuf,"Ql_GprsNetworkGetState(),ps_status=%d\r\n", ps_status);
                               if(ps_status == 1) //REG_STATE_REGISTERED
                                {
                                    nSTATE_TCPLONG = STATE_GET_APN; 
                                    OUT_DEBUG(textBuf,"GPRS Network Attach OK\r\n");
                                }
                                else
                                {
                                    //donot REG_STATE_REGISTERED, continue wait network gprs attch ok by timer
                                }
                            }
                            break;
                        }

                        case  STATE_GET_APN:
                        {
                            nSTATE_TCPLONG = STATE_GET_APN_ING;
                            ret = Ql_GprsAPNGet(0, CallBack_GprsAPNGet);
                            //if(QL_SOC_WOULDBLOCK != ret) failed
                            OUT_DEBUG(textBuf,"Ql_GprsAPNGet(%d)=%d\r\n",0, ret);
                            //will waiting CallBack_GprsAPNGet be called
                            break;
                        }

                        case  STATE_SET_APN:
                        {
                            nSTATE_TCPLONG = STATE_SET_APN_ING;
                            ret = Ql_GprsAPNSet(0, (u8*)APN_NAME, (u8*)"", (u8*)"", CallBack_GprsAPNSet);
                            //if(QL_SOC_WOULDBLOCK != ret) failed
                            OUT_DEBUG(textBuf,"Ql_GprsAPNSet()=%d\r\n",ret);
                            //will waiting CallBack_GprsAPNSet be called
                            break;
                        }

                        case  STATE_ACTIVE_PDP:
                        {
                            nSTATE_TCPLONG = STATE_ACTIVE_PDP_ING;
                            ret = Ql_GprsNetworkActive(0); // to active contxtid=0 pdp
                            //if((QL_SOC_SUCCESS != ret) && (QL_SOC_WOULDBLOCK != ret)) failed
                            OUT_DEBUG(textBuf,"Ql_GprsNetworkActive(contxtid=%d)=%d\r\n",0,ret);
                            //if QL_SOC_WOULDBLOCK will waiting CallBack_network_actived or CallBack_network_deactived
                            if(QL_SOC_SUCCESS == ret)
                                nSTATE_TCPLONG = STATE_QUERY_DNS; // will to Ql_GetHostIpbyName
                            break; 
                        }
                        
                        case  STATE_QUERY_DNS:
                        {
                            query_dns_number++;
                            //when network signal is bad, csq only 3-7, but you query dns will failed
                            if(query_dns_number > STATE_QUERY_DNS_MAX_NUMBER)
                            {
                                query_dns_number = 0;
                                nSTATE_TCPLONG = STATE_DEACTIVE_PDP;
                                OUT_DEBUG(textBuf,"STATE_QUERY_DNS %d >  STATE_QUERY_DNS_MAX_NUMBER(%d)\r\n",query_dns_number,STATE_QUERY_DNS_MAX_NUMBER);
                                Ql_GprsNetworkDeactive(0);
                                break;
                            }
                            nSTATE_TCPLONG = STATE_QUERY_DNS_ING;
                            ret = Ql_SocketCheckIp((u8*)HOST_NAME, (u32*)ipaddress);
                            if(ret == QL_SOC_SUCCESS) // is ip address, xxx.xxx.xxx.xxx
                            { 
                                 OUT_DEBUG(textBuf,"Ql_SocketCheckIp ip=%d.%d.%d.%d\r\n", ipaddress[0],ipaddress[1],ipaddress[2],ipaddress[3]);
					nSTATE_TCPLONG = STATE_CREATE_SOCKET;
					query_dns_number = 0;
                            }
                            else if(ret == QL_SOC_ERROR) // is host name
                            {
                                u8 addr[4*5];
                                u8 *entryaddr;
                                u8 addr_len = 0;
                                u8 out_entry_num,i;
                                ret = Ql_GetHostIpbyName(0, (u8*)HOST_NAME, addr, &addr_len, 5, &out_entry_num, CallBack_getipbyname);
                                OUT_DEBUG(textBuf,"Ql_GetHostIpbyName()=%d\r\n",ret);
                                if(ret == QL_SOC_SUCCESS)
                                {
                                    query_dns_number = 0;
                                    Ql_memcpy((void*)ipaddress, (void*)addr, 4);
                                    nSTATE_TCPLONG = STATE_CREATE_SOCKET;  
                                    entryaddr = (u8*)addr;
                                    for(i=0;i<out_entry_num;i++)
                                    {
                                        entryaddr += (i*4);
                                        OUT_DEBUG(textBuf,"entry=%d, ip=%d.%d.%d.%d\r\n",i,entryaddr[0],entryaddr[1],entryaddr[2],entryaddr[3]);
                                    }
                                }
                                else if(ret == QL_SOC_WOULDBLOCK)
                                {
                                    //waiting CallBack_getipbyname be called
                                    OUT_DEBUG(textBuf,"please wait CallBack_getipbyname\r\n");

                                    //start timer to wait callback
                                    timer_querydns.timeoutPeriod = Ql_MillisecondToTicks(QUERY_NDS_CALLBACK_TIME); //800 millisecond
                                    Ql_StartTimer(&timer_querydns);
                                }
                                else
                                {
                                    OUT_DEBUG(textBuf,"Ql_GetHostIpbyName run error\r\n");
                                    //ASSERT(0);
                                    if(ret == QL_SOC_BEARER_FAIL)  
                                    {
                                        query_dns_number = 0;
                                        nSTATE_TCPLONG = STATE_DEACTIVE_PDP;
                                         OUT_DEBUG(textBuf,"Ql_GetHostIpbyName error QL_SOC_BEARER_FAIL to Ql_GprsNetworkDeactive\r\n");
                                        Ql_GprsNetworkDeactive(0);
                                    }
                                    else
                                    {
                                        //re query dns
                                        nSTATE_TCPLONG = STATE_QUERY_DNS;
                                    }
                                }
                            }
                            else 
                            {
                                query_dns_number = 0;
                                //ASSERT(0);
                                //invalid argument
                            }
                            break;
                        }

                        case STATE_CREATE_SOCKET:
                        {
                            tcpsocket = Ql_SocketCreate(0, 0);
                            OUT_DEBUG(textBuf,"Ql_SocketCreate()=%d\r\n",tcpsocket);
                            nSTATE_TCPLONG = STATE_LISTEN;
					
                            break;
                        }

                        case  STATE_LISTEN:
                        {
                            s32 ret; 
                            OUT_DEBUG(textBuf,"begin to listen socket\r\n");
                            ret = Ql_SocketListen(tcpsocket, server_address, port, MAX_CONNECT);
                            if(ret < 0)
                            {
                                nSTATE_TCPLONG = STATE_LISTEN;
                                OUT_DEBUG(textBuf,"failed to listen socket! Ql_SocketListen()=%d\r\n", ret);
                            }else
                            {
                                nSTATE_TCPLONG = STATE_LISTEN_ING;
                                OUT_DEBUG(textBuf,"listening socket\r\n");
                            }
                            break;
                            
                        }
				
                        case STATE_ACCEPT:
                        {
                            s32 i;
                            OUT_DEBUG(textBuf,"begin to accept socket\r\n");
                            accept_socket = Ql_SocketAccept(tcpsocket, cli_address, &cli_port);
                            
                            if(accept_socket >= 0)
                            {
                                OUT_DEBUG(textBuf,"Ql_SocketAccept(accept_socket=%d, ip=%d.%d.%d.%d, port=%d)\r\n", accept_socket,cli_address[0],cli_address[1],cli_address[2],cli_address[3], cli_port);

                                for(i = 0; i < MAXCLIENT_NUM; i++)
                                {
                                    if(client[i].socketId == 0x7F)
                                    {
                                        OUT_DEBUG(textBuf,"Ql_SocketAccept success  Ql_SocketAccept()=%d\r\n", accept_socket);
                                        client_init(i);
                                        client[i].send_handle_callback = func_send_handle_callback;
                                        client[i].send_handle = func_send_handle;
                                        serialnum[i] = 0;  

                                        break;
                                    }
                                }
                        
                                accept_count = 0; //use to send connection success tips
                                OUT_DEBUG(textBuf,"nSTATE_TCPLONG = STATE_TRANSFERS_TCPDATA \r\n");						
                                nSTATE_TCPLONG = STATE_TRANSFERS_TCPDATA;
                            }
                            else
                            {
                                nSTATE_TCPLONG = STATE_ACCEPT;
                                accept_count++;
                                OUT_DEBUG(textBuf,"Ql_SocketAccept failed  Ql_SocketAccept()=%d\r\n", accept_socket);
                            }

                            if (accept_count == 50)//use to send broadcast message per 50 times
                            {
                                accept_count = 1;// no set 0
                                
                                for (i = 0; i < MAXCLIENT_NUM; i++)
                                {
                                    if(client[i].socketId != 0x7F)
                                    {
                                        OUT_DEBUG(textBuf,"nSTATE_TCPLONG = STATE_TRANSFERS_TCPDATA \r\n");
                                        nSTATE_TCPLONG = STATE_TRANSFERS_TCPDATA;
                                    }
                                }
                            }

                            break;
                        }

                        case STATE_TRANSFERS_TCPDATA:
                        {

                            static s32 i = 0; 
				 s32 index;
                            OUT_DEBUG(textBuf,"begin to transfers tcpdata\r\n");

                             OUT_DEBUG(textBuf,"accept_count =%d \r\n",accept_count);
                             
                            if (!accept_count) //send connection success tips
                            {
                                index = findClientBySockid(accept_socket);
                                client[index].send_handle(client[index].socketId, "welcome to connect server~\r\n");
                            }else //send broadcast message
                            {
                                for(; i < MAXCLIENT_NUM; i++)
                                {
                                    if(client[i].socketId != 0x7F)
                                    {
                                        client[i].send_handle(client[i].socketId, "This is broadcast message~\r\n");
                                    }
                                }
                                if(i == MAXCLIENT_NUM)
                                {
                                    i = 0;
                                }
                            }
                            break;
                        }
				
                        case  STATE_TRANSFERS_TCPDATA_CHECKACK:
                        {
                            u64 ackedNumCurr;
                            //check peer received all data
                            OUT_DEBUG(textBuf,"begin to Ql_SocketTcpAckNumber\r\n");
                            ret = Ql_SocketTcpAckNumber(client[index_ack].socketId, &ackedNumCurr);
                            if(QL_SOC_SUCCESS != ret)
                            {
                                OUT_DEBUG(textBuf,"SocketTcpAckNumber fail\r\n");
                                break;
                            }

                            nSTATE_TCPLONG = STATE_ACCEPT;//Keep circulating
                            OUT_DEBUG(textBuf,"Ql_SocketTcpAckNumber(socket=%d,ackedNumCurr=%llu)=%d\r\n",client[index_ack].socketId,ackedNumCurr,ret);
                            break;
                        }
                        
                    }

                    if(STATE_TRANSFERS_TCPDATA== nSTATE_TCPLONG)
                        timer.timeoutPeriod = Ql_MillisecondToTicks(SENDDATA_TIMER); 
                    else
                        timer.timeoutPeriod = Ql_MillisecondToTicks(NORMAL_TIMER); 
                    Ql_StartTimer(&timer); 
                }
                break;
            }

            case EVENT_MODEMDATA:
            {
                // BB AT Command return data, send to UART1
                Ql_SendToUart(ql_uart_port1, flSignalBuffer.eventData.uartdata_evt.data, flSignalBuffer.eventData.uartdata_evt.len);        
                break;
            }

            case EVENT_UARTDATA:
            {
                //AT command send to BB, goto run 
                Ql_SendToModem(ql_md_port1, flSignalBuffer.eventData.modemdata_evt.data, flSignalBuffer.eventData.modemdata_evt.len);                            
                break;
            }
            
            default:
                Ql_DebugTrace("eventType=%d",flSignalBuffer.eventType);
                break;
        }
    }
    
    Ql_GprsNetworkUnInitialize(0);
}

void CallBack_GprsAPNGet(u8 profileid, bool result, s32 error_code, u8 *apn, u8 *userId, u8 *password)
{
    if(result)
    {
        nSTATE_TCPLONG = STATE_SET_APN;  // will to Ql_GprsAPNSet
    }
    else
    {
        nSTATE_TCPLONG = STATE_GET_APN;  // will to Ql_GprsAPNSet
    }
    OUT_DEBUG(textBuf,"CallBack_GprsAPNGet(profileid=%d,result=%d,error_code=%d,apn=%s,userId=%s,password=%s)\r\n",profileid,result,error_code,apn,userId,password);
}

void CallBack_GprsAPNSet(bool result, s32 error_code)
{
    if(result)
    {
        nSTATE_TCPLONG = STATE_ACTIVE_PDP; // will to Ql_GprsNetworkActive
    }
    else
    {
        nSTATE_TCPLONG = STATE_SET_APN;  // will to Ql_GprsAPNSet
    }
   OUT_DEBUG(textBuf,"CallBack_GprsAPNSet(result=%d,error_code=%d)\r\n",result,error_code);
}



void CallBack_network_actived(u8 contexid)
{
    s8 ret;
    u8 ip_addr[4];

    OUT_DEBUG(textBuf,"CallBack_network_actived(contexid=%d)\r\n",contexid);
    ret = Ql_GetLocalIpAddress(contexid, ip_addr);
    OUT_DEBUG(textBuf,"Ql_GetLocalIpAddress(contexid=%d)=%d, ip=%d.%d.%d.%d\r\n",contexid, ret,ip_addr[0],ip_addr[1],ip_addr[2],ip_addr[3]);
    nSTATE_TCPLONG = STATE_QUERY_DNS; // will to Ql_GetHostIpbyName

    Ql_strcpy(server_address,ip_addr);
}

void CallBack_network_deactived(u8 contexid, s32 error_cause, s32 error)
{
    nSTATE_TCPLONG = STATE_QUERY_NETWORKSTATUS; // will to Ql_GprsNetworkGetState
    OUT_DEBUG(textBuf,"CallBack_network_deactived(contexid=%d,error_cause=%d, error=%d)\r\n",contexid,error_cause,error);
}


void CallBack_getipbyname(u8 contexid, bool result, s32 error, u8 num_entry, u8 *entry_address)
{
    u8 i;

    OUT_DEBUG(textBuf,"CallBack_getipbyname(contexid=%d, result=%d,error=%d,num_entry=%d)\r\n",contexid, result,error,num_entry);

    Ql_StopTimer(&timer_querydns);

    if(result)
    {
        Ql_memcpy((void*)ipaddress, (void*)entry_address, 4);
        nSTATE_TCPLONG = STATE_CREATE_SOCKET;
        for(i=0;i<num_entry;i++)
        {
            entry_address += (i*4);
            OUT_DEBUG(textBuf,"entry=%d, ip=%d.%d.%d.%d\r\n",i,entry_address[0],entry_address[1],entry_address[2],entry_address[3]);
        }
    }
    else
    {
        if((error == QL_SOC_BEARER_FAIL)  || (error >=0)) 
        {
            query_dns_number = 0;
            nSTATE_TCPLONG = STATE_DEACTIVE_PDP;
             OUT_DEBUG(textBuf,"CallBack_getipbyname error QL_SOC_BEARER_FAIL to Ql_GprsNetworkDeactive\r\n");
            Ql_GprsNetworkDeactive(0);
        }
        else
        {
            nSTATE_TCPLONG = STATE_QUERY_DNS;// will to Ql_GetHostIpbyName
        }
    }
}



void CallBack_socket_connect(u8 contexid, s8 sock, bool result, s32 error)
{
 #if 0   
    if(result)
    {
        OUT_DEBUG(textBuf,"CallBack_socket_connect(contexid=%d,sock=%d ) successed \r\n",contexid,sock);
         nSTATE_TCPLONG = STATE_TRANSFERS_TCPDATA;
    }
    else
    {
        OUT_DEBUG(textBuf,"CallBack_socket_connect(contexid=%d,sock=%d,result=%d,error=%d) failed\r\n",contexid,sock,result,error);
        Ql_SocketClose(tcpsocket);
        tcpsocket = 0x7F;

        if((error == QL_SOC_BEARER_FAIL)  || (error >=0)) 
        {
            nSTATE_TCPLONG = STATE_DEACTIVE_PDP;
             OUT_DEBUG(textBuf,"CallBack_socket_connect error  QL_SOC_BEARER_FAIL to Ql_GprsNetworkDeactive\r\n");
            Ql_GprsNetworkDeactive(0);
        }
        else
        {
            nSTATE_TCPLONG = STATE_QUERY_DNS;
        }
     }
#endif

}


void CallBack_socket_close(u8 contexid, s8 sock, bool result, s32 error)
{
    s32 index;
    OUT_DEBUG(textBuf,"CallBack_socket_close(contexid=%d,sock=%d,result=%d,error=%d)\r\n",contexid,sock,result,error);
    
    Ql_SocketClose(sock);
    index = findClientBySockid(sock);
    if(index >= 0)
    {
        client[index].socketId = 0x7F;
    }
	
    if((error == QL_SOC_BEARER_FAIL)  || (error >=0)) 
    {
        nSTATE_TCPLONG = STATE_DEACTIVE_PDP;
        OUT_DEBUG(textBuf,"CallBack_socket_close error  QL_SOC_BEARER_FAIL to Ql_GprsNetworkDeactive\r\n");
        Ql_GprsNetworkDeactive(0);
    }
    else
    {
        nSTATE_TCPLONG = STATE_ACCEPT;
    }
    
}

void CallBack_socket_accept(u8 contexid, s8 sock, bool result, s32 error)
{
    OUT_DEBUG(textBuf,"CallBack_socket_accept(contexid=%d,sock=%d,result=%d,error=%d)\r\n",contexid,sock,result,error);
    if(!result)
    {
        if((error == QL_SOC_BEARER_FAIL)  || (error >=0)) 
	  {
	  	Ql_GprsNetworkDeactive(0);
        }
    }
    else
    {
    		nSTATE_TCPLONG = STATE_ACCEPT;
    }
}

void CallBack_socket_read(u8 contexid, s8 sock, bool result, s32 error)
{
	s32 index;
	OUT_DEBUG(textBuf,"CallBack_socket_read(contexid=%d,sock=%d,result=%d,error=%d)\r\n",contexid,sock,result,error);

	index = findClientBySockid(sock);
       OUT_DEBUG(textBuf,"CallBack_socket_read(index=%d)\r\n",index);
	if(index >= 0)
	{
		client[index].read_handle_callback(contexid, sock, result, error, index);	
	}
}

s32 func_read_handle_callback(u8 contexid, s8 sock, bool result, s32 error, s32 index)
{
	s32 i;
	s32 ret;
	OUT_DEBUG(textBuf,"func_read_handle_callback(contexid=%d,sock=%d,result=%d,error=%d, index=%d)\r\n", contexid, sock, result, error, index);

	if(!result)
	{
		OUT_DEBUG(textBuf, "func_read_handle_callback() failed\r\n");
		Ql_SocketClose(client[index].socketId);

		client_uninit(index);

		if((error == QL_SOC_BEARER_FAIL) || (error > 0))
		{
			OUT_DEBUG(textBuf,"func_read_handle_callback error  QL_SOC_BEARER_FAIL\r\n");
			for(i = 0; i < MAXCLIENT_NUM; i++)
			{
				if(client[i].socketId != 0x7F)
				{
					break;
				}
			}
			if(i == MAXCLIENT_NUM)
			{
				nSTATE_TCPLONG = STATE_DEACTIVE_PDP;
				OUT_DEBUG(textBuf, "func_read_handle_callback error QL_SOC_BEARER_FAIL to Ql_GprsNetworkDeactive\r\n ");
				Ql_GprsNetworkDeactive(0);
			}
			else
			{
				//donot change the state into STATE_DEACTIVE_PDP 
				nSTATE_TCPLONG = STATE_ACCEPT;
			}

		}
		else
		{
			nSTATE_TCPLONG = STATE_ACCEPT;
		}
		
		return result;
	}

	do
	{
             Ql_memset(client[index].recvBuffer, 0, DATA_LEN);
		ret = Ql_SocketRecv(client[index].socketId, (u8 *)client[index].recvBuffer, DATA_LEN);

             if(ret == QL_SOC_WOULDBLOCK)
		{
			//you shoud wait next CallBack_socket_read   
			break;
		}
		else if((ret < 0) && (ret != QL_SOC_WOULDBLOCK))
		{
			OUT_DEBUG(textBuf, "Ql_SocketRecv(sock=%d)=%d error exit\r\n", client[i].socketId, ret);
			Ql_SocketClose(client[index].socketId);
			client_uninit(index);
			
			if(ret == QL_SOC_BEARER_FAIL)
			{
				for(i = 0; i < MAXCLIENT_NUM; i++)
				{
					if(client[i].socketId != 0x7F)
					{
						break;
					}
				}
				if(i == MAXCLIENT_NUM)
				{
					nSTATE_TCPLONG = STATE_DEACTIVE_PDP;
					OUT_DEBUG(textBuf, "func_read_handle_callback error QL_SOC_BEARER_FAIL to Ql_GprsNetworkDeactive\r\n ");
					Ql_GprsNetworkDeactive(0);
				}
				else
				{
					nSTATE_TCPLONG = STATE_ACCEPT;
				}		
			}
			else
			{
				nSTATE_TCPLONG = STATE_ACCEPT;
			}

			break;
		}
		else if(ret < DATA_LEN ||ret == DATA_LEN)
		{
                OUT_DEBUG(textBuf, "Recv from: sock(%d)len(%d) :%s\r\n", client[index].socketId, ret,client[index].recvBuffer);
                nSTATE_TCPLONG = STATE_TRANSFERS_TCPDATA_ING;
                client[index].send_handle(client[index].socketId, client[index].recvBuffer);//send back to client.

                if (ret < DATA_LEN)// send over
                {
                    break;
                }
		}
	}while(1);

    return ret;
}

s32 func_send_handle(s8 sock, char *PDtata)
{
	s32 ret;
	s32 i = 0; 
	s32 index;

	index = findClientBySockid(sock);

	client[index].sendRemain_len = Ql_strlen(PDtata);
	client[index].pSendCurrentPos = PDtata;
       do
	{
             Ql_memset(client[index].sendBuffer, 0, DATA_LEN);
             
             if (client[index].sendRemain_len > DATA_LEN)
             {
                Ql_strncpy(client[index].sendBuffer,client[index].pSendCurrentPos,DATA_LEN);
             }else
             {
                Ql_strncpy(client[index].sendBuffer,client[index].pSendCurrentPos,client[index].sendRemain_len);
             }
		ret = Ql_SocketSend(client[index].socketId, (u8*)client[index].sendBuffer,Ql_strlen((u8*)client[index].sendBuffer));
		if(ret == client[index].sendRemain_len)
		{
			//send complete
			client[index].sendRemain_len = 0;
			Ql_memset(client[index].sendBuffer, '\0', DATA_LEN);
			client[index].pSendCurrentPos = NULL;
                     index_ack = index;
			nSTATE_TCPLONG = STATE_TRANSFERS_TCPDATA_CHECKACK;
			break;
		}
		else if((ret < 0) && (ret == QL_SOC_WOULDBLOCK))
		{
			//you must wait CallBack_socket_write, then send data;     
			nSTATE_TCPLONG = STATE_TRANSFERS_TCPDATA_ING;
                   break;
		}
		else if(ret < 0) //error
		{
			Ql_SocketClose(client[index].socketId);
			client[index].socketId = 0x7F;
			client[index].userId = 0x7F;

			client[index].sendRemain_len = 0;
			Ql_memset(client[index].sendBuffer, '\0', DATA_LEN);
			client[index].pSendCurrentPos = NULL;
			client[index].send_handle_callback = NULL;
			client[index].send_handle = NULL;

			if(ret == QL_SOC_BEARER_FAIL)
			{
				for(i = 0; i < MAXCLIENT_NUM; i++)
				{
					if(client[i].socketId != 0x7F)
					{
						break;
					}
				}
				if(i == MAXCLIENT_NUM)
				{
					nSTATE_TCPLONG = STATE_DEACTIVE_PDP;
					OUT_DEBUG(textBuf, "Ql_SocketSend error QL_SOC_BEARER_FAIL to Ql_GprsNetworkDeactive\r\n ");
					Ql_GprsNetworkDeactive(0);
				}
				else
				{
					nSTATE_TCPLONG = STATE_ACCEPT;
				}
			}
			else
			{
				nSTATE_TCPLONG = STATE_ACCEPT;
			}

			break;
		}
		else if(ret < client[index].sendRemain_len)
		{
             	       client[index].sendRemain_len -= ret;
			(char *)client[index].pSendCurrentPos += ret;
			//continue send, do not send all data
		}
      
	}while(1);

    return ret;
}


void CallBack_socket_write(u8 contexid, s8 sock, bool result, s32 error)
{
	s32 index;
	OUT_DEBUG(textBuf,"CallBack_socket_write(contexid=%d,sock=%d,result=%d,error=%d)\r\n",contexid,sock,result,error);

	index = findClientBySockid(sock);
	if(index >= 0)
	{
		client[index].send_handle_callback(contexid, sock, result, error, index);
	}
}

s32 func_send_handle_callback(u8 contexid, s8 sock, bool result, s32 error, s32 index)
{
	s32 i;
	s32 ret;

    	client[index].sendRemain_len = Ql_strlen((u8*)client[index].sendBuffer);
	client[index].pSendCurrentPos = client[index].sendBuffer;
    
	OUT_DEBUG(textBuf,"func_send_handle_callback(contexid=%d,sock=%d,result=%d,error=%d, index=%d)\r\n", contexid, sock, result, error, index);
       
	while(result)
	{
		ret = Ql_SocketSend(client[index].socketId,client[index].pSendCurrentPos,client[index].sendRemain_len);
		OUT_DEBUG(textBuf, "Ql_SocketSend(socket=%d, sendRemain_len=%d)=%d\r\n", client[index].socketId,client[index].sendRemain_len, ret);

		if(ret == client[index].sendRemain_len)
		{
			//send complete
			client[index].sendRemain_len = 0;
			Ql_memset(client[index].sendBuffer, '\0', DATA_LEN);
			client[index].pSendCurrentPos = NULL;
                    index_ack = index;
			nSTATE_TCPLONG = STATE_TRANSFERS_TCPDATA_CHECKACK;
			break;
		}
		else if((ret < 0) && (ret == QL_SOC_WOULDBLOCK))
		{
			//you must wait CallBack_socket_write, then send data;     
			nSTATE_TCPLONG = STATE_TRANSFERS_TCPDATA_ING; 
                   break;
		}
		else if(ret < 0)
		{
			//error
			Ql_SocketClose(client[index].socketId);
			client[index].socketId = 0x7F;
			client[index].userId = 0x7F;

			client[index].sendRemain_len = 0;
			Ql_memset(client[index].sendBuffer, '\0', DATA_LEN);
			client[index].pSendCurrentPos = NULL;
			client[index].send_handle_callback = NULL;
			client[index].send_handle = NULL;

			if(ret == QL_SOC_BEARER_FAIL)
			{
				for(i = 0; i < MAXCLIENT_NUM; i++)
				{
					if(client[i].socketId != 0x7F)
					{
						break;
					}
				}
				if(i == MAXCLIENT_NUM)
				{
					nSTATE_TCPLONG = STATE_DEACTIVE_PDP;
					OUT_DEBUG(textBuf, "CallBack_socket_read error QL_SOC_BEARER_FAIL to Ql_GprsNetworkDeactive\r\n ");
					Ql_GprsNetworkDeactive(0);
				}
				else
				{
					nSTATE_TCPLONG = STATE_ACCEPT;
				}
			}
			else
			{
				nSTATE_TCPLONG = STATE_ACCEPT;
			}

			break;
		}
		else if(ret < client[index].sendRemain_len) 
		{
                    client[index].sendRemain_len -= ret;
			(char *)client[index].pSendCurrentPos += ret;
			//continue send, do not send all data
		}						
	}

	if(result)
	{
		OUT_DEBUG(textBuf, "CallBack_socket_write() failed\r\n");
		Ql_SocketClose(client[index].socketId);
		client[index].socketId = 0x7F;
		client[index].userId = 0x7F;

		client[index].sendRemain_len = 0;
		Ql_memset(client[index].sendBuffer, '\0', DATA_LEN);
		client[index].pSendCurrentPos = NULL;
		client[index].send_handle_callback = NULL;
		client[index].send_handle = NULL;

		if((error == QL_SOC_BEARER_FAIL) || (error > 0))
		{
			for(i = 0; i < MAXCLIENT_NUM; i++)
			{
				if(client[i].socketId != 0x7F)
				{
					break;
				}
			}
			if(i == MAXCLIENT_NUM)
			{
				nSTATE_TCPLONG = STATE_DEACTIVE_PDP;
				OUT_DEBUG(textBuf, "CallBack_socket_read error QL_SOC_BEARER_FAIL to Ql_GprsNetworkDeactive\r\n ");
				Ql_GprsNetworkDeactive(0);
			}
			else
			{
				nSTATE_TCPLONG = STATE_ACCEPT;
			}
		}
		else
		{
				nSTATE_TCPLONG = STATE_ACCEPT;
		}		
		
	}

}

#endif // __EXAMPLE_TCPSERVER__

