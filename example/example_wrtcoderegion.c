#ifdef __EXAMPLE_WRTCODEREGION__
/***************************************************************************************************
*   Example:
*       
*           WRTCODEREGION  Routine
*
*   Description:
*
*           This example demonstrates how to use WRTCODEREGION function with APIs in OpenCPU.
*           Through MAIN Uart port, input the specified command, and the response message will be 
*           printed out through DEBUG port.
*
*   Usage:
*
*           Compile & Run:
*
*               Use "make WRTCODEREGION" to compile, and download bin image to module to run.
*           
*           Operation: (Through MAIN port)
*
*               Step 1: download a bin file to the root directory.
*               Step 2: change the "FileName=" to downloaded file name.
*               Step 3: input "begin x" ,x represents offset, range 0~39(please check APP_SPACE_SCHEME_ENUM) , when you set it ,
*                          please ensure adequate offset, and should not coverage the current running code. 
*           
****************************************************************************************************/

#include "ql_appinit.h"
#include "ql_interface.h"
#include "ql_type.h"
#include "ql_trace.h"
#include "ql_audio.h"
#include "ql_timer.h"
#include "ql_stdlib.h"
#include "ql_filesystem.h"
#include "ql_error.h"
#include "ql_fcm.h"
#include <string.h>


/*  Test Code */

void ql_entry()
{
    bool           keepGoing = TRUE;
    QlEventBuffer  flSignalBuffer;
    char *p=NULL;
    char *pData= NULL;

    Ql_SetDebugMode(BASIC_MODE);   
    Ql_DebugTrace("filesystem: ql_entry(Ql_WrtCodeRegion Test)\r\n");
    Ql_OpenModemPort(ql_md_port1);  
	Ql_SendToUart(ql_uart_port1, (u8*)("\r\n_Write code region begin!\r\n"), Ql_strlen("\r\n_Write code region begin!\r\n"));

    while(keepGoing)
    {    
        Ql_GetEvent(&flSignalBuffer);
        switch(flSignalBuffer.eventType)
        {
            case EVENT_MODEMDATA:
            {
                PortData_Event* pPortEvt = (PortData_Event*)&flSignalBuffer.eventData.modemdata_evt;
                Ql_DebugTrace("%s\r\n", pPortEvt->data);
                //Ql_DebugTrace("\r\nComing vPort data type = %d \r\n", pPortEvt->type);
                break;
            }
            case EVENT_UARTDATA:
            {                
                if (flSignalBuffer.eventData.uartdata_evt.len>0)
                {
					pData = (char *)flSignalBuffer.eventData.uartdata_evt.data;
					Ql_SendToUart(ql_uart_port1, (u8 *)flSignalBuffer.eventData.uartdata_evt.data, flSignalBuffer.eventData.uartdata_evt.len);
					p = Ql_strstr(pData, "begin");

					if( NULL != p)
					{
						u8  FileName[] = "helloworld.bin";
						u8  FileBuffer[512];
						s32 s32BufferSize = 0;
						u32 u32FileSize = 0;
						s32 s32Res = -1;
						s32 s32FileHandle = -1;
						u32 u32ReadLen = 0;
						u32 u32AppSpaceScheme = 0;
                        u32 uiWrtLen = 0;
						s32 s32Tmp = -1;
						
						Ql_DebugTrace("\r\nstarting work!\r\n\\r\n");
						s32Tmp = Ql_FileGetFreeSize();
						if(s32Tmp < 0)
						{
							Ql_DebugTrace("\r\nFAILED to get file system free size!(Res:%d),exiting...\r\n", s32Tmp);
							break ;
						}
						else
						{
							Ql_DebugTrace("\r\n The free size of file system is %d", s32Tmp);
						}

						s32Res = Ql_FileGetSize(FileName, &u32FileSize);
						if(QL_RET_OK != s32Res)
						{
							Ql_DebugTrace("\r\n FAILED to get size of <%s>!(Res:%d),exiting...", FileName, s32Res);
							break ;
						}
						else 
						{
							Ql_DebugTrace("\r\n %d bytes will be written to cone region in flash", u32FileSize);
						}

						s32FileHandle = Ql_FileOpenEx(FileName, QL_FS_READ_ONLY);
						if(s32FileHandle < QL_RET_OK)
						{
							Ql_DebugTrace("\r\nOpen file FAILED!(Res:%d),exiting...", s32FileHandle);
							break ;
						}
						else
						{
							Ql_DebugTrace("\r\nOpen file OK!\r\n");
						}

						sscanf(p+Ql_strlen("begin "), "%x", &u32AppSpaceScheme);   //get starting addr
						s32BufferSize = sizeof(FileBuffer);         //remember filebuffer size
						Ql_DebugTrace("\r\nApp_Scheme=%d(d), %x(x)\r\n\r\n", u32AppSpaceScheme, u32AppSpaceScheme);
						s32Res = Ql_WrtCodeRegion_Init(u32AppSpaceScheme); 
						if(QL_RET_OK != s32Res)
						{
							Ql_DebugTrace("\r\n _Ql_WrtCodeRegion_Init Failed!(Res:%d),exting...\r\n", s32Res);
							Ql_FileClose(s32FileHandle);
							break ;
						}
						else
						{
							Ql_DebugTrace("\r\n _Ql_WrtCodeRegion_Init OK!\r\n");
						}
					
						
						while(u32FileSize > 0)
						{
							Ql_memset(FileBuffer, 0, s32BufferSize);
							
							if(u32FileSize > s32BufferSize)
							{
								s32Tmp = s32BufferSize;
							}
							else
							{
								s32Tmp = u32FileSize;
							}

							s32Res = Ql_FileRead(s32FileHandle,FileBuffer, s32Tmp, &u32ReadLen);
							if(s32Res != QL_RET_OK)
							{
								Ql_DebugTrace("\r\nRead file error!(Res:%d),exiting...", s32Res);
								Ql_FileClose(s32FileHandle);
								Ql_WrtCodeRegion_Finish();
								break ;
							}
							else
							{
								Ql_DebugTrace("\r\nRead file OK! read size:%d", u32ReadLen);
							}
							u32FileSize -= u32ReadLen;

                            if( 0 != u32ReadLen%2)   //to ensure that the written length is EVEN!!!!!  VERY IMPORTANT!!!
                            {
                                uiWrtLen = u32ReadLen+1;
                                FileBuffer[u32ReadLen] = 0xFF;
                            }
                            else
                            {
                                uiWrtLen = u32ReadLen;
                            }
                            
							s32Res = Ql_WrtCodeRegion_Write((s8 *)FileBuffer, uiWrtLen);
							if(QL_RET_OK != s32Res)
							{
								Ql_DebugTrace("\r\n Ql_WrtCodeRegion_Write FAILED!(Res:%d), exiting...", s32Res);
								Ql_FileClose(s32FileHandle);
								Ql_WrtCodeRegion_Finish();
								break ;
							}
							else
							{
								Ql_SendToUart(ql_uart_port1, FileBuffer, uiWrtLen);
								Ql_DebugTrace("\r\n Ql_WrtCodeRegion_Write OK!\r\n");
							}
                            Ql_Sleep(5);       // Wait for 5 seconds to output catcher log!
							
						}

						Ql_FileClose(s32FileHandle);
						Ql_WrtCodeRegion_Finish();
						Ql_DebugTrace("\r\nFile handle closed OK and Write code to region OK!\r\n\r\n");
						Ql_DebugTrace("\r\nProgram Ends!\r\n");

					}
					else
					{
						Ql_SendToUart(ql_uart_port1, (u8 *)("Unkown commander!\r\n"), Ql_strlen("Unkown commander!\r\n"));
					}
					break;
              }
                break;            
            }
            
            default:
                break;
        }
    }
}

#endif // __EXAMPLE_WRTCODEREGION__


