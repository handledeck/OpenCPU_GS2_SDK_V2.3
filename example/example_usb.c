 /***************************************************************************************************
*   Example:
*            USB to virtual serial port
*
*   Description:
*           
*           This example gives an example for USB to virtual serial port operation.
*           It Include pc transfer data to the app via virtual serial port,And app throgh
*           virtual serial port send data to PC.
*
*   Notes:
*           1.Config the configuration structure Customer_user_qlconfig.QlUsbMode=USB_AS_VCOM;
*           2.When there some data incoming via virtual serial port ,app will be received
*             a "EVENT_UARTDATA" event ,And then the app must invoke Ql_UartDirectnessReadData() 
*             function to read the data Immediately.
*   Usage:
*           
*       Compile & Run:
*           Config the configuration structure Customer_user_qlconfig.QlUsbMode=USB_AS_VCOM;
*           Use "make usb" to compile ,and download bin image to module to run    
*
*   Operation:
*           1.If send data to "ql_usb_com_port1" port ,then the same data will be print out via "ql_uart_port1"
*           2.If send data to "ql_usb_com_port2" port ,then the same data will be print out via "ql_uart_port2" 
*           3.If input "Ql_DownfileUSB_COM1=XXX", that will open and automatically create a named file, this file will receive content from USB_COM1 when you want to download file .
*           4.If input "Ql_DownfileUSB_COM2=XXX", that will open and automatically create a named file, this file will receive content from USB_COM2 when you want to download file .
*           5.If input "Ql_FinishLoadFileUSB_COM1",  that will stop download file (via USB_COM1) when  receiving  complete.
*           6.If input "Ql_FinishLoadFileUSB_COM2",  that will stop download file (via USB_COM2) when  receiving  complete.
*           7.If input "USB_COM1fileToPC", app will send a file out via USB_COM1.(must save a file in SD card and the file is named 1MUSB_COM1.txt) 
*           7.If input "USB_COM2fileToPC",app will send a file out via USB_COM2.(must save a file in SD card and the file is named 1MUSB_COM2.txt )
*           
****************************************************************************************************/
#ifdef __EXAMPLE_USB__
  
#include "ql_appinit.h"
#include "ql_interface.h"
#include "ql_filesystem.h"
#include "ql_type.h"
#include "ql_trace.h"
#include "ql_audio.h"
#include "ql_timer.h"
#include "ql_stdlib.h"
#include "ql_pin.h"
#include "ql_fcm.h"
#include "ql_error.h"
#include "ql_type.h"
#include "ql_interface.h"
s32 USB_COM1_filehandle=-1;
s32 USB_COM2_filehandle=-1;
s32 USB_COM1ToPCfilehandle = -1;
s32 USB_COM2ToPCfilehandle = -1;
QlTimer timerforUSB_COM1,timerforUSB_COM2;

s8 FileName[1024];
char ReadfilebufferUSB_COM1[1024];
char ReadfilebufferUSB_COM2[1024];
char Readbuffer[1024*1];
QlEventBuffer  g_event;

void ql_entry()
{
    bool           keepGoing = TRUE;
    s32 period = 10;
    char buffer[100];

    s32  Readlength=0;
    char *pData, *p, *p3,*p2;
    s32 iret;
    bool flag;

    timerforUSB_COM1.timerId =1;
    timerforUSB_COM1.timeoutPeriod =Ql_MillisecondToTicks(600);
    timerforUSB_COM2.timerId =2;
    timerforUSB_COM2.timeoutPeriod =Ql_MillisecondToTicks(800);
    
    Ql_SetDebugMode(BASIC_MODE);   
    Ql_DebugTrace("USB_COM: ql_entry\r\n");
    Ql_OpenModemPort(ql_md_port1);


    while(keepGoing)
    {
        Ql_GetEvent(&g_event);
        {
            switch(g_event.eventType)
            {
                case EVENT_UARTDATA:
                {
                    if(ql_usb_com_port1 == g_event.eventData.uartdata_evt.port)
                    {
                        s32 writeedlen;
                        s32 ret,TotalReadlength;
                        Readlength = 0;
                        TotalReadlength =0;
                        Ql_memset(Readbuffer,0x00, sizeof(Readbuffer)); 
                        do {
                                Readlength = Ql_UartDirectnessReadData(ql_usb_com_port1,Readbuffer + TotalReadlength,1024);
                                TotalReadlength += Readlength;
                        }
                        while (Readlength != 0 && 1024 == Readlength);
                        if(-1 != USB_COM1_filehandle)
                        {
    						ret = Ql_FileWrite(USB_COM1_filehandle, (u8*)Readbuffer, Readlength, &writeedlen);
                        }
                        else
                        {
    						Ql_memset(buffer,0,100);
    						Ql_sprintf(buffer,"%s\r\n",Readbuffer);
    						Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));
                        }
                        break;
                    }
                    else if(ql_usb_com_port2 == g_event.eventData.uartdata_evt.port)
                    {
                        s32 writeedlen;
                        s32 ret,TotalReadlength;
                        Readlength = 0;
                        TotalReadlength =0;
                        Ql_memset(Readbuffer,0x00, sizeof(Readbuffer)); 
                        do {
                                Readlength = Ql_UartDirectnessReadData(ql_usb_com_port2,Readbuffer + TotalReadlength,1024);
                                TotalReadlength += Readlength;
                        }
                        while (Readlength != 0 && 1024 == Readlength);
                        if(-1 != USB_COM2_filehandle)
                        {
    						ret = Ql_FileWrite(USB_COM2_filehandle, (u8*)Readbuffer, Readlength, &writeedlen);
                        }
                        else
                        {
                            Ql_memset(buffer,0,100);
    						Ql_sprintf(buffer,"%s\r\n",Readbuffer);
    						Ql_SendToUart(ql_uart_port2,(u8*)buffer,Ql_strlen(buffer));
                        }
                        break;
                    } 
                    else  // the UART EVENT from other there UART port
                    {
                        p = NULL;
                        pData = (char*)g_event.eventData.uartdata_evt.data; 
                        p = Ql_strstr(pData,"USB_MS");
                        if(p)
                        {
                            Ql_DebugTrace("USB to MS mode\r\n");
                            Ql_ChangeUsbMode(USB_AS_MS);
                        //    Ql_Sleep(20);
                            Ql_Reset(0);
                            break;
                        }
                        p = Ql_strstr(pData,"USB_VCOM");
                        if(p)
                        {
                            Ql_DebugTrace("USB to VCOM mode\r\n");
                            Ql_ChangeUsbMode(USB_AS_VCOM);
                        //     Ql_Sleep(20);
                            Ql_Reset(0);
                            break;
                        }
        				/*cmd:  Ql_DownfileUSB_COM1=dir1\dir2\password.txt*/
                        p3 = Ql_strstr(pData,"Ql_DownfileUSB_COM1=");
                        p2 = Ql_strstr(pData,"Ql_DownfileUSB_COM2=");
                        if (p3 || p2)
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
        							Ql_memset(buffer,0,100);
        							Ql_sprintf(buffer,"\r\n Dir(%s) is not exist, creating....\r\n", dir);
                            		Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));
        							
        							s32Res = Ql_FileCreateDir((u8 *)dir);
        							if(s32Res != QL_RET_OK)
        							{
        								Ql_memset(buffer,0,100);
        								Ql_sprintf(buffer,"\r\n Create Dir(%s) failed!\r\n", dir);
                            			Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));
        								break;
        							}
        							else
        							{
        								Ql_memset(buffer,0,100);
        								Ql_sprintf(buffer,"\r\n CreateDir(%s) OK!\r\n", dir);
                            			Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));
        							}
        						}	
        						else
        						{
        							Ql_memset(buffer,0,100);
        							Ql_sprintf(buffer,"\r\nDir(%s) has existed!", dir);
                            		Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));
        						}
        						
        						Ql_strcat(dir,"\\");//Realize the joining together of dir

        					}
        					
        					Ql_memset(FileName, 0, sizeof(FileName));  //File Name
        					Ql_strcpy((char *)FileName, p);

        					uiFlag |= QL_FS_READ_WRITE;
        					uiFlag |= QL_FS_CREATE;

        					flag = 1;

        				    Ql_DebugTrace("\r\nQl_DownLoadFile(%s, %d)\r\n", FileName, uiFlag);
                            ret = Ql_FileOpenEx((u8*)FileName, uiFlag);
                            if(ret >= QL_RET_OK)
                            {
                                if(p3)
                                {
                                    USB_COM1_filehandle = ret;
                                    Ql_memset(buffer,0,100);
                                    Ql_sprintf(buffer,"\r\nUSB_COM1 (%s,%08x)=%d\r\n", FileName, uiFlag, ret);
                                    Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));
                                }
                                if(p2)
                                {
                                    USB_COM2_filehandle = ret;
                                    Ql_memset(buffer,0,100);
                                    Ql_sprintf(buffer,"\r\nUSB_COM2(%s,%08x)=%d\r\n", FileName, uiFlag, ret);
                                    Ql_SendToUart(ql_uart_port2,(u8*)buffer,Ql_strlen(buffer));
                                }

                                p3 =NULL;
                                p2 =NULL;

                            }

        					break;
        				}
						p = Ql_strstr(pData,"Ql_FinishLoadFileUSB_COM1");
						if (p)
						{
							Ql_FileClose(USB_COM1_filehandle);
							USB_COM1_filehandle = -1;
							Ql_sprintf(buffer,"Ql_FinishLoadFileUSB_COM1()\r\n");
							Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));	 
							break;
						}
                        p = Ql_strstr(pData,"Ql_FinishLoadFileUSB_COM2");
						if (p)
						{
							Ql_FileClose(USB_COM2_filehandle);
							USB_COM2_filehandle = -1;
							Ql_sprintf(buffer,"Ql_FinishLoadFileUSB_COM2()\r\n");
							Ql_SendToUart(ql_uart_port2,(u8*)buffer,Ql_strlen(buffer));	 
							break;
						}
                        p = Ql_strstr(pData,"USB_COM1fileToPC");
						if (p)
						{
                            
                            USB_COM1ToPCfilehandle = Ql_FileOpenEx("SD:1MUSB_COM1.txt", QL_FS_READ_ONLY);
                            if(USB_COM1ToPCfilehandle >= 0)
                            {
                                Ql_StartTimer(&timerforUSB_COM1);
                                Ql_sprintf(buffer,"\r\nUSB_COM1 start transmit file to PC\r\n");
							    Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));	                                 
                            }
                            else
                            {
                                Ql_sprintf(buffer,"\r\nUSB_COM1 transmit there something error =%d\r\n",USB_COM1ToPCfilehandle);
							    Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));	
                            }
							break;
						}
                        p = Ql_strstr(pData,"USB_COM2fileToPC");
						if (p)
						{
                            USB_COM2ToPCfilehandle = Ql_FileOpenEx("SD:1MUSB_COM2.txt", QL_FS_READ_ONLY);
                            if(USB_COM2ToPCfilehandle >= 0)
                            {
                                Ql_StartTimer(&timerforUSB_COM2);
                                Ql_sprintf(buffer,"\r\nUSB_COM2 start transmit file to PC\r\n");
							    Ql_SendToUart(ql_uart_port2,(u8*)buffer,Ql_strlen(buffer));	 
                            }
                            else
                            {
                                Ql_sprintf(buffer,"\r\nUSB_COM1 transmit there something error =%d!!!\r\n",USB_COM1ToPCfilehandle);
							    Ql_SendToUart(ql_uart_port2,(u8*)buffer,Ql_strlen(buffer));	
                            }
							break;
						}
                        
                    break;
                    }
                }
                case EVENT_TIMER:
                {
                    if( timerforUSB_COM1.timerId ==g_event.eventData.timer_evt.timer_id)
                    {
                        s32 readedlen =0;
                        Ql_memset(ReadfilebufferUSB_COM1, 0, sizeof(ReadfilebufferUSB_COM1)); 
                        Ql_FileRead(USB_COM1ToPCfilehandle, ReadfilebufferUSB_COM1, sizeof(ReadfilebufferUSB_COM1), &readedlen);
                        if(readedlen <= 0)
                        {
                            Ql_sprintf(buffer,"\r\nUSB_COM1 send file end!!\r\n");
						    Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));	                                                             
                        }
                        else
                        {
						    Ql_SendToUart(ql_usb_com_port1,(u8*)ReadfilebufferUSB_COM1,readedlen);	 
                            Ql_StartTimer(&timerforUSB_COM1);
                        }
                        
                    }
                    if( timerforUSB_COM2.timerId ==g_event.eventData.timer_evt.timer_id)
                    {
                        s32 readedlen =0;
                        Ql_memset(ReadfilebufferUSB_COM2, 0, sizeof(ReadfilebufferUSB_COM2)); 
                        Ql_FileRead(USB_COM2ToPCfilehandle, ReadfilebufferUSB_COM2, sizeof(ReadfilebufferUSB_COM2), &readedlen);
                        if(readedlen <=0)
                        {
                            Ql_sprintf(buffer,"\r\nUSB_COM2 send file end\r\n");
						    Ql_SendToUart(ql_uart_port2,(u8*)buffer,Ql_strlen(buffer));	 
                        }
                        else
                        {
        					Ql_SendToUart(ql_usb_com_port2,(u8*)ReadfilebufferUSB_COM2,readedlen);	 
                            Ql_StartTimer(&timerforUSB_COM2);
                        }

                    }
                    break;
                }
                case EVENT_MODEMDATA:
                {
                    break;
                }                
            }
        }
      }
}


#endif
