/*==========================================================================
 |               Quectel OpenCPU --  User API
 |
 |              Copyright (c) 2010 Quectel Ltd.
 |
 |--------------------------------------------------------------------------
 | File Description
 | ----------------
 |      Definitions for SMS operations
 |
 |--------------------------------------------------------------------------
 |
 |  Designed by    :    Willis YANG
 |  Coded by       :    Willis YANG
 |  Tested by      :    Stanley YONG
 |
 \=========================================================================*/

#ifndef __QL_SMS_H__
#define __QL_SMS_H__

#include "ql_type.h"
#include "ql_timer.h"

#define QL_MAX_PHONE_NUM    41
#define QL_MAX_TEXT_SMS_LENGTH  160

typedef enum QlSMSChsetTag
{
    QL_SMS_CHSET_GSM = 0,
    QL_SMS_CHSET_UCS2 = 1,
    QL_SMS_CHSET_8BIT =  2,
    QL_SMS_CHSET_INVALID = 255
}QlSMSChset;

typedef enum QlSMSStatusTag
{
    QL_SMS_STAT_UNREAD = 0,
    QL_SMS_STAT_READ,
    QL_SMS_STAT_UNSENT,
    QL_SMS_STAT_SENT,
    QL_SMS_STAT_ALL,
    QL_SMS_STAT_INVALID = 255
}QlSMSStatus;

typedef enum QlSMSStorageTag
{
    QL_SMS_STORAGE_SM = 0,
    QL_SMS_STORAGE_ME,
    QL_SMS_STORAGE_MT,
    QL_SMS_STORAGE_UNKNOWN = 255
}QlSMSStorage;

typedef enum QlSMSDeleteFlagTag
{
    QL_SMS_DEL_INDEXED_MSG = 0, /* Single message by index */
    QL_SMS_DEL_READ_MSG,         /* Already read messages */
    QL_SMS_DEL_READ_SENT_MSG,   /* Read and sent messages */
    QL_SMS_DEL_READ_SENT_UNSENT_MSG, /* Read ,sent and unsent messages */
    QL_SMS_DEL_ALL_MSG /* All messages in current storage */
}QlSMSDeleteFlag;


typedef struct QlSMSTextMsgTag
{
    u8 phone_num[QL_MAX_PHONE_NUM];         
    u8 num_type;
    u8 udh[QL_MAX_TEXT_SMS_LENGTH - 1];                   /*user data header*/
    u8 udh_len;
    u8 data[QL_MAX_TEXT_SMS_LENGTH];
    u16 data_len;
    QlSMSChset chset;
    QlSysTimer scts;       /* SMS server center time stamp*/
}QlSMSTextMsg;

typedef enum QlSMSCSCSTag
{
    CSCS_CHSET_IRA,
    CSCS_CHSET_HEX,
    CSCS_CHSET_GSM,
    CSCS_CHSET_PCCP437,
    CSCS_CHSET_PCDN,
    CSCS_CHSET_88591,
    CSCS_CHSET_UCS2,
    CSCS_CHSET_UCS2_0X81,
    CSCS_CHSET_TOTAL_NUM

}QlSMSCSCS;

typedef struct QlSMSListNodeTag
{
    u16 index;
    QlSMSStatus status;
}QlSMSListNode;
/******************************************************************************
*Function: 
*           void  (*CB_NewSMS)(u16 index,QlSMSStorage storage)
*Description:
*           This callback function is invoked when a new messege comes. The new messege stores in current SMS strorage    
*
*Fuction:
*           void  (*CB_SendSMS)(bool result, s16 cause, u8 msg_ref)
*Description:
*            This callback function is invoked by "Ql_SendTextSMS" or "Ql_SendPDUSMS"
*
*Fuction:
*         void  (*CB_DeleteSMS)(bool result, s16 cause,u16 index);  
*
*Description:
*           This callback function is invoked by "Ql_DeleteSMS".
*  
*Fuction:
*          void (*CB_ReadPDUSMS)(bool result, s16 cause,u16 index, u8 status, u8* data, u16 length);
*
*Description:
*           This callback function is invoked by "Ql_ReadSMS" when "Ql_SetSMSFormat" is set to 0.
*
*Fuction:
*          void  (*CB_NewFlashPDUSMS)(u16 length, u8* pdu_string);
*
*Description:
*           This callback function is invoked when a flash messege comes. Flash messege doesn't store in strorage.
*
*Fuction:
*           void  (*CB_PDUStatusReport)(u16 length, u8* pdu_string);
*
*Description:
*           At first, send a SMS with SMS status report function was opened. 
*           After DA(destination address) receive SMS, SMS service centre will 
*           send SMS status report to SA(source address). If "Ql_SetSMSFormat" 
*           is set to 0,this callback function will be invoked.
*
*Fuction:
*       void  (*CB_ReadTextSMS)(bool result, s16 cause,u16 index, u8 status, QlSMSTextMsg* sms);
*
*Description:
*       This callback function is invoked by "Ql_ReadTextSMS" when "Ql_SetSMSFormat" is set to 1
*
*Fuction:
*        void  (*CB_NewFlashTextSMS)(QlSMSTextMsg* sms);
*
*Description:
*       This callback function is invoked when a flash messege comes. Flash messege doesn't store in strorage.
*
*Fuction:
*        void  (*CB_TextStatusReport)(u8 fo,u8 msg_ref, u8* phone_num, QlSysTimer* scts, QlSysTimer* dt, u8 st);
*Description:
*       At first, send a SMS with SMS status report function was opened. 
*       After DA(destination address) receive SMS, SMS service centre will
*       send SMS status report to SA(source address). If "Ql_SetSMSFormat" is set to 1, 
*       this callback function will be invoked.
*
******************************************************************************/
typedef struct
{
    void  (*CB_NewSMS)(u16 index,QlSMSStorage storage);
    void  (*CB_SendSMS)(bool result, s16 cause, u8 msg_ref);
    void  (*CB_DeleteSMS)(bool result, s16 cause,u16 index);
    void  (*CB_ReadPDUSMS)(bool result, s16 cause,u16 index, u8 status, u8* data, u16 length);
    void  (*CB_NewFlashPDUSMS)(u16 length, u8* pdu_string);
    void  (*CB_PDUStatusReport)(u16 length, u8* pdu_string);
    void  (*CB_ReadTextSMS)(bool result, s16 cause,u16 index, u8 status, QlSMSTextMsg* sms);
    void  (*CB_NewFlashTextSMS)(QlSMSTextMsg* sms);
    void  (*CB_TextStatusReport)(u8 fo,u8 msg_ref, u8* phone_num, QlSysTimer* scts, QlSysTimer* dt, u8 st);
}Ql_SMS_Callback;


/******************************************************************************
* Function:     Ql_SMSInitialize
*  
* Description:
*               This function does some initial work before sending or receiving a message.
*               It sets callback of SMS.
*
* Parameters:    
*               sms_cbFunc:
*                    A pointer to the "Ql_SMS_Callback" which contains pointers to "CB_NewSMS",
*                    "CB_SendSMS" and so on. If you don't need part of them, please set them to NULL.
*
* Return:  
*               The return value is QL_RET_OK if this function succeeds. 
*               Otherwise, the return value is an error code. 
*               To get extended error information, please see ERROR CODES.
******************************************************************************/
s16  Ql_SMSInitialize(Ql_SMS_Callback* sms_cbFunc);


/******************************************************************************
* Function:     Ql_SetSMSStorage
*  
* Description:
*               This function sets SMS storages.
*
* Parameters:    
*               storage:
*                    SMS storage , which is one value of "QlSMSStorage". 
*                    QL_SMS_STORAGE_SM is default.
*
* Return:  
*               The return value is QL_RET_OK if this function succeeds. 
*               Otherwise, the return value is an error code. 
*               To get extended error information, please see ERROR CODES.
******************************************************************************/
s16  Ql_SetSMSStorage(u8 storage);


/******************************************************************************
* Function:     Ql_GetSMSStorageInfo
*  
* Description:
*               This function gets SMS storages info.
*
* Parameters:    
*               current_mem:
*                   Buffer to get current storage.
*               used:
*                   Buffer to get used space of current storage.
*               total:
*             	Buffer to get total space of current storage.
*
* Return:  
*               The return value is QL_RET_OK if this function succeeds. 
*               Otherwise, the return value is an error code. 
*               To get extended error information, please see ERROR CODES.
******************************************************************************/
s16  Ql_GetSMSStorageInfo(u8* current_mem, u8* used,u8* total);


/******************************************************************************
* Function:     Ql_SetInfoCentreNum
*  
* Description:
*               This function sets SMS service centre number.
*
* Parameters:    
*               sca_number:[input]
*                   A pointer to the SMS service centre number string.
*                   eg: if set a CentreNum +8613800551500   sca_number="+8613800551500"
*                   
*
* Return:  
*               The return value is QL_RET_OK if this function succeeds. 
*               Otherwise, the return value is an error code. 
*               To get extended error information, please see ERROR CODES.
******************************************************************************/
s16  Ql_SetInfoCentreNum(u8* sca_number);


/******************************************************************************
* Function:     Ql_GetInfoCentreNum
*  
* Description:
*               This function gets SMS service centre number.
*
* Parameters:    
*               sca_number:
*                   Pointer to the centre number string buffer.
*               type:
*                   Pointer to type.
*                   If "sca_number" beginning with a plus, like "+8610086", type will be set to 145,
*                   else type is 129.
*
* Return:  
*               The return value is QL_RET_OK if this function succeeds. 
*               Otherwise, the return value is an error code. 
*               To get extended error information, please see ERROR CODES.
******************************************************************************/
s16  Ql_GetInfoCentreNum(u8* sca_number, u8 *type);


/******************************************************************************
* Function:     Ql_SetNewSMSDirectToTE
*  
* Description:
*               This function makes new messages not to save in storage, like flash SMS.
*
* Parameters:    
*               op:
*               	1 opens this function, 0 closes.
*
* Return:  
*               The return value is QL_RET_OK if this function succeeds. 
*               Otherwise, the return value is an error code. 
*               To get extended error information, please see ERROR CODES.
******************************************************************************/
s16  Ql_SetNewSMSDirectToTE(bool op);


/******************************************************************************
* Function:     Ql_SetSMSFormat
*  
* Description:
*               This function sets the format of SMS, PDU or text.
*
* Parameters:    
*               format:
*	                0 is PDU and 1 is text. 0 is default.
*
* Return:  
*               The return value is QL_RET_OK if this function succeeds. 
*               Otherwise, the return value is an error code. 
*               To get extended error information, please see ERROR CODES.
******************************************************************************/
s16  Ql_SetSMSFormat(u8 format);


/******************************************************************************
* Function:     Ql_ReadSMS
*  
* Description:
*               This function reads an indexed message. 
*               If "Ql_SetSMSFormat" is set to 0(PDU mode),"CB_ReadPDUSMS" will be invoked.
*               If "Ql_SetSMSFormat" is set to 1(text mode),"CB_ReadPDUSMS" will be invoked. 
*               PDU mode is default.
*
* Parameters:    
*               index:
*                    Message index to read in message list.
*               mode:
*               	 0 changes status of the specified SMS record(unread to read). 1 doesn't change.
*
* Return:  
*               The return value is QL_RET_OK if this function succeeds. 
*               Otherwise, the return value is an error code. 
*               To get extended error information, please see ERROR CODES.
******************************************************************************/
s16  Ql_ReadSMS(u16 index, bool mode);


/******************************************************************************
* Function:     Ql_DeleteSMS
*  
* Description:
*               This function deletes SMS messages in current storage.
*
* Parameters:    
*               index:
*                   The index number of SMS message.
*               flag:
*                   Delete flag , which is one value of 'QlSMSDeleteFlag'.
*
* Return:  
*               The return value is QL_RET_OK if this function succeeds. 
*               Otherwise, the return value is an error code. 
*               To get extended error information, please see ERROR CODES.
******************************************************************************/
s16  Ql_DeleteSMS(u16 index,u8 flag);


/******************************************************************************
* Function:     Ql_ListSMS
*  
* Description:
*               This function gets message list in current storage by "QlSMSStatus".
*
* Parameters:    
*               sms_list:
*       		A buffer to get message list in current storage. 'QlSMSListNode' define as below.
*               node_count:
*       		The node count of sms list buffer.
*               by_status:
*       		One value of 'QlSMSStatus'.
*               sms_count:
*		       Actual sms count found in current storage.
*
* Return:  
*               The return value is QL_RET_OK if this function succeeds. 
*               Otherwise, the return value is an error code. 
*               To get extended error information, please see ERROR CODES.
*notes:    node_count == sms_list nodes,  and node_count must enough, node_count>=sms_count
******************************************************************************/
s16  Ql_ListSMS(QlSMSListNode* sms_list, u16 node_count, u8 by_status, u16* sms_count);


/******************************************************************************
* Function:     Ql_GetUnReadSMS
*  
* Description:
*               This function reads first unread message. If no unread message in current storage, 
*               the return value is -509(QL_RET_ERR_SMS_MSG_EMPTY) defined in ERROR CODES.
*
* Parameters:    
*               mode:
*               	 0 changes status of the specified SMS record(unread to read). 1 doesn't change.
*
* Return:  
*               The return value is QL_RET_OK if this function succeeds. 
*               Otherwise, the return value is an error code. 
*               To get extended error information, please see ERROR CODES.
******************************************************************************/
s16  Ql_GetUnReadSMS(bool mode);


/******************************************************************************
* Function:     Ql_SendPDUSMS
*  
* Description:
*               This function sends a message with PDU format. 
*               SMS format will be set to 0 automatic.
*
* Parameters: 
*               pdu_string:
*                    A pointer to the PDU string.
*               length:
*                    The length of PDU string, including sca length.
*               pdu_string:
*                    A pointer to the PDU string.
*
* Return:  
*               The return value is QL_RET_OK if this function succeeds. 
*               Otherwise, the return value is an error code. 
*               To get extended error information, please see ERROR CODES.
******************************************************************************/
s16  Ql_SendPDUSMS(u8 * pdu_string,u16 length);


/******************************************************************************
* Function:     Ql_SetTextSMSPara
*  
* Description:
*               This function sets some parameters in text mode.
*
* Parameters:    
*               chset:
*                    TE character set, which is one value of 'Ql_SMSChset'. 
*                    QL_SMS_CHSET_GSM is default.
*               isflashsms:
*               	 1 is flash SMS and 0 is common SMS. 0 is default.
*               status_report:
*               	 1 means needing SMS status report. After DA(destination address) receive SMS, 
*                    SMS service centre will send SMS status report to SA(source address). 
*                    But, crossing different telecom operators. this function perhaps is not supported.
*                    0 means don't need SMS status report. 0 is default.
*
* Return:  
*               The return value is QL_RET_OK if this function succeeds. 
*               Otherwise, the return value is an error code. 
*               To get extended error information, please see ERROR CODES.
******************************************************************************/
s16  Ql_SetTextSMSPara(u8 chset,bool isflashsms, bool status_report);

/******************************************************************************
* Function:     Ql_SMS_DCS_ToCSCS
*  
* Description:
*               This function conversion the characters of sms->data to go with
*               the terminal 
*
* Parameters:    
*               cscs: The type of Character set
*                     
*               msg:
*                    A pointer to the message text buffer.
*
* Return:  
*               QL_RET_OK indicates this function successes.
*               QL_RET_ERR_PARAM,parameter error.
******************************************************************************/
s32 Ql_SMS_DCS_ToCSCS(QlSMSTextMsg* sms, QlSMSCSCS cscs);


/******************************************************************************
* Function:     Ql_SendTextSMS
*  
* Description:
*               This function sends a message with text format.
*               SMS format will be set to 1 automatic.
*
* Parameters:    
*               number:
*                    A pointer to the phone number buffer.
*               msg:
*                    A pointer to the message text buffer.
*
*               length:
*                    The length of text content. The max length determined by character set. 
*                    GSM is 160, UCS2 and 8BIT is 140.
* Return:  
*               The return value is QL_RET_OK if this function succeeds. 
*               Otherwise, the return value is an error code. 
*               To get extended error information, please see ERROR CODES.
******************************************************************************/
s16  Ql_SendTextSMS(u8 * phone_num,u8 * msg,u16 length);

#endif  // End-of __QL_SMS_H__

