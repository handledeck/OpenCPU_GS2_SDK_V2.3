#ifdef __EXAMPLE_TCPLONG__

/***************************************************************************************************
*   Example:
*       
*           TCPIP Routine
*
*   Description:
*
*           This example create tcp connect, interval 5s send 100byte to server, 
*           and server response data when disconnect, program first query dns, then reconnect tcp server, resend.
*           Through MAIN Uart port, you needn't input any command. This routine will be run automatically and you should analyse the output info.
*           
*   Usage:
*
*           Compile & Run:
*
*               Use "make tcpip" to compile, and download bin image to module to run.
*           
*           Operation: (Through MAIN port)
*                        
*               In this routine, at first you should config network param. Modify host name, APN name and port you want to connect with, like this:
*
*                      #define HOST_NAME "quectel.3322.org"    // host name
*                       u16 port = 7021; // ip port
*                       #define APN_NAME  "CMNET\0"
*
*               Then Compile & Run it. 
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
#include "ql_audio.h"
#include "ql_timer.h"
#include "ql_stdlib.h"
#include "ql_pin.h"
#include "Ql_multitask.h"
#include "Ql_tcpip.h"
#include "Ql_fcm.h"

/*************************************************************
* config network param
**************************************************************/
#define HOST_NAME "quectel.3322.org"    // host name
//#define HOST_NAME "116.226.37.104"    //  host ip
u16 port = 7021; // ip port
#define APN_NAME  "CMNET\0"

#define NORMAL_TIMER  800 // 800ms
#define SENDDATA_TIMER  30000 // 20000ms

#define STATE_QUERY_DNS_MAX_NUMBER 10
#define QUERY_NDS_CALLBACK_TIME 60000


#define DATA_LEN 2048


/*************************************************************
* implement
**************************************************************/
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
    STATE_CREATE_TCPCONNECT,
    STATE_CREATE_TCPCONNECT_ING,
    STATE_TRANSFERS_TCPDATA,
    STATE_TRANSFERS_TCPDATA_ING,
    STATE_TRANSFERS_TCPDATA_CHECKACK,
    STATE_DEACTIVE_PDP,
    STATE_TOTAL_NUM
}STATE_TCPLONG;


STATE_TCPLONG  nSTATE_TCPLONG = STATE_QUERY_NETWORKSTATUS;
u8 ipaddress[4];
u8 tcpsocket;

//////////////////////////////////////////////////////////////////////
//Debug
char textBuf[100];
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
//send data
void *pMemSend = NULL;
void *pCurrentPos = NULL;
u32  remain_len;
u64 ackedNum = 0;

u32  serialnum = 0;


u32 query_dns_number = 0;
QlTimer timer_querydns;

/**************************************************************
* this is main task
***************************************************************/
 
QlEventBuffer    flSignalBuffer; // Set flSignalBuffer to global variables  may as well, otherwise it will occupy stack space
void ql_entry()
{
    bool keepGoing = TRUE;
    s32 ret;
    QlTimer timer;
    
    Ql_SetDebugMode(BASIC_MODE);   
    Ql_DebugTrace("tcplong: ql_entry\r\n");
    Ql_OpenModemPort(ql_md_port1);


    ret = Ql_GprsNetworkInitialize(0,  0, &callback_func);
    //if(ret != QL_SOC_SUCCESS) failed


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
                                nSTATE_TCPLONG = STATE_CREATE_TCPCONNECT; // to connect tcp server
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
                                    nSTATE_TCPLONG = STATE_CREATE_TCPCONNECT; // to connect tcp server
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

                        case  STATE_CREATE_TCPCONNECT:
                        {
                            nSTATE_TCPLONG = STATE_CREATE_TCPCONNECT_ING;

                            serialnum = 0;
                            tcpsocket = Ql_SocketCreate(0, 0);
                            //if(tcpsocket < 0) failed
                            OUT_DEBUG(textBuf,"Ql_SocketCreate()=%d\r\n",tcpsocket);

                            ret = Ql_SocketConnect(tcpsocket, ipaddress, port);
                            OUT_DEBUG(textBuf,"Ql_SocketConnect(ip=%d,%d,%d,%d,port=%d)=%d\r\n",ipaddress[0],ipaddress[1],ipaddress[2],ipaddress[3],port,ret);
                            if(ret == QL_SOC_SUCCESS)
                            {
                                nSTATE_TCPLONG = STATE_TRANSFERS_TCPDATA;
                                OUT_DEBUG(textBuf,"Ql_SocketConnect connect successed\r\n");
                            }
                            else if(ret == QL_SOC_WOULDBLOCK)
                            {
                                OUT_DEBUG(textBuf,"please wait CallBack_socket_connect\r\n");
                                //to waiting CallBack_socket_connect
                            }
                            else
                            {
                                OUT_DEBUG(textBuf,"Ql_SocketConnect run error\r\n");

                                Ql_SocketClose(tcpsocket);
                                tcpsocket = 0xFF;
                                
                                if(ret == QL_SOC_BEARER_FAIL)  
                                {
                                    nSTATE_TCPLONG = STATE_DEACTIVE_PDP;
                                     OUT_DEBUG(textBuf,"Ql_SocketConnect error QL_SOC_BEARER_FAIL to Ql_GprsNetworkDeactive\r\n");
                                    Ql_GprsNetworkDeactive(0);
                                }
                                else
                                {
                                    nSTATE_TCPLONG = STATE_QUERY_DNS;
                                }
                            }
                            break;
                        }
                        
                        case  STATE_TRANSFERS_TCPDATA:
                        {
                            ///////////////////////////////////////////////////////////////////////////////
                            //alloc data begin
                            pMemSend = Ql_GetMemory(DATA_LEN);
                            remain_len = DATA_LEN;
                            pCurrentPos = pMemSend;
                            OUT_DEBUG(textBuf,"Ql_GetMemory(%d)=%x\r\n",DATA_LEN,pMemSend);
                            if(!pMemSend)
                                break;
                            Ql_sprintf((char*)pMemSend,"%d",serialnum);
                            Ql_memset((char*)pMemSend+Ql_strlen((char*)pMemSend), 'A', DATA_LEN-Ql_strlen((char*)pMemSend));
                            serialnum++;
                            //alloc data end
                            ///////////////////////////////////////////////////////////////////////////////
                            
                            ret = Ql_SocketTcpAckNumber(tcpsocket, &ackedNum);
                            //if(QL_SOC_SUCCESS != ret) failed
                            OUT_DEBUG(textBuf,"Ql_SocketTcpAckNumber(socket=%d,ackedNum=%llu)=%d,DATA_LEN=%d\r\n",tcpsocket,ackedNum,ret,DATA_LEN);
                            do
                            {
                                ret = Ql_SocketSend(tcpsocket , (u8*)pCurrentPos , remain_len );
                                OUT_DEBUG(textBuf,"Ql_SocketSend(socket=%d,remain_len=%d)=%d,serialnum=%d\r\n",tcpsocket ,remain_len ,ret,serialnum);
                                if(ret == remain_len)
                                {
                                    //send compelete
                                    remain_len = 0;
                                    Ql_FreeMemory(pMemSend);
                                    pMemSend = NULL;
                                    pCurrentPos = NULL;
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
                                    //error , Ql_SocketClose
                                    Ql_SocketClose(tcpsocket);
                                    tcpsocket = 0xFF;

                                    remain_len = 0;
                                    Ql_FreeMemory(pMemSend);
                                    pMemSend = NULL;
                                    pCurrentPos = NULL; 
                                    

                                    if(ret == QL_SOC_BEARER_FAIL)  
                                    {
                                        nSTATE_TCPLONG = STATE_DEACTIVE_PDP;
                                     OUT_DEBUG(textBuf,"Ql_SocketSend error QL_SOC_BEARER_FAIL to Ql_GprsNetworkDeactive\r\n");
                                        Ql_GprsNetworkDeactive(0);
                                    }
                                    else
                                    {
                                        nSTATE_TCPLONG = STATE_QUERY_DNS; // will to Ql_GetHostIpbyName
                                    }
                                    
                                    break;
                                }
                                else if(ret < remain_len)
                                {
                                    remain_len -= ret;
                                    (char*)pCurrentPos += ret;
                                     //continue send, do not send all data
                                }
                            }while(1);
                            break;
                        }

                        case  STATE_TRANSFERS_TCPDATA_CHECKACK:
                        {
                            u64 ackedNumCurr;
                            //check peer received all data
                            ret = Ql_SocketTcpAckNumber(tcpsocket, &ackedNumCurr);
                            //if(QL_SOC_SUCCESS != ret) failed
                            if((ackedNum + DATA_LEN) == ackedNumCurr)
                            {
                                nSTATE_TCPLONG = STATE_TRANSFERS_TCPDATA;
                            }
                            OUT_DEBUG(textBuf,"Ql_SocketTcpAckNumber(socket=%d,ackedNumCurr=%llu)=%d\r\n",tcpsocket,ackedNumCurr,ret);
                            break;
                         }
                    }

                    if(STATE_TRANSFERS_TCPDATA == nSTATE_TCPLONG)
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
                Ql_SendToUart(ql_uart_port1, (u8*)flSignalBuffer.eventData.uartdata_evt.data, flSignalBuffer.eventData.uartdata_evt.len);        
                break;
            }

            case EVENT_UARTDATA:
            {
                //AT command send to BB, goto run 
                Ql_SendToModem(ql_md_port1, (u8*)flSignalBuffer.eventData.modemdata_evt.data, flSignalBuffer.eventData.modemdata_evt.len);                            
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
        nSTATE_TCPLONG = STATE_CREATE_TCPCONNECT; // to connect tcp server
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
    if(result)
    {
        OUT_DEBUG(textBuf,"CallBack_socket_connect(contexid=%d,sock=%d ) successed \r\n",contexid,sock);
         nSTATE_TCPLONG = STATE_TRANSFERS_TCPDATA;
    }
    else
    {
        OUT_DEBUG(textBuf,"CallBack_socket_connect(contexid=%d,sock=%d,result=%d,error=%d) failed\r\n",contexid,sock,result,error);
        Ql_SocketClose(tcpsocket);
        tcpsocket = 0xFF;

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
}

void CallBack_socket_close(u8 contexid, s8 sock, bool result, s32 error)
{
    OUT_DEBUG(textBuf,"CallBack_socket_close(contexid=%d,sock=%d,result=%d,error=%d)\r\n",contexid,sock,result,error);

    remain_len = 0;
    if(pMemSend)
        Ql_FreeMemory(pMemSend);
    pMemSend = NULL;
    pCurrentPos = NULL;

    Ql_SocketClose(tcpsocket);
    tcpsocket = 0xFF;

    if((error == QL_SOC_BEARER_FAIL)  || (error >=0)) 
    {
        nSTATE_TCPLONG = STATE_DEACTIVE_PDP;
             OUT_DEBUG(textBuf,"CallBack_socket_close error  QL_SOC_BEARER_FAIL to Ql_GprsNetworkDeactive\r\n");
        Ql_GprsNetworkDeactive(0);
    }
    else
    {
        nSTATE_TCPLONG = STATE_QUERY_DNS;
    }

    
}

void CallBack_socket_accept(u8 contexid, s8 sock, bool result, s32 error)
{
    if(!result)
    {
        //if((error == QL_SOC_BEARER_FAIL)  || (error >=0)) Ql_GprsNetworkDeactive(0);
    }
}

void CallBack_socket_read(u8 contexid, s8 sock, bool result, s32 error)
{
    s32 ret;
    u8 data_p[31];
    s32 dataLen;
    u8 address_peer[4];
    u16 port_peer;
    
    OUT_DEBUG(textBuf,"CallBack_socket_read(contexid=%d,sock=%d,result=%d,error=%d)\r\n",contexid,sock,result,error);

    if(!result)
    {
        OUT_DEBUG(textBuf,"CallBack_socket_read() failed\r\n");
        Ql_SocketClose(tcpsocket);
        tcpsocket = 0xFF;

        remain_len = 0;
        if(pMemSend)
            Ql_FreeMemory(pMemSend);
        pMemSend = NULL;
        pCurrentPos = NULL;

        if((error == QL_SOC_BEARER_FAIL)  || (error >=0)) 
        {
            nSTATE_TCPLONG = STATE_DEACTIVE_PDP;
             OUT_DEBUG(textBuf,"CallBack_socket_read error  QL_SOC_BEARER_FAIL to Ql_GprsNetworkDeactive\r\n");
            Ql_GprsNetworkDeactive(0);
        }
        else
        {
            nSTATE_TCPLONG = STATE_QUERY_DNS;
        }
        
        return;
    }

    do
    {
        ret = Ql_SocketRecv(sock, data_p, 30 );

        if(ret == 0)
        {
            //peer close socket
            nSTATE_TCPLONG = STATE_QUERY_DNS;
            OUT_DEBUG(textBuf,"Ql_SocketRecv()=%d, peer close socket\r\n",ret );

            remain_len = 0;
            if(pMemSend)
                Ql_FreeMemory(pMemSend);
            pMemSend = NULL;
            pCurrentPos = NULL;

            Ql_SocketClose(tcpsocket);
            tcpsocket = 0xFF;
            
        }
        else if((ret < 0) && (ret == QL_SOC_WOULDBLOCK)) 
        {
            //you shoud wait next CallBack_socket_read
            break;
        }
        else if(ret < 0) 
        {
            OUT_DEBUG(textBuf,"Ql_SocketRecv()=%d, error exit\r\n",ret );

            remain_len = 0;
            if(pMemSend)
                Ql_FreeMemory(pMemSend);
            pMemSend = NULL;
            pCurrentPos = NULL;

            Ql_SocketClose(tcpsocket);
            tcpsocket = 0xFF;

            if(ret == QL_SOC_BEARER_FAIL) 
            {
                nSTATE_TCPLONG = STATE_DEACTIVE_PDP;
             OUT_DEBUG(textBuf,"Ql_SocketRecv error  in CallBack_socket_readQL_SOC_BEARER_FAIL to Ql_GprsNetworkDeactive\r\n");
                Ql_GprsNetworkDeactive(0);
            }
            else
            {
                nSTATE_TCPLONG = STATE_QUERY_DNS;
            }
            
            break;
        }
        else 
        {
             OUT_DEBUG(textBuf,"Ql_SocketRecv: sock(%d),len(%d),data(%s) \r\n",sock,ret,(char*)data_p);
             // continue to recv data
        }
    }
    while(1);

}
void CallBack_socket_write(u8 contexid, s8 sock, bool result, s32 error)
{
    s32 ret;
    OUT_DEBUG(textBuf,"CallBack_socket_write(contexid=%d,sock=%d,result=%d,error=%d)\r\n",contexid,sock,result,error);
    //now , here , you can continue use Ql_SocketSend to send data 
    while(result)
    {
        ret = Ql_SocketSend(tcpsocket , (u8*)pCurrentPos , remain_len );
        OUT_DEBUG(textBuf,"Ql_SocketSend(socket=%d,remain_len=%d)=%d\r\n",tcpsocket ,remain_len ,ret);
        if(ret == remain_len)
        {
            //send compelete
            remain_len = 0;
            Ql_FreeMemory(pMemSend);
            pMemSend = NULL;
            pCurrentPos = NULL;
            nSTATE_TCPLONG = STATE_TRANSFERS_TCPDATA_CHECKACK;
            break;
        }
        else if((ret < 0) && (ret == QL_SOC_WOULDBLOCK)) 
        {
            //you must next wait CallBack_socket_write, then send data;     
            nSTATE_TCPLONG = STATE_TRANSFERS_TCPDATA_ING;
            break;
        }
        else if(ret < 0)
        {
            //error , Ql_SocketClose
            Ql_SocketClose(tcpsocket);
            tcpsocket = 0xFF;

            remain_len = 0;
            Ql_FreeMemory(pMemSend);
            pMemSend = NULL;
            pCurrentPos = NULL;


            if(ret == QL_SOC_BEARER_FAIL) 
            {
                nSTATE_TCPLONG = STATE_DEACTIVE_PDP;
             OUT_DEBUG(textBuf,"CallBack_socket_write errro QL_SOC_BEARER_FAIL to Ql_GprsNetworkDeactive\r\n");
                Ql_GprsNetworkDeactive(0);
            }
            else
            {
                nSTATE_TCPLONG = STATE_QUERY_DNS; // will to Ql_GetHostIpbyName
            }
            
            break;
        }
        else if(ret < remain_len)
        {
            remain_len -= ret;
            ((char*)pCurrentPos) += ret;
             //continue send, do not send all data
        }
    }

    if(result)
    {

        OUT_DEBUG(textBuf,"CallBack_socket_write() failed\r\n");

        //error , Ql_SocketClose
        Ql_SocketClose(tcpsocket);
        tcpsocket = 0xFF;

        remain_len = 0;
        if(pMemSend)
            Ql_FreeMemory(pMemSend);
        pMemSend = NULL;
        pCurrentPos = NULL;


        if((error == QL_SOC_BEARER_FAIL)  || (error >=0)) 
        {
            nSTATE_TCPLONG = STATE_DEACTIVE_PDP;
             OUT_DEBUG(textBuf,"Ql_SocketSend error  in CallBack_socket_write QL_SOC_BEARER_FAIL to Ql_GprsNetworkDeactive\r\n");
            Ql_GprsNetworkDeactive(0);
        }
        else
        {
            nSTATE_TCPLONG = STATE_QUERY_DNS; // will to Ql_GetHostIpbyName
        }
        
    }
}

#endif // __EXAMPLE_TCPLONG__

