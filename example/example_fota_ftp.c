#ifdef __EXAMPLE_FOTA_FTP__
/***************************************************************************************************
*   Example:
*       
*           	FOTA_FTP upgrade function
*
*   Description:
*
*           This example demonstrates the process of fota_ftp upgrading in OpenCPU.
*           Application bin must be put in ftp servers.It will be used to upgrade data through the air.
*
*   Usage:
*
*           Compile & Run:
*
*               Use "make fota_ftp" to compile, and download bin image to module.
*           
*           Operation: (Through MAIN port)
*           step 1: you must put your application bin in your server.
*           step 2: replace the "ftp service" with your own .
*           step 3: input string : start fota=XXXX, XXXX stands for file name
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

typedef enum tagATCmdType {
    AT_General,
    AT_QISTAT,
    AT_QFTPOPEN,
    AT_QFTPCFG,
    AT_QFTPPATH,
    AT_QFTPGET,
    AT_QFTPCLOSE,
    AT_QIDEACT
}ATCmdType;

FEED_DOG    Q_t_Watch_dog;
void DoFOTAUpgrade();

#define FTP_SVR_ADDR   "124.74.41.170"
#define FTP_SVR_PORT   "21"
#define FTP_SVR_PATH   "/stanley/"
#define FTP_USER_NAME  "max.tang"
#define FTP_PASSWORD   "quectel!~@"

ascii appBin_fName_svr[100] = { 0x0 };

QlTimer  timer;
char * pData=NULL;
char buffer[100];
#define UA_DATA_BUfFER_LEN 128
u8 dataBuf[UA_DATA_BUfFER_LEN];

u8   g_cmd_type;
u16  g_cmd_idx;
void SendAtCmd();
bool  bDoNexAT = TRUE;

void FTP2FOTA_Init()
{  
    /*---------------------------------------------------*/
    Ql_memset((void *)(&Q_t_Watch_dog), 0, sizeof(FEED_DOG)); //Do not enable  watch_dog
    Q_t_Watch_dog.Q_gpio_pin1 = Ql_GetGpioByName(QL_PINNAME_DTR);
    Q_t_Watch_dog.Q_feed_interval1 = 100;
    Q_t_Watch_dog.Q_gpio_pin2 = Ql_GetGpioByName(QL_PINNAME_NETLIGHT);
    Q_t_Watch_dog.Q_feed_interval2 = 500;
    
    /*---------------------------------------------------*/
}

QlEventBuffer    g_event;

void ql_entry()
{
    s32 ret;    
    g_cmd_idx = 0;
    
    Ql_SetDebugMode(BASIC_MODE);   
    Ql_DebugTrace("fota_ftp: ql_entry\r\n");
    Ql_OpenModemPort(ql_md_port1);
	FTP2FOTA_Init();

    while(TRUE)
    {    
        Ql_GetEvent(&g_event);
        switch(g_event.eventType)
        {
            case EVENT_UARTDATA:
            {
                char* pos;
                u16 fLen;
                PortData_Event* pDataEvt = (PortData_Event*)&g_event.eventData.uartdata_evt;

                // start fota (start fota=app.bin)
                Ql_sprintf(buffer, "start fota=");
                pos = Ql_strstr((char*)pDataEvt->data, buffer);
                if (pos != NULL)
                {
                    fLen = pDataEvt->len - Ql_strlen(buffer) - 2; // 2 for '\r\n'
                    Ql_strncpy(appBin_fName_svr, pos + Ql_strlen(buffer), fLen);   
                    Ql_sprintf(buffer, "ftp file name: %s\r\n", appBin_fName_svr);
                    Ql_SendToUart(ql_uart_port1, (u8 *)buffer, Ql_strlen(buffer));

                    // Start to work...
                    g_cmd_idx = 1;
                    SendAtCmd();
                    break;
                }
                break;            
            }
            
            case EVENT_MODEMDATA:
            {
                PortData_Event* pPortEvt = (PortData_Event*)&g_event.eventData.modemdata_evt;
                //Ql_DebugTrace("\r\nComing vPort data type = %d \r\n", pPortEvt->type);
                if (DATA_AT == pPortEvt->type)
                {
                    //Ql_DebugTrace("Modem data =%s\r\n", pPortEvt->data);
                    Ql_DebugTrace("%s\r\n", pPortEvt->data);
                    if (Ql_strstr((char *)pPortEvt->data, "Call Ready") != NULL)
                    {
                        // Wait 2s for the stable signal quality
                        //timer.timeoutPeriod = Ql_SecondToTicks(2);
                        //Ql_StartTimer(&timer);
                    }
                    else if ((g_cmd_type == AT_QFTPOPEN  && Ql_strstr((char*)pPortEvt->data,"+QFTPOPEN:0") != NULL)
                           || (g_cmd_type == AT_QFTPCFG  && Ql_strstr((char*)pPortEvt->data, "+QFTPCFG:0")   != NULL)
                           || (g_cmd_type == AT_QFTPPATH && Ql_strstr((char*)pPortEvt->data, "+QFTPPATH:0") != NULL)
                           || (g_cmd_type == AT_QFTPGET  && Ql_strstr((char*)pPortEvt->data, "+QFTPGET:")   != NULL)
                           || (g_cmd_type == AT_QFTPCLOSE && Ql_strstr((char*)pPortEvt->data, "+QFTPCLOSE:0") != NULL)
                           )
                    {
                        g_cmd_idx++;
                        bDoNexAT = TRUE;
                        SendAtCmd();
                    }
                    else if ((  Ql_strstr((char*)pPortEvt->data, "\r\nOK") != NULL 
                              || Ql_strstr((char*)pPortEvt->data, "OK\r\n") != NULL 
                              || Ql_strstr((char*)pPortEvt->data, "ERROR") != NULL)
                            && bDoNexAT != FALSE)
                    {
                        g_cmd_idx++;
                        SendAtCmd();
                    }
                }
                break;
            }
            
            default:
                break;
        }
    }
}

void SendAtCmd()
{
    bool exec = TRUE;
    switch (g_cmd_idx)
    {
    case 1:// Echo mode off
        Ql_sprintf((char *)buffer, "ATE0\n");
        g_cmd_type = AT_General;
        break;
        
    case 2:// Select a foreground context
        Ql_sprintf((char *)buffer, "AT+QIFGCNT=1\n");
        g_cmd_type = AT_General;
        break;
        
    case 3:// Select a bearer (0=CSD, 1=GPRS), if 'GPRS', set APN
        Ql_sprintf((char *)buffer, "AT+QICSGP=1,\"CMNET\"\n");
        g_cmd_type = AT_General;
        break;
        
    case 4:// Set user name
        Ql_sprintf((char *)buffer, "AT+QFTPUSER=\"%s\"\n", FTP_USER_NAME);
        g_cmd_type = AT_General;
        break;
        
    case 5:// Set password
        Ql_sprintf((char *)buffer, "AT+QFTPPASS=\"%s\"\n", FTP_PASSWORD);
        g_cmd_type = AT_General;
        break;
        
    case 6:// Open FTP
        Ql_sprintf((char *)buffer, "AT+QFTPOPEN=\"%s\",\"%s\"\n", FTP_SVR_ADDR, FTP_SVR_PORT);
        g_cmd_type = AT_QFTPOPEN;
        bDoNexAT = FALSE;
        break;
        
    case 7:// Set local path
        Ql_sprintf((char *)buffer, "AT+QFTPCFG=4,\"/UFS/\"\n");
        g_cmd_type = AT_QFTPCFG;
        bDoNexAT = FALSE;
        break;
        
    case 8:// Set server path
        Ql_sprintf((char *)buffer, "AT+QFTPPATH=\"%s\"\n", FTP_SVR_PATH);
        g_cmd_type = AT_QFTPPATH;
        bDoNexAT = FALSE;
        break;
        
    case 9:// Start to download file
        Ql_sprintf((char *)buffer, "AT+QFTPGET=\"%s\"\n", appBin_fName_svr);
        g_cmd_type = AT_QFTPGET;
        bDoNexAT = FALSE;
        break;
        
    case 10:// List files
        Ql_sprintf((char *)buffer, "AT+QFLST\n");
        g_cmd_type = AT_General;
        break;
        
    case 11:// Close FTP connection
        Ql_sprintf((char *)buffer, "AT+QFTPCLOSE\n");
        g_cmd_type = AT_QFTPCLOSE;
        bDoNexAT = FALSE;
        break;
        
    case 12:// Deactivate GPRS PDP context
        Ql_sprintf((char *)buffer, "AT+QIDEACT\n");
        g_cmd_type = AT_General;
        break;

    default:
        Ql_DebugTrace("at commands finished.\r\n");
        g_cmd_type = AT_General;
        exec = FALSE;

        // Start to fota upgrade
        DoFOTAUpgrade();
        break;
    }
    if (exec)
    {
      Ql_DebugTrace((char *)buffer);
      Ql_SendToModem(ql_md_port1, (u8*)buffer, Ql_strlen(buffer));
    }
}

void DoFOTAUpgrade()
{
    u32 filesize,fd_file;
    u8 *file_buffer=NULL;
    u16 off=0;
    s32 ret2,ret3;
    u32 realLen;
    bool initialized = FALSE;

    // Prepare updata data
    ret2 = Ql_FileCheck((u8*)appBin_fName_svr);
    if ( QL_RET_OK == ret2)
    {
        if(Ql_FileGetSize((u8*)appBin_fName_svr, &filesize) < 0)
        {
            Ql_DebugTrace("\r\n Fail to get size (App)\n");
            return;
        }
        else
        {
            Ql_DebugTrace("\r\n Get size Successfully. App bin size=%d\n",filesize);
        }

        fd_file = Ql_FileOpen((u8*)appBin_fName_svr, 0,1);
        if(fd_file < 0)
        {
            Ql_DebugTrace("\r\n Fail to open (App)\n");
            return ;
        }
        file_buffer = dataBuf;
        off=0;

        /*Write App  bin to flash*/
        ret3=Ql_Fota_App_Init(&Q_t_Watch_dog);
		if(ret3==0)
		{
			Ql_DebugTrace("\r\n Fota App Init Success!\n");
        }else
        {
        	Ql_DebugTrace("\r\n Fota App Init Fail!\n");
        }
        
        while(filesize>0)
        {
            if (filesize <= UA_DATA_BUfFER_LEN)
            {
                Ql_FileRead(fd_file, file_buffer, filesize, &realLen);
                Ql_Fota_App_Write_Data(filesize,(s8*)file_buffer);
                filesize=0;
            }else
            {
                Ql_FileRead(fd_file, file_buffer, UA_DATA_BUfFER_LEN, &realLen);
                ret3 = Ql_Fota_App_Write_Data(UA_DATA_BUfFER_LEN,(s8*)file_buffer);
                filesize -= UA_DATA_BUfFER_LEN;                    
            }
        }

        ret3 = Ql_Fota_App_Finish();
		if(ret3 == 0)
		{
			Ql_DebugTrace("\r\n Fota App Finish Success!\r\n");
		}else
		{
			Ql_DebugTrace("\r\n Fota App Finish Fail!\r\n");
		}
        
        Ql_FileClose(fd_file);
    }
    else if (QL_RET_ERR_FILENOTFOUND == ret2)
    {
        Ql_DebugTrace("\r\nApp Delta Bin does not exsit\n");
    }

	Ql_FileDelete((u8*)appBin_fName_svr);

    // Start to upgrade
    ret3=Ql_Fota_Update();
	if(0 != ret3)
	{
		Ql_DebugTrace("\r\nQl_Fota_Update FAILED!\r\n");
		Ql_DebugTrace("\r\nReboot 3 seconds later ...\r\n");
		Ql_Sleep(3000);
		Ql_Reset(0);
	}
	else
	{
		//If update OK, module will reboot automaticly
	}

}
#endif // __EXAMPLE_FOTA_HTTP__

