
#ifndef __QL_SIM_H__
#define __QL_SIM_H__

#include "ql_type.h"

typedef void (*Ql_CallBack_SIM)(s8 SIM_type);

typedef enum
{
   QL_ID_READY,    // SIM ready
   QL_ID_SIM_PIN1, // PIN1 locked
   QL_ID_SIM_PUK1, // PUK1 locked
   QL_ID_SIM_BUSY =23  /* This add for WM CPIN? with SML check */
}QL_SIM_PIN_TYPE_enum;


/******************************************************************************
* Function:     Ql_SIM_GetLockState
*
* Description:
*               This function get current SIM card lock state.
*
* Parameters:
*               lock_type:
*                       [out] one of the of "QL_SIM_PIN_TYPE_enum"
*
* Return:
*               QL_RET_OK: indicates success.
*               QL_RET_ERR_PARAM: indicates param error.
*               Ql_RET_ERR_SIM_NOT_INSERTED: SIM card not Inserted.
******************************************************************************/
s32 Ql_SIM_GetLockState(s8* lock_type);

/******************************************************************************
* Function:     Ql_SIM_FeedPIN
*
* Description:
*               This function feed PIN1 code or PUK1 code.
*               note: if feed PUK1 success,the PIN1 code will be set to "1234" .
*
* Parameters:
*               pin_type:
*                       [in] the PIN type ,one of the of "QL_SIM_PIN_TYPE_enum"
*               pin_code: PIN1 code or PUK1 code
*
*               callback:
*                       the callback function will be invoked when feed PIN complete

*               (*Ql_CallBack_SIM)(s8 SIM_type) 
*                       SIM_type: 
*                               [out]  the SIM card state 
*
* Return:
*               QL_RET_OK: indicates success.
*               QL_RET_ERR_PARAM: indicates param error.
*               Ql_RET_ERR_SIM_TYPE_ERROR: PIN type not match
*
******************************************************************************/

s32 Ql_SIM_FeedPIN(QL_SIM_PIN_TYPE_enum pin_type, u8*pin_code, Ql_CallBack_SIM callback);


/******************************************************************************
* Function:     Ql_SIM_GetPINCntAvail
*
* Description:
*               This function is used to get the times remain to input sim PIN and PUK.
*
* Parameters:
*               pin_cnt:
*                       [out] the times remain to input sim PIN.
*               puk_cnt: 
*                       [out] the times remain to input sim PUK.
*
* Return:
*               QL_RET_OK indicates success.
*               Negative indicates failure. please see Error Code Definition.
*
******************************************************************************/
s32 Ql_SIM_GetPINCntAvail(u8* pin_cnt, u8* puk_cnt);

/******************************************************************************
* Function:     Ql_SIM_GetIMSI
*
* Description:
*               This function gets the IMSI(International Mobile Subscriber Identity) of SIM Card.
*
* Parameters:
*               ptr_imei:
*                   [out] A pointer to the IMSI buffer.
*               len:
*                   [in] It must be equal or greater than the length of version ID.
*                         Otherwise error code will be returned.
*
* Return:
*               QL_RET_OK indicates success.
*               Negative indicates failure. please see Error Code Definition.
******************************************************************************/
s32 Ql_SIM_GetIMSI(u8* ptr_imsi, u16 len);

/******************************************************************************
* Function:     Ql_SIM_GetCCID
*
* Description:
*               This function gets the ICCID(Integrate circuit card identity) of SIM Card.
*               It is made up of 20 characters.
* Parameters:
*               ptr_ccid:
*                   [out] A pointer to the CCID buffer.
*               len:
*                   [in] It must be equal or greater than the length of CCID.
*                         Otherwise error code will be returned.
*
* Return:
*               the length of CCID.
******************************************************************************/
s32 Ql_SIM_GetCCID(u8* ptr_ccid, u16 len);
#endif 


