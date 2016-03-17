#ifdef __EXAMPLE_FOTA_HTTP__
/***************************************************************************************************
*   Example:
*       
*           	FOTA_HTTP upgrade function
*
*   Description:
*
*           This example demonstrates the process of fota_http upgrading in OpenCPU.
*           Application bin must be put in servers.It will be used to upgrade data through the air.
*
*   Usage:
*
*           Compile & Run:
*
*               Use "make fota_http" to compile, and download bin image to module.
*           
*           Operation: (Through MAIN port)
*           step 1: you must put your application bin in your server.
*           step 2: replace the "APP_BIN_URL" with your own .
*           step 3: input string : start=[XXXX], XXXX stands for URL.
*
****************************************************************************************************/
#include "ql_appinit.h"
#include "ql_interface.h"
#include "ql_type.h"
#include "ql_trace.h"
#include "ql_audio.h"
#include "ql_timer.h"
#include "ql_stdlib.h"
#include "ql_error.h"
#include "ql_fcm.h"
#include "ql_filesystem.h"
#include "ql_fota.h"


char textBuf[1024];

#ifdef OUT_DEBUG(x,...)
#undef OUT_DEBUG(x,...)
#endif

#define OUT_DEBUG(...)  \
    Ql_memset(textBuf, 0, sizeof(textBuf));  \
    Ql_sprintf(textBuf,__VA_ARGS__);   \
    Ql_SendToUart(ql_uart_port1,(u8*)(textBuf),Ql_strlen(textBuf));

#define START_FLAG_STRING "\r\n@=-=-=-=-=-=-= HTTP2FOTA_APP_XXX =-=-=-=-=-=-=\r\n\r\n\r\n"
#define APP_BIN_URL   "http://124.74.41.170/ftpsvr/max/"

#define MAX_BUF_SIZE 1024
#define MAX_URL_LENGTH 1024
#define MAX_NAME_LENGTH 120

typedef enum
{
    PROCESS_STATE_TYPE_NONE,
    PROCESS_STATE_TYPE_ATE0,
    PROCESS_STATE_TYPE_CHECK_CGATT,
    PROCESS_STATE_TYPE_QIFGCNT,
    PROCESS_STATE_TYPE_QICSGP,
//    PROCESS_STATE_TYPE_QIREGAPP,
//    PROCESS_STATE_TYPE_QIACT,
    PROCESS_STATE_TYPE_QHTTPURL,
    PROCESS_STATE_TYPE_INPUTURL,
    PROCESS_STATE_TYPE_QHTTGET,
    PROCESS_STATE_TYPE_QHTTPREAD,
    PROCESS_STATE_TYPE_QIDEACT,
    PROCESS_STATE_TYPE_UPDATEFLAGS
}PROCESS_STATE_TYPE;


typedef struct _TProcessState
{
    PROCESS_STATE_TYPE iState;
    u64 uiStateBeginTime;
}TProcessState;

typedef struct _TH2FData
{
    TProcessState ProcessState;    
    QlTimer Timer;
    FEED_DOG    Q_t_Watch_dog;
    unsigned int uiTimeOut;
    char Buffer[MAX_BUF_SIZE];
    bool bBeginFlag;
}TH2FData;


static TH2FData s_H2FData;
static bool s_bDataMode = FALSE;
static bool s_bDEACTReboot;
static char s_szURLBuf[MAX_URL_LENGTH];
static char APP_NAME[MAX_NAME_LENGTH];

void H2F_SendCMDCtrl();

/*****************************************************************
 *
 *Check if command timeout.if time out,it means download failed!
 *the program will restart module to upgrade again
 *
*******************************************************************/
bool H2F_CheckTimeOut()
{
   if(Ql_GetRelativeTime() - s_H2FData.ProcessState.uiStateBeginTime > s_H2FData.uiTimeOut)
   {
        return TRUE;
   }
   else
   {
        return FALSE;
   }
}


/********************************************************************
 *
 *Init s_H2FData.s_H2FData is a very important parameter 
 *
*********************************************************************/

void H2F_Init()
{
    s_H2FData.bBeginFlag = FALSE;
    

    /*---------------------------------------------------*/
    Ql_memset((void *)(&s_H2FData.Q_t_Watch_dog), 0, sizeof(FEED_DOG)); //Do not enable  watch_dog
    s_H2FData.Q_t_Watch_dog.Q_gpio_pin1 = Ql_GetGpioByName(QL_PINNAME_DTR);
    s_H2FData.Q_t_Watch_dog.Q_feed_interval1 = 100;
    s_H2FData.Q_t_Watch_dog.Q_gpio_pin2 = Ql_GetGpioByName(QL_PINNAME_NETLIGHT);
    s_H2FData.Q_t_Watch_dog.Q_feed_interval2 = 500;
    
    /*---------------------------------------------------*/
    
    s_H2FData.ProcessState.iState = PROCESS_STATE_TYPE_NONE;
    s_H2FData.ProcessState.uiStateBeginTime = Ql_GetRelativeTime();

    s_H2FData.uiTimeOut = 125*1000;  

    Ql_memset(s_H2FData.Buffer, 0, sizeof(s_H2FData.Buffer));
    
    s_H2FData.Timer.timeoutPeriod = Ql_MillisecondToTicks(500);
    Ql_StartTimer(&s_H2FData.Timer);

    Ql_memset(s_szURLBuf, 0, sizeof(s_szURLBuf));
    Ql_strcpy(s_szURLBuf, APP_BIN_URL);
    s_bDataMode = FALSE;
    s_bDEACTReboot = FALSE;

}


/********************************************************************
 *
 *When download data and write to fota cache, you must call this 
 *functin for initialization.
 *
*********************************************************************/

void H2F_WriteData_Init()
{
    int iRet = -1;

    iRet = Ql_Fota_App_Init(&s_H2FData.Q_t_Watch_dog);
    if(QL_RET_OK != iRet)
    {
        OUT_DEBUG("\r\n[max] : Ql_Fota_App_Init FAILED!\r\n");
        OUT_DEBUG("\r\n[max] : Reboot 3 seconds later ...\r\n");
        Ql_Sleep(3000);
        Ql_Reset(0);
    }
    else
    {
        OUT_DEBUG("\r\n[max] : Ql_Fota_App_Init OK!\r\n");
    }
}

/********************************************************************
 *
 *Write data to Fota cache
 *
*********************************************************************/

void  H2F_WriteData_Write(s32 iLen, s8 *buffer)
{
    int iRet = -1;
    int i = 0;
    static int s_iSizeRem = 0;

    for(i=0; i<iLen; i++)
    {
        if(0x00 == buffer[i])
        {
            continue;
        }
        else
        {
            break;
        }
    }

    if(i == iLen)
    {
        OUT_DEBUG("[max] : ALL Data is ZERO!!!!\r\n");
    }
    
    iRet = Ql_Fota_App_Write_Data(iLen, buffer);
    if(QL_RET_OK != iRet)
    {
        OUT_DEBUG("\r\n[max] : Ql_Fota_App_Write_Data FAILED!\r\n");
    }
    else
    {
        s_iSizeRem += iLen;
        OUT_DEBUG("[max] : Ql_Fota_App_Write_Data -> %-5d bytes OK!(TotalWrite=%-5d)\r\n", iLen, s_iSizeRem);
    }

}

/**********************************************************************
 *
 *Finish writing data to Fota Cache.
 *
************************************************************************/
void  H2F_WriteData_Finish()
{
    int iRet = -1;

    OUT_DEBUG("\r\n[max] : H2F_WriteData_Finish is finishing...\r\n ");
    iRet = Ql_Fota_App_Finish();
    if(QL_RET_OK != iRet)
    {
        OUT_DEBUG("\r\n[max] : Ql_Fota_App_Finish FAILED!\r\n");
        OUT_DEBUG("\r\n[max] : Reboot 3 seconds later ...\r\n");
        Ql_Sleep(3000);
        Ql_Reset(0);
    }
    else
    {
        s_H2FData.ProcessState.iState++;
//        s_H2FData.ProcessState.iState += 2;
        H2F_SendCMDCtrl();
        OUT_DEBUG("\r\n[max] : Ql_Fota_App_Finish OK!\r\n");
    }
}

/*****************************************************************
 *
 *When get successful result,just like "OK\r\n", "CONNECT\r\n",
 *and so on,The H2F_StateMachine will jump to next state Automatically
 *
*******************************************************************/
void H2F_StateMachine(char *pTriggerStr)
{
    int iRet = -1;
    bool bFlags = TRUE;
    if((NULL != Ql_strstr(pTriggerStr, "OK")) &&  
       PROCESS_STATE_TYPE_ATE0 == s_H2FData.ProcessState.iState)
    {
        OUT_DEBUG("\r\n[max] : ATE0 OK!");
        s_H2FData.ProcessState.iState++;
        H2F_SendCMDCtrl();
    }
    else if((NULL != Ql_strstr(pTriggerStr, "+CGATT: 0")) &&  
       PROCESS_STATE_TYPE_CHECK_CGATT == s_H2FData.ProcessState.iState)
    {
        Ql_Sleep(500);
        OUT_DEBUG("\r\n[max] : CGATT = 0!");
        H2F_SendCMDCtrl();
    }
    else if((NULL != Ql_strstr(pTriggerStr, "+CGATT: 1")) &&  
       PROCESS_STATE_TYPE_CHECK_CGATT == s_H2FData.ProcessState.iState)
    {
        OUT_DEBUG("\r\n[max] : CGATT OK!");
        s_H2FData.ProcessState.iState++;
        H2F_SendCMDCtrl();
    }
    else if((NULL != Ql_strstr(pTriggerStr, "OK")) &&  
       PROCESS_STATE_TYPE_QIFGCNT == s_H2FData.ProcessState.iState)
    {
        OUT_DEBUG("\r\n[max] : QIFGCNTA OK!");
        s_H2FData.ProcessState.iState++;
        H2F_SendCMDCtrl();
    }
    else if((NULL != Ql_strstr(pTriggerStr, "OK")) &&  
       PROCESS_STATE_TYPE_QICSGP == s_H2FData.ProcessState.iState)
    {
        OUT_DEBUG("\r\n[max] : QICSGP OK!");
        s_H2FData.ProcessState.iState++;
        H2F_SendCMDCtrl();
    }
#if 0
    else if((NULL != Ql_strstr(pTriggerStr, "OK")) &&  
       PROCESS_STATE_TYPE_QIREGAPP == s_H2FData.ProcessState.iState)
    {
        OUT_DEBUG("\r\n[max] : QIREGAPP OK!");
        s_H2FData.ProcessState.iState++;
        H2F_SendCMDCtrl();
    }
    else if((NULL != Ql_strstr(pTriggerStr, "OK")) &&  
       PROCESS_STATE_TYPE_QIACT == s_H2FData.ProcessState.iState)
    {
        OUT_DEBUG("\r\n[max] : QIACT OK!");
        s_H2FData.ProcessState.iState++;
        H2F_SendCMDCtrl();
    }
#endif
    else if((NULL != Ql_strstr(pTriggerStr, "CONNECT")) &&  
       PROCESS_STATE_TYPE_QHTTPURL == s_H2FData.ProcessState.iState)
    {
        OUT_DEBUG("\r\n[max] : QHTTPURL CONNECT...");
        s_H2FData.ProcessState.iState++;
        H2F_SendCMDCtrl();
    }
    else if((NULL != Ql_strstr(pTriggerStr, "OK")) &&  
       PROCESS_STATE_TYPE_INPUTURL == s_H2FData.ProcessState.iState)
    {
        OUT_DEBUG("\r\n[max] : INPUTURL OK!");
        s_H2FData.ProcessState.iState++;
        H2F_SendCMDCtrl();
    }
    else if((NULL != Ql_strstr(pTriggerStr, "OK")) &&  
       PROCESS_STATE_TYPE_QHTTGET == s_H2FData.ProcessState.iState)
    {
        OUT_DEBUG("\r\n[max] : QHTTPGET OK!");
        s_H2FData.ProcessState.iState++;
        H2F_SendCMDCtrl();
    }
    else if((NULL != Ql_strstr(pTriggerStr, "CONNECT")) &&  
       PROCESS_STATE_TYPE_QHTTPREAD == s_H2FData.ProcessState.iState)
    {
        OUT_DEBUG("\r\n[max] : QHTTPREAD CONNECT...\r\n");
        H2F_WriteData_Init();
        s_bDataMode = TRUE;
    }
    else if((NULL != Ql_strstr(pTriggerStr, "OK")) &&  
       PROCESS_STATE_TYPE_QHTTPREAD == s_H2FData.ProcessState.iState)
    {
        OUT_DEBUG("\r\n[max] : QHTTPREAD OK!\r\n");
        H2F_WriteData_Finish();
    }
    else if((NULL != Ql_strstr(pTriggerStr, "OK")) &&
        PROCESS_STATE_TYPE_QIDEACT == s_H2FData.ProcessState.iState)
    {
        OUT_DEBUG("\r\n[max] : QIDEACT OK!");
        if(TRUE == s_bDEACTReboot)
        {
            OUT_DEBUG("\r\n[max] : State time out! Reboot....");
            Ql_Sleep(2000);
            Ql_Reset(0);
        }
        else
        {
            s_H2FData.ProcessState.iState++;
            H2F_SendCMDCtrl();
        }
    }
    else
    {
        bFlags = FALSE;
    }

    if(TRUE == bFlags)
    {
        s_H2FData.ProcessState.uiStateBeginTime = Ql_GetRelativeTime();
    }
    return ;
}


/*****************************************************************
 *
 * According to current state, H2F_SendCMDCtrl will send proper
 * command to core.
 *
*******************************************************************/

void H2F_SendCMDCtrl()
{
    int iRet = -1;
    
    switch(s_H2FData.ProcessState.iState)
    {
        case PROCESS_STATE_TYPE_ATE0:
        {
            Ql_memset(s_H2FData.Buffer, 0, sizeof(s_H2FData.Buffer));
            Ql_sprintf(s_H2FData.Buffer, "ATE0\n");
            break;
        }
        case PROCESS_STATE_TYPE_CHECK_CGATT:
        {
            Ql_memset(s_H2FData.Buffer, 0, sizeof(s_H2FData.Buffer));
            Ql_sprintf(s_H2FData.Buffer, "AT+CGATT?\n");
            break;
        }
        case PROCESS_STATE_TYPE_QIFGCNT:
        {
            Ql_memset(s_H2FData.Buffer, 0, sizeof(s_H2FData.Buffer));
            Ql_sprintf(s_H2FData.Buffer, "AT+QIFGCNT=1\n");
            break;
        }
        case PROCESS_STATE_TYPE_QICSGP:
        {
            Ql_memset(s_H2FData.Buffer, 0, sizeof(s_H2FData.Buffer));
            Ql_sprintf(s_H2FData.Buffer, "AT+QICSGP=1,\"CMNET\"\n");
            break;
        }
#if 0
        case PROCESS_STATE_TYPE_QIREGAPP:
        {
            Ql_memset(s_H2FData.Buffer, 0, sizeof(s_H2FData.Buffer));
            Ql_sprintf(s_H2FData.Buffer, "AT+QIREGAPP\n");
            break;
        }
        case PROCESS_STATE_TYPE_QIACT:
        {
            Ql_memset(s_H2FData.Buffer, 0, sizeof(s_H2FData.Buffer));
            Ql_sprintf(s_H2FData.Buffer, "AT+QIACT\n");
            break;
        }
#endif
        case PROCESS_STATE_TYPE_QHTTPURL:
        {
            Ql_memset(s_H2FData.Buffer, 0, sizeof(s_H2FData.Buffer));
            Ql_sprintf(s_H2FData.Buffer, "AT+QHTTPURL=%d,%d\n", Ql_strlen(s_szURLBuf), 120);
            break;
        }
        case PROCESS_STATE_TYPE_INPUTURL:
        {
            Ql_memset(s_H2FData.Buffer, 0, sizeof(s_H2FData.Buffer));
            Ql_sprintf(s_H2FData.Buffer, "%s", s_szURLBuf);
            break;
        }
        case PROCESS_STATE_TYPE_QHTTGET:
        {
            Ql_memset(s_H2FData.Buffer, 0, sizeof(s_H2FData.Buffer));
            Ql_sprintf(s_H2FData.Buffer, "AT+QHTTPGET=%d\n", 120);
            break;
        }
        case PROCESS_STATE_TYPE_QHTTPREAD:
        {
            Ql_memset(s_H2FData.Buffer, 0, sizeof(s_H2FData.Buffer));
            Ql_sprintf(s_H2FData.Buffer, "AT+QHTTPREAD=%d\n", 120);
            break;
        }
        case PROCESS_STATE_TYPE_QIDEACT:
        {
            Ql_memset(s_H2FData.Buffer, 0, sizeof(s_H2FData.Buffer));
            Ql_sprintf(s_H2FData.Buffer, "AT+QIDEACT\n");
            break;
        }
        case PROCESS_STATE_TYPE_UPDATEFLAGS:
        {
            iRet = Ql_Fota_Update();
            if(QL_RET_OK != iRet)
            {
                OUT_DEBUG("\r\n[max] : Ql_Fota_Update FAILED!\r\n");
                OUT_DEBUG("\r\n[max] : Reboot 3 seconds later ...\r\n");
                Ql_Sleep(3000);
                Ql_Reset(0);
            }
            else
            {
                //If update OK, module will reboot automaticly
            }
        }
        default:
        {
            OUT_DEBUG("\r\n[max] : ++++++++++++++++ INVALID state, Fetal ERROR!!!\r\n");
            break;
        }
    }

    iRet = Ql_SendToModem(ql_md_port1, (u8*)s_H2FData.Buffer, Ql_strlen(s_H2FData.Buffer));
    if(iRet < 0)
    {
        OUT_DEBUG("\r\n[max] : ERROR! Failed send data to modem port![state=%d]\r\n", s_H2FData.ProcessState.iState);
        OUT_DEBUG("\r\n[max] : Reboot 3 seconds later ...\r\n");
        Ql_Sleep(3000);
        Ql_Reset(0);
        return ;
    }
    else
    {
        char sz[64];
        char *p = NULL;
        int off = 0;
        Ql_memset(sz, 0, sizeof(sz));
        Ql_memcpy(sz, s_H2FData.Buffer, sizeof(sz));
        p = Ql_strstr(sz, "\r\n");
        if(NULL != p)
        {

            Ql_strcpy(p, "\\r\\n");
        }
        
        OUT_DEBUG("\r\n[max] : Send CMD ->[%s]\r\n", sz);
    }
    
    
}


QlEventBuffer    g_event;
void ql_entry()
{
    int iRet = -1;
    char *pData = NULL;
    bool keepGoing = TRUE;
    
    
    Ql_SetDebugMode(BASIC_MODE);   
    Ql_OpenModemPort(ql_md_port1);
    H2F_Init();
    
    OUT_DEBUG(START_FLAG_STRING);
    
    while(keepGoing)
    {    
        Ql_GetEvent(&g_event);
        switch(g_event.eventType)
        {
            case EVENT_TIMER:
            {
                if(g_event.eventData.timer_evt.timer_id == s_H2FData.Timer.timerId)
                {
                    if(TRUE == H2F_CheckTimeOut())
                    {
                        s_bDEACTReboot = TRUE;
                        s_H2FData.ProcessState.iState = PROCESS_STATE_TYPE_QIDEACT;
                        H2F_SendCMDCtrl();
                    }
                    else
                    {
                        Ql_StartTimer(&s_H2FData.Timer);
                    }
                }
                break;
            }
            case EVENT_MODEMDATA:
            {
                if(TRUE == s_H2FData.bBeginFlag)
                {
                    PortData_Event* pPortEvt = (PortData_Event*)&g_event.eventData.modemdata_evt;
                    //OUT_DEBUG("\r\n%s\r\n", (char*)pPortEvt->data);
                    if (DATA_AT == pPortEvt->type)
                    {
                        H2F_StateMachine((char*)pPortEvt->data);
                    }
                    else if (DATA_TCP_T == pPortEvt->type)
                    {
                        if(TRUE == s_bDataMode)
                        {
                            H2F_WriteData_Write(pPortEvt->len, pPortEvt->data);
                        }
                        else
                        {
                            OUT_DEBUG("[max] : Ileagle data!!!!!\r\n");
                        }
                    }
                }
                break;
            }
                            // CMD Mode : start=[xxx], xxx stands for app_bin's URL. if xxx == NULL, the default URL will be connect.
            case EVENT_UARTDATA:
            {
                char *p = NULL;
                char *q = NULL;
                int  iLen = 0;
                PortData_Event* pDataEvt = (PortData_Event*)&g_event.eventData.uartdata_evt;

                if(ql_uart_port1 == pDataEvt->port)
                {
                    p = Ql_strstr(pDataEvt->data, "start=[");
                    q = Ql_strstr(pDataEvt->data, "]");

                    if((NULL != p) && (NULL != q)) 
                    {
                        p = Ql_strstr(p, "[");
                        p++;
                        q--;
                        
                        iLen = q-p+1;

                        if(iLen > 0)
                        {
                            Ql_memset(APP_NAME, 0, sizeof(APP_NAME));
                            Ql_memcpy(APP_NAME, p, iLen);
                        }
                        Ql_strncat(s_szURLBuf, APP_NAME,sizeof(APP_NAME));

                        OUT_DEBUG("[max] : Test begin! 3 seconds later...\r\n");
                        Ql_Sleep(3000);
                        s_H2FData.bBeginFlag = TRUE;
                        s_H2FData.ProcessState.iState = PROCESS_STATE_TYPE_ATE0;
                        H2F_SendCMDCtrl();
                    }
                    else
                    {
                        OUT_DEBUG("[max] : SYNTAX ERROR!\r\n");
                    }
                }
                
                break;            
            }
            
            case EVENT_SERIALSTATUS:
            {
                PortStatus_Event* pPortStatus = (PortStatus_Event*)&g_event.eventData.portstatus_evt;
                bool val = pPortStatus->val;
                u8 port = pPortStatus->port;
                u8 type = pPortStatus->type;

                OUT_DEBUG("EVENT_SERIALSTATUS port=%d type=%d val=%d\r\n",port,type,val);
                break;
            }
            
            default:
                break;
        }
    }
}


#endif // __EXAMPLE_FOTA_HTTP__

