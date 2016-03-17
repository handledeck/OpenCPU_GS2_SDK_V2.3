#ifdef __EXAMPLE_SYSTEM__
/***************************************************************************************************
*   Example:
*       
*           SYSTEM Routine
*
*   Description:
*
*           This example demonstrates how to use system function with APIs in OpenCPU.
*           Through MAIN Uart port, input the specified command, and the response message will be 
*           printed out through main port.
*
*   Usage:
*
*           Compile & Run:
*
*               Use "make SYSTEM" to compile, and download bin image to module to run.
*           
*           Operation: (Through MAIN port)
*
*               If input "Ql_Reset=", that will reset the module.
*               If input "Ql_Sleep=", that will suspends the execution of the current task until the time-out interval elapses.
*               If input "Ql_PowerDown=", that will switch off the power supply to the module.
*               If input "Ql_PowerOnAck", that will power on module at a proper time. 
*               If input "Ql_GetCoreVer", that will  get the version ID of the core.
*               If input "Ql_GetSDKVer", that will  get the version ID of the SDK.
*               If input "Ql_GetPowerOnReason", that will get Power on Reason.
*               If input "Ql_StartWatchdog=", that will start watch-dog service.
*               If input "Ql_FeedWatchdog", that will reset watch-dog.
*               If input "Ql_StopWatchdog", that will stop watch-dog service.
*
*               command: Ql_SecureData_Store=<n> , n=1~13.
*               command: Ql_SecureData_Read=<n> , n=1~13
****************************************************************************************************/

#include "ql_type.h"
#include "ql_appinit.h"
#include "ql_trace.h"
#include "ql_fcm.h"
#include "ql_stdlib.h"
#include "ql_timer.h"
#include "ql_interface.h"


char DebugBuf[1024];

#ifdef OUT_DEBUG(x,...)
#undef OUT_DEBUG(x,...)
#endif

#define OUT_DEBUG(...)  \
    Ql_memset(DebugBuf, 0, sizeof(DebugBuf));  \
    Ql_sprintf(DebugBuf,__VA_ARGS__);   \
    Ql_SendToUart(ql_uart_port1,(u8*)(DebugBuf),Ql_strlen(DebugBuf));

typedef struct {
    u32     baudrate; 
    u32     dataBits;
    u32     stopBits;
    u32     parity;
    u32     flowCtrl;
}ST_UARTDCB;
static  void SecureData_Store_callback(s8 result, u8* pData, u32 len);
static  void SecureData_Read_callback(s8 result, u8* pData, u32 len);


extern QlCustomerConfig Customer_user_qlconfig;

char textBuf[512];

QlEventBuffer QlEvent;

void ql_entry()
{
	char *pData = NULL;
	char *p = NULL;
	char *q = NULL;

    int iRet = 0;
    int iTmp = 0;
    u8  u8Tmp = 0;

    QlSysTimer SysTime;
    QlTimer MyTimer;
    
	Ql_SetDebugMode(BASIC_MODE);
	OUT_DEBUG("\r\nAPI Test: System API\r\n");
    Ql_SetPortOwner(ql_md_port1, ql_main_task);
	Ql_OpenModemPort(ql_md_port1);

	while(1)
	{	
		Ql_GetEvent(&QlEvent);
		switch(QlEvent.eventType)
		{
			case EVENT_UARTDATA:
			{
				if(QlEvent.eventData.uartdata_evt.len > 0)
				{
                    s32 ret;
                    char* p1 = NULL;
                    char* p2 = NULL;
					pData = (char *)QlEvent.eventData.uartdata_evt.data;
    
                    //command: Ql_SecureData_Store=<n> , n=1~13
                    p= Ql_strstr(pData,"Ql_SecureData_Store=");
                    if(p)
                    {
                        char arrIdx[3];
                        u8 index = 1;
                        ST_UARTDCB testparam;
                        testparam.baudrate = 9600;
                        testparam.dataBits = 7;
                        testparam.flowCtrl = 1;
                        testparam.parity = 2;
                        testparam.stopBits = 1;
                        p1 = Ql_strstr(pData, "<");
                        p2 = Ql_strstr(pData, ">");
                        Ql_memset(arrIdx, 0x0, sizeof(arrIdx));
                        Ql_memcpy(arrIdx, p1 + 1, p2 - p1 - 1);
                        index = (u8)Ql_atoi(arrIdx);
                        ret = Ql_SecureData_Store(index, (u8* )&testparam, sizeof(ST_UARTDCB), SecureData_Store_callback);
                        OUT_DEBUG("<-- Ql_SecureData_Store(index=%d)=%d -->\r\index", index, ret);   
                        break;
                    }

                    //command: Ql_SecureData_Read=<n> , n=1~13
                    p= Ql_strstr(pData,"Ql_SecureData_Read=");
                    if(p)
                    {
                        char arrIdx[3];
                        u8 index = 1;
                        p1 = Ql_strstr(pData, "<");
                        p2 = Ql_strstr(pData, ">");
                        Ql_memset(arrIdx, 0x0, sizeof(arrIdx));
                        Ql_memcpy(arrIdx, p1 + 1, p2 - p1 - 1);
                        index = (u8)Ql_atoi(arrIdx);
                	    ret = Ql_SecureData_Read(index, sizeof(ST_UARTDCB), SecureData_Read_callback);
                        OUT_DEBUG("<-- Ql_Userdata_Read(index=%d)=%d -->\r\n", index, ret);   
                        break;
                    }

					if(NULL != (p = Ql_strstr(pData, "Ql_Reset")))//
					{                                               //0:WDT_RESET 1:DRV_ABN_RESET 2:DRV_RESET 3:ASSERT(0)
                                                                    // Ql_Reset=1 
                        p = Ql_strstr(p, "=");
                        p++;
                        if(((int)*p<48)||((int)*p>52))
                        {
                            OUT_DEBUG("\r\nQl_Reset(%s) -> parameter ERROR!\r\n",p);
                            break; // if it is not ¡®0¡¯'1','2','3' break 
                        }
                        iTmp = Ql_atoi(p);
                        if((0<=iTmp)&&(iTmp<=3))//(iTmp > 3 || iTmp < 0)
                        {
                            OUT_DEBUG("\r\nQl_Reset(%d) is excuting...\r\n", iTmp);
                            
                        }
                        else
                        {
                            OUT_DEBUG("\r\nQl_Reset(%d) -> parameter ERROR!\r\n", iTmp);
                            break;
                        }
                        Ql_Reset(iTmp);
					}
					else if(NULL != (p = Ql_strstr(pData, "Ql_Sleep")))//
					{                                             //Ql_Sleep=1000, unit:millisecond
                        p = Ql_strstr(p, "=");
                        p++;
                        iTmp = Ql_atoi(p);
                        Ql_GetLocalTime(&SysTime);
                        OUT_DEBUG("\r\nQl_Sleep(%d) Begin Time:%d:%d:%d\r\n", iTmp, SysTime.hour, SysTime.month, SysTime.second);
                        Ql_Sleep(iTmp);
                        Ql_GetLocalTime(&SysTime);
                        OUT_DEBUG("\r\nQl_Sleep(%d) End Time:%d:%d:%d\r\n", iTmp, SysTime.hour, SysTime.month, SysTime.second);
					}
                    else if(NULL != (p = Ql_strstr(pData, "Ql_PowerDown")))//
                    {                                             //Ql_PowerDown=0,1; 0:Urgently power off 1:Normal power off
                        p = Ql_strstr(p, "=");
                        p++;
                        iTmp = Ql_atoi(p);
                        OUT_DEBUG("\r\nQl_PowerDown(%d) is excuting...\r\n", iTmp);
                        Ql_PowerDown(iTmp);
                    }
                    else if(NULL != (p = Ql_strstr(pData, "Ql_PowerOnAck")))
                    {
                        Customer_user_qlconfig.powerautoon = FALSE;
                        Ql_PowerOnAck();
                        OUT_DEBUG("\r\nQl_PowerOnAck OK!\r\n");
                    }
                    else if(NULL != (p = Ql_strstr(pData, "Ql_GetCoreVer")))//
                    {
                        Ql_memset(textBuf, 0, sizeof(textBuf));
                        iRet = Ql_GetCoreVer((u8 *)textBuf, (u16)sizeof(textBuf)-1);
                        if(iRet < 0)
                        {
                            OUT_DEBUG("\r\nFailed : Ql_GetCoreVer");
                        }
                        else 
                        {
                            OUT_DEBUG("\r\nSUCCESSFU : Ql_GetCoreVer");
                            OUT_DEBUG("\r\nCORE VER:%s, Length=%d\r\n", textBuf, iRet);
                        }
                    }
                    else if(NULL != (p = Ql_strstr(pData, "Ql_GetSDKVer")))//
                    {
                        Ql_memset(textBuf, 0, sizeof(textBuf));
                        iRet = Ql_GetSDKVer((u8 *)textBuf, (u16)sizeof(textBuf)-1);
                        if(iRet < 0)
                        {
                            OUT_DEBUG("\r\nFailed : Ql_GetSDKVer");
                        }
                        else 
                        {
                            OUT_DEBUG("\r\nSUCCESSFU : Ql_GetSDKVer");
                            OUT_DEBUG("\r\nSDK VER:%s, Length=%d\r\n", textBuf, iRet);
                        }
                    }
                    else if(NULL != (p = Ql_strstr(pData, "Ql_GetPowerOnReason")))//
                    {
                        Ql_GetPowerOnReason(&u8Tmp);
                        OUT_DEBUG("\r\nQl_GetPowerOnReason : u8Tmp = %d\r\n", u8Tmp);
                    }
                    else if(NULL != (p = Ql_strstr(pData, "Ql_StartWatchdog")))//
                    {                                       //Ql_StartWatchdog=A,B,C   C:0->WDT_RESET, 1->PowerDown, 2->Assert
                        u16 u16Tick10ms = 0;
                        u32 u32OverFeedCount = 0;
                        u16 u16ResetType = 0;
                        bool bRet = FALSE;

                        p = Ql_strstr(p, "=");
                        p++;
                        q = Ql_strstr(p, ",");
                        (*q) = 0;
                        u16Tick10ms = Ql_atoi(p);

                        q++;
                        p = Ql_strstr(q, ",");
                        (*p) = 0;
                        u32OverFeedCount = Ql_atoi(q);

                        p++;
                        u16ResetType = Ql_atoi(p);

                        MyTimer.timeoutPeriod = Ql_MillisecondToTicks(10*u16Tick10ms*u32OverFeedCount-1000);
                        bRet = Ql_StartWatchdog(u16Tick10ms, u32OverFeedCount, u16ResetType);
                        if(TRUE == bRet)
                        {
                            OUT_DEBUG("\r\nSUCCESS: Ql_StartWatchDog(%d, %d, %d) , bRet = %d\r\n", u16Tick10ms, u32OverFeedCount, u16ResetType, bRet);
                        }
                        else
                        {
                            OUT_DEBUG("\r\nFAILLED: Ql_StartWatchDog(%d, %d, %d) , bRet = %d\r\n", u16Tick10ms, u32OverFeedCount, u16ResetType, bRet);
                        }

                    }
                    else if(NULL != (p = Ql_strstr(pData, "Ql_FeedWatchdog")))//
                    {
                        Ql_FeedWatchdog();
                        if(MyTimer.timeoutPeriod <= 0)
                        {
                            OUT_DEBUG("\r\nAuto Feed WatchDog Failed! Please Feed WatchDog MANUALLY\r\n");
                        }
                        else 
                        {
                            OUT_DEBUG("\r\nQl_FeedWatchdog OK!\r\n");
                            Ql_StartTimer(&MyTimer);
                        }
                        
                    }
                    else if(NULL != (p = Ql_strstr(pData, "Ql_StopWatchdog")))//
                    {
                        Ql_StopWatchdog();
                        if(MyTimer.timeoutPeriod >= 0)
                        {
                            MyTimer.timeoutPeriod = 0;
                            Ql_StopTimer(&MyTimer);
                        }
                        OUT_DEBUG("\r\Ql_StopWatchdog OK!\r\n");
                    }
					else
					{
						Ql_SendToModem(ql_md_port1, (u8 *)QlEvent.eventData.uartdata_evt.data, QlEvent.eventData.uartdata_evt.len);
					}
				}
				break;
			}
            case EVENT_TIMER:
            {
                if(QlEvent.eventData.timer_evt.timer_id == MyTimer.timerId)
                {
                    Ql_GetLocalTime(&SysTime);
                    Ql_FeedWatchdog();
                    OUT_DEBUG("\r\nFeed Watch Dog OK!(%d:%d:%d)\r\n", SysTime.hour, SysTime.minute, SysTime.second);
                    Ql_StartTimer(&MyTimer);
                }
                break;
            }
			case EVENT_MODEMDATA:
			{
				Ql_SendToUart(ql_uart_port1, (u8 *)QlEvent.eventData.modemdata_evt.data, QlEvent.eventData.modemdata_evt.len);
				break;
			}
			default :
				break;
		}
	}
}

static  void SecureData_Store_callback(s8 result, u8* pData, u32 len)
{
    if(1 == result)// 1 means backup successfully
    {
        OUT_DEBUG("<--callback:Sysparam backUp successfully !!!-->\r\n");
    }
    else  // -1 means backup failed.
    {
        OUT_DEBUG("<--callback:User Data backUP failed!!!-->\r\n");
    }
}

static  void SecureData_Read_callback(s8 result, u8* pData, u32 len)
{
    ST_UARTDCB* pBackupData = (ST_UARTDCB* )pData;
    
    if(1 == result)// 1 means backup successfully 
    {
      
        OUT_DEBUG("<--callback: Sysparam(len=%d) Read out successfully! BaudRate=%d,DataBit=%d,FlowCtrl=%d,Parity=%d,StopBit=%d-->\r\n", \
            len,pBackupData->baudrate,pBackupData->dataBits,pBackupData->flowCtrl,pBackupData->parity,pBackupData->stopBits);
    }    
    else  // -1 means backup failed.
    {
        OUT_DEBUG("<--callback: User Data Read out failed!!!-->\r\n");
    }
}

#endif

