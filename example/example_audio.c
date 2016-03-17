
#ifdef __EXAMPLE_AUDIO__
/***************************************************************************************************
*   Example:
*       
*           AUDIO Routine
*
*   Description:
*
*           This example demonstrates how to use AUDIO function with APIs in OpenCPU.
*           Through MAIN Uart port, input the specified command, there will be given the 
*           response about AUDIO operation.
*
*   Usage:
*
*           Compile & Run:
*
*               Use "make audio" to compile, and download bin image to module to run.
*           
*           Operation: (Through MAIN port)
*            
*               If input "PlayFile=xxx", that will start playing an Audio file,xxx is Audio file name.
*               If input "StopFile", that will stop playing the Audio file.
*               If input "PlayS=mp3", that will start playing an audio stream,the audio stream  is mp3 format.
*               If input "PlayS=wav",  that will start playing an audio stream,the audio stream  is wav format.
*               If input "PlayS=amr", that will start playing an audio stream,the audio stream  is amr format.
*               If input "Ql_SetRingToneEnable=0/1",that can open/close incoming ring.
*               If input "StopS", that will stop playing the audio stream.
*               If input "PlayBgSnd=xxx", that will start playing an audio stream with 8 k sample when you are phoning,
*               If input "StopBgSnd", that will stop playing an audio stream with 8 k sample.
*               If input "audio path=x", that will change audio path,x range: 0~2.  
*               If input "audio path?", that will retrive current audio path.
*               If input "volume=x", that will change audio volume,x range: 0~100.   
*               If input "volume?", that will retrive current audio volume.
*               If input "gain=x,y", that will change the micphone gain level,x range:0~2,y range: 0~15.    
*               If input "mic gain?", that will retrive current micphone gain level.
*               If input "audio src=xxx(0..29)", that will play built-in audio tone.
*               If input "stop audsrc=xxx(0..29)", that will stop playing built-in audio tone.
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
#include "ql_timer.h"
#include "ql_stdlib.h"
#include "ql_filesystem.h"
#include "ql_error.h"
#include "resource_audio.h"
#include "ql_fcm.h"

char textBuf[100];
char notes[100];
s32 filehandle = -1;
static u8 audPath = QL_AUDIO_PATH_NORMAL;

void ql_entry()
{
    bool           keepGoing = TRUE;
    QlEventBuffer  flSignalBuffer;
    //s32 freesize;
    char *p=NULL;
    char *pData= NULL;
    s32 ret = 0;

    Ql_SetDebugMode(BASIC_MODE);   
    Ql_DebugTrace("audio: ql_entry\r\n");   
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

                /*set last tail data eq 0*/
                pData[flSignalBuffer.eventData.uartdata_evt.len+1] = '\0';
				
                /*cmd:  Play=xxx.mp3*/
                /*cmd:  Play=xxx.wav*/
                /*cmd:  Play=xxx.amr*/
                /* Without '\r\n' */
                p = Ql_strstr(pData,"PlayFile=");
                if (p)
                {
                    s32 ret;
                    char* p1 = NULL;
                    char* p2 = NULL;
                    p1 = Ql_strstr((char*)pDataEvt->data, "=");
                    p2 = Ql_strstr((char*)pDataEvt->data, "\r\n");
                    Ql_memset(notes, 0x0, sizeof(notes));
                    Ql_memcpy(notes, p1 + 1, p2 - p1 -1);
                    if ( NULL == p1 || NULL == p2)
                    {
                        Ql_DebugTrace("Invalid parameter\r\n");
                        break;
                    }
                    ret = Ql_StartPlayAudioFile((u8*)notes , FALSE, QL_AUDIO_VOLUME_LEVEL6, audPath);
                    if(ret == QL_RET_OK)
                        ;//play ok
                    Ql_DebugTrace((char*)pDataEvt->data);
                    Ql_sprintf(textBuf, "Ql_StartPlayAudioFile(%s)=%d\r\n", notes, ret);
                    Ql_SendToUart(ql_uart_port1, (u8*)textBuf, Ql_strlen(textBuf));    
                    break;
                }

                /*cmd:  Stop*/
                p = Ql_strstr(pData,"StopFile");
                if (p)
                {
                    s32 ret;
                    ret = Ql_StopPlayAudioFile();
                    if(ret == QL_RET_OK)
                        ;//stop ok
                    Ql_sprintf(textBuf,"Ql_StopPlayAudioFile()=%d\r\n", ret);
                    Ql_SendToUart(ql_uart_port1, (u8*)textBuf,Ql_strlen(textBuf));    
                    break;
                }

                /*cmd:  PlayS=mp3*/
                /*cmd:  PlayS=wav*/
                /*cmd:  PlayS=amr*/
                p = Ql_strstr(pData,"PlayS=mp3");
                if (p)
                {
                    s32 ret;
                    p += 6;
                    ret = Ql_StartPlayAudioStream((u8*)mp344k_mp3,sizeof(mp344k_mp3), 
                        QL_AUDIO_STREAMFORMAT_MP3, 
                        TRUE, 
                        QL_AUDIO_VOLUME_LEVEL7, 
                        audPath
                        );
                    if(ret == QL_RET_OK)
                        ;//play ok
                    Ql_sprintf(textBuf,"Ql_StartPlayAudioStream(mp344k_mp3,sizeof=%d)=%d\r\n",sizeof(mp344k_mp3), ret);
                    Ql_SendToUart(ql_uart_port1, (u8*)textBuf,Ql_strlen(textBuf));    
                    break;
                }

                /*cmd:  PlayS=wav*/
                p = Ql_strstr(pData,"PlayS=wav");
                if (p)
                {
                    s32 ret;
                    p += 6;
                    ret = Ql_StartPlayAudioStream((u8*)wav11k_wav, sizeof(wav11k_wav), 
                        QL_AUDIO_STREAMFORMAT_WAV, 
                        TRUE, 
                        QL_AUDIO_VOLUME_LEVEL6, 
                        audPath
                        );
                    if(ret == QL_RET_OK)
                        ;//play ok
                    Ql_sprintf(textBuf,"Ql_StartPlayAudioStream(wav11k_wav,sizeof=%d)=%d\r\n",sizeof(wav11k_wav), ret);
                    Ql_SendToUart(ql_uart_port1, (u8*)textBuf,Ql_strlen(textBuf));    
                    break;
                }

                /*cmd:  PlayS=amr*/
                p = Ql_strstr(pData,"PlayS=amr");
                if (p)
                {
                    s32 ret;
                    p += 6;
                    ret = Ql_StartPlayAudioStream((u8*)amr44k_amr, sizeof(amr44k_amr),
                        QL_AUDIO_STREAMFORMAT_AMR,
                        FALSE, 
                        QL_AUDIO_VOLUME_LEVEL6, 
                        audPath
                        );
                    if(ret == QL_RET_OK)
                        ;//play ok
                    Ql_sprintf(textBuf,"Ql_StartPlayAudioStream(amr44k_amr,sizeof=%d)=%d\r\n",sizeof(amr44k_amr), ret);
                    Ql_SendToUart(ql_uart_port1, (u8*)textBuf,Ql_strlen(textBuf));    
                    break;
                }

                /*cmd:  Ql_SetRingToneEnable=0/1*/
                p = Ql_strstr(pData,"Ql_SetRingToneEnable=");
                if (p)
                {
                     s32 ret;
                     QlRingToneEnable ring_enable;
                     p = Ql_strstr(pData,"=");
                     p +=1;
                    ring_enable =Ql_atoi(p);
                     ret = Ql_SetRingToneEnable(ring_enable);
                    Ql_sprintf(textBuf,"Ql_SetRingToneEnable()=%d\r\n", ret);
                    Ql_SendToUart(ql_uart_port1, (u8*)textBuf,Ql_strlen(textBuf));    
                    break;
                }

                /*cmd:  StopS*/
                p = Ql_strstr(pData,"StopS");
                if (p)
                {
                    s32 ret;
                    ret = Ql_StopPlayAudioStream();
                    if(ret == QL_RET_OK)
                        ;//stop ok
                    Ql_sprintf(textBuf,"Ql_StopPlayAudioStream()=%d\r\n", ret);
                    Ql_SendToUart(ql_uart_port1, (u8*)textBuf,Ql_strlen(textBuf));    
                    break;
                }

                /*cmd:  tone*/
                p = Ql_strstr(pData,"playtone");
                if (p)
                {
                    s32 ret;
                    ret = Ql_PlayTone(450,400,100,3000);
                    //if(ret == QL_RET_OK)
                    //    ;
                    Ql_sprintf(textBuf,"Ql_PlayTone()=%d\r\n", ret);
                    Ql_SendToUart(ql_uart_port1, (u8*)textBuf,Ql_strlen(textBuf));    
                    break;
                }

                /*cmd:  stoptone*/
                p = Ql_strstr(pData,"stoptone");
                if (p)
                {
                    s32 ret;
                    ret = Ql_StopTone();                    
                    Ql_sprintf(textBuf,"Ql_StopTone()=%d\r\n", ret);
                    Ql_SendToUart(ql_uart_port1, (u8*)textBuf,Ql_strlen(textBuf));    
                    break;
                }


                p = Ql_strstr(pData, "PlayBgSnd=");
                if(p)
                {
                    s32 ret;
                    p += Ql_strlen("PlayBgSnd=");
                    //ret = Ql_PlayAudioFile_8k((u8*)p, 1, 6, 1, 6);
                    ret = Ql_PlayAudioFile_8k((u8*)p, 1, 6, audPath, 6);
                    Ql_sprintf(textBuf,"Ql_PlayAudioFile_8k()=%d\r\n", ret);
                    Ql_SendToUart(ql_uart_port1, (u8*)textBuf,Ql_strlen(textBuf));    
                    break;
                }

                p = Ql_strstr(pData, "StopBgSnd");
                if(p)
                {
                    s32 ret;
                    ret = Ql_StopAudioFile_8k();
                    Ql_sprintf(textBuf,"Ql_StopAudioFile_8k()=%d\r\n", ret);
                    Ql_SendToUart(ql_uart_port1, (u8*)textBuf,Ql_strlen(textBuf));    
                    break;
                }

                p = Ql_strstr(pData, "selectetone=");
                if(p)
                {
                    s32 ret;
                    ret = Ql_SelectRingTone(Ql_atoi(p+Ql_strlen("selectetone=")));
                    Ql_sprintf(textBuf,"Ql_SelectRingTone()=%d\r\n", ret);
                    Ql_SendToUart(ql_uart_port1, (u8*)textBuf,Ql_strlen(textBuf));    
                    break;
                }

                p = Ql_strstr(pData,"startrecord=");
                if (p)
                {
                    QlAudioRecordFormat fmt = QL_AUD_REC_FMT_BY_EXTENSION;
                    char* p1 = NULL;
                    char* p2 = NULL;
                    p1 = p+Ql_strlen("startrecord=");                     
                    p2 = Ql_strstr(p1, ",");
                    if(!p2)                    
                    {
                        Ql_DebugTrace("Invalid parameter\r\n");
                        break;
                    }
                    *p2 = '\0';
                    Ql_strncpy(notes, p1, 100);
                    p2++;
                    if(p2 && (p1 = Ql_strstr(p2, "\r")))
                    {
                        *p1 = '\0';
                        fmt = (QlAudioRecordFormat)Ql_atoi(p2);
                    }
                    ret = Ql_StartRecord(notes, fmt);
                    
                    Ql_DebugTrace((char*)pDataEvt->data);
                    Ql_sprintf(textBuf, "Ql_StartRecord(%s,%d)=%d\r\n", notes, fmt, ret);
                    Ql_SendToUart(ql_uart_port1, (u8*)textBuf, Ql_strlen(textBuf));    
                    break;
                }

                p = Ql_strstr(pData, "stoprecord");
                if(p)
                {
                    ret = Ql_StopRecord();
                    Ql_sprintf(textBuf, "Ql_StopRecord()=%d\r\n", ret);
                    Ql_SendToUart(ql_uart_port1, (u8*)textBuf, Ql_strlen(textBuf));    
                    break;
                }

                p = Ql_strstr(pData, "record_fullfunc");
                if(p)
                {
                    ret = Ql_StartRecord_FullFunc("tst_fullf.amr", QL_AUD_REC_FMT_AMR, QL_AUD_REC_QUALITY_LOW, TRUE, 0, 10<<10, 10);
                    Ql_sprintf(textBuf, "Ql_StartRecord_FullFunc(...)=%d\r\n", ret);
                    Ql_SendToUart(ql_uart_port1, (u8*)textBuf, Ql_strlen(textBuf));    
                    break;
                }
                    
                // Change audio path
                Ql_memset(textBuf, 0x0, sizeof(textBuf));
                Ql_sprintf(textBuf, "audio path=");
                pChar = Ql_strstr((char*)pDataEvt->data, textBuf);
                if (pChar)
                {
                    char* p1;
                    char* p2;
                    Ql_DebugTrace((char*)pDataEvt->data);
                    p1 = Ql_strstr((char*)pDataEvt->data, "=");
                    p2 = Ql_strstr((char*)pDataEvt->data, "\r\n");
                    if ( NULL == p1 || NULL == p2)
                    {
                        Ql_DebugTrace("Invalid parameter\r\n");
                        break;
                    }
                    Ql_memset(notes, 0x0, sizeof(notes));
                    Ql_memcpy(notes, p1 + 1, p2 - p1 -1);
                    if (Ql_strcmp(notes, "1") == 0)
                    {
                        audPath = QL_AUDIO_PATH_HEADSET;
                    }
                    else if (Ql_strcmp(notes, "2") == 0)
                    {
                        audPath = QL_AUDIO_PATH_LOUDSPEAKER;
                    }
                    else
                    {
                        audPath = QL_AUDIO_PATH_NORMAL;
                    }
                    if (!Ql_VoiceCallChangePath((QlAudioPlayPath)audPath))
                    {
                        Ql_DebugTrace("Fail to change audio path.\r\n");
                    }
                    else
                    {
                        Ql_DebugTrace("Change audio path to %d.\r\n", audPath);
                    }
                    break;
                }

                // Retrive current audio path
                Ql_memset(textBuf, 0x0, sizeof(textBuf));
                Ql_sprintf(textBuf, "audio path?\r\n");
                ret = Ql_strncmp((char*)pDataEvt->data, textBuf, Ql_strlen(textBuf));
                if (0 == ret)
                {
                    u8 aud_path;
                    Ql_DebugTrace(textBuf);
                    Ql_memset(textBuf, 0x0, sizeof(textBuf));
                    aud_path = Ql_VoiceCallGetCurrentPath();
                    Ql_DebugTrace("Current audio path: %d.\r\n", aud_path);
                    break;
                }
                
                // Play Audio tone
                Ql_memset(textBuf, 0x0, sizeof(textBuf));
                Ql_sprintf(textBuf, "audio src=");
                pChar = Ql_strstr((char*)pDataEvt->data, textBuf);
                if (pChar)
                {
                    u8 audSrc;
                    char* p1;
                    char* p2;
                    Ql_DebugTrace((char*)pDataEvt->data);
                    p1 = Ql_strstr((char*)pDataEvt->data, "=");
                    p2 = Ql_strstr((char*)pDataEvt->data, "\r\n");
                    Ql_memset(textBuf, 0x0, sizeof(textBuf));
                    memcpy(textBuf, p1 + 1, p2 - p1 -1);
                    audSrc = Ql_atoi(textBuf);
                    ret = Ql_PlayAudio(audSrc, FALSE);
                    Ql_DebugTrace("Ql_PlayAudio(%d)=%d\r\n", audSrc, ret);
                    break;
                }

                // Stop Playing Audio Tone
                Ql_memset(textBuf, 0x0, sizeof(textBuf));
                Ql_sprintf(textBuf, "stop audsrc=");
                pChar = Ql_strstr((char*)pDataEvt->data, textBuf);
                if (pChar)
                {
                    u8 audSrc;
                    char* p1;
                    char* p2;
                    Ql_DebugTrace((char*)pDataEvt->data);
                    p1 = Ql_strstr((char*)pDataEvt->data, "=");
                    p2 = Ql_strstr((char*)pDataEvt->data, "\r\n");
                    Ql_memset(textBuf, 0x0, sizeof(textBuf));
                    memcpy(textBuf, p1 + 1, p2 - p1 -1);
                    audSrc = Ql_atoi(textBuf);
                    ret = Ql_StopAudio(audSrc);
                    Ql_DebugTrace("Ql_StopAudio(%d)=%d\r\n", audSrc, ret);
                    break;
                }
                
                // Set volume
                Ql_memset(textBuf, 0x0, sizeof(textBuf));
                Ql_sprintf(textBuf, "volumeex=");
                pChar = Ql_strstr((char*)pDataEvt->data, textBuf);
                if (pChar)
                {
                    pChar = Ql_strstr((char*)pDataEvt->data, "=");
                    if (pChar)
                    {
                        //int i;
                        char* p2 = NULL;
                        u8 vol, path, type;
                        
                        pChar ++;
                        p2 = Ql_strstr(pChar, ",");
                        if(p2)
                        {
                            *p2 = '\0';
                            type = Ql_atoi(pChar);
                            p2 ++;
                            pChar = Ql_strstr(p2, ",");
                            if(pChar)
                            {
                                *pChar = '\0';
                                pChar++;
                                path = Ql_atoi(p2);
                                p2 = Ql_strstr(pChar, "\r\n");
                                if(p2)
                                {
                                    *p2 = '\0';
                                    vol = Ql_atoi(pChar);
                                }
                            }
                        }
                        ret = Ql_SetVolume_Ex(type, path, vol);
                        Ql_DebugTrace("Ql_SetVolume_Ex(%d,%d,%d) = %d;\r\n", type, path, vol, ret);
                        
                        #if 0
                        p2 = Ql_strstr((char*)pDataEvt->data, "\r\n");
                        Ql_memcpy(textBuf, pChar + 1, p2 - pChar -1);
                        vol = Ql_atoi(textBuf);
                        ret = Ql_SetVolume_Ex(VOL_TYPE_MEDIA, QL_AUDIO_PATH_HEADSET, vol);
                        Ql_DebugTrace("Ql_SetVolume_Ex(VOL_TYPE_MEDIA, %d) = %d;\r\n", vol, ret);
                        #endif
                        /*
                        ret = Ql_SetVolume(VOL_TYPE_SPH, vol);
                        Ql_DebugTrace("Ql_SetVolume(VOL_TYPE_SPH, %d) = %d;\r\n", vol, ret);
                        Ql_sprintf(textBuf, "AT+CLVL?\n");
                        Ql_SendToModem(ql_md_port1, (u8*)textBuf, Ql_strlen(textBuf));
                        */
                    }
                    break;
                }

                Ql_memset(textBuf, 0x0, sizeof(textBuf));
                Ql_sprintf(textBuf, "volume=");
                pChar = Ql_strstr((char*)pDataEvt->data, textBuf);
                if (pChar)
                {
                    pChar = Ql_strstr((char*)pDataEvt->data, "=");
                    if (pChar)
                    {
                        //int i;
                        char* p2 = NULL;
                        u8 vol, type;
                        
                        pChar ++;
                        p2 = Ql_strstr(pChar, ",");
                        if(p2)
                        {
                            *p2 = '\0';
                            type = Ql_atoi(pChar);
                            p2 ++;
                            pChar = Ql_strstr(p2, "\r\n");
                            if(pChar)
                            {
                                *pChar = '\0';
                                
                                vol = Ql_atoi(p2);                                
                            }
                        }
                        ret = Ql_SetVolume(type, vol);
                        Ql_DebugTrace("Ql_SetVolume(%d,%d) = %d;\r\n", type, vol, ret);   
                    }
                    break;
                }
               // Get volume
                Ql_memset(textBuf, 0x0, sizeof(textBuf));
                Ql_sprintf(textBuf, "volume?\r\n");
                ret = Ql_strncmp((char*)pDataEvt->data, textBuf, Ql_strlen(textBuf));
                if (0 == ret)
                {
                    u8 i;
                    u8 vol_level;
                    Ql_DebugTrace(textBuf);
                    Ql_memset(textBuf, 0x0, Ql_strlen(textBuf));
                    #if 0
                    vol_level = Ql_GetVolume(VOL_TYPE_MIC);
                    Ql_DebugTrace("Ql_GetVolume(VOL_TYPE_MIC) = %d.\r\n", vol_level);
                    vol_level = Ql_GetVolume(VOL_TYPE_SPH);
                    Ql_DebugTrace("Ql_GetVolume(VOL_TYPE_SPH) = %d.\r\n", vol_level);
                    #else
                    for (i = 0; i < MAX_VOL_TYPE; i++)
                    {
                        vol_level = Ql_GetVolume(i);
                        Ql_DebugTrace("Ql_GetVolume(%d) = %d.\r\n", i, vol_level);
                    }
                    #endif
                    
                    break;
                }
               
                // Set MIC gain (gain=channel,gain)
                Ql_memset(textBuf, 0x0, sizeof(textBuf));
                Ql_sprintf(textBuf, "gain=");
                pChar = Ql_strstr((char*)pDataEvt->data, textBuf);
                if (pChar)
                {
                    pChar = Ql_strstr((char*)pDataEvt->data, "=");
                    if (pChar)
                    {
                        char* p2;
                        u8 channel;
                        u8 micGain;
                        
                        p2 = Ql_strstr((char*)pDataEvt->data, ",");
                        Ql_memcpy(textBuf, pChar + 1, p2 - pChar -1);
                        channel = Ql_atoi(textBuf);
                        
                        pChar = p2;
                        p2 = Ql_strstr((char*)pDataEvt->data, "\r\n");
                        Ql_memcpy(textBuf, pChar + 1, p2 - pChar -1);
                        micGain = Ql_atoi(textBuf);
                        
                       ret = Ql_SetMicGain(channel, micGain);
                        //Ql_SetSideToneGain(channel, micGain);
                        if(QL_RET_OK == ret)
                        {
                            Ql_DebugTrace("Set mic gain at channel %d: %d.\r\n", channel, micGain);
                        }
                        else
                        {
                            Ql_DebugTrace("Set mic gain failed %d.\r\n",ret);
                        }
                    }
                    break;
                }

                // Get MIC gain
                Ql_memset(textBuf, 0x0, sizeof(textBuf));
                Ql_sprintf(textBuf, "mic gain?\r\n");
                ret = Ql_strncmp((char*)pDataEvt->data, textBuf, Ql_strlen(textBuf));
                if (0 == ret)
                {
                    u8 micGain;
                    Ql_DebugTrace(textBuf);
                    Ql_memset(textBuf, 0x0, Ql_strlen(textBuf));
                    #if 1
                    micGain = Ql_GetMicGain(QL_AUDIO_PATH_NORMAL);
                    Ql_DebugTrace("Mic gain at channel %d: %d.\r\n", QL_AUDIO_PATH_NORMAL, micGain);
                    micGain = Ql_GetMicGain(QL_AUDIO_PATH_HEADSET);
                    Ql_DebugTrace("Mic gain at channel %d: %d.\r\n", QL_AUDIO_PATH_HEADSET, micGain);
                    micGain = Ql_GetMicGain(QL_AUDIO_PATH_LOUDSPEAKER);
                    Ql_DebugTrace("Mic gain at channel %d: %d.\r\n", QL_AUDIO_PATH_LOUDSPEAKER, micGain);
                    #else
                    micGain = Ql_GetSideToneGain(QL_AUDIO_PATH_NORMAL);
                    Ql_DebugTrace("Mic gain at channel %d: %d.\r\n", QL_AUDIO_PATH_NORMAL, micGain);
                    micGain = Ql_GetSideToneGain(QL_AUDIO_PATH_HEADSET);
                    Ql_DebugTrace("Mic gain at channel %d: %d.\r\n", QL_AUDIO_PATH_HEADSET, micGain);
                    micGain = Ql_GetSideToneGain(QL_AUDIO_PATH_LOUDSPEAKER);
                    Ql_DebugTrace("Mic gain at channel %d: %d.\r\n", QL_AUDIO_PATH_LOUDSPEAKER, micGain);
                    #endif
                    break;
                }                

                // transpass
                Ql_SendToModem(ql_md_port1, (u8*)pData, flSignalBuffer.eventData.uartdata_evt.len);
                break;
            }
            case EVENT_MODEMDATA:
            {
                Ql_SendToUart(ql_uart_port1, (u8*)flSignalBuffer.eventData.modemdata_evt.data,
                                            flSignalBuffer.eventData.modemdata_evt.len);
                break;
            }
            case EVENT_MEDIA_FINISH:
            {
                Ql_sprintf(textBuf, "type=%d, reason=%d\r\n", flSignalBuffer.eventData.mediafinish_evt.media_type, flSignalBuffer.eventData.mediafinish_evt.reason);
                Ql_SendToUart(ql_uart_port1, (u8*)textBuf, Ql_strlen(textBuf));
                break;
            }
            default:
                break;
        }
    }
}

#endif //__EXAMPLE_AUDIO__

