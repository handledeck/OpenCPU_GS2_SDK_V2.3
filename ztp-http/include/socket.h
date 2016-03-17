#ifndef __SOCKET__
#define __SOCKET__


#include "ql_type.h"
#include "Ql_stdlib.h"
#include "global.h"
#include "utils.h"
#include "transport.h"
#include "ql_interface.h"
#include "Ql_tcpip.h"
#include "websrv.h"
#include "files.h"
/*************************************************************
* config network param
**************************************************************/

#define DATA_LEN 10240
#define MAX_CONNECT 5 
#define MAXCLIENT_NUM 5

typedef enum
{
    NONE=0,
    HTTP=1,
    STREAM=2
}Protocol;

/*************************************************************
* implement
**************************************************************/

typedef struct
{
    Protocol protocol=NONE;
    s8 socketId;
    s8 userId;
    u8 sendBuffer[DATA_LEN];
    u8 recvBuffer[DATA_LEN];
    s32   sendRemain_len;
    s32   recvRemain_len;
    void *pSendCurrentPos;
    void *pRecvCurrentPos;
    s32 (*read_handle_callback)(u8 contexid, s8 sock, bool result, s32 error, s32 index);
    s32 (*send_handle_callback)(u8 contexid, s8 sock, bool result, s32 error, s32 index);
    s32 (*send_handle)(s8 sock, char *PData);
}QlClient;

extern QlClient __client[];



s32 func_read_handle_callback(u8 contexid, s8 sock, bool result, s32 error, s32 index);
s32 func_send_handle_callback(u8 contexid, s8 sock, bool result, s32 error, s32 index);
s32 func_send_handle(s8 sock, char *PDtata);   
 
s32 func_send_handle(s8 sock, char *PDtata);  
s32 findClientBySockid(s8 sock);  
void client_init(s32 index,s8 accept_socket);
void client_uninit(s32 index);
void client_socketId_init(void);
void send_all_stream(u8* data,u16 len);

#endif 
