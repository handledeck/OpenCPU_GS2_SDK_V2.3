#ifdef __EXAMPLE_TCPIP__

/***************************************************************************************************
*   Example:
*       
*           TCPIP Routine
*
*   Description:
*
*           This example demonstrates how to use function about TCPIP with APIs in OpenCPU.
*           Through MAIN Uart port, input the specified command, there will be given the 
*           response about TCPIP operation.
*
*   Usage:
*
*           Compile & Run:
*
*               Use "make tcpip" to compile, and download bin image to module to run.
*           
*           Operation: (Through MAIN port)
*            
*               In this routine, at first you should modify server IP address and port you want to connect with, like this:
*                       u16 port = 7008;
*                       u8 address[4] = {116,226,50,11};
*               Then Compile & Run it.
*
*               If input "APNSet=x", that will set the authentication parameters apn/login/password to use with a profile id during PDP activation. "x" is PDP context profile, which is 0 or 1.
*               If input "APNGet=x", that will get the the above information, "x" is PDP context profile, which is 0 or 1.
*               If input "NetworkActive=y",that will active the PDP context,"y"is context id,which is 0 or 1.
*               If input "NetworkDeactive=y", that will deactive the PDP context,"y"is context id,which is 0 or 1.
*               If input "NetworkGetState=y", that will get the state of GPRS network and PDP context,"y"is context id,which is 0 or 1.
*               If input "GetHostIpbyName=y,name", that will retrieve host IP corresponding to a host name."y"is context id,which is 0 or 1."name" is like "www.quectel.com" and don't input quotes.
*               If input "socketcreate=y", that will create a TCP socket and establish a connection to the socket. "y"is context id,which is 0 or 1.
*               If input "socketsend=y", that will send data through a connected socket."y"is context id,which is 0 or 1.
*               If input "socketack=y", that will get the TCP socket ACK number."y"is context id,which is 0 or 1.
*               If input "socketclose=y", that will close the TCP socket. "y"is context id,which is 0 or 1.
*               If input "udpcreate=y", that will create a UDP socket. "y"is context id,which is 0 or 1.
*               If input "udpsend=y", that will send data through UDP socket. "y"is context id,which is 0 or 1.
*               If input "udpclose=", that will close the UDP socket. "y"is context id,which is 0 or 1.
*
*   Note: 
*		   In most of TCPIP functions,  return -2(QL_SOC_WOULDBLOCK) doesn't indicate failed.
*               It means app should wait, till the callback function is called.
*               The app can get the information of success or failure in callback function.
*               Get more info about return value. Please read the "OPEN_CPU_DGD" document.
****************************************************************************************************/ 

#include "ql_appinit.h"
#include "ql_interface.h"
#include "ql_type.h"
#include "ql_trace.h"
#include "ql_tcpip.h"
#include "ql_timer.h"
#include "ql_stdlib.h"
#include "ql_fcm.h"

char textBuf[100];

#define OUT_DEBUG(x,...)  \
    Ql_memset((void*)(x),0,100);  \
    Ql_sprintf((char*)(x),__VA_ARGS__);   \
    Ql_SendToUart(ql_uart_port1,(u8*)(x),Ql_strlen((const char*)(x)));


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

OpenCpuTcpIp_Callback_t     callback_func = 
{
    CallBack_network_actived,
    CallBack_network_deactived,
    CallBack_socket_connect,
    CallBack_socket_close,
    CallBack_socket_accept,
    CallBack_socket_read,
    CallBack_socket_write
};


u16 port = 8604;
u8 address[4] = {61,190,19,174};


#define DATA_LEN 400/*1000*/

s8  socketonly[2] = {0xFF, 0xFF}; // tcp socket
s8  udponly[2] = {0xFF, 0xFF}; // udp socket

s32 dataLen[2] ={DATA_LEN};
s32 send_dataLen[2] ={0};

u8 data_p[DATA_LEN]={0};



void ql_entry()
{
    bool           keepGoing = TRUE;
    QlEventBuffer  flSignalBuffer;
    char *p=NULL;
    char *p1=NULL;
    char *pData= NULL;

    Ql_memset(data_p, '*', DATA_LEN);
    
    data_p[0] = 'B';
    data_p[1] = 'e';
    data_p[2] = 'g';
    data_p[3] = 'i';
    data_p[4] = 'n';


    data_p[DATA_LEN-3] = 'E';
    data_p[DATA_LEN-2] = 'n';
    data_p[DATA_LEN-1] = 'D';

    Ql_SetDebugMode(BASIC_MODE);
    Ql_DebugTrace("tcpip: ql_entry\r\n");

    Ql_GprsNetworkInitialize(0,  0, &callback_func);
    Ql_GprsNetworkInitialize(1,  1, &callback_func);

    while(keepGoing)
    {    
        Ql_GetEvent(&flSignalBuffer);
        switch(flSignalBuffer.eventType)
        {
            case EVENT_UARTDATA:
            {
                pData = (char*)flSignalBuffer.eventData.uartdata_evt.data;
                pData[flSignalBuffer.eventData.uartdata_evt.len+1] = '\0';

                /*APNSet=0-1*/
                p = Ql_strstr(pData,"APNSet=");
                if (p)
                {
                    s32 ret;
                    p = Ql_strstr(pData,"=");
                    p += 1;
                    ret = Ql_GprsAPNSet(Ql_atoi(p), (u8*)"CMNET", (u8*)"", (u8*)"", CallBack_GprsAPNSet);
                    OUT_DEBUG(textBuf,"Ql_GprsAPNSet(%d)=%d\r\n",Ql_atoi(p),ret);
                    break;
                }

                /*APNGet=0-1*/
                p = Ql_strstr(pData,"APNGet=");
                if (p)
                {
                    s32 ret;
                    p = Ql_strstr(pData,"=");
                    p += 1;
                    ret = Ql_GprsAPNGet(Ql_atoi(p), CallBack_GprsAPNGet);
                    OUT_DEBUG(textBuf,"Ql_GprsAPNGet(%d)=%d\r\n",Ql_atoi(p),ret);
                    break;
                }

                /*NetworkActive=0-1*/
                p = Ql_strstr(pData,"NetworkActive=");
                if (p)
                {
                    s32 ret;
                    p = Ql_strstr(pData,"=");
                    p += 1;
                    ret = Ql_GprsNetworkActive(Ql_atoi(p));
                    OUT_DEBUG(textBuf,"Ql_GprsNetworkActive(contxtid=%d)=%d\r\n",Ql_atoi(p),ret);
                    break;
                }

                /*NetworkDeactive=0-1*/
                p = Ql_strstr(pData,"NetworkDeactive=");
                if (p)
                {
                    s32 ret;
                    p = Ql_strstr(pData,"=");
                    p += 1;

                    //you must close socket
                    if(socketonly[Ql_atoi(p)] >= 0)
                    {
                        Ql_SocketClose(socketonly[Ql_atoi(p)]);
                        socketonly[Ql_atoi(p)] = 0xFF;
                        send_dataLen[Ql_atoi(p)] = 0;
                    }
                    

                    if(udponly[Ql_atoi(p)] >= 0)
                    {
                        Ql_SocketClose(udponly[Ql_atoi(p)]);
                        udponly[Ql_atoi(p)] = 0xFF;
                        send_dataLen[Ql_atoi(p)] = 0;
                    }

                    ret = Ql_GprsNetworkDeactive(Ql_atoi(p));
                    OUT_DEBUG(textBuf,"Ql_GprsNetworkDeactive(contxtid=%d)=%d\r\n",Ql_atoi(p),ret);
                    break;
                }

                /*NetworkGetState=0-1*/
                p = Ql_strstr(pData,"NetworkGetState=");
                if (p)
                {
                    s32 ret;
                    OpenCpuNetWorkState_e networkstate;
                    u8 ps_status;
                    p = Ql_strstr(pData,"=");
                    p += 1;
                    ret = Ql_GprsNetworkGetState(Ql_atoi(p), &networkstate, &ps_status);
                    OUT_DEBUG(textBuf,"Ql_GprsNetworkGetState(contxtid=%d,networkstate=%d,ps_status=%d)=%d\r\n",Ql_atoi(p),networkstate,ps_status,ret);
                    break;
                }

                /*GetHostIpbyName=0-1,www.quectel.com*/
                p = Ql_strstr(pData,"GetHostIpbyName=");
                if (p)
                {
                    s32 ret;
                    OpenCpuNetWorkState_e networkstate;
                    u8 addr[4*5];
                    u8 addr_len = 0;
                    u8 out_entry_num;
                    u8 ps_status;
                    p = Ql_strstr(pData,"=");
                    p += 1;
                    p1 = Ql_strstr(pData,",");
                    *p1 = '\0';
                    p1 += 1;
                    ret = Ql_GetHostIpbyName(Ql_atoi(p), (u8*)p1, addr, &addr_len, 5, &out_entry_num, CallBack_getipbyname);
                    OUT_DEBUG(textBuf,"Ql_GetHostIpbyName(contxtid=%d,\"%s\")=%d\r\n",Ql_atoi(p),p1,ret);
                    if(ret == 0)
                    {
                        OUT_DEBUG(textBuf,"addr_len=%d,ip=%d.%d.%d.%d,out_entry_num=%d\r\n",addr_len, addr[0],addr[1],addr[2],addr[3],out_entry_num);
                    }
                    else if(ret == -2)
                    {
                        OUT_DEBUG(textBuf,"please wait CallBack_getipbyname\r\n");
                    }
                    else
                    {
                        OUT_DEBUG(textBuf,"Ql_GetHostIpbyName run error\r\n");
                    }

                    break;
                }

                /*socketcreate=0-1*/
                p = Ql_strstr(pData,"socketcreate=");   // tcp socket
                if (p)
                {
                    s32 ret;
                    OpenCpuNetWorkState_e networkstate;
                    u8 ps_status;
                    
                    p = Ql_strstr(pData,"=");
                    p += 1;
                    ret = Ql_SocketCreate(Ql_atoi(p), 0);
                    OUT_DEBUG(textBuf,"Ql_SocketCreate(contxtid=%d)=%d\r\n",Ql_atoi(p),ret);
                    if(ret < 0)
                    {
                        OUT_DEBUG(textBuf,"failed to create socket\r\n");
                        break;
                    }

                   socketonly[Ql_atoi(p)] = ret;

                    ret = Ql_SocketConnect(ret, address, port);
                    OUT_DEBUG(textBuf,"Ql_SocketConnect(ip=%d,%d,%d,%d,port=%d)=%d\r\n",address[0],address[1],address[2],address[3],port,ret);
                    if(ret == 0)
                    {
                        OUT_DEBUG(textBuf,"Ql_SocketConnect connect successed\r\n");
                    }
                    else if(ret == -2)
                    {
                        OUT_DEBUG(textBuf,"please wait CallBack_socket_connect\r\n");
                    }
                    else
                    {
                        OUT_DEBUG(textBuf,"Ql_SocketConnect run error\r\n");
                    }
                    
                    break;
                }

                /*socketclose=0-1*/
                p = Ql_strstr(pData,"socketclose=");
                if (p)
                {
                    s32 ret;
                    p = Ql_strstr(pData,"=");
                    p += 1;
                    if(socketonly[Ql_atoi(p)] == 0xFF)
                    {
                        OUT_DEBUG(textBuf,"socket already close\r\n");
                        break;
                    }
                    ret = Ql_SocketClose(socketonly[Ql_atoi(p)]);
                    OUT_DEBUG(textBuf,"Ql_SocketClose(socket=%d)=%d\r\n",socketonly[Ql_atoi(p)],ret);
                    socketonly[Ql_atoi(p)] = 0xFF;
                    send_dataLen[Ql_atoi(p)] = 0;
                    break;
                }

                /*socketsend=0-1*/
                p = Ql_strstr(pData,"socketsend="); //tcp send
                if (p)
                {
                    s32 ret;
                    p = Ql_strstr(pData,"=");
                    p += 1;
                    if(socketonly[Ql_atoi(p)]  == 0xFF)
                    {
                        OUT_DEBUG(textBuf,"socket not create\r\n");
                        break;
                    }

                    if(send_dataLen[Ql_atoi(p)]  > 0)
                    {
                        OUT_DEBUG(textBuf,"socket now busy, sending\r\n");
                        break;
                    }
                    
                    send_dataLen[Ql_atoi(p)]  = 0;
                    dataLen[Ql_atoi(p)]  = DATA_LEN;
                    do
                    {
                        ret = Ql_SocketSend(socketonly[Ql_atoi(p)] , data_p + send_dataLen[Ql_atoi(p)]  ,dataLen[Ql_atoi(p)]  - send_dataLen[Ql_atoi(p)] );
                        OUT_DEBUG(textBuf,"Ql_SocketSend(socket=%d,dataLen=%d)=%d\r\n",socketonly[Ql_atoi(p)] ,dataLen[Ql_atoi(p)] ,ret);
                        if(ret == (dataLen[Ql_atoi(p)]  - send_dataLen[Ql_atoi(p)] ))
                        {
                            //send compelete
                            send_dataLen[Ql_atoi(p)]  = 0;
                            break;
                        }
                        else if((ret < 0) && (ret == -2)) 
                        {
                            //you must wait CallBack_socket_write, then send data;                                                    
                            break;
                        }
                        else if(ret < 0)
                        {
                            //error , Ql_SocketClose
                            Ql_SocketClose(socketonly[Ql_atoi(p)]);
                            socketonly[Ql_atoi(p)] = 0xFF;
                            send_dataLen[Ql_atoi(p)]  = 0;
                            break;
                        }
                        else if(ret <= dataLen[Ql_atoi(p)] )
                        {
                            send_dataLen[Ql_atoi(p)]  += ret;
                            //continue send
                        }
                    }while(1);
                    break;
                }

                /*socketack=*/
                p = Ql_strstr(pData,"socketack=");  // tcp ack
                if (p)
                {
                    s8 ret;
                    u64 ackedNum;
                    p = Ql_strstr(pData,"=");
                    p += 1;
                    if(socketonly[Ql_atoi(p)] == 0xFF)
                    {
                        OUT_DEBUG(textBuf,"socket not create\r\n");
                        break;
                    }
                    ret = Ql_SocketTcpAckNumber(socketonly[Ql_atoi(p)], &ackedNum);
                    OUT_DEBUG(textBuf,"Ql_SocketTcpAckNumber(socket=%d,ackedNum=%d)=%d\r\n",socketonly[Ql_atoi(p)],ackedNum,ret);
                    break;
                }
                
                /*udpcreate=0-1*/
                p = Ql_strstr(pData,"udpcreate="); //udp socket
                if (p)
                {
                    s32 ret;
                    OpenCpuNetWorkState_e networkstate;
                    u8 ps_status;
                    
                    p = Ql_strstr(pData,"=");
                    p += 1;
                    ret = Ql_SocketCreate(Ql_atoi(p), 1);
                    OUT_DEBUG(textBuf,"Ql_SocketCreate(contxtid=%d)=%d\r\n",Ql_atoi(p),ret);
                    if(ret < 0)
                    {
                        OUT_DEBUG(textBuf,"failed to create socket\r\n");
                        break;
                    }
                    udponly[Ql_atoi(p)] = ret;
                    break;
                }

                /*udpclose=0-1*/
                p = Ql_strstr(pData,"udpclose=");
                if (p)
                {
                    s32 ret;
                    p = Ql_strstr(pData,"=");
                    p += 1;
                    if(udponly[Ql_atoi(p)] == 0xFF)
                    {
                        OUT_DEBUG(textBuf,"socket already close\r\n");
                        break;
                    }
                    ret = Ql_SocketClose(udponly[Ql_atoi(p)]);
                    OUT_DEBUG(textBuf,"Ql_SocketClose(socket=%d)=%d\r\n",udponly[Ql_atoi(p)],ret);
                    udponly[Ql_atoi(p)] = 0xFF;
                    send_dataLen[Ql_atoi(p)] = 0;
                    break;
                }

                /*SendHeartBeat*/
                p = Ql_strstr(pData,"SendHeartBeat");
                if (p)
                {
                    s32 ret;
                    ret = Ql_SwitchHeartBeat(1, 70, 15);
                    OUT_DEBUG(textBuf,"!!!!SET heart Beat =%d!!!!\r\n",ret);
                    break;
                }

                /*udpsend=0-1*/
                p = Ql_strstr(pData,"udpsend=");
                if (p)
                {
                    s32 ret;
                    p = Ql_strstr(pData,"=");
                    p += 1;
                    if(udponly[Ql_atoi(p)]  == 0xFF)
                    {
                        OUT_DEBUG(textBuf,"socket not create\r\n");
                        break;
                    }

                    if(send_dataLen[Ql_atoi(p)]  > 0)
                    {
                        OUT_DEBUG(textBuf,"socket now busy, sending\r\n");
                        break;
                    }
                    
                    send_dataLen[Ql_atoi(p)]  = 0;
                    dataLen[Ql_atoi(p)]  = DATA_LEN;
                    ret = Ql_SocketSendTo(udponly[Ql_atoi(p)], data_p, DATA_LEN, address, port);

                    OUT_DEBUG(textBuf,"Ql_SocketSendTo(socket=%d,dataLen=%d)=%d\r\n",udponly[Ql_atoi(p)] ,DATA_LEN ,ret);
                    if(ret == DATA_LEN)
                    {
                        //send success
                    }
                    else if(ret < 0)
                    {
                        //error , Ql_SocketClose
                        Ql_SocketClose(udponly[Ql_atoi(p)]);
                        udponly[Ql_atoi(p)] = 0xFF;
                        send_dataLen[Ql_atoi(p)]  = 0;
                        OUT_DEBUG(textBuf,"Ql_SocketSendTo send error\r\n");
                    }
                    else
                    {
                        //error , Ql_SocketClose
                        Ql_SocketClose(udponly[Ql_atoi(p)]);
                        udponly[Ql_atoi(p)] = 0xFF;
                        send_dataLen[Ql_atoi(p)]  = 0;
                        OUT_DEBUG(textBuf,"Ql_SocketSendTo send error\r\n");
                    }
                    break;
                }
                break;            
            }
            
            default:
                break;
        }
    }

    Ql_GprsNetworkUnInitialize(0);
    Ql_GprsNetworkUnInitialize(1);
}


void CallBack_GprsAPNSet(bool result, s32 error_code)
{
    OUT_DEBUG(textBuf,"CallBack_GprsAPNSet(result=%d,error_code=%d)\r\n",result,error_code);
}

void CallBack_GprsAPNGet(u8 profileid, bool result, s32 error_code, u8 *apn, u8 *userId, u8 *password)
{
    OUT_DEBUG(textBuf,"CallBack_GprsAPNGet(profileid=%d,result=%d,error_code=%d,apn=%s,userId=%s,password=%s)\r\n",profileid,result,error_code,apn,userId,password);
}

void CallBack_network_actived(u8 contexid)
{
    s8 ret;
    u8 ip_addr[4];
    
    OUT_DEBUG(textBuf,"CallBack_network_actived(contexid=%d)\r\n",contexid);

    ret = Ql_GetLocalIpAddress(contexid, ip_addr);
    OUT_DEBUG(textBuf,"Ql_GetLocalIpAddress(contexid=%d)=%d, ip=%d.%d.%d.%d\r\n",contexid, ret,ip_addr[0],ip_addr[1],ip_addr[2],ip_addr[3]);
}

void CallBack_network_deactived(u8 contexid, s32 error_cause, s32 error)
{
    OUT_DEBUG(textBuf,"CallBack_network_deactived(contexid=%d,error_cause=%d, error=%d)\r\n",contexid,error_cause,error);
    if(socketonly[0] >= 0)
    {
        Ql_SocketClose(socketonly[0]);
        socketonly[0] = 0xFF;
        send_dataLen[0]  = 0;
    }
    if(socketonly[1] >= 0)
    {
        Ql_SocketClose(socketonly[1]);
        socketonly[1] = 0xFF;
        send_dataLen[1]  = 0;
    }

    if(udponly[0] >= 0)
    {
        Ql_SocketClose(udponly[0]);
        udponly[0] = 0xFF;
        send_dataLen[0]  = 0;
    }
    if(udponly[1] >= 0)
    {
        Ql_SocketClose(udponly[1]);
        udponly[1] = 0xFF;
        send_dataLen[1]  = 0;
    }
    
}


void CallBack_getipbyname(u8 contexid, bool result, s32 error, u8 num_entry, u8 *entry_address)
{
    u8 i;
    OUT_DEBUG(textBuf,"CallBack_getipbyname(contexid=%d, result=%d,error=%d,num_entry=%d)\r\n",contexid, result,error,num_entry);
    for(i=0;i<num_entry;i++)
    {
        entry_address += (i*4);
        OUT_DEBUG(textBuf,"entry=%d, ip=%d.%d.%d.%d\r\n",i,entry_address[0],entry_address[1],entry_address[2],entry_address[3]);
    }
}


void CallBack_socket_connect(u8 contexid, s8 sock, bool result, s32 error)
{
    OUT_DEBUG(textBuf,"CallBack_socket_connect(contexid=%d,sock=%d,result=%d,error=%d)\r\n",contexid,sock,result,error);
    //now , here , you can use Ql_SocketSend to send data 
}
void CallBack_socket_close(u8 contexid, s8 sock, bool result, s32 error)
{
    OUT_DEBUG(textBuf,"CallBack_socket_close(contexid=%d,sock=%d,result=%d,error=%d)\r\n",contexid,sock,result,error);
    Ql_SocketClose(sock);
    if(socketonly[0] == sock)
    {
        socketonly[0] = 0xFF;
        send_dataLen[0]  = 0;
    }
    if(socketonly[1] == sock)
    {
        socketonly[1] = 0xFF;
        send_dataLen[1]  = 0;
    }
}
void CallBack_socket_accept(u8 contexid, s8 sock, bool result, s32 error)
{
}
void CallBack_socket_read(u8 contexid, s8 sock, bool result, s32 error)
{
    s32 ret;
    u8 data_p[31];
    s32 dataLen;
    bool istcp = TRUE;


    if(udponly[0] == sock)
    {
        istcp = FALSE;
    }
    if(udponly[1] == sock)
    {
        istcp = FALSE;
    }       
            
    
    OUT_DEBUG(textBuf,"%s:CallBack_socket_read(contexid=%d,sock=%d,result=%d,error=%d)\r\n",istcp?"TCP":"UDP",contexid,sock,result,error);

    if(istcp) // tcp recv
    {
        do
        {
            ret = Ql_SocketRecv(sock, data_p, 30);

            if((ret < 0) && (ret != -2))
            {
                OUT_DEBUG(textBuf,"TCP Ql_SocketRecv()=%d, error exit\r\n",ret );
                Ql_SocketClose(sock); //you can close this socket
                if(socketonly[0] == sock)
                {
                    socketonly[0] = 0xFF;
                    send_dataLen[0]  = 0;
                }
                if(socketonly[1] == sock)
                {
                    socketonly[1] = 0xFF;
                    send_dataLen[1]  = 0;
                }            
            }
            else if(ret == -2)
            {
                //wait next CallBack_socket_read
            }
            else if(ret < 30)
            {
                data_p[ret]='\0';
                OUT_DEBUG(textBuf,"TCP sock(%d)len(%d):%s\r\n",sock,ret,data_p);
                //wait next CallBack_socket_read
                break;
            }
            else if(ret == 30)
            {
                data_p[ret]='\0';
                OUT_DEBUG(textBuf,"TCP sock(%d)len(%d):%s\r\n",sock,ret,data_p);
                //continue loop recv data
            }
            else if(ret > 30)
            {
                //do not here
            }
        }while(1);
    }
    else  // udp recv
    {
        u8 address_peer[4];
        u16 port_peer;

        do
        {
            ret = Ql_SocketRecvFrom(sock, data_p, 30, address_peer, &port_peer);

            if((ret < 0) && (ret == -2)) 
            {
                //you shoud wait next CallBack_socket_read
                break;
            }
            else if(ret < 0) 
            {
                OUT_DEBUG(textBuf,"UDP Ql_SocketRecvFrom()=%d, error exit\r\n",ret );
                Ql_SocketClose(sock); //you can close this socket
                if(udponly[0] == sock)
                {
                    udponly[0] = 0xFF;
                    send_dataLen[0]  = 0;
                }
                if(udponly[1] == sock)
                {
                    udponly[1] = 0xFF;
                    send_dataLen[1]  = 0;
                }        
                break;
            }
            else 
            {
                OUT_DEBUG(textBuf,"UDP: ip=%d.%d.%d.%d, port=%d\r\n",address_peer[0],address_peer[1],address_peer[2],address_peer[3],ret,port_peer);
                data_p[ret]='\0';
                OUT_DEBUG(textBuf,"UDP: sock(%d)len(%d):%s\r\n",sock,ret,data_p);
            }
        }
        while(1);
    }

}
void CallBack_socket_write(u8 contexid, s8 sock, bool result, s32 error)
{
    s32 ret;
    s32 i;
    OUT_DEBUG(textBuf,"CallBack_socket_write(contexid=%d,sock=%d,result=%d,error=%d)\r\n",contexid,sock,result,error);
    //now , here , you can continue use Ql_SocketSend to send data 

    for(i=0;i<2;i++)
    {
        if(socketonly[i] == sock)
        {
            break;
        }
    }

    if(i>=2)
    {
        OUT_DEBUG(textBuf,"unknown socket\r\n");
        return;
    }
    
    while(send_dataLen[i] > 0)
    {
        ret = Ql_SocketSend(sock, data_p + send_dataLen[i] ,dataLen[i] - send_dataLen[i]);
        OUT_DEBUG(textBuf,"Ql_SocketSend(socket=%d,dataLen=%d)=%d\r\n",socketonly[i],dataLen[i],ret);
        if(ret == (dataLen[i] - send_dataLen[i]))
        {
            //send compelete
            send_dataLen[i] = 0;
            break;
        }
        else if((ret < 0) && (ret == -2)) 
        {
            //you must wait next CallBack_socket_write, then send data;                                                    
            break;
        }
        else if(ret < 0)
        {
            //error , Ql_SocketClose
            Ql_SocketClose(sock); //you can close this socket
            socketonly[i] = 0xFF;
            send_dataLen[i]  = 0;
            break;
        }
        else if(ret <= dataLen[i])
        {
            send_dataLen[i] += ret;
            //continue send
        }
    }

}

#endif // __EXAMPLE_TCPIP__

