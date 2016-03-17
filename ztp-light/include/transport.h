#include "ql_type.h"
#include "Ql_tcpip.h"

#ifndef __TRANSPORT_H__
#define __TRANSPORT_H__

#define LEN_SOCKET_BUFFER 1024
#define TIME_SECOND 1000
#define TIME_REBOOT_WAIT (40*TIME_SECOND)
#define TIME_WAIT_CONFIRM_MSG 4000


typedef enum{
    STATE_NONE=0,
    STATE_WAIT_CALLBACK=1,    
    STATE_WAIT_GPRS=2,
    STATE_QUERY_NETWORKSTATUS=3,
    STATE_SET_APN=4,
    STATE_ACTIVE_PDP=5,
    STATE_SOCKET_CREATE=6,

    STATE_SOCKET_CONNECT=7,
    STATE_SOCKET_SEND=8,
    STATE_SOCKET_RECIVE=9,
    STATE_ING=10,
    STATE_LISTEN=11,
    STATE_EST_SOCKET_CREATE=12,
    STATE_EST_SOCKET_CLOSE=13,
    STATE_EST_SOCKET_PAUSE=14,
    STATE_TIME_SYNC=15
}STATE_TCP;



extern STATE_TCP nSTATE;
extern u8 __socketBuffer[];
extern u8 __waitConfirm;
extern unsigned char __local_ip_addr[4];
extern char __str_loc_ip_addr[15];
extern s8 __http_socketID;
extern s8 __est_connector_socket;
extern bool __est_connection;

void GprsState(void);
void InitNetwork(void);
void hwTimer_handler(void* param);
void CheckAnyMessage(void* param);
bool CheckSendEmptyMessage(void);
bool StopEstConnection(void);

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



#endif 
