#ifdef __EXAMPLE_RECORD__
/***************************************************************************************************
*   Example:
*       
*           Record Routine
*
*   Description:
*
*           This example demonstrates how to use  record function with APIs in OpenCPU.
*           Through MAIN Uart port, input the specified command, there will be given the 
*           response about record operation.
*
*   Usage:
*
*           Compile & Run:
*
*               Use "make record" to compile, and download bin image to module to run.
*           
*           Operation: (Through MAIN port)
*               If input "StreamRecordStart=n"  n= 0,1,2,3 .n is the record data format,you can refer 'QlStreamFromat' enum.
*		    If input "StreamRecordStop" stop record.
*
*   Note: 
*		When test playing audio file in this example, DO NOT quote file name between quotation marks("").
*
****************************************************************************************************/ 

#include "ql_appinit.h"
#include "ql_interface.h"
#include "ql_type.h"
#include "ql_trace.h"
#include "ql_audio.h"
#include "ql_stdlib.h"
#include "ql_filesystem.h"
#include "ql_error.h"
#include "ql_fcm.h"

#define RECORD_DATA_FILE  "RecordData" // file name ,the file is create in UFS. 
char textBuf[100];// for debug trace
char RecordBuffer[12000];// the buffer used to store the record data,The buffer you can't write or read it , it is controlled by the driver task. 
u8 tempbuffer[6500];//temp buffer, 
s32 Record_filehd = -1;//file handler, the file used to store the record data.

void StreamRecord_data_callbck(u8 * pData, u32 dataLen) // pData is not used
{
	s32 ret,readLen,writenLen=0;
  //  Ql_DebugTrace("audio example %s(%d) pdata=%x, datalen=%d,pdata[11]=%x,pdata[21]=%x\r\n", __func__,__LINE__,pdata,dataLen,*(pdata+11),*(pdata+21));   

	readLen = Ql_StreamRecordReadBuffer(tempbuffer,dataLen);
  	ret = Ql_FileWrite(Record_filehd, tempbuffer, dataLen,&writenLen);
	Ql_sprintf(textBuf,"Ql_FileWrite return value=%d, read data(%d),save to file writenLen=%d\r\n\0", ret,readLen,writenLen);
	Ql_SendToUart(ql_uart_port1, (u8*)textBuf,Ql_strlen(textBuf));
}

void ql_entry()
{
    bool           keepGoing = TRUE;
    QlEventBuffer  flSignalBuffer;
    char *p=NULL;
    char *pData= NULL;
    s32 ret = 0;
    u8 format;

    Ql_SetDebugMode(BASIC_MODE);   
    Ql_sprintf(textBuf,"audio: ql_entry\r\n\0");
    Ql_SendToUart(ql_uart_port1, (u8*)textBuf,Ql_strlen(textBuf));
    Ql_OpenModemPort(ql_md_port1);       
	
    while(keepGoing)
    {    
        Ql_GetEvent(&flSignalBuffer);
        switch(flSignalBuffer.eventType)
        {
            case EVENT_UARTDATA:
            {
                char* pChar = NULL;
                PortData_Event* pDataEvt = NULL;
                if (flSignalBuffer.eventData.uartdata_evt.len <= 0)
                {
                    break;
                }
                pDataEvt = (PortData_Event*)&flSignalBuffer.eventData.uartdata_evt;
                pData = (char*)flSignalBuffer.eventData.uartdata_evt.data;
                pData[flSignalBuffer.eventData.uartdata_evt.len+1] = '\0';

		   // command : "StreamRecordStart=n"  (n= 0,1,2,3  the record data format) 
                p = Ql_strstr(pData,"StreamRecordStart=");
                if (p)// record command ,start to record 
                {	
                    p = Ql_strstr(pData,"=");
                    p +=1;
                    format =Ql_atoi(p);// record data format , you can refer 'QlStreamFromat' enum
					
                    ret = Ql_VoiceCallChangePath(QL_AUDIO_PATH_HEADSET);// set the record path.
			if(ret != QL_RET_OK)
			{
				Ql_DebugTrace("Ql_VoiceCallChangePath failed ! ret=%d\r\n", ret);
			    	Ql_sprintf(textBuf,"Ql_VoiceCallChangePath failed ! ret=%d\r\n\0", ret);
    				Ql_SendToUart(ql_uart_port1, (u8*)textBuf,Ql_strlen(textBuf));
			}
			Ql_memset(RecordBuffer, 0x00, sizeof(RecordBuffer));
			ret = Ql_StreamRecordSetBuffer(RecordBuffer, sizeof(RecordBuffer));// set the record data buffer.
			if(ret != QL_RET_OK)
			{
			    	Ql_sprintf(textBuf,"Ql_StreamRecordSetbuffer failed ! ret=%d\r\n\0", ret);
    				Ql_SendToUart(ql_uart_port1, (u8*)textBuf,Ql_strlen(textBuf));
			}
			ret = Ql_StreamRecordStart(format,StreamRecord_data_callbck);// start to record
			if(ret != QL_RET_OK)
			{
			    	Ql_sprintf(textBuf,"Ql_StreamRecordStart failed ! ret=%d  \r\n\0", ret);
    				Ql_SendToUart(ql_uart_port1, (u8*)textBuf,Ql_strlen(textBuf));				
			}			
		    	Ql_sprintf(textBuf,"Ql_StreamRecordStart (format=%d) ret=%d  \r\n\0",format, ret);
			Ql_SendToUart(ql_uart_port1, (u8*)textBuf,Ql_strlen(textBuf));

			Record_filehd =Ql_FileOpenEx(RECORD_DATA_FILE, (QL_FS_READ_WRITE|QL_FS_CREATE));
			if(Record_filehd < 0)
			{
			    	Ql_sprintf(textBuf,"\r\n Record file Create (%s) failed!!!!ret= %d \r\n\0", RECORD_DATA_FILE,Record_filehd);
				Ql_SendToUart(ql_uart_port1, (u8*)textBuf,Ql_strlen(textBuf));
			}
			else
			{
				Ql_sprintf(textBuf,"\r\n Create File(%s)  OK!Record_filehd=%d\r\n", RECORD_DATA_FILE,Record_filehd);
				Ql_SendToUart(ql_uart_port1, (u8*)textBuf,Ql_strlen(textBuf));
			}	

			break;
		    }	

		   // command "StreamRecordStop" to stop the record.
		   p = Ql_strstr(pData,"StreamRecordStop");
                if (p)
                {				
			ret = Ql_StreamRecordStop();
			if(ret != QL_RET_OK)
			{
				Ql_sprintf(textBuf,"Ql_StreamRecordStop failed ! ret=%d\r\n", ret);
				Ql_SendToUart(ql_uart_port1, (u8*)textBuf,Ql_strlen(textBuf));
			}			
			Ql_sprintf(textBuf,"Stream Record stop!!\r\n");
			Ql_SendToUart(ql_uart_port1, (u8*)textBuf,Ql_strlen(textBuf));		
			
			Ql_FileClose(Record_filehd);
			Ql_sprintf(textBuf," Record_filehd =%d  Ql_FileClose !!!\r\n", Record_filehd);
			Ql_SendToUart(ql_uart_port1, (u8*)textBuf,Ql_strlen(textBuf));
			Record_filehd =-1;

			break;
		    }	

                // transpass
                Ql_SendToModem(ql_md_port1, (u8*)pData, flSignalBuffer.eventData.uartdata_evt.len);
                break;
            }

            default:
                break;
        }
    }
}

#endif //


