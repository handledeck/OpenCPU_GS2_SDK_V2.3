#include "ql_type.h"
#include "Ql_tcpip.h"

#ifndef __TRANSPORT_H__
#define __TRANSPORT_H__

#define LEN_SOCKET_BUFFER 1024
#define TIME_SECOND 1000
#define TIME_REBOOT_WAIT (40*TIME_SECOND)



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
    STATE_SOCKET_RECIVE=9
}STATE_TCP;

extern STATE_TCP nSTATE;
extern u8 __socketBuffer[];
extern u8 __waitConfirm;
extern u8 __local_ip_addr[4];

void GprsState(void);
void InitNetwork(void);
void CheckReboot(bool repeat_send);

void CbGprsAPNSet(bool result, s32 error_code);
void Cbnetwork_actived(u8 contexid);
void Cbnetwork_deactived(u8 contexid,  s32 error_cause, s32 error);
void Cbsocket_connect(u8 contexid, s8 sock, bool result, s32 error);
void Cbsocket_close(u8 contexid, s8 sock, bool result, s32 error);   
void Cbsocket_read(u8 contexid, s8 sock, bool result, s32 error);
void Cbsocket_write(u8 contexid, s8 sock, bool result, s32 error);


#endif 
