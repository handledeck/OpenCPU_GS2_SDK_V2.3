#ifdef __EXAMPLE_FILESYSTEM__
/***************************************************************************************************
*   Example:
*       
*           	FILE Routine
*
*   Description:
*
*           This example demonstrates how to use FILE function with APIs in OpenCPU.
*           Through MAIN Uart port, input the specified command, and the response message will be 
*           printed out through MAIN Uart port or debug port.
*
*   Usage:
*
*           Compile & Run:
*
*               Use "make file" to compile, and download bin image to module to run.
*               Use 'make file_sd' to test sd card access.
*           
*           Operation: (Through MAIN port)
*            
*               If input "Ql_FileGetFreeSize?", that will obtain the amount of free space on Flash.
*               If input "Ql_FileGetFreeSizeEx", that will obtain the amount of free space on Flash and SD card.
*               If input "Ql_FileOpen=", that will open and automatically create a named file, but this function is deprecated.   
*               If input "Ql_FileOpenEx=", that will open and automatically create a named file.
*               If input "Ql_FileRead", that will read data from the specified file.
*               If input "Ql_FileWrite=", that will write data to a file.
*               If input "Ql_FileSeek=", that will reposition the pointer in the previously opened file.
*               If input "Ql_FileGetFilePosition", that will get the current value of the file pointer.
*               If input "Ql_FileClose", that will close the file associated with the file handle.
*               If input "Ql_FileGetSize=", that will retrieve the size, in bytes, of the specified file.
*               If input "Ql_FileDelete=", that will delete an existing file from a file system.
*               If input "Ql_FileRename=", that will rename a file.
*               If input "Ql_FileCheck=", that will  check whether the file exists or not.
*               If input "Ql_FileCreateDir=", that will create a directory.
*               If input "Ql_FileRemoveDir=", that will remove a existing directory.
*               If input "Ql_FileCheckDir=", that will check whether the file exists or not.
*               If input "Ql_FileFindFirst=", that will searche a directory for a file or subdirectory whose name matches the specified file name. 
*               If input "Ql_FileXDelete=", that will delete a file or directory.
*               If input "Ql_FileXMove=", that will provide a facility to move/copy a file or folder.
*               If input "Ql_DownLoadFile=", that will open and automatically create a named file, this file will receive content from uart when you want to download file .
*               If input "Ql_FinishLoadFile",  that will stop download file when  receiving  complete.
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

#define  MY_FILE_TAG  		"\r\nQuectel GS2 : file test\r\n"
#define  FILE_CONTENT   	((u8 *)"this is file %d")

#ifndef __TEST_FOR_MEMORY_CARD__
#define  PATH_ROOT    		((u8 *)"myroot")
#define  PATH_SUB_1     	((u8 *)"myroot\\sub1")
#define  PATH_SUB_2     	((u8 *)"myroot\\sub2")
#define  PATH_FILE_1    	((u8 *)"myroot\\sub1\\file1")
#define  PATH_FILE_2    	((u8 *)"myroot\\sub2\\file2")
#define  PATH_FILE_3    	((u8 *)"myroot\\sub2\\file3")

#define  PATH_SUB_3     	((u8 *)"myroot\\sub3")
#define  PATH_SUB_3_1   	((u8 *)"myroot\\sub3\\sub3_1")
#define  PATH_SUB_3_2   	((u8 *)"myroot\\sub3\\sub3_2")
#define  PATH_FILE_3_1  	((u8 *)"myroot\\sub3\\file1")
#define  PATH_FILE_3_2  	((u8 *)"myroot\\sub3\\file2")
#define  PATH_FILE_3_2_1	((u8 *)"myroot\\sub3\\sub3_2\\file1")
#else   //!__TEST_FOR_MEMORY_CARD__
#define  PATH_ROOT    		((u8 *)"SD:myroot")
#define  PATH_SUB_1     	((u8 *)"SD:myroot\\sub1")
#define  PATH_SUB_2     	((u8 *)"SD:myroot\\sub2")
#define  PATH_FILE_1    	((u8 *)"SD:myroot\\sub1\\file1")
#define  PATH_FILE_2    	((u8 *)"SD:myroot\\sub2\\file2")
#define  PATH_FILE_3    	((u8 *)"SD:myroot\\sub2\\file3")

#define  PATH_SUB_3     	((u8 *)"SD:myroot\\sub3")
#define  PATH_SUB_3_1   	((u8 *)"SD:myroot\\sub3\\sub3_1")
#define  PATH_SUB_3_2   	((u8 *)"SD:myroot\\sub3\\sub3_2")
#define  PATH_FILE_3_1  	((u8 *)"SD:myroot\\sub3\\file1")
#define  PATH_FILE_3_2  	((u8 *)"SD:myroot\\sub3\\file2")
#define  PATH_FILE_3_2_1	((u8 *)"SD:myroot\\sub3\\sub3_2\\file1")
#endif  //__TEST_FOR_MEMORY_CARD__

#define OUT_DEBUG(x,...)  \
    Ql_memset((void*)(x),0,100);  \
    Ql_sprintf((char*)(x),__VA_ARGS__);   \
    Ql_SendToUart(ql_uart_port1,(u8*)(x),Ql_strlen((const char*)(x)));

#define __MAX_BYTES__ 64

s8 s_s8TmpBuf[1024];
/*  Test Code */
char textBuf[100];
u8 strBuf[100];
s32 filehandle = -1;


/*********************************************************
 *Name : CheckFileOK
 *Description: This function will create a structure as below:
  myroot[F]
  |
  |--------sub1[F]
  |          |
  |          |----file1[B]
  |
  |--------sub2[F]
  |          |
  |          |----file2[B]
  |          |----file3[B]
  |
  |--------sub3[F]
           |----file1[B]
           |----file2[B]
           |-------sub3_1[F]
           |-------sub3_2[F]
                       |-----file1[B]
                       
  [F] --> folder    [B] --> file
 *Parameters: None
 *Return : -1 : Fatal Error! 
*********************************************************/

s32 CheckFileOK()
{
	s32 s32Res = -1;
	s32 s32FileHandle =-1;
	char  buf[128];

//PATH_ROOT
	s32Res = Ql_FileCheckDir(PATH_ROOT);
	if(s32Res != QL_RET_OK)
	{
		Ql_DebugTrace("\r\n Dir(%s) is not exist, creating....", PATH_ROOT);
		s32Res = Ql_FileCreateDir(PATH_ROOT);
		if(s32Res != QL_RET_OK)
		{
			Ql_DebugTrace("\r\n Create Dir(%s) failed!", PATH_ROOT);
			return -1;
		}
		else
		{
			Ql_DebugTrace("\r\n CreateDir(%s) OK!", PATH_ROOT);
		}
	}
	else
	{
		Ql_DebugTrace("\r\nDir(%s) has existed!", PATH_ROOT);
	}

//PATH_SUB_1
	s32Res = Ql_FileCheckDir(PATH_SUB_1);
	if(s32Res != QL_RET_OK)
	{
		Ql_DebugTrace("\r\n Dir(%s) is not exist, creating....", PATH_SUB_1);
		s32Res = Ql_FileCreateDir(PATH_SUB_1);
		if(s32Res != QL_RET_OK)
		{
			Ql_DebugTrace("\r\n Create Dir(%s) failed!", PATH_SUB_1);
			return -1;
		}
		else
		{
			Ql_DebugTrace("\r\n CreateDir(%s) OK!", PATH_SUB_1);
		}
	}
	else
	{
		Ql_DebugTrace("\r\nDir(%s) has existed!", PATH_SUB_1);
	}

//PATH_SUB_2
	s32Res = Ql_FileCheckDir(PATH_SUB_2);
	if(s32Res != QL_RET_OK)
	{
		Ql_DebugTrace("\r\n Dir(%s) is not exist, creating....", PATH_SUB_2);
		s32Res = Ql_FileCreateDir(PATH_SUB_2);
		if(s32Res != QL_RET_OK)
		{
			Ql_DebugTrace("\r\n Create Dir(%s) failed!", PATH_SUB_2);
			return -1;
		}
		else
		{
			Ql_DebugTrace("\r\n Create Dir(%s) OK!", PATH_SUB_2);
		}
	}
	else
	{
		Ql_DebugTrace("\r\nDir(%s) has existed!", PATH_SUB_2);
	}

//PATH_SUB_3
	s32Res = Ql_FileCheckDir(PATH_SUB_3);
	if(s32Res != QL_RET_OK)
	{
		Ql_DebugTrace("\r\n Dir(%s) is not exist, creating....", PATH_SUB_3);
		s32Res = Ql_FileCreateDir(PATH_SUB_3);
		if(s32Res != QL_RET_OK)
		{
			Ql_DebugTrace("\r\n Create Dir(%s) failed!", PATH_SUB_3);
			return -1;
		}
		else
		{
			Ql_DebugTrace("\r\n Create Dir(%s) OK!", PATH_SUB_3);
		}
	}
	else
	{
		Ql_DebugTrace("\r\nDir(%s) has existed!", PATH_SUB_3);
	}

//PATH_SUB_3_1
	s32Res = Ql_FileCheckDir(PATH_SUB_3_1);
	if(s32Res != QL_RET_OK)
	{
		Ql_DebugTrace("\r\n Dir(%s) is not exist, creating....", PATH_SUB_3_1);
		s32Res = Ql_FileCreateDir(PATH_SUB_3_1);
		if(s32Res != QL_RET_OK)
		{
			Ql_DebugTrace("\r\n Create Dir(%s) failed!", PATH_SUB_3_1);
			return -1;
		}
		else
		{
			Ql_DebugTrace("\r\n Create Dir(%s) OK!", PATH_SUB_3_1);
		}
	}
	else
	{
		Ql_DebugTrace("\r\nDir(%s) has existed!", PATH_SUB_3_1);
	}

//PATH_SUB_3_2
	s32Res = Ql_FileCheckDir(PATH_SUB_3_2);
	if(s32Res != QL_RET_OK)
	{
		Ql_DebugTrace("\r\n Dir(%s) is not exist, creating....", PATH_SUB_3_2);
		s32Res = Ql_FileCreateDir(PATH_SUB_3_2);
		if(s32Res != QL_RET_OK)
		{
			Ql_DebugTrace("\r\n Create Dir(%s) failed!", PATH_SUB_3_2);
			return -1;
		}
		else
		{
			Ql_DebugTrace("\r\n Create Dir(%s) OK!", PATH_SUB_3_2);
		}
	}
	else
	{
		Ql_DebugTrace("\r\nDir(%s) has existed!", PATH_SUB_3_2);
	}


// PATH_FILE_1
	s32Res = Ql_FileCheck(PATH_FILE_1);
	if(s32Res != QL_RET_OK)
	{
		s32FileHandle = Ql_FileOpenEx(PATH_FILE_1, (QL_FS_READ_WRITE|QL_FS_CREATE));
		if(s32FileHandle < 0)
		{
			Ql_DebugTrace("\r\n Create File(%s) failed!", PATH_FILE_1);
			return -1;
		}
		else
		{
			Ql_DebugTrace("\r\n Create File(%s) OK!", PATH_FILE_1);
		}
		Ql_memset(buf, 0, sizeof(buf));
		Ql_sprintf(buf, (char *)FILE_CONTENT, 1);
		Ql_FileWrite(s32FileHandle , (u8 *)buf, Ql_strlen(buf), (u32 *)(&s32Res));
		Ql_FileClose(s32FileHandle);
	}
	else
	{
		Ql_DebugTrace("\r\nFile(%s) has existed!", PATH_FILE_1);
	}
	
// PATH_FILE_2
	s32Res = Ql_FileCheck(PATH_FILE_2);
	if(s32Res != QL_RET_OK)
	{
		s32FileHandle = Ql_FileOpenEx(PATH_FILE_2, (QL_FS_READ_WRITE|QL_FS_CREATE));
		if(s32FileHandle < 0)
		{
			Ql_DebugTrace("\r\n Create File(%s) failed!", PATH_FILE_2);
			return -1;
		}
		else
		{
			Ql_DebugTrace("\r\n Create File(%s) OK!", PATH_FILE_2);
		}
		Ql_memset(buf, 0, sizeof(buf));
		Ql_sprintf(buf, (char *)FILE_CONTENT, 2);
		Ql_FileWrite(s32FileHandle , (u8 *)buf, Ql_strlen(buf), (u32 *)(&s32Res));
		Ql_FileClose(s32FileHandle);
	}
	else
	{
		Ql_DebugTrace("\r\nFile(%s) has existed!", PATH_FILE_2);
	}
	
// PATH_FILE_3
	s32Res = Ql_FileCheck(PATH_FILE_3);
	if(s32Res != QL_RET_OK)
	{
		s32FileHandle = Ql_FileOpenEx(PATH_FILE_3, (QL_FS_READ_WRITE|QL_FS_CREATE));
		if(s32FileHandle < 0)
		{
			Ql_DebugTrace("\r\n Create File(%s) failed!", PATH_FILE_3);
			return -1;
		}
		else
		{
			Ql_DebugTrace("\r\n Create File(%s) OK!", PATH_FILE_3);
		}
		Ql_memset(buf, 0, sizeof(buf));
		Ql_sprintf(buf, (char *)FILE_CONTENT, 3);
		Ql_FileWrite(s32FileHandle , (u8 *)buf, Ql_strlen(buf), (u32 *)(&s32Res));
		Ql_FileClose(s32FileHandle);
	}
	else
	{
		Ql_DebugTrace("\r\nFile(%s) has existed!", PATH_FILE_3);
	}

// PATH_FILE_3_1
	s32Res = Ql_FileCheck(PATH_FILE_3_1);
	if(s32Res != QL_RET_OK)
	{
		s32FileHandle = Ql_FileOpenEx(PATH_FILE_3_1, (QL_FS_READ_WRITE|QL_FS_CREATE));
		if(s32FileHandle < 0)
		{
			Ql_DebugTrace("\r\n Create File(%s) failed!", PATH_FILE_3_1);
			return -1;
		}
		else
		{
			Ql_DebugTrace("\r\n Create File(%s) OK!", PATH_FILE_3_1);
		}
		Ql_memset(buf, 0, sizeof(buf));
		Ql_sprintf(buf, (char *)FILE_CONTENT, 3);
		Ql_FileWrite(s32FileHandle , (u8 *)buf, Ql_strlen(buf), (u32 *)(&s32Res));
		Ql_FileClose(s32FileHandle);
	}
	else
	{
		Ql_DebugTrace("\r\nFile(%s) has existed!", PATH_FILE_3_1);
	}

// PATH_FILE_3_2
	s32Res = Ql_FileCheck(PATH_FILE_3_2);
	if(s32Res != QL_RET_OK)
	{
		s32FileHandle = Ql_FileOpenEx(PATH_FILE_3_2, (QL_FS_READ_WRITE|QL_FS_CREATE));
		if(s32FileHandle < 0)
		{
			Ql_DebugTrace("\r\n Create File(%s) failed!", PATH_FILE_3_2);
			return -1;
		}
		else
		{
			Ql_DebugTrace("\r\n Create File(%s) OK!", PATH_FILE_3_2);
		}
		Ql_memset(buf, 0, sizeof(buf));
		Ql_sprintf(buf, (char *)FILE_CONTENT, 3);
		Ql_FileWrite(s32FileHandle , (u8 *)buf, Ql_strlen(buf), (u32 *)(&s32Res));
		Ql_FileClose(s32FileHandle);
	}
	else
	{
		Ql_DebugTrace("\r\nFile(%s) has existed!", PATH_FILE_3_2);
	}

//PATH_FILE_3_2_1
	s32Res = Ql_FileCheck(PATH_FILE_3_2_1);
	if(s32Res != QL_RET_OK)
	{
		s32FileHandle = Ql_FileOpenEx(PATH_FILE_3_2_1, (QL_FS_READ_WRITE|QL_FS_CREATE));
		if(s32FileHandle < 0)
		{
			Ql_DebugTrace("\r\n Create File(%s) failed!", PATH_FILE_3_2_1);
			return -1;
		}
		else
		{
			Ql_DebugTrace("\r\n Create File(%s) OK!", PATH_FILE_3_2_1);
		}
		Ql_memset(buf, 0, sizeof(buf));
		Ql_sprintf(buf, (char *)FILE_CONTENT, 3);
		Ql_FileWrite(s32FileHandle , (u8 *)buf, Ql_strlen(buf), (u32 *)(&s32Res));
		Ql_FileClose(s32FileHandle);
	}
	else
	{
		Ql_DebugTrace("\r\nFile(%s) has existed!", PATH_FILE_3_2_1);
	}


	return QL_RET_OK;
}

void _print_bytes(void *data, u8 bytes)
{
    u8 buf[__MAX_BYTES__];
    u8 idx = 0;
    u8 *ptr = (u8*)data;
    if(bytes > __MAX_BYTES__)
        bytes = __MAX_BYTES__;
    for(idx = 0; idx < bytes; ++idx)
    {
        OUT_DEBUG(textBuf, "%02x ", ptr[idx]);
    }
}

/******************************************************************************/
/*                              Main Code                                     */
/******************************************************************************/

void ql_entry()
{
    bool           keepGoing = TRUE;
    QlEventBuffer  flSignalBuffer;
    s32 freesize;    
    char *pData= NULL;
    s32 ret;
	s32 s32Res;
	s32 plen=0;
	u32 flag=0;
	
    Ql_SetDebugMode(BASIC_MODE);   
    Ql_DebugTrace(MY_FILE_TAG);
    Ql_OpenModemPort(ql_md_port1);       


	Ql_DebugTrace("\r\nChecking whether files exist...\r\n");
	s32Res= CheckFileOK();
	if(s32Res != QL_RET_OK)
	{
		Ql_DebugTrace("\r\nThe tested file system struct is not present or broken!");
		return ;
	}
    while(keepGoing)
    {   
        char *p = NULL; 
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
                    pData = (char*)flSignalBuffer.eventData.uartdata_evt.data;
					plen = flSignalBuffer.eventData.uartdata_evt.len;
                    /*set last tail data eq 0*/
                    pData[flSignalBuffer.eventData.uartdata_evt.len+1] = '\0';

                    /*cmd:  Ql_Fs_Format() */
                    // 1 format C:/UFS   3  format SD card
                    p = Ql_strstr(pData,"Ql_Fs_Format=");
                    if(p)
                    {
                        s8 type;
                        p+=13;
                        type = Ql_atoi(p);
                        ret = Ql_Fs_Format(type);
                        Ql_sprintf(textBuf,"Ql_Fs_Format()=%d  type =%d\r\n",ret,type);
                        Ql_SendToUart(ql_uart_port1,(u8*)textBuf,Ql_strlen(textBuf));  
                        break;
                    }                
                   /*cmd:  Ql_FileGetFreeSize?*/
                    p = Ql_strstr(pData,"Ql_FileGetFreeSize?");
                    if (p)
                    {
                        s32 size;
                        size=Ql_FileGetFreeSize();
                        Ql_sprintf(textBuf,"Ql_FileGetFreeSize()=%d\r\n",size);
                        Ql_SendToUart(ql_uart_port1,(u8*)textBuf,Ql_strlen(textBuf));    
                        break;
                    }
                    
                    /*cmd:  Ql_FileGetFreeSizeEx=1   1:flash region;2:sd card*/
                    p = Ql_strstr(pData,"Ql_FileGetFreeSizeEx=");
                    if (p)
                    {
                        u64 size = 0, totalSpace;
                        u8 flag = Ql_FS_UFS;
                        char *p1 = NULL;
                        p1 = Ql_strstr(pData,"=");
                        p1 += 1;
                        if ('1' == *p1)
                        {
                            flag = Ql_FS_UFS;
                        }else if('3' == *p1)
                        {
                            flag = Ql_FS_SD;
                        }
                        else
                        {
                            OUT_DEBUG(textBuf, "Ql_FileGetFreeSizeEx  param error !!\r\n");
                            break;
                        }
                        
                        s32Res=Ql_FileGetFreeSizeEx(flag, &size);
                        Ql_sprintf(textBuf,"Ql_FileGetFreeSizeEx()=%d\r\n",s32Res);
                        Ql_SendToUart(ql_uart_port1,(u8*)textBuf,Ql_strlen(textBuf));
                        _print_bytes(&size, sizeof(u64));
                        s32Res = Ql_FileSys_GetSpaceInfo(flag, &size, &totalSpace);
                        OUT_DEBUG(textBuf, "Ql_FileSys_GetSpaceInfo=%d\r\n", s32Res);
                        _print_bytes(&size, sizeof(u64));
                        OUT_DEBUG(textBuf, "total:\r\n");
                        _print_bytes(&totalSpace, sizeof(u64));
                        break;
                    }

                    /*cmd:  Ql_FileOpen=#dir#bcreate#breadonly# */
                    /*Create file*/
                    p = Ql_strstr(pData,"Ql_FileOpen=");
                    if (p)
                    {
                        s32 ret;
						bool bCreated = FALSE;
						bool bReadOnly = FALSE;
						char *p = NULL;
						char *q = NULL;
                        p = Ql_strstr(pData,"#");
                        if(!p)
                        {
                            Ql_DebugTrace("bad format!");
                            break;
                        }
						(*p) = 0;
						p++;
                        q = Ql_strstr(p, "#");
                        if(!q)
                        {
                            Ql_DebugTrace("bad format!");
                            break;
                        }
                        
						(*q) = 0;
						q++;
                                                                  //Dir 
						Ql_memset(s_s8TmpBuf, 0, sizeof(s_s8TmpBuf));
						Ql_strcpy((char *)s_s8TmpBuf, p);

						p = Ql_strstr(q, "#");
                        if(!p)
                        {
                            Ql_DebugTrace("bad format!");
                            break;
                        }
						(*p) = 0;
						p++;
						if(0 == Ql_strcmp(q, "TRUE")) //bcreated
						{
							bCreated = TRUE;
						}
						else
						{  
							bCreated = FALSE;
						}

						q = Ql_strstr(p, "#");   //breadonly
						if(!q)
                        {
                            Ql_DebugTrace("bad format!");
                            break;
                        }
						(*q) = 0;
						if(0 == Ql_strcmp(p, "TRUE")) //bcreated
						{
							bReadOnly = TRUE;
						}
						else
						{  
							bReadOnly = FALSE;
						}

						Ql_DebugTrace("\r\nQl_FileOpen(%s ,%d, %d)\r\n", s_s8TmpBuf, bCreated, bReadOnly);
						
                        ret = Ql_FileOpen((u8*)s_s8TmpBuf , bCreated, bReadOnly);
                        if(ret >= QL_RET_OK)
                        {
                            filehandle = ret;
                        }
                        Ql_sprintf(textBuf,"Ql_FileOpen Create (%s)=%d\r\n", s_s8TmpBuf, ret);
                        Ql_SendToUart(ql_uart_port1,(u8*)textBuf,Ql_strlen(textBuf));    
                        break;
                    }        

                    /*cmd:  Ql_FileOpenEx=#FileName#RW|RO|CR|CA|OS#*/
                    /*Open file*/
                    p = Ql_strstr(pData,"Ql_FileOpenEx=");
                    if (p)
                    {
                        s32 ret;
                        char *p = NULL;
						char *q = NULL;
						u32 uiFlag = 0;

						p = Ql_strstr(pData, "#");
                        if(!p)
                        {
                            Ql_DebugTrace("bad format!");
                            break;
                        }
						(*p) = 0;
						p++;
						
						q = Ql_strstr(p, "#");
                        if(!q)
                        {
                            Ql_DebugTrace("bad format!");
                            break;
                        }
						(*q) = 0;
						q++;

						Ql_memset(s_s8TmpBuf, 0, sizeof(s_s8TmpBuf));  //File Name
						Ql_strcpy((char *)s_s8TmpBuf, p);

						if(NULL != Ql_strstr(q, "RW"))
						{
							uiFlag |= QL_FS_READ_WRITE;
							OUT_DEBUG(textBuf, "\r\nOpenEx Flag |= QL_FS_READ_WRITE");
						}

						if(NULL != Ql_strstr(q, "RO"))
						{
							uiFlag |= QL_FS_READ_ONLY;
							OUT_DEBUG(textBuf, "\r\nOpenEx Flag |= QL_FS_READ_ONLY");
						}

						if(NULL != Ql_strstr(q, "CR"))
						{
							uiFlag |= QL_FS_CREATE;
							OUT_DEBUG(textBuf, "\r\nOpenEx Flag |= QL_FS_CREATE");
							
						}

						if(NULL != Ql_strstr(q, "CA"))
						{
							uiFlag |= QL_FS_CREATE_ALWAYS;
							OUT_DEBUG(textBuf, "\r\nOpenEx Flag |= QL_FS_CREATE_ALWAYS");
						}

						if(NULL != Ql_strstr(q, "OS"))
						{
							uiFlag |= QL_FS_OPEN_SHARED;
							OUT_DEBUG(textBuf, "\r\nOpenEx Flag |= QL_FS_OPEN_SHARED");
						}

 						Ql_DebugTrace("\r\nQl_FileOpenEx(%s, %d)\r\n", s_s8TmpBuf, uiFlag);
                        ret = Ql_FileOpenEx((u8*)s_s8TmpBuf, uiFlag);
                        if(ret >= QL_RET_OK)
                        {
                            filehandle = ret;
                        }
                        Ql_sprintf(textBuf," Ql_FileOpenEx Create (%s,%08x)=%d\r\n", s_s8TmpBuf, uiFlag, ret);
                        Ql_SendToUart(ql_uart_port1,(u8*)textBuf,Ql_strlen(textBuf));    
                        break;
                    }

                    p = Ql_strstr(pData,"Ql_FileRead");
                    if (p)
                    {
                        s32 ret;
                        u32 readedlen;
                        Ql_memset(strBuf,0,100);
                        Ql_memset(textBuf,0,100);
                        ret = Ql_FileRead(filehandle, strBuf, 100, &readedlen);
                        Ql_sprintf(textBuf,"Ql_FileRead()=%d: readedlen=%d, strBuf=%s\r\n",ret, readedlen, strBuf);
                        Ql_SendToUart(ql_uart_port1,(u8*)textBuf,Ql_strlen(textBuf));    
                        break;
                    }

                    /*cmd:  Ql_FileWrite=xxxxxxxxxxx*/
                    p = Ql_strstr(pData,"Ql_FileWrite=");
                    if (p)
                    {
                        s32 ret;
                        u32 writeedlen;
                        p = Ql_strstr(pData,"=");
                        p += 1;
                        Ql_memset(textBuf,0,100);
                        ret = Ql_FileWrite(filehandle, (u8*)p, Ql_strlen((char*)p), &writeedlen);
                        Ql_sprintf(textBuf,"Ql_FileWrite()=%d: writeedlen=%d\r\n",ret, writeedlen);
                        Ql_SendToUart(ql_uart_port1,(u8*)textBuf,Ql_strlen(textBuf));    
                        break;
                    }

                    /*cmd:  Ql_FileSeek=xxx                 xxx is pos*/
                    p = Ql_strstr(pData,"Ql_FileSeek=");
                    if (p)
                    {
                        s32 ret;
						int iPos = 2;

						p = Ql_strstr(p, "=");
						p++;
						iPos = Ql_atoi(p);

						OUT_DEBUG(textBuf, "\r\niPos = %d\r\n", iPos);
                        ret = Ql_FileSeek(filehandle, iPos , QL_FS_FILE_BEGIN);
						OUT_DEBUG(textBuf, "\r\nQl_FileSeek End\r\n");
						Ql_memset(textBuf,0,100);
                        Ql_sprintf(textBuf,"Ql_FileSeek()=%d: offset=%d\r\n",ret, iPos);
                        Ql_SendToUart(ql_uart_port1,(u8*)textBuf,Ql_strlen(textBuf));    
                        break;
                    }

                    p = Ql_strstr(pData,"Ql_FileGetFilePosition");
                    if (p)
                    {
                        s32 ret;
                        u32 Position;
                        ret = Ql_FileGetFilePosition(filehandle, &Position);
                        Ql_sprintf(textBuf,"Ql_FileGetFilePosition()=%d: Position=%d\r\n",ret, Position);
                        Ql_SendToUart(ql_uart_port1,(u8*)textBuf,Ql_strlen(textBuf));    
                        break;
                    }

                    p = Ql_strstr(pData,"Ql_FileClose");
                    if (p)
                    {
                        Ql_FileClose(filehandle);
                        filehandle = -1;
                        Ql_sprintf(textBuf,"Ql_FileClose()\r\n");
                        Ql_SendToUart(ql_uart_port1,(u8*)textBuf,Ql_strlen(textBuf));    
                        break;
                    }

                    /*cmd:  Ql_FileGetSize=Dir\File1.txt*/
                    p = Ql_strstr(pData,"Ql_FileGetSize=");
                    if (p)
                    {
                        s32 ret;
                        u32 filesize;
                        p = Ql_strstr(pData,"=");
                        p += 1;
                        ret = Ql_FileGetSize((u8*)p ,&filesize);
                        Ql_sprintf(textBuf,(char*)("Ql_FileGetSize(%s)=%d, filesize=%d\r\n"), p, ret, filesize);
                        Ql_SendToUart(ql_uart_port1,(u8*)textBuf,Ql_strlen(textBuf));    
                        break;
                    }

                    /*cmd:  Ql_FileDelete=Dir\File1.txt*/
                    p = Ql_strstr(pData,"Ql_FileDelete=");
                    if (p)
                    {
                        s32 ret;
                        s32 filesize;
                        p = Ql_strstr(pData,"=");
                        p += 1;
                        ret = Ql_FileDelete((u8*)p );
                        Ql_sprintf(textBuf,"Ql_FileDelete(%s)=%d\r\n", p, ret);
                        Ql_SendToUart(ql_uart_port1,(u8*)textBuf,Ql_strlen(textBuf));    
                        break;
                    }

                    /*cmd: Ql_FileRename=oldname,newname*/
                    p = Ql_strstr(pData,"Ql_FileRename=");
                    if (p)
                    {
                        char oldName[255] = {'\0'};
                        char newName[255] = {'\0'};
                        s32 ret;
                        char* p2;
                        p2 = Ql_strstr(pData, ",");
                        if (NULL == p2)
                        {
                            Ql_DebugTrace("Input error parameter\r\n");
                            break;
                        }
                        p = Ql_strstr(pData, "=");
                        Ql_strncpy(oldName, p + 1, p2 - p - 1);
                        p = p2;
                        p2 = Ql_strstr(pData, "\r\n");
						if(NULL == p2)
						{
							OUT_DEBUG(textBuf, "\r\nPlease end with \\r\\n!\r\n");
							break;
						}
                        Ql_strncpy(newName, p + 1, p2 - p - 1);
						OUT_DEBUG(textBuf,"\r\nOldName:%s, NewName:%s\r\n", oldName, newName);
                        ret = Ql_FileRename((u8*)oldName, (u8*)newName);
                        Ql_sprintf(textBuf, "Ql_FileRename(\"%s\",\"%s\")=%d\r\n", oldName, newName, ret);
                        Ql_SendToUart(ql_uart_port1, (u8*)textBuf, Ql_strlen(textBuf));
                        break;
                    }

                    /*cmd:  Ql_FileCheck=Dir\File1.txt*/
                    p = Ql_strstr(pData,"Ql_FileCheck=");
                    if (p)
                    {
                        s32 ret;
                        s32 filesize;
                        p = Ql_strstr(pData,"=");
                        p += 1;
                        ret = Ql_FileCheck((u8*)p );
                        Ql_sprintf(textBuf,"Ql_FileCheck(%s)=%d\r\n", p, ret);
                        Ql_SendToUart(ql_uart_port1,(u8*)textBuf,Ql_strlen(textBuf));    
                        break;
                    }

                    /*cmd:  Ql_FileCreateDir=Dir1\Dir2*/
                    p = Ql_strstr(pData,"Ql_FileCreateDir=");
                    if (p)
                    {
                        s32 ret;
                        s32 filesize;
                        p = Ql_strstr(pData,"=");
                        p += 1;
                        ret = Ql_FileCreateDir((u8*)p );
                        Ql_sprintf(textBuf,"Ql_FileCreateDir(%s)=%d\r\n", p, ret);
                        Ql_SendToUart(ql_uart_port1,(u8*)textBuf,Ql_strlen(textBuf));    
                        break;
                    }

                    /*cmd:  Ql_FileRemoveDir=Dir1\Dir2*/
                    p = Ql_strstr(pData,"Ql_FileRemoveDir=");
                    if (p)
                    {
                        s32 ret;
                        s32 filesize;
                        p = Ql_strstr(pData,"=");
                        p += 1;
                        ret = Ql_FileRemoveDir((u8*)p );
                        Ql_sprintf(textBuf,"Ql_FileRemoveDir(%s)=%d\r\n", p, ret);
                        Ql_SendToUart(ql_uart_port1,(u8*)textBuf,Ql_strlen(textBuf));    
                        break;
                    }

                    /*cmd:  Ql_FileCheckDir=Dir1\Dir2*/
                    p = Ql_strstr(pData,"Ql_FileCheckDir=");
                    if (p)
                    {
                        s32 ret;
                        s32 filesize;
                        p = Ql_strstr(pData,"=");
                        p += 1;
                        ret = Ql_FileCheckDir((u8*)p );
                        Ql_sprintf(textBuf,"Ql_FileCheckDir(%s)=%d\r\n", p, ret);
                        Ql_SendToUart(ql_uart_port1,(u8*)textBuf,Ql_strlen(textBuf));    
                        break;
                    }

                    /*cmd:  Ql_FileFindFirst=Dir1\Dir2\*.txt    */
                    /*cmd:  Ql_FileFindFirst=Dir1\Dir2\*    */
                    p = Ql_strstr(pData,"Ql_FileFindFirst=");
                    if (p)
                    {
                        s32 ret;
                        u32 filesize;
                        bool isdir;
                        s32 findfile;
                        p = Ql_strstr(pData,"=");
                        p += 1;
						Ql_DebugTrace("\r\nbefore:strBuf=[%s]",strBuf);
                        findfile = Ql_FileFindFirst((u8*)p, (u8*)strBuf, 100, &filesize, &isdir);
						Ql_DebugTrace("\r\nLater:strBuf=[%s]",strBuf);
                        if(findfile < 0)
                        {
                            Ql_sprintf(textBuf,"Failed Ql_FileFindFirst(%s)=%d\r\n", p, findfile);
                            Ql_SendToUart(ql_uart_port1,(u8*)textBuf,Ql_strlen(textBuf));    
                            break;
                        }
                        else
                        {
                            Ql_sprintf(textBuf,"Sueecss Ql_FileFindFirst(%s)\r\n", p);
                            Ql_SendToUart(ql_uart_port1,(u8*)textBuf,Ql_strlen(textBuf));    
                        }
                        ret = findfile;
                        while(ret >= 0)
                        {
                            Ql_sprintf(textBuf,"filesize(%d),isdir(%d),Name(%s)\r\n", filesize, isdir, strBuf);
                            Ql_SendToUart(ql_uart_port1,(u8*)textBuf,Ql_strlen(textBuf));    
                            ret = Ql_FileFindNext(findfile, (u8*)strBuf, 100, &filesize, &isdir);
							Ql_DebugTrace("\r\n ql_FileFineNext Res = %d \r\n", ret);
                            if(ret != QL_RET_OK)
                                break;
                        }
                        Ql_FileFindClose(findfile);
                        break;
                    }

                    /*cmd:  Ql_FileXDelete=Dir1\Dir2#flag*/
                    p = Ql_strstr(pData,"Ql_FileXDelete=");
                    if (p)
                    {
                        s32 ret;
						u32 uiFlag = 0;
                        char *pflag;
						
                        p = Ql_strstr(pData,"=");
                        p += 1;
                        pflag = Ql_strstr(p,"#");
                        pflag[0] = '\0';
                        pflag++;
						Ql_memset(s_s8TmpBuf, 0, sizeof(s_s8TmpBuf));
						Ql_strcpy((char *)s_s8TmpBuf, p);

						if(NULL != Ql_strstr(pflag, "FILE"))
						{
							uiFlag |= QL_FS_FILE_TYPE;
							OUT_DEBUG(textBuf, "\r\nQl_FileXDelete Flag:QL_FS_FILE_TYPE");
						}

						if(NULL != Ql_strstr(pflag, "DIR"))
						{
							uiFlag |= QL_FS_DIR_TYPE;
							OUT_DEBUG(textBuf, "\r\nQl_FileXDelete Flag:QL_FS_DIR_TYPE");
						}

						if(NULL != Ql_strstr(pflag, "REC"))
						{
							uiFlag |= QL_FS_FILE_TYPE |QL_FS_DIR_TYPE|QL_FS_RECURSIVE_TYPE;
							OUT_DEBUG(textBuf, "\r\nQl_FileXDelete Flag:QL_FS_RECURSIVE_TYPE");
						}

						if(uiFlag == 0)
						{
							OUT_DEBUG(textBuf, "\r\nBAD Format\r\n");
							return ;
						}

						Ql_DebugTrace("\r\nQl_FileXDelete uiFlag = %d\r\n", uiFlag);
						
						OUT_DEBUG(textBuf, "\r\nQl_FileXDelete is excuted!\r\n"); 
                        ret = Ql_FileXDelete((u8*)p , uiFlag);
                        Ql_sprintf(textBuf,"Ql_FileXDelete(%s,%x)=%d\r\n", p, uiFlag, ret);
                        Ql_SendToUart(ql_uart_port1, (u8*)textBuf,Ql_strlen(textBuf));    
                        break;
                    }

                    /*cmd:  Ql_FileXMove=Dir1\Dir2,Dir1\Dir2,flag*/
                    p = Ql_strstr(pData,"Ql_FileXMove=");
                    if (p)
                    {
                        s32 ret;
                        char *p2;
                        char *pflag;
						u32 uiFlag = 0;


                        p = Ql_strstr(pData,"=");
                        p += 1;
                        p2 = Ql_strstr(p ,",");
                        p2[0] = '\0';
                        p2 += 1;
                        pflag = Ql_strstr(p2,",");
                        pflag[0] = '\0';
                        pflag += 1;

						if(pflag[0] == '1')
						{
							uiFlag = QL_FS_MOVE_COPY;
						}
						else if(pflag[0] == '2')
						{
							uiFlag = QL_FS_MOVE_KILL;
						}
						else if(pflag[0] == '3')
						{
							uiFlag = QL_FS_MOVE_OVERWRITE;
						}
						else
						{
							OUT_DEBUG(textBuf, "\r\nBad Format!\r\n");
							return ;
						}

						Ql_DebugTrace("\r\nQl_FileXMove uiFlag = %d\r\n", uiFlag);
						
                        ret = Ql_FileXMove((u8*)p,(u8*)p2, uiFlag);
                        Ql_sprintf(textBuf,"Ql_FileXMove(%s.%s,%x)=%d\r\n", p, p2, uiFlag, ret);
                        Ql_SendToUart(ql_uart_port1,(u8*)textBuf,Ql_strlen(textBuf));    
                        break;
                    }

					/*cmd:  Ql_DownLoadFile=dir1\dir2\password.txt*/
                    p = Ql_strstr(pData,"Ql_DownLoadFile=");
                    if (p)
                    {
                        s32 ret;
						s32 s32Res = -1;
						u32 uiFlag = 0;
						u32 i=0;
						char stmpdir[100],dir[100];
						char *p1[100];
				
						p = Ql_strstr(pData, "=");
						(*p) = '\0';
						p++;
						Ql_memset(dir,0,100);
						Ql_strcpy(stmpdir,p);
						p1[0]=stmpdir;
						while(p1[i+1]=Ql_strstr(p1[i],"\\"))
						{
							*p1[i+1] = '\0';
							p1[i+1]++;

							Ql_strcat(dir,p1[i]);
							p1[i]=p1[i+1];
                        	i++;
							
							s32Res = Ql_FileCheckDir((u8 *)dir);
							if(s32Res != QL_RET_OK)
							{
								Ql_memset(textBuf,0,100);
								Ql_sprintf(textBuf,"\r\n Dir(%s) is not exist, creating....\r\n", dir);
                        		Ql_SendToUart(ql_uart_port1,(u8*)textBuf,Ql_strlen(textBuf));
								
								s32Res = Ql_FileCreateDir((u8 *)dir);
								if(s32Res != QL_RET_OK)
								{
									Ql_memset(textBuf,0,100);
									Ql_sprintf(textBuf,"\r\n Create Dir(%s) failed!\r\n", dir);
                        			Ql_SendToUart(ql_uart_port1,(u8*)textBuf,Ql_strlen(textBuf));
									break;
								}
								else
								{
									Ql_memset(textBuf,0,100);
									Ql_sprintf(textBuf,"\r\n CreateDir(%s) OK!\r\n", dir);
                        			Ql_SendToUart(ql_uart_port1,(u8*)textBuf,Ql_strlen(textBuf));
								}
							}	
							else
							{
								Ql_memset(textBuf,0,100);
								Ql_sprintf(textBuf,"\r\nDir(%s) has existed!", dir);
                        		Ql_SendToUart(ql_uart_port1,(u8*)textBuf,Ql_strlen(textBuf));
							}
							
							Ql_strcat(dir,"\\");//Realize the joining together of dir

						}
						
						Ql_memset(s_s8TmpBuf, 0, sizeof(s_s8TmpBuf));  //File Name
						Ql_strcpy((char *)s_s8TmpBuf, p);

						uiFlag |= QL_FS_READ_WRITE;
						uiFlag |= QL_FS_CREATE;

						flag = 1;

 						Ql_DebugTrace("\r\nQl_DownLoadFile(%s, %d)\r\n", s_s8TmpBuf, uiFlag);
                        ret = Ql_FileOpenEx((u8*)s_s8TmpBuf, uiFlag);
                        if(ret >= QL_RET_OK)
                        {
                            filehandle = ret;
                        }
						Ql_memset(textBuf,0,100);
                        Ql_sprintf(textBuf,"\r\nQl_DownLoadFile (%s,%08x)=%d\r\n", s_s8TmpBuf, uiFlag, ret);
                        Ql_SendToUart(ql_uart_port1,(u8*)textBuf,Ql_strlen(textBuf));

						break;
					}
					
					//Receiving DownLoadFile and Ql_StopLoadFile
					if (flag)
					{
					
						s32 ret;
						u32 writeedlen;
						p = Ql_strstr(pData,"Ql_FinishLoadFile");
						if (p)
						{
							Ql_FileClose(filehandle);
							filehandle = -1;
							flag =0;
							Ql_sprintf(textBuf,"Ql_FinishLoadFile()\r\n");
							Ql_SendToUart(ql_uart_port1,(u8*)textBuf,Ql_strlen(textBuf));	 
							break;
						}
						ret = Ql_FileWrite(filehandle, (u8*)pData, plen, &writeedlen);
						Ql_memset(textBuf,0,100);
						Ql_sprintf(textBuf,"Ql_File()=%d: writeedlen=%d,Download times=%d\r\n",ret, writeedlen,flag++);
						Ql_SendToUart(ql_uart_port1,(u8*)textBuf,Ql_strlen(textBuf));
					}
                }
                break;            
            }
            
            default:
                break;
        }
    }
}
#endif // __EXAMPLE_FILESYSTEM__


