#ifdef __EXAMPLE_FOTA__
/***************************************************************************************************
*   Example:
*       
*           	Fota upgrade function
*
*   Description:
*
*           This example demonstrates the process of fota upgrading in OpenCPU.
*           application bin must be put in file system .It will be used to upgrade data.
*
*   Usage:
*
*           Compile & Run:
*
*               Use "make fota" to compile, and download bin image to module.
*           
*           Operation: (Through MAIN port)
*           step 1: you must put your application bin in your file system.
*           step 2: input string : start=[XXXX], XXXX stands for file name
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


        /******************************************************************************/
        /*                                                                            */
        /* ONLY support for application upgrade!NOT support for CORE upgrade!!!!!!!   */
        /*                                                                            */
        /******************************************************************************/


char textBuf[512];

#ifdef OUT_DEBUG(x,...)
#undef OUT_DEBUG(x,...)
#endif


#define READ_SIZE 512  //bytes

#define OUT_DEBUG(...)  \
    Ql_memset(textBuf, 0, sizeof(textBuf));  \
    Ql_sprintf(textBuf,__VA_ARGS__);   \
    Ql_SendToUart(ql_uart_port1,(u8*)(textBuf),Ql_strlen(textBuf));


FEED_DOG    Q_t_Watch_dog;
u8 g_AppBinFile[64] = "CAT";          //File name in file system


/*---------------------------------------------*/
/*
/*Function: Output flag fields 
/*
/*---------------------------------------------*/
void OUT_CUS(FEED_DOG    Q_t_Watch_dog)
{
    OUT_DEBUG("\r\n______________BEGIN______________");
    OUT_DEBUG("\r\n--Q_t_Watch_dog.Q_feed_interval1 =%x\r\n", Q_t_Watch_dog.Q_feed_interval1);
    OUT_DEBUG("\r\n--Q_t_Watch_dog.Q_feed_interval2 =%x\r\n", Q_t_Watch_dog.Q_feed_interval2);
    OUT_DEBUG("\r\n--Q_t_Watch_dog.Q_gpio_pin1 =%x\r\n", Q_t_Watch_dog.Q_gpio_pin1);
    OUT_DEBUG("\r\n--Q_t_Watch_dog.Q_gpio_pin2 =%x\r\n", Q_t_Watch_dog.Q_gpio_pin2);
    OUT_DEBUG("\r\n______________ END ______________\r\n");
}

/*---------------------------------------------*/
/*
/*Function: Start to upgrade program 
/*
/*---------------------------------------------*/
int StartAppUpdate()
{
    int iRet = -1;
    int iFileSize = 0;
    int iReadSize = 0;
    int iReadLen = 0;
    int hFile = -1;
    char buf[512];
    char *p = NULL;
    static int s_iSizeRem = 0;
    
    OUT_DEBUG("\r\n\r\n====================Start App Upgrade ====================\r\n\r\n");

    //1. Init some param.
    Ql_memset((void *)(&Q_t_Watch_dog), 0, sizeof(FEED_DOG)); //Do not enable  watch_dog
    Q_t_Watch_dog.Q_gpio_pin1 = Ql_GetGpioByName(QL_PINNAME_DTR);
    Q_t_Watch_dog.Q_feed_interval1 = 100;
    Q_t_Watch_dog.Q_gpio_pin2 = Ql_GetGpioByName(QL_PINNAME_NETLIGHT);
    Q_t_Watch_dog.Q_feed_interval2 = 500;
    
    //2. begin
    iRet = Ql_FileGetSize((u8 *)g_AppBinFile, &iFileSize); //Get the size of upgrade file from file system
    if(QL_RET_OK != iRet)
    {
        OUT_DEBUG("[max] Failed to get %s size!(iRet=%d)\r\n", g_AppBinFile, iRet);
        return -1;
    }
    else
    {
        OUT_DEBUG("[max] Get [%s] size OK!(size=%x)\r\n", g_AppBinFile, iFileSize);
    }

    iRet = Ql_FileOpen((u8 *)g_AppBinFile, 0, 1);
    if(iRet < 0)
    {
        OUT_DEBUG("[max] Open file failed!(iRet=%d)\r\n", iRet);
        return -1;
    }
    else
    {
        OUT_DEBUG("[max] Open [%s] OK!\r\n", g_AppBinFile);
        hFile = iRet;
    }

     /*Write App bin to flash*/
    iRet = Ql_Fota_App_Init(&Q_t_Watch_dog);   //Initialise the upgrade operation
    if(QL_RET_OK != iRet)
    {
        OUT_DEBUG("[max] Ql_Fota_App_Init Failed!(iRet=%d)\r\n", iRet);
        return -1;
    }
    else
    {
        OUT_DEBUG("[max] Ql_Fota_App_Init OK!\r\n");
    }
    
    OUT_DEBUG("\r\nCopying data,please waiting....\r\n");
    
    while(iFileSize > 0)
    {
        Ql_memset(buf, 0, sizeof(buf));
        if (iFileSize <= READ_SIZE)
        {
            iReadSize = iFileSize;
        }
        else
        {
            iReadSize = READ_SIZE;
        }

        iRet = Ql_FileRead(hFile, buf, iReadSize, &iReadLen);   //read upgrade data from file system
        if(QL_RET_OK != iRet)
        {
            OUT_DEBUG("[max] Read file failed!(iRet = %x)\r\n", iRet);
            return -1;
        }
        
        iRet = Ql_Fota_App_Write_Data(iReadSize,(s8*)buf);      //write upgrade data to FOTA Cache
        if(QL_RET_OK != iRet)
        {
            OUT_DEBUG("[max] Fota write file failed!(iRet = %d)\r\n", iRet);
            return -1;
        }
        else
        {
                s_iSizeRem += iReadSize;
                OUT_DEBUG("[max] : Ql_Fota_App_Write_Data -> %-5d bytes OK!(TotalWrite=%-5d)\r\n", iReadSize, s_iSizeRem);
        }
        iFileSize -= iReadLen;

        Ql_Sleep(5);  // sleep 5 seconds for outputing catcher log!!!
    }

    iRet = Ql_Fota_App_Finish();     //Finish the upgrade operation ending with calling this API
    if(QL_RET_OK != iRet)
    {
        OUT_DEBUG("[max] Ql_Fota_App_Finish failed!(iRet = %d)\r\n", iRet);
        return -1;
    }
    
    Ql_FileClose(hFile);
    OUT_DEBUG("[max] All Fota_App's operation is completely!Then writing Update Flag....\r\n");

    OUT_CUS(Q_t_Watch_dog);
    iRet = Ql_Fota_Update();   //Update flag fields in the FOTA Cache.
    if(QL_RET_OK != iRet)           //if this function succeeds, the module will automatically restart to upgrade application
    {
        OUT_DEBUG("[max] Ql_Fota_Update failed!(iRet=%d)\r\n", iRet);
        return -1;
    }
    return 0;
}


QlEventBuffer flSignalBuffer;
void ql_entry()
{
    bool keepGoing = TRUE;
    int iRet = -1;
    int i=0;
    char *pData = NULL;
    char *p = NULL;
    
    Ql_SetDebugMode(ADVANCE_MODE); 
	Ql_DebugTrace("fota: ql_entry\r\n");
    // Open Virtual Modem Serial Port1
    Ql_OpenModemPort(ql_md_port1);

    while(keepGoing)
    {    
        Ql_GetEvent(&flSignalBuffer);
        switch(flSignalBuffer.eventType)
        {
            case EVENT_MODEMDATA:
            {                
                Ql_SendToUart(ql_uart_port1, 
                    (u8 *)flSignalBuffer.eventData.modemdata_evt.data,
                    (u16 )flSignalBuffer.eventData.modemdata_evt.len);
            }
            case EVENT_UARTDATA:     //start upgrade application with "start=[XXXX]". "XXXX" stands for upgrade file name
            {
                
                if (flSignalBuffer.eventData.uartdata_evt.len > 0)
                {
                    pData = (char*)flSignalBuffer.eventData.uartdata_evt.data;
                    if(ql_uart_port1 != flSignalBuffer.eventData.uartdata_evt.port)
                    {
                        break;
                    }

                    if(NULL == pData)
                    {
                        OUT_DEBUG("[max] No Data !!!");
                        break;
                    }
                    

                    if(NULL != Ql_strstr(pData, "start=["))
                    {
                        p = Ql_strstr(pData, "[");
                        if(NULL != p)
                        {
                            p++;
                            i=0;
							Ql_memset(g_AppBinFile, 0, sizeof(g_AppBinFile));
                            while((*p) != ']')
                            {
                                if(i >= sizeof(g_AppBinFile))
                                {
                                    OUT_DEBUG("[max] CMD Param ERROR!\r\n");
                                    break;
                                }
                                g_AppBinFile[i] = (*p);
                                p++;
                                i++;
                            }
                                
                        }
                        else
                        {
                            OUT_DEBUG("[max] CMD ERROR!\r\n");
                            break;
                        }
                        
                        iRet = StartAppUpdate();
                        if(0 != iRet)
                        {
                            OUT_DEBUG("[max] failed to update app!!\r\n");
                            break ;
                        }
                    }
                    else
                    {
                        Ql_SendToModem(ql_md_port1, (u8*)pData, flSignalBuffer.eventData.uartdata_evt.len);
                    }
                }
                break;
			}
            default:
            {
                break;
            }
        }
    }
}

#endif // __EXAMPLE_FOTA__


