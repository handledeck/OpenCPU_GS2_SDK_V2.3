/*==========================================================================
 |               Quectel OpenCPU --  User API
 |
 |              Copyright (c) 2010 Quectel Ltd.
 |
 |--------------------------------------------------------------------------
 | File Description
 | ----------------
 |      Definitions for Call
 |
 |--------------------------------------------------------------------------
 |
 |  Designed by   :    Stanley YONG
 |  Coded by        :    Stanley YONG
 |  Tested by        :    Stanley YONG
 |
 \=========================================================================*/

#ifndef __QL_VOICE_H__
#define __QL_VOICE_H__

#include "ql_type.h"
#include "ql_appinit.h"

#define QL_MAX_CALL_ADDR_LEN          41
#define QL_MAX_CALL_NUM         6

typedef enum tagQl_CallType
{
   VOICE_CALL,                                  /* Voice Call */
   CSD_CALL = VOICE_CALL + 4,   /* Circuit Switched Data */
   INVALID_CALL_TYPE = 255
} Ql_CallType;
typedef enum tagQl_SIMCard_Status
{
    SIMCARD_ALREADY = 0,
    SIMCARD_NOT_INSERTED = 1,
    SIMCARD_PIN_LOCKED = 2,
    SIMCARD_PUK_LOCKED = 3,
    SIMCARD_NOT_READY  = 4
}Ql_SIMCard_Status;

typedef enum tagCallResponse
{
  CALL_RESPONSE_OK,
  CALL_RESPONSE_CONNECT,
  CALL_RESPONSE_RING,
  CALL_RESPONSE_NO_CARRIER,
  CALL_RESPONSE_ERROR,
  CALL_RESPONSE_INV,
  CALL_RESPONSE_NO_DIALTONE,
  CALL_RESPONSE_BUSY,
  CALL_RESPONSE_NO_ANSWER,
  CALL_RESPONSE_PROCEEDING,
  CALL_RESPONSE_FCERROR,
  CALL_RESPONSE_INTERMEDIATE,
  RESULT_CODE_INVALID_COMMAND,
}Ql_CallResponse;

typedef enum ocpu_call_error_code_enum_tip
{
  QL_RET_ERR_CALL_SIM_IS_NOT_INSERTED = 1, 
  QL_RET_ERR_CALL_SIM_CARD_PIN = 2,
  QL_RET_ERR_CALL_SIM_CARD_PUK = 3,    
  QL_RET_ERR_CALL_OK = 0,
  QL_RET_ERR_CALL_ERROR  = -1,
  QL_RET_ERR_CALL_NOT_EXIST = -2,  
  QL_RET_ERR_CALL_NOT_READY = -3,
  QL_RET_ERR_CALL_INVALID_NUMBER =-4,
  QL_RET_ERR_CALL_INVALID_POINTER =-5 ,
  QL_RET_ERR_CALL_NOT_INIT = -6,
  QL_RET_ERR_CALL_FUNC_ALREADY_OCCUPIED = -7
}ocpu_call_error_code_enum;

typedef struct
{
    void (*OCPU_CB_DIAL)(Ql_CallResponse result);
    void (*OCPU_CB_RING)(u8* coming_num);
    void (*OCPU_CB_HANGUP)(void);
}Ql_STCall_Callback;


typedef struct
{
    u8 call_id; //call identification number 
    u8 call_orig; //originated or terminated call  
                       // 0 Mobile originated (MO) call
                       // 1 Mobile terminated (MT) call
    u8 call_state; //state of the call
                        // 0  Active
                        // 1  Held
                        // 2  Dialing (MO call)
                        // 3  Alerting (MO call)
                        // 4  Incoming (MT call)
                        // 5  Waiting (MT call)
    u8 call_mode;//Bearer/tele service
                        // 0  Voice
                        // 1  Data
                        // 2  FAX
                        // 9  Unknown 
    u8 call_mpty; //multiparty call or not
                        // 0  Call is not one of multiparty (conference) call parties
                        // 1  Call is one of multiparty (conference) call parties
    u8 call_number[QL_MAX_CALL_ADDR_LEN];//phone number
    u8 call_type; //The specified character type of phone number.
                       // 129 Unknown type(IDSN format number)   
                       // 145 International number type(ISDN format)
}QlCallInfo;

typedef struct
{
    s32 call_count;//the count of calls.
    QlCallInfo call_info[QL_MAX_CALL_NUM];//support seven calls.
}QlCallParam;

typedef void (*OCPU_CB_COMING_CALL)(QlCallParam *call_param);

/******************************************************************************
* Function:     Ql_Call_Initialize
*  
* Description:
*                Do some initial work before using call API.
*
* Parameters:    
*                call_cbFunc:
*                       A pointer to the "Ql_STCall_Callback".
*                       
*
* Return:  
*               QL_RET_OK indicates success.
*               QL_RET_ERR_CALL_INVALID_POINTER  indicates failure. 
******************************************************************************/
s32 Ql_Call_Initialize(Ql_STCall_Callback* call_cbFunc);

/******************************************************************************
* Function:     Ql_GetCallCntByType
*
* Description:
*               Get the current call count by call type.
*
* Parameters:
*               call_type:
*                       Call Type, which is one value of 'Ql_CallType'.
*
* Return:
*               A nonnegative number as the current call number.
******************************************************************************/
u8 Ql_GetCallCntByType(Ql_CallType call_type);


/******************************************************************************
* Function:     Ql_Call_Dial
*
* Description:
*               Dial a number.
*               This function will check the status of SIM card.
*
* Parameters:
*               call_type:
*                       Call Type, which is one value of 'Ql_CallType'.
*
*               callNo:
*                       Call number.
* Return:
*               QL_RET_ERR_CALL_SIM_IS_NOT_INSERTED indicates SIM card is not inserted.
*               QL_RET_ERR_CALL_SIM_CARD_PIN,indicates SIM card pin is locked.
*               QL_RET_ERR_CALL_SIM_CARD_PUK,indicates SIM card PUK is locked.
*               QL_RET_ERR_CALL_ERROR indicates the function failed.
*               QL_RET_ERR_CALL_NOT_READY indicates the modem not ready before call. 
*               QL_RET_ERR_CALL_NOT_INIT indicates not init.
*               QL_RET_ERR_CALL_FUNC_ALREADY_OCCUPIED indicates the function has been occupied .
*
******************************************************************************/
s32 Ql_Call_Dial(Ql_CallType call_type, char* callNo);

/******************************************************************************
* Function:     Ql_Call_Answer
*
* Description:
*               This function answer a coming call.
*
* Parameters:
*               None.
*
* Return:
*               QL_RET_OK indicates success.
*               QL_RET_ERR_CALL_NOT_EXIST indicates not exist.
*               QL_RET_ERR_CALL_NOT_INIT indicates not init.
******************************************************************************/
s32 Ql_Call_Answer(void);


/******************************************************************************
* Function:     Ql_Call_Hangup
*
* Description:
*               This function hang up a call.
*
* Parameters:
*               None.
*
* Return:
*               QL_RET_OK indicates success.
*               QL_RET_ERR_CALL_NOT_EXIST indicates not exist.
*               QL_RET_ERR_CALL_NOT_INIT indicates not init.
******************************************************************************/
s32 Ql_Call_Hangup(void);

/******************************************************************************
* Function:     Ql_Call_GetCurrCall
*
* Description:
*               This function get current calls of ME.
*
* Parameters:
*               call_callback:
*                         [in] point to callback function.
*
* Return:
*               QL_RET_OK indicates success.
*               QL_RET_ERR_PARAM indicates param error.
******************************************************************************/
s32 Ql_Call_GetCurrCall(OCPU_CB_COMING_CALL call_callback);

#endif  // End-of __QL_VOICE_H__

