#ifdef __EXAMPLE_SMS__
 /***************************************************************************************************
 *	 Example:
 *		 
 *			 SMS Routine
 *
 *	 Description:
 *
 *			 This example demonstrates how to use sms function with APIs in OpenCPU.
 *			 Through MAIN Uart port, input the specified command, and the response message will be 
 *			 printed out through MAIN port.
 *
 *	 Usage:
 *
 *			 Compile & Run:
 *
 *				 Use "make SMS" to compile, and download bin image to module to run.
 *			 
 *			 Operation: (Through MAIN port)
 *                   If input "Ql_SetSMSStorage=(0-3)", that will set SMS storages.
 *                   If input "Ql_GetSMSStorageInfo", that will get SMS storages info.
 *                   If input "Ql_SetNewSMSDirectToTE=(0,1)", that will get SMS storages info.
 *                   If input "Ql_SetInfoCentreNum="<sca number>"", that will set SMS service centre number.
 *                   If input "Ql_GetInfoCentreNum", that will get SMS service centre number.
 *                   If input "Ql_SetNewSMSDirectToTE=(0,1)", that will choose new messages save in storage or not. 
 *                   If input "Ql_SetSMSFormat=(0,1)", that will set the format of SMS, PDU or text.
 *                   If input "Ql_ReadSMS=(1,300),(0,1)", that will read an indexed message.
 *                   If input "Ql_DeleteSMS=(1,300),(0,4)", that will delete SMS messages in current storage.
 *                   If input "Ql_ListSMS=(0,4)", that will get message list in current storage by "QlSMSStatus".
 *                   If input "Ql_GetUnReadSMS=(0,1)", that will read first unread message.
 *                   If input "Ql_SendPDUSMS", that will send a message with PDU format.
 *                   If input "Ql_SetTextSMSPara=(0-2),(0,1),(0,1)", that will set some parameters in text mode.
 *                   If input "Ql_SendTextSMS="<phone number>"", that will send a message with text format.
 *			 
 ****************************************************************************************************/

#include "ql_appinit.h"
#include "ql_interface.h"
#include "ql_type.h"
#include "ql_trace.h"
#include "ql_error.h"
#include "ql_timer.h"
#include "ql_stdlib.h"
#include "ql_sms.h"
#include "ql_fcm.h"
#include "ql_api_type.h"
#include "ql_tcpip.h"



#define MAX_LIST_NODE_COUNT     100

#define DEBUG_BUFFER_SIZE   100

#define OUT_DEBUG(x,...)  \
    Ql_memset((void*)(x),0,DEBUG_BUFFER_SIZE);  \
    Ql_sprintf((char*)(x),__VA_ARGS__);   \
    Ql_SendToUart(ql_uart_port1,(u8*)(x),Ql_strlen((const char*)(x)));

void CallBack_NewSMS(u16 index,QlSMSStorage storage);
void CallBack_SendSMS(bool result, s16 cause, u8 msg_ref);
void CallBack_DeleteSMS(bool result, s16 cause,u16 index);
void CallBack_ReadPDUSMS(bool result, s16 cause,u16 index, u8 status, u8* data, u16 length);
void CallBack_NewFlashPDUSMS(u16 length, u8* pdu_string);
void CallBack_PDUStatusReport(u16 length, u8* pdu_string);
void CallBack_ReadTextSMS(bool result, s16 cause,u16 index, u8 status, QlSMSTextMsg* sms);
void CallBack_NewFlashTextSMS(QlSMSTextMsg* sms);
void CallBack_TextStatusReport(u8 fo,u8 msg_ref, u8* phone_num, QlSysTimer* scts, QlSysTimer* dt, u8 st);



char debug_buffer[DEBUG_BUFFER_SIZE];

QlEventBuffer g_event; // Keep this variable a global variable due to its big size


void ql_entry(void)
{
    bool keepGoing = TRUE;
    s32 ret;
    s32 i;
    char notes[50];
    
    OUT_DEBUG(debug_buffer,"sms: ql_entry\r\n");
    Ql_SetDebugMode(ADVANCE_MODE);
    Ql_OpenModemPort(ql_md_port1);
    Ql_SMS_Callback cb_func={CallBack_NewSMS,CallBack_SendSMS,CallBack_DeleteSMS,                 //public
                                       CallBack_ReadPDUSMS,CallBack_NewFlashPDUSMS,CallBack_PDUStatusReport,               //pdu
                                       CallBack_ReadTextSMS,CallBack_NewFlashTextSMS,CallBack_TextStatusReport};           //text
    ret=Ql_SMSInitialize(&cb_func);
    
    OUT_DEBUG(debug_buffer,"\r\nQl_SMSInitialize=%d\r\n",ret);
    //OUT_DEBUG(debug_buffer,"CallBack_ReadPDUSMS=%X\r\n",CallBack_ReadPDUSMS);
    while(keepGoing)
    {    
        Ql_GetEvent(&g_event);
        switch(g_event.eventType)
        {
            case EVENT_MODEMDATA:
            {
                //TODO: receive and hanle data from CORE through virtual modem port
                PortData_Event* pPortEvt = (PortData_Event*)&g_event.eventData.modemdata_evt;
                OUT_DEBUG(debug_buffer,"%s", pPortEvt->data);
                break;
            }
            
            case EVENT_UARTDATA:
            {
                char* pChar=NULL;
                char* pChar2=NULL;
                PortData_Event* pDataEvt = (PortData_Event*)&g_event.eventData.uartdata_evt;

                //Ql_SetSMSStorage=(0-3)
                Ql_memset(notes, 0x0, sizeof(notes));
                Ql_sprintf(notes, "Ql_SetSMSStorage=");
                pChar = Ql_strstr((char*)pDataEvt->data, notes);
                if (NULL != pChar)
                {
                    u8 storage;
                    pChar+=17;
                    storage=Ql_atoi(pChar);
                    ret=Ql_SetSMSStorage(storage);
                    OUT_DEBUG(debug_buffer,"\r\nQl_SetSMSStorage(storage=%d)=%d\r\n",storage,ret);
                    break;
                }
                
                //Ql_GetSMSStorageInfo
                Ql_memset(notes, 0x0, sizeof(notes));
                Ql_sprintf(notes, "Ql_GetSMSStorageInfo");
                pChar = Ql_strstr((char*)pDataEvt->data, notes);
                if (NULL != pChar)
                {
                    u8 current_mem,used,total;
                    ret=Ql_GetSMSStorageInfo(&current_mem, &used, &total);
                    OUT_DEBUG(debug_buffer,"\r\nQl_GetSMSStorageInfo(current_mem=%d,used=%d,total=%d)=%d\r\n",
                                                            current_mem,used,total,ret);
                    break;
                }

                //Ql_SetInfoCentreNum="<sca number>" eg:Ql_SetInfoCentreNum="+8613800551500"
                Ql_memset(notes, 0x0, sizeof(notes));
                Ql_sprintf(notes, "Ql_SetInfoCentreNum=\"");
                pChar = Ql_strstr((char*)pDataEvt->data, notes);
                if (NULL != pChar)
                {
                    pChar+=21;
                    pChar2 = Ql_strstr(pChar,"\"");
                    *pChar2=0; 
                    ret=Ql_SetInfoCentreNum((u8*)pChar);
                    OUT_DEBUG(debug_buffer,"\r\nQl_SetInfoCentreNum(sca_num=%s)=%d\r\n", pChar,ret);
                    break;
                }
                
                //Ql_GetInfoCentreNum
                Ql_memset(notes, 0x0, sizeof(notes));
                Ql_sprintf(notes, "Ql_GetInfoCentreNum");
                pChar = Ql_strstr((char*)pDataEvt->data, notes);
                if (NULL != pChar)
                {
                    u8 sca_number[40];
                    u8 type;
                    ret=Ql_GetInfoCentreNum(sca_number,&type);
                    OUT_DEBUG(debug_buffer,"\r\nQl_GetInfoCentreNum(sca_number=\"%s\",type=%d)=%d\r\n",
                                                            sca_number,type,ret);
                    break;
                }
                
                //Ql_SetNewSMSDirectToTE=(0,1)
                Ql_memset(notes, 0x0, sizeof(notes));
                Ql_sprintf(notes, "Ql_SetNewSMSDirectToTE=");
                pChar = Ql_strstr((char*)pDataEvt->data, notes);
                if (NULL != pChar)
                {
                    u8 op;
                    pChar+=23;
                    op=Ql_atoi(pChar);
                    ret=Ql_SetNewSMSDirectToTE(op);
                    OUT_DEBUG(debug_buffer,"\r\nQl_SetNewSMSDirectToTE(op=%d)=%d\r\n",op,ret);
                    break;
                }

                //Ql_SetSMSFormat=(0,1)
                Ql_memset(notes, 0x0, sizeof(notes));
                Ql_sprintf(notes, "Ql_SetSMSFormat=");
                pChar = Ql_strstr((char*)pDataEvt->data, notes);
                if (NULL != pChar)
                {
                    u8 format;
                    pChar+=16;
                    format=Ql_atoi(pChar);
                    ret=Ql_SetSMSFormat(format);
                    OUT_DEBUG(debug_buffer,"\r\nQl_SetSMSFormat(format=%d)=%d\r\n",format,ret);
                    break;
                }

                //Ql_ReadSMS=(1,300),(0,1)
                Ql_memset(notes, 0x0, sizeof(notes));
                Ql_sprintf(notes, "Ql_ReadSMS=");
                pChar = Ql_strstr((char*)pDataEvt->data, notes);
                if (NULL != pChar)
                {
                    u16 index;
                    u8 mode;
                    pChar+=11;
                    index=Ql_atoi(pChar);
                    pChar=Ql_strstr(pChar, ",");
                    pChar++;
                    mode=Ql_atoi(pChar);
                    ret=Ql_ReadSMS(index,mode);
                    OUT_DEBUG(debug_buffer,"\r\nQl_ReadSMS(index=%d,mode=%d)=%d\r\n",index,mode,ret);
                    break;
                }
                 
                //Ql_DeleteSMS=(1,300),(0,4)
                Ql_memset(notes, 0x0, sizeof(notes));
                Ql_sprintf(notes, "Ql_DeleteSMS=");
                pChar = Ql_strstr((char*)pDataEvt->data, notes);
                if (NULL != pChar)
                {
                    u16 index;
                    u8 flag;
                    pChar+=13;
                    index=Ql_atoi(pChar);
                    pChar=Ql_strstr(pChar, ",");
                    pChar++;
                    flag=Ql_atoi(pChar);
                    ret=Ql_DeleteSMS(index,flag);
                    OUT_DEBUG(debug_buffer,"\r\nQl_DeleteSMS(index=%d,flag=%d)=%d\r\n",index,flag,ret);
                    break;
                }

                //Ql_ListSMS=(0,4)
                Ql_memset(notes, 0x0, sizeof(notes));
                Ql_sprintf(notes, "Ql_ListSMS=");
                pChar = Ql_strstr((char*)pDataEvt->data, notes);
                if (NULL != pChar)
                {
                    u8 by_status;
                    u16 sms_count=0;                    
                    QlSMSListNode sms_list[MAX_LIST_NODE_COUNT];  //MAX_LIST_NODE_COUNT should be set large enough
                    pChar+=11;
                    by_status=Ql_atoi(pChar);
                    
                    ret=Ql_ListSMS(sms_list, MAX_LIST_NODE_COUNT, by_status, &sms_count);
                    OUT_DEBUG(debug_buffer,"\r\nQl_ListSMS(by_status=%d,node_count=%d,sms_count=%d)=%d\r\n",
                        by_status,MAX_LIST_NODE_COUNT,sms_count,ret);
                    if(0==ret)
                    {
                        for(i=0;i<sms_count;i++)
                        {
                            OUT_DEBUG(debug_buffer,"index=%d,status=%d\r\n",sms_list[i].index,sms_list[i].status);
                        }
                    }
                    break;
                }

                //Ql_GetUnReadSMS=(0,1)
                Ql_memset(notes, 0x0, sizeof(notes));
                Ql_sprintf(notes, "Ql_GetUnReadSMS=");
                pChar = Ql_strstr((char*)pDataEvt->data, notes);
                if (NULL != pChar)
                {
                    u8 mode;
                    pChar+=16;
                    mode=Ql_atoi(pChar);
                    ret=Ql_GetUnReadSMS(mode);
                    OUT_DEBUG(debug_buffer,"\r\nQl_GetUnReadSMS(mode=%d)=%d\r\n",mode,ret);
                    break;
                }
                
                //Ql_SendPDUSMS
                Ql_memset(notes, 0x0, sizeof(notes));
                Ql_sprintf(notes, "Ql_SendPDUSMS");
                pChar = Ql_strstr((char*)pDataEvt->data, notes);
                if (NULL != pChar)
                {
                    u8 pdu_len;
                    u8 pdu_string[]={0x00,       //Set aside for SCA
                                                 0x31,      //FO
                                                 0xff,      //Set aside for MR   
                                                 /*
                                                 0x05,      //DA Length
                                                 0x81,      //DA type
                                                 0x01,      
                                                 0x80,
                                                 0xf6,      //DA:10086
                                                 */
                                                 0x0b,     //DA Length
                                                 0x81,     //DA type
                                                 0x31,
                                                 0x58,
                                                 0x15,
                                                 0x64,
                                                 0x79,
                                                 0xf3,    //DA:13855146973
                                                 
                                                 0x00,      //PID
                                                 0x00,      //DCS
                                                 0x47,      //VP
                                                 0x07,      //UDH
                                                 0xd1,
                                                 0x7a,
                                                 0x79,
                                                 0x4c,
                                                 0x2f,
                                                 0xb3,
                                                 0x01      //UD:Quectel, GSM 7-bit coding
                                             };
                    pdu_len=sizeof(pdu_string);
                    
                    ret=Ql_SendPDUSMS(pdu_string,pdu_len);
                    OUT_DEBUG(debug_buffer,"\r\nQl_SendPDUSMS(pdu_len=%d)=%d\r\n",pdu_len,ret);
                    if(0==ret)
                    {
                        for(i=0;i<pdu_len;i++)
                        {
                            OUT_DEBUG(debug_buffer,"%02X",pdu_string[i]);
                        }
                        OUT_DEBUG(debug_buffer,"\r\n");
                    }
                    break;
                }
                           

                //Ql_SetTextSMSPara=(0-2),(0,1),(0,1)
                Ql_memset(notes, 0x0, sizeof(notes));
                Ql_sprintf(notes, "Ql_SetTextSMSPara=");
                pChar = Ql_strstr((char*)pDataEvt->data, notes);
                if (NULL != pChar)
                {
                    u8 chset,isflashsms,status_report;
                    pChar+=18;
                    chset=Ql_atoi(pChar);
                    pChar=Ql_strstr(pChar, ",");
                    pChar++;
                    isflashsms=Ql_atoi(pChar);
                    pChar=Ql_strstr(pChar, ",");
                    pChar++;
                    status_report=Ql_atoi(pChar);
                    
                    ret=Ql_SetTextSMSPara(chset, isflashsms, status_report);
                    OUT_DEBUG(debug_buffer,
                       "\r\nQl_SetTextSMSPara(chset=%d,isflashsms=%d,status_report=%d)=%d\r\n",chset,isflashsms,status_report,ret);
                    break;
                }
                
                //Ql_SendTextSMS="<phone number>"
                Ql_memset(notes, 0x0, sizeof(notes));
                Ql_sprintf(notes, "Ql_SendTextSMS=\"");
                pChar = Ql_strstr((char*)pDataEvt->data, notes);
                if (NULL != pChar)
                {
                    pChar+=16;
                    pChar2 = Ql_strstr(pChar,"\"");
                    *pChar2=0; 
                    ret=Ql_SendTextSMS((u8*)pChar,(u8*)"Welcome to use Quectel module!",
                        Ql_strlen((u8*)"Welcome to use Quectel module!"));
                    OUT_DEBUG(debug_buffer, "\r\nQl_SendTextSMS(phone_num=\"%s\")=%d\r\n",pChar,ret);
                    break;
                }
                
                Ql_SendToModem(ql_md_port1, (u8*)pDataEvt->data, pDataEvt->len);
                break;
            }
            default:
                break;
        }
    }
}


void CallBack_NewSMS(u16 index,QlSMSStorage storage)
{
    s16 ret;
    OUT_DEBUG(debug_buffer,"\r\nCB_NewSMS: index=%d,storage=%d\r\n",index,storage);
// you can read it right now
/*
    ret=Ql_ReadSMS(index,0);
    OUT_DEBUG(debug_buffer,"\r\nQl_ReadSMS(index=%d,mode=%d)=%d\r\n",index,0,ret);
*/
}

void CallBack_SendSMS(bool result, s16 cause, u8 msg_ref)
{
    OUT_DEBUG(debug_buffer,"\r\nCB_SendSMS: result=%d,cause=%d,msg_ref=%d\r\n",result,cause,msg_ref);
}

void  CallBack_DeleteSMS(bool result, s16 cause,u16 index)
{
    OUT_DEBUG(debug_buffer,"\r\nCB_DeleteSMS: result=%d,cause=%d,index=%d\r\n",result,cause,index);
}

void CallBack_ReadPDUSMS(bool result, s16 cause,u16 index, u8 status, u8* data, u16 length)
{
    OUT_DEBUG(debug_buffer,"\r\nCB_ReadPDUSMS: result=%d,cause=%d,index=%d,status=%d,length=%d\r\n",
                                                    result,cause,index,status,length);
    if(result)
    {
        if(length)
        {
            int i;
            for(i=0;i<length;i++)
            {
                OUT_DEBUG(debug_buffer,"%02X",data[i]);
            }
            OUT_DEBUG(debug_buffer,"\r\n");
        }
    }
}

void CallBack_NewFlashPDUSMS(u16 length, u8* pdu_string)
{
    OUT_DEBUG(debug_buffer,"\r\nCB_NewFlashPDUSMS: length=%d\r\n",length);
    
    if(pdu_string)
    {
        int i;
        for(i=0;i<length;i++)
        {
            OUT_DEBUG(debug_buffer,"%02X",pdu_string[i]);
        }
        OUT_DEBUG(debug_buffer,"\r\n");
    }
}

void CallBack_PDUStatusReport(u16 length, u8* pdu_string)
{
    OUT_DEBUG(debug_buffer,"\r\nCB_PDUStatusReport: length=%d\r\n",length);    
    if(length)
    {
        int i;
        for(i=0;i<length;i++)
        {
            OUT_DEBUG(debug_buffer,"%02X",pdu_string[i]);
        }
        OUT_DEBUG(debug_buffer,"\r\n");
    }
}

void CallBack_ReadTextSMS(bool result, s16 cause,u16 index, u8 status, QlSMSTextMsg* sms)
{
    OUT_DEBUG(debug_buffer,"\r\nCB_ReadTextSMS: result=%d,cause=%d,index=%d,status=%d\r\n",
                                                    result,cause,index,status);
    if(result)
    {
        if(sms)
        {
            int i;
            OUT_DEBUG(debug_buffer,"phone_num=\"%s\",num_type=%d,chset=%d\r\n",
                                                        sms->phone_num,sms->num_type,sms->chset);
            if(sms->scts.year != 0)
            {
                OUT_DEBUG(debug_buffer,"scts=20%d-%d-%d %d:%d:%d\r\n",
                                                            sms->scts.year,sms->scts.month,sms->scts.day,sms->scts.hour,sms->scts.minute,sms->scts.second);     
            }
            
            if(sms->udh_len)
            {
                for(i=0;i<sms->udh_len;i++)
                {
                    OUT_DEBUG(debug_buffer,"%02X", sms->data_len);
                }
                OUT_DEBUG(debug_buffer,"\r\n");
            }
            
            if(sms->data)
            {
                s16 ret;
                Ql_SMS_DCS_ToCSCS(sms,CSCS_CHSET_GSM);
                OUT_DEBUG(debug_buffer,"data_len=%d\r",sms->data_len); 
                for(i=0;i<sms->data_len;i++)
                {
                    if(QL_SMS_CHSET_GSM == sms->chset)
                    {
                        OUT_DEBUG(debug_buffer,"%c",(sms->data)[i]);
                    }
                    else
                    {
                        OUT_DEBUG(debug_buffer,"%02X",(sms->data)[i]);
                    }
                }   
                OUT_DEBUG(debug_buffer,"\r\n");
//you can send message right now
/* 
                ret = Ql_SendTextSMS(sms->phone_num, sms->data_len, sms->data);
                OUT_DEBUG(debug_buffer, "\r\nQl_SendTextSMS(phone_num=\"%s\")=%d\r\n",sms->phone_num,ret);
*/
            }
        }
    }
}

void CallBack_NewFlashTextSMS(QlSMSTextMsg* sms)
{
    if(sms)
    {
        int i;
        OUT_DEBUG(debug_buffer,"\r\nCB_NewFlashTextSMS: phone_num=%s,num_type=%d,chset=%d\r\n",
                                                    sms->phone_num,sms->num_type,sms->chset);
        OUT_DEBUG(debug_buffer,"scts=20%d-%d-%d  %d:%d:%d\r\n",
                                                    sms->scts.year,sms->scts.month,sms->scts.day,sms->scts.hour,sms->scts.minute,sms->scts.second);        
        if(sms->udh)
        {
            for(i=0;i<sms->udh_len;i++)
            {
                OUT_DEBUG(debug_buffer,"%02X",(sms->udh)[i]);
            }
            OUT_DEBUG(debug_buffer,"\r\n");
        }
        
        if(sms->data)
        {
            for(i=0;i<sms->data_len;i++)
            {
                if(QL_SMS_CHSET_GSM == sms->chset)
                {
                    OUT_DEBUG(debug_buffer,"%c",(sms->data)[i]);
                }
                else
                {
                    OUT_DEBUG(debug_buffer,"%02X",(sms->data)[i]);
                }
            }   
            OUT_DEBUG(debug_buffer,"\r\n");
        }
    }
}

void CallBack_TextStatusReport(u8 fo,u8 msg_ref, u8* phone_num, QlSysTimer* scts, QlSysTimer* dt, u8 st)
{
    OUT_DEBUG(debug_buffer,"\r\nCB_TextStatusReport: fo=%d,msg_ref=%d,phone_num=%s,st=%d\r\n",
                                                fo,msg_ref,phone_num,st);
    OUT_DEBUG(debug_buffer,"scts=20%d-%d-%d %d:%d:%d\r\n",
                                                scts->year,scts->month,scts->day,scts->hour,scts->minute,scts->second);
    OUT_DEBUG(debug_buffer,"dt=20%d-%d-%d %d:%d:%d\r\n",
                                                scts->year,scts->month,scts->day,scts->hour,scts->minute,scts->second);
}

#endif //__EXAMPLE_SMS__

