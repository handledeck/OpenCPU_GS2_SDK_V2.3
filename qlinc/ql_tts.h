/*==========================================================================
 |               Quectel OpenCPU --  User API
 |
 |              Copyright (c) 2011 Quectel Ltd.
 |
 |--------------------------------------------------------------------------
 | File Description
 | ----------------
 |      Definitions for TTS
 |
 |--------------------------------------------------------------------------
 |
 |  Designed by   :     Stanley YONG
 |  Coded by      :     Stanley YONG
 |  Tested by     :     Stanley YONG
 |
 \=========================================================================*/

#ifndef __QL_TTS_H__
#define __QL_TTS_H__

#include "ql_type.h"


/*TTS Playing State */
typedef enum tagQl_TTSStatus
{
   TTS_STATUS_IDLE    = 0,
   TTS_STATUS_PLAYING = 1
}Ql_TTSStatus;

typedef enum tagQl_TTSStyle
{
/* constants for values of field nParamID */
    TTS_PARAM_VOICE_SPEED,  /* voice speed */
    TTS_PARAM_VOLUME,       /* volume value */      
    TTS_PARAM_VEMODE,       /* voice effect - predefined mode */
}QlTTSStyle;


/* constants for values of parameter TTS_PARAM_VOICE_SPEED */
/* the range of voice speed value is from -32768 to +32767 */
#define TTS_SPEED_MIN					-32768		/* slowest voice speed */
#define TTS_SPEED_NORMAL				0			/* normal voice speed (default) */
#define TTS_SPEED_MAX					+32767		/* fastest voice speed */

/* constants for values of parameter TTS_PARAM_VOLUME */
/* the range of volume value is from -32768 to +32767 */
#define TTS_VOLUME_MIN				-32768		/* minimized volume */
#define TTS_VOLUME_NORMAL				0			/* normal volume */
#define TTS_VOLUME_MAX				+32767		/* maximized volume (default) */

/* constants for values of parameter TTS_PARAM_VEMODE */
#define TTS_VEMODE_NONE				    0			/* none */
#define TTS_VEMODE_WANDER				1			/* wander */
#define TTS_VEMODE_ECHO				    2			/* echo */
#define TTS_VEMODE_ROBERT				3			/* robert */
#define TTS_VEMODE_CHROUS				4			/* chorus */
#define TTS_VEMODE_UNDERWATER			5			/* underwater */
#define TTS_VEMODE_REVERB				6			/* reverb */
#define TTS_VEMODE_ECCENTRIC			7			/* eccentric */

typedef void (*OCPU_CB_TTS_PLAY)(s32 res);


/*****************************************************************
* Function:     Ql_TTS_Initialize 
* 
* Description:
*               Initialize the TTS function.
* Parameters:
*               cb_play:
*                     A pointer to callback function.
* Return:        
*               QL_RET_OK, suceess
*               or other error codes, please see 'Error Code Definition'
*****************************************************************/
s32 Ql_TTS_Initialize(OCPU_CB_TTS_PLAY cb_play);


/*****************************************************************
* Function:     Ql_TTS_Play 
* 
* Description:
*               Play TTS Text.
* Parameters:
*               content:
*                        [in]pointer to text.
*               len:
*                    [in]The lenth of text to speech. len <= 1024
* Return:        
*               QL_RET_OK, suceess
*               or other error codes, please see 'TTS Error Code Definition'
*****************************************************************/
s32 Ql_TTS_Play(u8* content, u8 len);


/*****************************************************************
* Function:     Ql_TTS_PlayInCall 
* 
* Description:
*               Play TTS Text as background audio during calling.
* Parameters:
*               content:
*                     pointer to text.
*               len:
*                     The lenth of text to speech. len <= 1024
*               ul_volume:
*                     the volume at the remote end. The value ranges
*                     from 0 to 7.
*               dl_volume:
*                     The volume at the local.The value ranges
*                     from 0 to 7. If set to 0, the side will not hear
*                     the TTS sound.
* Return:        
*               QL_RET_OK, suceess
*               QL_RET_ERR_GENERAL_FAILURE, fail to play tts.
*               Ql_RET_NOT_SUPPORT, not support this function.
*****************************************************************/
s32 Ql_TTS_PlayInCall(u8* content, u8 len, u8 ul_volume, u8 dl_volume);


/*****************************************************************
* Function:     Ql_TTS_Stop 
* 
* Description:
*               Stop playing.
* Parameters:
*               None.
* Return:        
*               QL_RET_OK, suceess
*               or other error codes, please see 'Error Code Definition'
*****************************************************************/
s32 Ql_TTS_Stop(void);


/*****************************************************************
* Function:     Ql_TTS_Query 
* 
* Description:
*               Check status of TTS.
* Parameters:
*               None.
*  return:
*	          0 = ivTTS_STATUS_IDLE,		 invalid 
*	          1 = ivTTS_STATUS_INITIALIZED,	 Initialized
*	          2 = ivTTS_STATUS_PAUSE,		 pause 
*	          3 = ivTTS_STATUS_SYNTHESIZING, synthesizing 
*	          4 = ivTTS_STATUS_PLAYING,		 playing 
*	          5 = ivTTS_STATUS_SYNTHESIZED,	 synthesizined
*	          6 = ivTTS_STATUS_PALYEND		 played
*               Negative indicates failed. Please see 'Error Code Definition'
***********************************/
s32 Ql_TTS_Query(void);

/*****************************************************************
* Function:     Ql_TTS_SetStyle 
* 
* Description:
*               Set the TTS Style which include voice speed ,volume value and voice effect
* Parameters:
*               Style:
*                     TTS Style ,which is one value of "QlTTSStyle"
*               value:
*                     the parameter of the style,please reference :
*                       constants for values of parameter TTS_PARAM_VOICE_SPEED,
*                       constants for values of parameter TTS_PARAM_VOLUME
*                       constants for values of parameter TTS_PARAM_VEMODE

*  return:
*           QL_RET_OK , setting successfully
*           QL_RET_ERR_PARAM ,parameter error
*               
***********************************/
s32 Ql_TTS_SetStyle(QlTTSStyle Style, s32 value);

#endif  // End-of __QL_TTS_H__

