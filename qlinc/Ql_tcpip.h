/*==========================================================================
 |               Quectel OpenCPU --  User API
 |
 |              Copyright (c) 2010 Quectel Ltd.
 |
 |--------------------------------------------------------------------------
 | File Description
 | ----------------
 |      TCP/IP
 |
 |--------------------------------------------------------------------------
 |
 |  Designed by     :   Jay XIN, Willis YANG
 |  Coded    by     :   Jay XIN, Willis YANG
 |  Tested   by     :   Stanley YONG
 |--------------------------------------------------------------------------
 | Revision History
 | ----------------
 |  Sep. 26, 2010       Stanley Yong        Make the notes clear.
 |
 \=========================================================================*/

#ifndef __QL_TCPIP_H__
#define __QL_TCPIP_H__


/****************************************************************************
 * Return Codes Definition for TCP/IP
 ***************************************************************************/
typedef enum
{
    QL_SOC_SUCCESS             = 0,
    QL_SOC_ERROR               = -1,
    QL_SOC_WOULDBLOCK          = -2,
    QL_SOC_LIMIT_RESOURCE      = -3,    /* limited resource */
    QL_SOC_INVALID_SOCKET      = -4,    /* invalid socket */
    QL_SOC_INVALID_ACCOUNT     = -5,    /* invalid account id */
    QL_SOC_NAMETOOLONG         = -6,    /* address too long */
    QL_SOC_ALREADY             = -7,    /* operation already in progress */
    QL_SOC_OPNOTSUPP           = -8,    /* operation not support */
    QL_SOC_CONNABORTED         = -9,    /* Software caused connection abort */
    QL_SOC_INVAL               = -10,   /* invalid argument */
    QL_SOC_PIPE                = -11,   /* broken pipe */
    QL_SOC_NOTCONN             = -12,   /* socket is not connected */
    QL_SOC_MSGSIZE             = -13,   /* msg is too long */
    QL_SOC_BEARER_FAIL         = -14,   /* bearer is broken */
    QL_SOC_CONNRESET           = -15,   /* TCP half-write close, i.e., FINED */
    QL_SOC_DHCP_ERROR          = -16,
    QL_SOC_IP_CHANGED          = -17,
    QL_SOC_ADDRINUSE           = -18,
    QL_SOC_CANCEL_ACT_BEARER   = -19    /* cancel the activation of bearer */
} ql_soc_error_enum;

typedef enum
{
    OPENCPU_NETWORK_STATE_FREE=0,
    OPENCPU_NETWORK_STATE_INITIAL,
    OPENCPU_NETWORK_STATE_ACTIVING,
    OPENCPU_NETWORK_STATE_ACTIVED,
    OPENCPU_NETWORK_STATE_DEACTIVING,
    /*OPENCPU_NETWORK_STATE_DEACTIVED,*/
    OPENCPU_NETWORK_STATE_END
}OpenCpuNetWorkState_e;


/****************************************************************************
 * SIM card state
 ***************************************************************************/
typedef enum
{
    Ql_SIM_NORMAL = 1,              // The normally working state
    Ql_PH_SIM_PIN_REQUIRED = 5,
    Ql_PH_FSIM_PIN_REQUIRED = 6,
    Ql_PH_FSIM_PUK_REQUIRED = 7,
    Ql_PH_SIM_PUK_REQUIRED = 8,
    Ql_SIM_NOT_INSERTED = 10,
    Ql_SIM_PIN_REQUIRED = 11,
    Ql_SIM_PUK_REQUIRED = 12,
    Ql_SIM_FAILURE = 13,
    Ql_SIM_BUSY = 14,
    Ql_SIM_WRONG = 15,
    Ql_SIM_PIN2_REQUIRED = 17,
    Ql_SIM_PUK2_REQUIRED = 18,
    Ql_RMMI_ERR_UNSPECIFIED = 604,  //unspecified parsing error
    Ql_RMMI_CME_SIM_POWERED_DOWN =3772
} Ql_SIM_State;


/****************************************************************************
 * (GPRS) Network registion state
 ***************************************************************************/
typedef enum
{
   Ql_REG_STATE_NOT_REGISTERED=0,  // Not register to network
   Ql_REG_STATE_REGISTERED,        // The normal network state
   Ql_REG_STATE_SEARCHING,         // Searching network
   Ql_REG_STATE_REG_DENIED,        // The request to register to network is denied
   Ql_REG_STATE_UNKNOWN,
   Ql_REG_STATE_NOT_ACTIVE         // The network is inactive
} Ql_Reg_State;


typedef struct
{
    void(*callback_network_actived)(u8 contexid);
    void(*callback_network_deactived)(u8 contexid, s32 error_cause, s32 error);
    void(*callback_socket_connect)(u8 contexid, s8 sock, bool result, s32 error_code);
    void(*callback_socket_close)(u8 contexid, s8 sock, bool result, s32 error_code);
    void(*callback_socket_accept)(u8 contexid, s8 sock, bool result, s32 error_code);
    void(*callback_socket_read)(u8 contexid, s8 sock, bool result, s32 error_code);
    void(*callback_socket_write)(u8 contexid, s8 sock, bool result, s32 error_code);
}OpenCpuTcpIp_Callback_t;

/* Socket Type */ 
typedef enum
{
    SOC_TYPE_TCP = 0,  /* TCP */ 
    SOC_TYPE_UDP       /* datagram socket, UDP */ 
} Ql_Socket_Type;

/****************************************************************************
 * Callback Functions Definition
 ***************************************************************************/
typedef void (*Ql_CallBack_GprsAPNSet)(bool result, s32 error_code);

typedef void (*Ql_CallBack_GprsAPNGet)(u8 profileid,
                                          bool result,
                                          s32 error_code,
                                          u8 *apn,
                                          u8 *userId,
                                          u8 *password);

typedef void (*Ql_CallBack_GetIpByName)(u8 contexid,
                                           bool result,
                                           s32 error_code,
                                           u8 num_entry,
                                           u8 *entry_address);


/*****************************************************************
* Function:     Ql_GprsAPNSet
*
* Description:
*               This function sets the authentication parameters
*               apn/login/password to use with a profile id during
*               PDP activation.
*
* Parameters:
*               profileid:
*                   PDP context profile, which is ranges from 0 to 1.
*
*               apn:
*                   NULL-terminated APN characters.
*
*               userId:
*                   User Id, NULL-terminated characters.
*
*               password:
*                   Password, NULL-terminated characters.
*
*               gprsapnset:
*                   Callback function. The Core System will invoke
*                   this callback function to notify Embedded Application
*                   whether this function succeeds or not.
*
* Return:
*               QL_SOC_WOULDBLOCK:
*                   The app should wait, till the callback function is called.
*                   And the app can get the information of success or
*                   failure in callback function.
*
*               QL_SOC_INVAL:
*                   Invalid argument.
*
*               QL_SOC_ALREADY:
*                   The function is running.
*****************************************************************/
s32 Ql_GprsAPNSet(u8 profileid,
                            u8 *apn,
                            u8 *userId,
                            u8 *password,
                            Ql_CallBack_GprsAPNSet gprsapnset);


/*****************************************************************
* Function:     Ql_GprsAPNGet
*
* Description:
*               This function gets the authentication parameters
*               apn/login/password with a profile id.
*
* Parameters:
*               profileid:
*                   PDP context profile, which ranges from 0 to 1.
*
*               gprsapnget:
*                   Callback function. The Core System will invoke
*                   this callback function to notify Embedded Application
*                   whether this function succeeds or not.
*
* Return:
*               QL_SOC_WOULDBLOCK:
*                   The app should wait, till the callback function is called.
*                   The app can get the information of success or failure in callback function.
*
*               QL_SOC_INVAL:
*                   Invalid argument.
*
*               QL_SOC_ALREADY:
*                   The function is running.
*****************************************************************/
s32   Ql_GprsAPNGet(u8 profileid, Ql_CallBack_GprsAPNGet gprsapnget);


/*****************************************************************
* Function:     Ql_GprsNetworkInitialize
*
* Description:
*               This function initializes the PDP context.
*
* Parameters:
*               contxtid:
*                   OpenCPU supports two PDP-contexts to the destination
*                   host at a time. This parameter can be 0 or 1.
*
*               profileid:
*                   PDP context profile, which ranges from 0 to 1.
*
*               callback_func:
*                   This callback function is called by OpenCPU to inform
*                   Embedded Application whether this function succeeds or not.
*                   And this callback function should be implemented by Embedded Application.
*
* Return:
*               QL_SOC_SUCCESS:
*                   This function suceeds.
*
*               QL_SOC_INVAL:
*                   Invalid argument.
*
*               QL_SOC_ALREADY:
*                   The GPRS network is already initialized.
*****************************************************************/
s32   Ql_GprsNetworkInitialize(u8 contxtid,
                                         u8 profileid,
                                         OpenCpuTcpIp_Callback_t * callback_func);


/*****************************************************************
* Function:     Ql_GprsNetworkUnInitialize
*
* Description:
*               This function restores the PDP context.
*
* Parameters:
*               contxtid:
*                   OpenCPU supports two PDP-contexts to the destination
*                   host at a time. This parameter can be 0 or 1.
*
* Return:
*               QL_SOC_SUCCESS if this function succeeds. Otherwise,
*               other Error Code will be returned. To get extended
*               information, please see Possible Error Codes.
*****************************************************************/
s32   Ql_GprsNetworkUnInitialize(u8 contxtid);


/*****************************************************************
* Function:     Ql_GprsNetworkActive
*
* Description:
*               This function actives the PDP context.
*
* Parameters:
*               contxtid:
*                   OpenCPU supports two PDP-contexts to the destination
*                   host at a time. This parameter can be 0 or 1.
*
* Return:
*               QL_SOC_SUCCESS:
*                   This function suceeds.
*
*               QL_SOC_WOULDBLOCK:
*                   The app should wait, till the callback function is called.
*                   The app can get the information of success or failure in callback function.
*
*               QL_SOC_INVAL:
*                   Invalid argument.
*
*               QL_SOC_ALREADY:
*                   The function is running.
*
*               QL_SOC_BEARER_FAIL:
*                   Bearer is broken.
*****************************************************************/
s32   Ql_GprsNetworkActive(u8 contxtid);


/*****************************************************************
* Function:     Ql_GprsNetworkDeactive
*
* Description:
*               This function deactivates the PDP context.
*
* Parameters:
*               contxtid:
*                   OpenCPU supports two PDP-contexts to the destination
*                   host at a time. This parameter can be 0 or 1.
*
* Return:
*               QL_SOC_SUCCESS:
*                   This function suceeds.
*
*               QL_SOC_WOULDBLOCK:
*                   The app should wait, till the callback function is called.
*                   The app can get the information of success or failure in callback function.
*
*               QL_SOC_INVAL:
*                   Invalid argument.
*
*               QL_SOC_ERROR
*
*****************************************************************/
s32   Ql_GprsNetworkDeactive(u8 contxtid);


/*****************************************************************
* Function:     Ql_GprsNetworkGetState
*
* Description:
*               This function gets the state of GPRS network and PDP context.
*
* Parameters:
*               contxtid:
*                   [in]OpenCPU supports two PDP-contexts to the destination
*                   host at a time. This parameter can be 0 or 1.
*
*               networkstate:
*                   [out] Pointer to OpenCpuNetWorkState_e, in which
*                   the state of PDP context is stored
*
*               ps_status:
*                   [out] The GPRS network state, a value of reg_state_enum.
*                   If the GPRS network is registered, this parameter will be
*                   output with the number 1; any other value indicates that
*                   the GPRS network is not registered.
*
* Return:
*               This return value will be QL_SOC_SUCCESS (0) if the
*               function succeeds; or a negative number (Error Code)
*               will be returned.
*****************************************************************/
s32   Ql_GprsNetworkGetState(u8 contxtid,
                                       OpenCpuNetWorkState_e * networkstate,
                                       u8 * ps_status);


/*****************************************************************
* Function:     Ql_SocketCreate
*
* Description:
*               This function creates a socket. The maximum number of socket is 6.
*
* Parameters:
*               contxtid:
*                   OpenCPU supports two PDP-contexts to the destination
*                   host at a time. This parameter can be 0 or 1.
*
*               socket_type:
*                   A value of socket_type_enum.
*
* Return:
*               The socket ID, or other Error Codes. To get extended
*               information, please see soc_error_enum.
*****************************************************************/
s8     Ql_SocketCreate(u8 contxtid, u8 socket_type);


/*****************************************************************
* Function:     Ql_SocketClose
*
* Description:
*               This function closes a socket.
*
* Parameters:
*               socketId:
*                   A socket Id.
*
* Return:
*               This return value will be QL_SOC_SUCCESS (0) if
*               the function succeeds; or a negative number
*               (Error Code) will be returned.
*****************************************************************/
s32   Ql_SocketClose(s8 socketId);


/*****************************************************************
* Function:     Ql_SocketConnect
*
* Description:
*               Establishes a connection to the socket. The host
*               is specified by an IP address and a port number.
*
* Parameters:
*               socketId:
*                   A socket Id.
*
*               address:
*                   Peer IPv4  address.
*
*               port:
*                   A socket port.
* Return:
*               This return value will be QL_SOC_SUCCESS (0) if
*               the function succeeds; or a negative number
*               (Error Code) will be returned.
*****************************************************************/
s32   Ql_SocketConnect(s8 socketId,
                                 u8 address[4],
                                 u16 port);


/*****************************************************************
* Function:     Ql_SocketSend
*
* Description:
*               The function sends data to a connected socket.
*
* Parameters:
*               socketId:
*                   A socket Id.
*
*               data_p:
*                   Pointer to the data to send.
*
*               dataLen:
*                   Number of bytes to send.
* Return:
*               If no error occurs, Ql_SocketSend returns the total
*               number of bytes sent, which can be less than the number
*               requested to be sent in the dataLen parameter.
*               Otherwise, a value of ql_soc_error_enum is returned.
*****************************************************************/
s32   Ql_SocketSend(s8 socket, u8 * data_p, s32 dataLen);


/*****************************************************************
* Function:     Ql_SocketRecv
*
* Description:
*               The function receives data from a bound socket.
*
* Parameters:
*               socketId:
*                   A socket Id.
*
*               data_p:
*                   Pointer to a buffer that is the storage space
*                   for the received data.
*
*               dataLen:
*                   Length of data_p, in bytes.
* Return:
*               If no error occurs, Ql_SocketRecv returns the total
*               number of bytes received. Otherwise, a value of
*               ql_soc_error_enum is returned.
*****************************************************************/
s32   Ql_SocketRecv(s8 socket, u8 * data_p, s32 dataLen);


/*****************************************************************
* Function:     Ql_SocketTcpAckNumber
*
* Description:
*               The function gets the TCP socket ACK number.
*
* Parameters:
*               socket:
*                   [in] Socket Id.
*
*               ackedNum:
*                   [out] Pointer to a u64 type that is the storage
*                   space for the TCP ACK number
* Return:
*               If no error occurs, Ql_SocketRecv returns the total
*               number of bytes received. Otherwise, a value of
*               ql_soc_error_enum is returned.
*****************************************************************/
s32   Ql_SocketTcpAckNumber(s8 socket, u64 * ackedNum);


/*****************************************************************
* Function:     Ql_SocketSendTo
*
* Description:
*               The function sends data to a specific destination
*               through UDP socket.
*
* Parameters:
*               socket:
*                   [in] Socket Id to send to.
*
*               data_p:
*                   [in] Buffer containing the data to be transmitted.
*
*               dataLen:
*                   [in] Length of the data in data_p, in bytes.
*
*               address:
*                   [in] Pointer to the address of the target socket.
*
*               port:
*                   [in] The target port number.
* Return:
*               If no error occurs, this function returns the number
*               of bytes actually sent. Otherwise, a value of
*               ql_soc_error_enum is returned.
*****************************************************************/
s32   Ql_SocketSendTo(s8 socket,
                                u8 * data_p,
                                s32 dataLen,
                                u8 address[4],
                                u16 port);


/*****************************************************************
* Function:     Ql_SocketRecvFrom
*
* Description:
*               The function receives a datagram data through TCP socket.
*
* Parameters:
*               socket:
*                   [in] Socket Id to receive from.
*
*               data_p:
*                   [out] Buffer for the incoming data.
*
*               dataLen:
*                   [in] Length of data_p, in bytes.
*
*               address:
*                   [out] An optional pointer to a buffer that
*                   receives the address of the connecting entity.
*
*               port:
*                   [out] An optional pointer to an integer that
*                   contains hte port number of the connecting entity.
* Return:
*               If no error occurs, this function returns the number
*               of bytes received. Otherwise, a value of
*               ql_soc_error_enum is returned.
*****************************************************************/
s32   Ql_SocketRecvFrom(s8 socket,
                                  u8 * data_p,
                                  s32 dataLen,
                                  u8 address[4],
                                  u16 * port);


/*****************************************************************
* Function:     Ql_SocketListen
*
* Description:
*               The function places a socket in a state in which
*               it is listening for an incoming connection.
*
* Parameters:
*               listensocket:
*                   The listened socket id.
*
*               address:
*                   Local IPv4  address.
*
*               port:
*                   Local IPv4 listen port.
*
*               maxconnections:
*                   Maximum connection number.
* Return:
*               If no error occurs, this function returns QL_SOC_SUCCESS (0).
*               Otherwise, a value of ql_soc_error_enum is returned.
*****************************************************************/
s32   Ql_SocketListen(s8 listensocket,
                                u8 address[4],
                                u16 port,
                                u8 maxconnections);


/*****************************************************************
* Function:     Ql_SocketBind
*
* Description:
*               This function associates a local address with a socket.
*
* Parameters:
*               socketId:
*                   Descriptor identifying an unbound socket.
*
*               socktype:
*                   Socket type, which can be
*                       0 (socket stream),
*                       1 (socket datagram).
*
*               port:
*                   Socket port number.
*
* Return:
*               If no error occurs, this function returns QL_SOC_SUCCESS (0).
*               Otherwise, a value of ql_soc_error_enum is returned.
*****************************************************************/
s32   Ql_SocketBind(s8 socketId, u8 socktype, u16 port);


/*****************************************************************
* Function:     Ql_SocketAccept
*
* Description:
*               The function permits a connection attempt on a socket.
*
* Parameters:
*               listensocket:
*                   [in]  The listened socket id.
*
*               address:
*                   [out] An optional pointer to a buffer that
*                   receives the address of the connecting entity.
*
*               port:
*                   [out] An optional pointer to an integer that
*                   contains hte port number of the connecting entity.
*
* Return:
*               If no error occurs, this function returns a socket Id,
*               which is greater than or equal to zero.
*               Otherwise, a value of ql_soc_error_enum is returned.
*****************************************************************/
s8  Ql_SocketAccept(s8 listensocket,
                              u8 address[4],
                              u16 * port);


/*****************************************************************
* Function:     Ql_GetHostIpbyName
*
* Description:
*               The function retrieves host IP corresponding to a host name.
*
* Parameters:
*               contxtid:
*                   [in] OpenCPU supports two PDP-contexts to the destination
*                   host at a time. This parameter can be 0 or 1.
*
*               hostname:
*                   [in] The host name.
*
*               addr:
*                   [out] The buffer of the host IPv4 address.
*
*               addr_len:
*                   [out] The length of addr.
*
*               in_entry_num:
*                   [in] address number
*
*               out_entry_num:
*                   [out] get address number
*
*               callback_getipbyname:
*                   [in] This callback is called by Core System to notify
*                   whether this function retrieves host IP successfully or not.
*
* Return:
*               If no error occurs, this function returns QL_SOC_SUCCESS (0).
*               Otherwise, a value of ql_soc_error_enum is returned.
*****************************************************************/
s32 Ql_GetHostIpbyName(u8 contxtid,
                                 u8 *hostname,
                                 u8 *addr,
                                 u8 *addr_len,
                                 u8 in_entry_num,
                                 u8 *out_entry_num,
                                 Ql_CallBack_GetIpByName callback_getipbyname);


/*****************************************************************
* Function:     Ql_GetLocalIpAddress
*
* Description:
*               This function retrieves local IP corresponding
*               to the specified PDP context.
*
* Parameters:
*               contxtid:
*                   [in] OpenCPU supports two PDP-contexts to the destination
*                   host at a time. This parameter can be 0 or 1.
*
*               ip_addr:
*                   [out] Pointer to the buffer that is the storage
*                   space for the local IPv4 address.
*
* Return:
*               If no error occurs, this function returns QL_SOC_SUCCESS (0).
*               Otherwise, a value of ql_soc_error_enum is returned.
*****************************************************************/
s32   Ql_GetLocalIpAddress(u8 contxtid, u8 ip_addr[4]);


/*****************************************************************
* Function:     Ql_GetDnsServerAddress
*
* Description:
*               This function retrieves the DNS server's IP address.
*
* Parameters:
*               contxtid:
*                   [in] OpenCPU supports two PDP-contexts to the destination
*                   host at a time. This parameter can be 0 or 1.
*
*               primary_address:
*                   [out] Pointer to the buffer that is the storage
*                   space for the primary DNS server's IP address.
*
*               secondary_address:
*                   [out] Pointer to the buffer that is the storage
*                   space for the secondary DNS server's IP address.
*
* Return:
*               If no error occurs, this function returns QL_SOC_SUCCESS (0).
*               Otherwise, a value of ql_soc_error_enum is returned.
*****************************************************************/
s32   Ql_GetDnsServerAddress(u8 contextId,
                                       u8 primary_address[4],
                                       u8 secondary_address[4]);


/*****************************************************************
* Function:     Ql_SetDnsServerAddress
*
* Description:
*               This function sets the DNS server's IP address.
*
* Parameters:
*               contxtid:
*                   [in] OpenCPU supports two PDP-contexts to the destination
*                   host at a time. This parameter can be 0 or 1.
*
*               primary_set:
*                   [in] TRUE or FALSE. Enable to set the primary
*                   DNS server's IPv4 address.
*
*               primary_address:
*                   [in] Pointer to the buffer that is the storage
*                   space for the primary DNS server's IP address
*
*               secondary_set:
*                   [in] TRUE or FALSE. Enable to set the secondary
*                   DNS server's IPv4 address.
*
*               secondary_address:
*                   [in] Pointer to the buffer that is the storage
*                   space for the secondary DNS server's IPv4 address.
*
* Return:
*               If no error occurs, this function returns QL_SOC_SUCCESS (0).
*               Otherwise, a value of ql_soc_error_enum is returned.
*****************************************************************/
s32  Ql_SetDnsServerAddress(u8 contextId,
                                     bool primary_set,
                                     u8 primary_address[4],
                                     bool secondary_set,
                                     u8 secondary_address[4]);


/*****************************************************************
* Function:     Ql_SocketHtonl
*
* Description:
*               This function reverses the byte order in u32 integer.
*
* Parameters:
*               a:
*                   An u32 integer.
*
* Return:
*               An u32 integer reversed.
*****************************************************************/
u32   Ql_SocketHtonl(u32 a);


/*****************************************************************
* Function:     Ql_SocketHtons
*
* Description:
*               This function reverses the byte order in u16 integer.
*
* Parameters:
*               a:
*                   An u16 integer.
*
* Return:
*               An u16 integer reversed.
*****************************************************************/
u16   Ql_SocketHtons(u16 a);


/*****************************************************************
* Function:     Ql_SocketCheckIp
*
* Description:
*               This function checks whether an IP address is valid
*               IP address or not. If 'yes', each segment of the
*               IP address string will be converted into integer to
*               store in ipaddr parameter.
*
* Parameters:
*               addressstring:
*                   [in] IP address string.
*
*               ipaddr:
*                   [out] Pointer to u32, each byte stores the IP digit
*                   converted from the corresponding IP string.
*
* Return:
*               If no error occurs, this function returns QL_SOC_SUCCESS (0).
*               Otherwise, a value of ql_soc_error_enum is returned.
*****************************************************************/
s32   Ql_SocketCheckIp(u8 * addressstring, u32 * ipaddr);


/*****************************************************************
* Function:     Ql_SocketSetSendBufferSize
*
* Description:
*               This function sets the length of send buffer.
*
* Parameters:
*               socket:
*                   Socket Id. The send buffer length would better
*                   be less than (8 * 1360) bytes.
*
*               bufferSize:
*                   Length of send buffer, in bytes.
*
* Return:
*               If no error occurs, this function returns QL_SOC_SUCCESS (0).
*               Otherwise, a value of ql_soc_error_enum is returned.
*****************************************************************/
s32   Ql_SocketSetSendBufferSize(s8 socket, u32 bufferSize);


/*****************************************************************
* Function:     Ql_SocketSetRecvBufferSize
*
* Description:
*               This function sets the length of receive  buffer.
*
* Parameters:
*               socket:
*                   Socket Id. The receive buffer length would better
*                   be less than (8 * 1360) bytes.
*
*               bufferSize:
*                   Length of receive buffer, in bytes.
*
* Return:
*               If no error occurs, this function returns QL_SOC_SUCCESS (0).
*               Otherwise, a value of ql_soc_error_enum is returned.
*****************************************************************/
s32   Ql_SocketSetRecvBufferSize(s8 socket, u32 bufferSize);


/*****************************************************************
* Function:     Ql_SwitchHeartBeat
*
* Description:
*               This function switch on/off the keep alive function
*               of tcp/ip connection.
*
* Parameters:
*               enable:
*                   enable control, 1 switches on this function, 
*                                   0 switches off this function.
*                   if this parameter is set to 0, the time related 
*                   parameters will be ignored.
*
*               cycle_time (unit in second):
*                   the cycle time for sending heartbeat data packet,
*                   in second. If the cycle timer is timeout, the module
*                   sends a QUERY PACKET to server to keep the connection
*                   alive. 
*                   The default value is 60, and the allowed value ranges
*                   from 60 to 1800. -1 indicates the default value will 
*                   be adopted.
*
*               repeat_time (unit in second):
*                   the interval for re-sending heartbeat data packet
*                   after the failureof of sending heartbeat packet. 
*                   The default value is 10, and the allowed value ranges
*                   from 10 to 20. -1 indicates the default value will 
*                   be adopted.
*
* Return:
*               QL_SOC_SUCCESS, indicates this function successes.
*               QL_RET_ERR_INVALID_PARAMETER, indicates the wrong input parameter.
*****************************************************************/
s32   Ql_SwitchHeartBeat(u8 enable, s16 cycle_time, s16 repeat_time);


#endif // End-of '__QL_TCPIP_H__'

