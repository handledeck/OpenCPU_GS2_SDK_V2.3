
#include "socket.h"
#include "ql_fota.h"

QlClient __client[5];


s32 func_send_handle(s8 sock, char *PDtata)
{
	s32 ret;
	s32 i = 0; 
	s32 index;

	index = findClientBySockid(sock);

	__client[index].sendRemain_len = Ql_strlen(PDtata);
    //OUTD("send-rem:%d pos:%d",__client[index].sendRemain_len,__client[index].pSendCurrentPos);
    __client[index].pSendCurrentPos = PDtata;
       do
	{
             Ql_memset(__client[index].sendBuffer, 0, DATA_LEN);
             
             if (__client[index].sendRemain_len > DATA_LEN)
             {
                Ql_strncpy(__client[index].sendBuffer,__client[index].pSendCurrentPos,DATA_LEN);
             }else
             {
                Ql_strncpy(__client[index].sendBuffer,__client[index].pSendCurrentPos,__client[index].sendRemain_len);
             }
		ret = Ql_SocketSend(__client[index].socketId, (u8*)__client[index].sendBuffer,Ql_strlen((u8*)__client[index].sendBuffer));
		if(ret == __client[index].sendRemain_len)
		{
			//send complete
			__client[index].sendRemain_len = 0;
			Ql_memset(__client[index].sendBuffer, '\0', DATA_LEN);
			__client[index].pSendCurrentPos = NULL;
			break;
		}
		else if((ret < 0) && (ret == QL_SOC_WOULDBLOCK))
		{
			//you must wait CallBack_socket_write, then send data;     
			//nSTATE = STATE_ING;
            break;
		}
		else if(ret < 0) //error
		{
			Ql_SocketClose(__client[index].socketId);
			__client[index].socketId = 0x7F;
			__client[index].userId = 0x7F;

			__client[index].sendRemain_len = 0;
			Ql_memset(__client[index].sendBuffer, '\0', DATA_LEN);
			__client[index].pSendCurrentPos = NULL;
			__client[index].send_handle_callback = NULL;
			__client[index].send_handle = NULL;

			if(ret == QL_SOC_BEARER_FAIL)
			{
				for(i = 0; i < MAXCLIENT_NUM; i++)
				{
					if(__client[i].socketId != 0x7F)
					{
						break;
					}
				}
			}
			break;
		}
		else if(ret < __client[index].sendRemain_len)
		{
            __client[index].sendRemain_len -= ret;
			(char *)__client[index].pSendCurrentPos += ret;
		
		}
	}while(1);

    return ret;
}



/*s32 func_read_handle_callback(u8 contexid, s8 sock, bool result, s32 error, s32 index){
    s32 i;
	s32 ret;
	//OUTD("read handle callback contexid=%d,sock=%d,result=%d,error=%d, index=%d", contexid, sock, result, error, index);

	if(!result)
	{
		///OUTD("func_read_handle_callback() failed\r\n",NULL);
		Ql_SocketClose(__client[index].socketId);

		client_uninit(index);

		if((error == QL_SOC_BEARER_FAIL) || (error > 0))
		{
			//OUTD("func_read_handle_callback error  QL_SOC_BEARER_FAIL\r\n",NULL);
			for(i = 0; i < MAXCLIENT_NUM; i++)
			{
				if(__client[i].socketId != 0x7F)
				{
					break;
				}
			}
		}
		return result;
	}

	do
	{
             Ql_memset(__client[index].recvBuffer, 0, DATA_LEN);
             ret = Ql_SocketRecv(__client[index].socketId, (u8 *)__client[index].recvBuffer, DATA_LEN);

        if(ret == QL_SOC_WOULDBLOCK)
		{
            //OUTD("you shoud wait next CallBack_socket_read QL_SOC_WOULDBLOCK",NULL)
			//you shoud wait next CallBack_socket_read   
			break;
		}
		else if((ret < 0) && (ret != QL_SOC_WOULDBLOCK))
		{
			//OUTD("Ql_SocketRecv(sock=%d)=%d error exit\r\n", __client[i].socketId, ret);
			Ql_SocketClose(__client[index].socketId);
			client_uninit(index);
			if(ret == QL_SOC_BEARER_FAIL)
			{
				for(i = 0; i < MAXCLIENT_NUM; i++)
				{
					if(__client[i].socketId != 0x7F)
					{
						break;
					}
				}
			}
			break;
		}
		else if(ret < DATA_LEN ||ret == DATA_LEN)
		{
            //OUTD("Recv from: sock(%d)len(%d) :%s", __client[index].socketId, ret,__client[index].recvBuffer);
            break;
            //read_request(__client[index].recvBuffer, sock); 
		}
	}while(1);

    return ret;

}*/

s32 func_read_handle_callback(u8 contexid, s8 sock, bool result, s32 error, s32 index){
    s32 i;
	s32 ret;
    unsigned char buf[1024];
	//OUTD("read handle callback contexid=%d,sock=%d,result=%d,error=%d, index=%d", contexid, sock, result, error, index);
	if(result)
	{
        Ql_memset(__client[index].recvBuffer, 0, DATA_LEN);
        ret = Ql_SocketRecv(__client[index].socketId, /*(u8 *)__client[index].recvBuffer*/buf, 1024);
        if(ret >0)
		{
            //u32 pos=__client[index].pRecvCurrentPos;
           
            if (__client[index].protocol==NONE) {
                if((Ql_strstr((char*)buf, "GET") || Ql_strstr((char*)buf, "POST")))
                    __client[index].protocol = HTTP;
                else if (Ql_strstr((char*)buf, "QUECTEL_FOTA_UPGRADE")) {
                     __client[index].protocol =UPGRADE;     
                     OUTD("socket upgarede",NULL);
                     FEED_DOG Q_t_Watch_dog;
                     Ql_memset((void *)(&Q_t_Watch_dog), 0, sizeof(FEED_DOG)); //Do not enable  watch_dog
                      Q_t_Watch_dog.Q_gpio_pin1 = Ql_GetGpioByName(QL_PINNAME_DTR);
                      Q_t_Watch_dog.Q_feed_interval1 = 100;
                      Q_t_Watch_dog.Q_gpio_pin2 = Ql_GetGpioByName(QL_PINNAME_NETLIGHT);
                      Q_t_Watch_dog.Q_feed_interval2 = 500;
                      s32 init=Ql_Fota_App_Init(&Q_t_Watch_dog);
                }
             else 
                __client[index].protocol = STREAM;
            }
            Ql_memcpy(&__client[index].recvBuffer[0], buf, ret);
            __client[index].pRecvCurrentPos=ret;
            //__client[index].pRecvCurrentPos += ret;
            //OUTD("Recv from: sock(%d)len(%d) :%s", __client[index].socketId, ret,__client[index].recvBuffer);
            //read_request(__client[index].recvBuffer, sock); 
             //OUTD("buf pos:%d",pos);
		}
     }
    return ret;

}

s32 func_send_handle_callback(u8 contexid, s8 sock, bool result, s32 error, s32 index){
   s32 i;
	s32 ret;

    __client[index].sendRemain_len = Ql_strlen((u8*)__client[index].sendBuffer);
	__client[index].pSendCurrentPos = __client[index].sendBuffer;
    
	//OUTD("func_send_handle_callback(contexid=%d,sock=%d,result=%d,error=%d, index=%d)\r\n", contexid, sock, result, error, index);
       
	while(result)
	{
		ret = Ql_SocketSend(__client[index].socketId,__client[index].pSendCurrentPos,__client[index].sendRemain_len);
		//OUTD("Ql_SocketSend(socket=%d, sendRemain_len=%d)=%d\r\n", __client[index].socketId,__client[index].sendRemain_len, ret);

		if(ret == __client[index].sendRemain_len)
		{
			//send complete
            //OUTD("send complete",NULL);
			__client[index].sendRemain_len = 0;
			Ql_memset(__client[index].sendBuffer, '\0', DATA_LEN);
			__client[index].pSendCurrentPos = NULL;
			break;
		}
		else if((ret < 0) && (ret == QL_SOC_WOULDBLOCK))
		{
			//OUTD("you must wait CallBack_socket_write, then send data",NULL);     
			//nSTATE = STATE_ING;
                   break;
            
		}
		else if(ret < 0)
		{
			OUTD("error",NULL);
			Ql_SocketClose(__client[index].socketId);
			__client[index].socketId = 0x7F;
			__client[index].userId = 0x7F;

			__client[index].sendRemain_len = 0;
			Ql_memset(__client[index].sendBuffer, '\0', DATA_LEN);
			__client[index].pSendCurrentPos = NULL;
			__client[index].send_handle_callback = NULL;
			//client[index].send_handle = NULL;

			if(ret == QL_SOC_BEARER_FAIL)
			{
				for(i = 0; i < MAXCLIENT_NUM; i++)
				{
					if(__client[i].socketId != 0x7F)
					{
						break;
					}
				}
			}
			break;
		}
		else if(ret < __client[index].sendRemain_len) 
		{
             __client[index].sendRemain_len -= ret;
			(char *)__client[index].pSendCurrentPos += ret;
			//OUTD("continue send, do not send all data",NULL);
		}						
	}
	if(!result)
	{
		//OUTD("CallBack_socket_write() failed\r\n",NULL);
		Ql_SocketClose(__client[index].socketId);
		__client[index].socketId = 0x7F;
		__client[index].userId = 0x7F;

		__client[index].sendRemain_len = 0;
		Ql_memset(__client[index].sendBuffer, '\0', DATA_LEN);
		__client[index].pSendCurrentPos = NULL;
		__client[index].send_handle_callback = NULL;

		if((error == QL_SOC_BEARER_FAIL) || (error > 0))
		{
			for(i = 0; i < MAXCLIENT_NUM; i++)
			{
				if(__client[i].socketId != 0x7F)
				{
					break;
				}
			}
		}
	}
}

s32 findClientBySockid(s8 sock)
{
    s32 i;
    s32 index = -1;
    for( i = 0; i < MAXCLIENT_NUM; i++)
    {
        if(sock == __client[i].socketId)
        {
            index = i;
            break;
        }
    }

    return index;	
}

void client_init(s32 index,s8 accept_socket)
{
    __client[index].socketId = accept_socket;
    __client[index].userId = accept_socket;

    __client[index].read_handle_callback = func_read_handle_callback;
 
    Ql_memset(__client[index].recvBuffer, '\0', DATA_LEN); 
    __client[index].recvRemain_len = DATA_LEN; 
    __client[index].pRecvCurrentPos = 0;//__client[index].recvBuffer;
}

void client_uninit(s32 index)
{
    __client[index].socketId = 0x7F;
    __client[index].userId = 0x7F;
	
    __client[index].read_handle_callback = NULL; 
    Ql_memset(__client[index].recvBuffer, '\0', DATA_LEN);
    __client[index].recvRemain_len = 0;
    __client[index].pRecvCurrentPos = 0;//NULL; 
}

void client_socketId_init()
{
    s32 i;
    for(i = 0; i < MAXCLIENT_NUM; i++)
    {
        __client[i].socketId = 0x7F;
        __client[i].userId = 0x7F;
    }
}

void send_all_stream(u8* data,u16 len){
    s32 i;
    for(i = 0; i < MAXCLIENT_NUM; i++)
    {
        if (__client[i].socketId != 0x7F && __client[i].protocol==STREAM) {
            //__client[i].send_handle(__client[i].socketId,data);
            Ql_SocketSend(__client[i].socketId,data,len);
           
        }
    }
}

