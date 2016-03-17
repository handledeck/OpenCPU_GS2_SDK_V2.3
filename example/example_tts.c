#ifdef __EXAMPLE_TTS__
/***************************************************************************************************
*   Example:
*       
*           TTS Routine
*
*   Description:
*
*           This example demonstrates how to use tts function with APIs in OpenCPU.
*           Through MAIN Uart port, input the specified command, and the response message will be 
*           printed out through DEBUG port.
*
*   Usage:
*
*           Compile & Run:
*
*               Use "make TTS" to compile, and download bin image to module to run.
*           
*           Operation: (Through MAIN port)
*
*               Input "audio path=0/1/2" to change the audio path, which TTS voice is output through.
*               Input "start tts=..." to play TTS.
*               Input "stop tts" to stop playing TTS.
*               Input "tts?" to query the playing status.
*               Input "tts vol=..." to set the volume of TTS voice.
*           
****************************************************************************************************/
#include "ql_appinit.h"
#include "ql_interface.h"
#include "ql_type.h"
#include "ql_trace.h"
#include "ql_timer.h"
#include "ql_stdlib.h"
#include "ql_fcm.h"
#include "ql_tts.h"
#include "ql_audio.h"


QlEventBuffer g_event; // Keep this variable a global variable due to its big size
ascii g_buffer[1024];


/***********************************************
* Callback function for play TTS. 
* it will be invoked after the playing TTS
* action finishes or something wrong happends
************************************************/
void Callback_TTS_Play(s32 result)
{
    Ql_DebugTrace("Callback_TTS_Play(), result: %d\r\n", result);
}

void ql_entry(void)
{
    s32 ret;
    bool keepGoing = TRUE;
    bool isContinued = FALSE;
    u32 cnt = 0; 
    
    Ql_DebugTrace("OpenCPU: Hello TTS !\r\n\r\n");    /* Print out messages through DEBUG port */
    Ql_SetDebugMode(BASIC_MODE);    /* Only after the device reboots, 
                                     *  the set debug-mode takes effect. 
                                     */

    // Initialize TTS, and register callback function
    Ql_TTS_Initialize(&Callback_TTS_Play);
    
    while(keepGoing)
    {
        Ql_GetEvent(&g_event);
        switch(g_event.eventType)
        {
            case EVENT_UARTDATA:
            {
                // TODO: receive and handle data from UART
                ascii* pChar;
                s16  strLen;
                PortData_Event* pDataEvt = (PortData_Event*)&g_event.eventData.uartdata_evt;

                /* Change audio path (Command: audio path=0/1/2)
                *  0 = the main audio path
                *  1 = earphone audio path
                *  2 = loudspeaker audio path
                */ 
                Ql_memset(g_buffer, 0x0, sizeof(g_buffer));
                strLen = Ql_sprintf(g_buffer, "audio path=");
                pChar = Ql_strstr((char*)pDataEvt->data, g_buffer);
                if (pChar)
                {
                    u8 audPath;
                    char* p1;
                    char* p2;
                    Ql_DebugTrace((char*)pDataEvt->data);
                    p1 = Ql_strstr((char*)pDataEvt->data, "=");
                    p2 = Ql_strstr((char*)pDataEvt->data, "\r\n");
                    Ql_memset(g_buffer, 0x0, sizeof(g_buffer));
                    memcpy(g_buffer, p1 + 1, p2 - p1 -1);
                    if (Ql_strcmp(g_buffer, "1") == 0)
                    {
                        audPath = QL_AUDIO_PATH_HEADSET;
                    }
                    else if (Ql_strcmp(g_buffer, "2") == 0)
                    {
                        audPath = QL_AUDIO_PATH_LOUDSPEAKER;
                    }
                    else
                    {
                        audPath = QL_AUDIO_PATH_NORMAL;
                    }
                    if (!Ql_VoiceCallChangePath(audPath))
                    {
                        Ql_DebugTrace("Fail to change audio path.\r\n");
                    }
                    else
                    {
                        Ql_DebugTrace("Change audio path to %d.\r\n", audPath);
                    }
                    break;
                }

                // Start tts speech (Command: start tts=...)
                strLen = Ql_sprintf(g_buffer, "start tts=");
                ret = Ql_strncmp((char*)pDataEvt->data, g_buffer, strLen);
                if (0 == ret)
                {
                    char* pCh = NULL;
                    pCh = Ql_strstr((char*)pDataEvt->data, "=");
                    if (pCh != NULL)
                    {
                        Ql_memset(g_buffer, 0x0, sizeof(g_buffer));
                        Ql_strncpy(g_buffer, pCh + 1, pDataEvt->len - (pCh - (char*)pDataEvt->data + 1) - 2);
                        Ql_DebugTrace("Ql_TTS_Play(), len=%d\r\n", Ql_strlen(g_buffer));
                        ret = Ql_TTS_Play((u8*)g_buffer, sizeof(g_buffer));
                        if (ret < 0)
                        {
                            Ql_DebugTrace("Fail to play TTS.\r\n");
                            break;
                        }
                        else
                        {
                            Ql_DebugTrace("Playing finished.\r\n");
                        }
                    }
                    break;
                }
                
                // Stop TTS (Command: stop tts)
                Ql_memset(g_buffer, 0x0, Ql_strlen(g_buffer));
                strLen = Ql_sprintf(g_buffer, "stop tts\r\n");
                ret = Ql_strncmp((char*)pDataEvt->data, g_buffer, strLen);
                if (0 == ret)
                {
                    Ql_DebugTrace(g_buffer);
                    Ql_TTS_Stop();
                    break;
                }
                
                // Query TTS' status (Command: tts?)
                Ql_memset(g_buffer, 0x0, Ql_strlen(g_buffer));
                strLen = Ql_sprintf(g_buffer, "tts?\r\n");
                ret = Ql_strncmp((char*)pDataEvt->data, g_buffer, strLen);
                if (0 == ret)
                {
                    Ql_DebugTrace(g_buffer);
                    ret = Ql_TTS_Query();
                    switch (ret)
                    {
                    	case TTS_STATUS_IDLE:
                            Ql_DebugTrace("TTS: idle\r\n");
                    	    break;
                    	case TTS_STATUS_PLAYING:
                    	    Ql_DebugTrace("TTS: playing\r\n");
                    	    break;
                    	default:
                    	    Ql_DebugTrace("Fail to query TTS' status. Error code: %d\r\n", ret);
                    	    break;
                    }
                    break;
                }

                // Set volume (Command: tts vol=0..100)
                Ql_memset(g_buffer, 0x0, sizeof(g_buffer));
                strLen = Ql_sprintf(g_buffer, "tts vol=");
                pChar = Ql_strstr((char*)pDataEvt->data, g_buffer);
                if (pChar)
                {
                    u8 ttsVol;
                    char* p1;
                    char* p2;
                    Ql_DebugTrace((char*)pDataEvt->data);
                    p1 = Ql_strstr((char*)pDataEvt->data, "=");
                    p2 = Ql_strstr((char*)pDataEvt->data, "\r\n");
                    Ql_memset(g_buffer, 0x0, sizeof(g_buffer));
                    memcpy(g_buffer, p1 + 1, p2 - p1 -1);
                    ttsVol = Ql_atoi(g_buffer);
                    ttsVol = (ttsVol > 100) ? 100 : ttsVol;
                    Ql_SetVolume_Ex(VOL_TYPE_MEDIA, QL_AUDIO_PATH_HEADSET, ttsVol);
                    break;
                }
                
               // Set TTS Style 
               // Set TTS fastest voice speed
                Ql_memset(g_buffer, 0x0, sizeof(g_buffer));
                strLen = Ql_sprintf(g_buffer, "tts Style1=");
                pChar = Ql_strstr((char*)pDataEvt->data, g_buffer);
                if (pChar)
                {
                    u32 ret;
                    ret = Ql_TTS_SetStyle(TTS_PARAM_VOICE_SPEED,TTS_SPEED_MAX);
                    Ql_DebugTrace("Set TTS fastest voice speed.\r\n");
                    break;
                }
                // Set TTS minimized volume.
                Ql_memset(g_buffer, 0x0, sizeof(g_buffer));
                strLen = Ql_sprintf(g_buffer, "tts Style2=");
                pChar = Ql_strstr((char*)pDataEvt->data, g_buffer);
                if (pChar)
                {
                    u32 ret;
                    ret = Ql_TTS_SetStyle(TTS_PARAM_VOLUME,TTS_VOLUME_MIN);
                    Ql_DebugTrace("Set TTS minimized volume.\r\n");
                    break;
                }
                // Set TTS voice effect --eccentric
                Ql_memset(g_buffer, 0x0, sizeof(g_buffer));
                strLen = Ql_sprintf(g_buffer, "tts Style3=");
                pChar = Ql_strstr((char*)pDataEvt->data, g_buffer);
                if (pChar)
                {
                    u32 ret;
                    ret = Ql_TTS_SetStyle(TTS_PARAM_VEMODE,TTS_VEMODE_ECCENTRIC);
                    Ql_DebugTrace("Set TTS voice effect --eccentric.\r\n");
                    break;
                }
            }

            /*************************************
            *  'Case' another EVENT if needed.
            *
            **************************************/
            
            default:
                break;
        }
    }
}

#endif // __EXAMPLE_TTS__

