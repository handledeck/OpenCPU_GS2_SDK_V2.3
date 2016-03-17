#ifdef __EXAMPLE_CALL__

/***************************************************************************************************
*   Example:
*       
*           Call Routine
*
*   Description:
*
*           This example demonstrates how to use call function with APIs in OpenCPU.
*           Through MAIN Uart port, input the specified command, there will be given the 
*           response about call operation.
*           
*   Usage:
*
*           Compile & Run:
*
*                   Use "make call" to compile, and download bin image to module.
*           
*           Through Uart port:
*
*               If input "ATDxxx;", that will make a voice call,xxx is phone number."ATD" should be capital and don't forget semicolon in the end.
*               If input "ATDxxx",  that will make a data  call (CSD call).
*               If have a incoming call,"Callback_Ring(): xxx\r\nRING" will be displayed in the main port.
*               If input "answer", that will answer an incoming call.
*               If input "hang up",  that will hang up a call.
*               If input "Get IMEI",  that will get the IMEI of GSM.
*               If input "Get IMSI",  that will get the IMSI of GSM..
*               If input "Get version",  that will get the version ID of the core.
*               If input "Check SIM Card",  that will check SIM Card inserted or not.
*               If input "call count?",  that will get the current call count by call type.
*               If input "audio path=x",  that will change audio path,x range: 0~2.  
*               If input "audio path?",  that will retrive current audio path.
*               If input "volume=x",  that will change audio volume,x range: 0~100.   
*               If input "volume?",  that will retrive current audio volume.
*               If input "gain=x,y", that will change the micphone gain level,x range:0~2,y range: 0~15.    
*               If input "mic gain?", that will retrive current micphone gain level.
*               If input "echo",  that control echo cancellation.
*               If input "vts=xxxx",  that will play DTMF tones and arbitrary tones to listen to the other side of the phone.
*               If input "qstk=xxxx",  that will play TTS Text as background audio during calling.
*               If input "operator?",  that will get the current operator name.
*               If input "device state?",  that will retrieve the current run-state,including SIM card state, 
*                                                  network registion state,GPRS network registion state, signal strength and bit error rate.
*               If input "loc?",  that will get location information of module..
*               If input "loc ex?",  that will get address information of module by the input cell information.
*               If input "loc by coor?",  that will get address information of module by the input coordinate information.
*               If input "set addr lang",  that will set language and charset for the responsed address information..
*               If input "StartJammingDet",  that will start Jamming Detection.
*               If input "StopJammingDet",  that will stop Jamming Detection.
*               If input "Play=xxx", that will start playing an Audio file,xxx is Audio file name.
*               If input "Stop",  that will stop playing the Audio file.
*               If input "play 8k",  that will start playing an audio stream with 8k HZ sample when you are phoning.
*               If input "stop 8k",  that will stop playing an audio stream with 8k HZ sample.
*               If input "Ql_WDTMF=x,x",  that will play DTMF tones and arbitrary tones to listen to both sides of the phone.
*               If input "Ql_Call_GetCurrCall",  that will get current calls of ME.
*               If input "Ql_EnableDTMF",  that will enable dtmf detecting.
*               If input "Ql_DisableDTMF",  that will disable dtmf detecting..
*               If input "Ql_GetDTMFConfig=x",  that will get DTMF config.
*               If input "Ql_SetDTMFConfig=x,x,x,x",  that will set DTMF config.
*           
****************************************************************************************************/

#include "ql_appinit.h"
#include "ql_interface.h"
#include "ql_type.h"
#include "ql_timer.h"
#include "ql_stdlib.h"
#include "ql_fcm.h"
#include "ql_trace.h"
#include "ql_call.h"
#include "ql_audio.h"
#include "ql_sms.h"
#include "ql_tts.h"
#include "ql_tcpip.h"
#include "ql_error.h"
#include "ql_sim.h"

#define OUT_DEBUG(x,...)  \
    Ql_memset((void*)(x),0,100);  \
    Ql_sprintf((char*)(x),__VA_ARGS__);   \
    Ql_SendToUart(ql_uart_port1,(u8*)(x),Ql_strlen((const char*)(x)));

QlEventBuffer g_event; // Keep this variable a global variable due to its big size
char notes[100];
char buffer[100];
u8   codec[200];
char testdata[1024] = {0x9};
static u8 g_cnt = 0;
static u8 g_addrLang = LANG_ENG;
extern void *memcpy(void *dest, const void *src, s32 count);
void Callback_Dial(s32 result);
void Callback_Ring(u8* coming_call_num);
void Callback_Hangup(void);
void Callback_TTS_Play(s32 result);
void Callback_RetrieveBSInfo(QL_STNodeB_Info* bsInfo);
void Callback_JammingDet(s32 result);
void Callback_Call_Info(QlCallParam *call_param);
void Callback_DTMF_Info(QlDTMFCode dtmfcode);
void callback_PIN(s8 SIM_type);

Ql_STCall_Callback call_cb = 
{
    Callback_Dial,
    Callback_Ring,
    Callback_Hangup
};
void ql_entry(void)
{
    s32 ret;
    bool bRet;
    bool keepGoing = TRUE;
    QlTimer tm;
    u32 cnt = 0; 
    s8 locktype;
    
    u32 t1, t2;
    char AtCmd[] = "AT+CPIN?\r";
    
    Ql_SetDebugMode(BASIC_MODE);    /* Only after the device reboots, 
                                     *  the set debug-mode takes effect. 
                                     */
    OUT_DEBUG(notes,"OpenCPU: Call !\r\n\r\n");    /* Print out message through DEBUG port */
    Ql_OpenModemPort(ql_md_port1);  /* or ql_md_port2, two virtual modem ports are available.*/
#if 1
    //Ql_UartClrRxBuffer(ql_uart_port3);
    Ql_SetPortOwner(ql_uart_port3, ql_main_task);
    Ql_SetUartDCBConfig(ql_uart_port3, 9600, 8, 1, 0);
#endif
    // Start a timer
    //tm.timeoutPeriod = Ql_SecondToTicks(2); /* Set the interval of timer */
    //Ql_StartTimer(&tm);
    //OUT_DEBUG(notes,"The timer starts.\r\n\r\n");

    Ql_Call_Initialize(&call_cb);
    Ql_TTS_Initialize(&Callback_TTS_Play);

    Ql_Sleep(2000);
    ret = Ql_SIM_GetLockState(&locktype);
    OUT_DEBUG(notes,"Ql_SIM_GetLockState=%d locktype =%d \r\n\r\n" ,ret,locktype); 

    while(keepGoing)
    {
        Ql_GetEvent(&g_event);
        switch(g_event.eventType)
        {
            case EVENT_INTR:
            {
                //TODO: add your codes for interruption handling here 
                Intr_Event* pIntrEvt = (Intr_Event *)&g_event.eventData.intr_evt;
                OUT_DEBUG(notes,"pinName = %d, pinState = %d", pIntrEvt->pinName, (u8)pIntrEvt->pinState);
                break;
            }
            case EVENT_UARTDATA:
            {
                //TODO: receive and handle data from UART
                u8 temp;
                char* pChar=NULL;

                PortData_Event* pDataEvt = (PortData_Event*)&g_event.eventData.uartdata_evt;
                if (ql_uart_port3 == pDataEvt->port)
                {// gps data
                    OUT_DEBUG(notes,"UART3: %s", (char*)pDataEvt->data);
                    break;
                }
                // feed PIN code
                pChar = Ql_strstr((char*)pDataEvt->data, "PIN=");
                if (pChar == (char*)pDataEvt->data)
                {
                    char* p1=NULL;
                    char Code_buffer[10];
                    p1 = Ql_strstr((char*)pDataEvt->data, "\r\n");
                    Ql_memset(Code_buffer, 0x00,10);
                    Ql_memcpy(Code_buffer,pChar+4,p1-(pChar+4));
                    OUT_DEBUG(notes,"Feed PIN code =");
                    ret =  Ql_SIM_FeedPIN(QL_ID_SIM_PIN1, (u8 *)Code_buffer,callback_PIN);
                    OUT_DEBUG(notes,"Feed PIN code =%s ret =%d \r\n" ,(char *)Code_buffer,ret);
                    break;
                }
                // feed PUK code. note: if feed PUK1 success,the PIN1 code will be set to "1234" .
                pChar = Ql_strstr((char*)pDataEvt->data, "PUK=");
                if (pChar == (char*)pDataEvt->data)
                {
                    char* p1=NULL;
                    char Code_buffer[10];
                    p1 = Ql_strstr((char*)pDataEvt->data, "\r\n");
                    Ql_memset(Code_buffer, 0x00,10);
                    Ql_memcpy(Code_buffer,pChar+4,8);//
                    ret =  Ql_SIM_FeedPIN(QL_ID_SIM_PUK1, (u8 *)Code_buffer,callback_PIN);                            
                    OUT_DEBUG(notes,"Feed PUK code =%s ret =%d \r\n" ,(char *)Code_buffer,ret);
                    break;
                }

                pChar = Ql_strstr((char*)pDataEvt->data, "PINCntAvail");
                if (pChar == (char*)pDataEvt->data)
                {
                    u8 pin_cnt, puk_cnt;
                    ret =  Ql_SIM_GetPINCntAvail(&pin_cnt, &puk_cnt);
                    OUT_DEBUG(notes,"pin_cnt =%d,puk_cnt =%d, ret =%d\r\n" ,pin_cnt, puk_cnt, ret);
                    break;
                }

                Ql_memset(buffer, 0x0, sizeof(buffer));
                Ql_sprintf(buffer, "tst1\r\n");
                ret = Ql_strncmp((char*)pDataEvt->data, buffer, Ql_strlen(buffer));
                if (0 == ret) {                    
                    OUT_DEBUG(notes,"tst1111\r\n");
                    bRet = Ql_VoiceCallChangePath(QL_AUDIO_PATH_LOUDSPEAKER);
                    if (!bRet)
                    {
                        OUT_DEBUG(notes,"Fail to Ql_VoiceCallChangePath()\r\n");
                        break;
                    }
                    //Ql_Sleep(30);
                    
                    ret = Ql_SetVolume(VOL_TYPE_SPH, 80);
                    if (ret != QL_RET_OK)
                    {
                        OUT_DEBUG(notes,"Fail to Ql_SetVolume(): %d\r\n", ret);
                        break;
                    }
                    //Ql_Sleep(30);
                    
                    ret = Ql_SetMicGain(QL_AUDIO_PATH_LOUDSPEAKER, 8);
                    if (ret != QL_RET_OK)
                    {
                        OUT_DEBUG(notes,"Fail to Ql_SetMicGain(): %d\r\n", ret);
                        break;
                    }
                    //Ql_Sleep(30);
                    
                    ret = Ql_CtrlEchoCancel(224, 2000, 30000, 374, QL_AUDIO_PATH_LOUDSPEAKER);
                    if (ret != QL_RET_OK)
                    {
                        OUT_DEBUG(notes,"Fail to Ql_CtrlEchoCancel(): %d\r\n", ret);
                        break;
                    }
                    //Ql_Sleep(30);
                    
                    ret = Ql_SetSideToneGain(QL_AUDIO_PATH_LOUDSPEAKER, 8);
                    if (ret != QL_RET_OK)
                    {
                        OUT_DEBUG(notes,"Fail to Ql_SetSideToneGain(): %d\r\n", ret);
                        break;
                    }
                    //Ql_Sleep(30);
                    
                    break;
                }
                
                Ql_memset(buffer, 0x0, sizeof(buffer));
                Ql_sprintf(buffer, "tst2\r\n");
                ret = Ql_strncmp((char*)pDataEvt->data, buffer, Ql_strlen(buffer));
                if (0 == ret) {
                    OUT_DEBUG(notes,"tst2222\r\n");
                    
                    Ql_VoiceCallChangePath(QL_AUDIO_PATH_NORMAL);
                    OUT_DEBUG(notes,"Ql_VoiceCallChangePath(QL_AUDIO_PATH_NORMAL);\r\n");
                    //Ql_Sleep(30);
                    
                    ret = Ql_SetVolume(VOL_TYPE_SPH, 80);
                    OUT_DEBUG(notes,"Ql_SetVolume(VOL_TYPE_SPH, 80)=%d\r\n", ret);
                    //Ql_Sleep(30);

                    ret = Ql_SetMicGain(QL_AUDIO_PATH_NORMAL, 8);
                    OUT_DEBUG(notes,"Ql_SetMicGain(QL_AUDIO_PATH_NORMAL, 8)=%d\r\n", ret);
                    //Ql_Sleep(30);
                    
                    ret = Ql_CtrlEchoCancel(224, 2000, 30000, 374, QL_AUDIO_PATH_NORMAL);
                    OUT_DEBUG(notes,"Ql_CtrlEchoCancel(224, 2000, 30000, 374, QL_AUDIO_PATH_NORMAL)=%d\r\n", ret);
                    //Ql_Sleep(30);
                    
                    ret = Ql_SetSideToneGain(QL_AUDIO_PATH_NORMAL, 8);
                    OUT_DEBUG(notes,"Ql_SetSideToneGain(QL_AUDIO_PATH_NORMAL, 8)=%d\r\n", ret);
                    //Ql_Sleep(30);
                                        
                    break;
                }


                // Init (init\r\n)

                Ql_memset(buffer, 0x0, sizeof(buffer));
                Ql_sprintf(buffer, "init\r\n");
                ret = Ql_strncmp((char*)pDataEvt->data, buffer, Ql_strlen(buffer));
                if (0 == ret)
                {
                    Ql_Call_Initialize(&call_cb);
                    OUT_DEBUG(notes,buffer);
                    break;
                }
                
                // Dial (ATD10086;\r\n)
                pChar = Ql_strstr((char*)pDataEvt->data, "ATD");
                if (pChar == (char*)pDataEvt->data)
                {
                #if 1
                    char* p1;
                    pChar = (char*)pDataEvt->data;
                    p1 = Ql_strstr((char*)pDataEvt->data, ";");
                    if (p1)
                    {
                        OUT_DEBUG(notes,"p2 != NULL\r\n");
                        Ql_memset(buffer, 0x0, Ql_strlen(buffer));
                        memcpy(buffer, (char*)pDataEvt->data + 3, pDataEvt->len - 3 - 3); // Exclude 'ATD' and '\r\n'
                        OUT_DEBUG(notes,"Start to dial.\r\n");
                        ret = Ql_Call_Dial(VOICE_CALL, buffer);
                    }
                    else
                    {
                        //OUT_DEBUG(notes,"Incorrect input.\r\n");
                        //break;
                        p1 = Ql_strstr((char*)pDataEvt->data, "\r\n");
                        OUT_DEBUG(notes,"CSD dial...\r\n");
                        Ql_memset(buffer, 0x0, Ql_strlen(buffer));
                        memcpy(buffer, (char*)pDataEvt->data + 3, pDataEvt->len - 3 - 2); // Exclude 'ATD' and '\r\n'
                        OUT_DEBUG(notes,"Start to dial.\r\n");
                        ret = Ql_Call_Dial(CSD_CALL, buffer);
                    }
                    OUT_DEBUG(notes,"Ql_Call_Dial(%s)=%d\r\n",buffer,ret);
                    if(ret < 0)
                    {
                        OUT_DEBUG(notes,"Ql_Call_Dial Failed\r\n",buffer,ret);                       
                        break;
                    }
                    
                    Ql_VoiceCallChangePath(QL_AUDIO_PATH_HEADSET);
                    Ql_SetVolume(VOL_TYPE_SPH, 80);
                    Ql_SetMicGain(QL_AUDIO_PATH_HEADSET, 8);
                    Ql_CtrlEchoCancel(221, 2000, 30000, 374, QL_AUDIO_PATH_HEADSET);
                    Ql_SetSideToneGain(QL_AUDIO_PATH_HEADSET, 8);
                #else
                
                // ATD...
                t1 = Ql_GetRelativeTime();
                Ql_SendToModem(ql_md_port2, (u8*)pDataEvt->data, pDataEvt->len);
                t2 = Ql_GetRelativeTime();
                OUT_DEBUG(notes,"Time Consumption for Ql_SendToModem('ATD...'): %d.\r\n", t2 - t1);

                #endif
                    break;
                }

                // Answer
                Ql_memset(buffer, 0x0, sizeof(buffer));
                Ql_sprintf(buffer, "answer\r\n");
                ret = Ql_strncmp((char*)pDataEvt->data, buffer, Ql_strlen(buffer));
                if (0 == ret)
                {
                    ret = Ql_Call_Answer();
                    OUT_DEBUG(notes,buffer);
                    OUT_DEBUG(notes,"Ql_Call_Answer()=%d\r\n",ret);                    
                    break;
                }
                
                Ql_memset(buffer, 0x0, sizeof(buffer));
                Ql_sprintf(buffer, "ata\r\n");
                ret = Ql_strncmp((char*)pDataEvt->data, buffer, Ql_strlen(buffer));
                if (0 == ret)
                {
                    Ql_SendToModem(ql_md_port1, buffer, Ql_strlen(buffer));
                    //OUT_DEBUG(notes,buffer);
                    break;
                }
                
                // Hang up
                Ql_memset(buffer, 0x0, sizeof(buffer));
                Ql_sprintf(buffer, "hang up\r\n");
                ret = Ql_strncmp((char*)pDataEvt->data, buffer, Ql_strlen(buffer));
                if (0 == ret)
                {
                    OUT_DEBUG(notes, "<-- Start to hang up the call -->");
                    ret = Ql_Call_Hangup();
                    OUT_DEBUG(notes,buffer);
                    OUT_DEBUG(notes,"Ql_Call_Hangup()=%d\r\n",ret);                    
                    break;
                }
                
                Ql_memset(buffer, 0x0, sizeof(buffer));
                Ql_sprintf(buffer, "ath\r\n");
                ret = Ql_strncmp((char*)pDataEvt->data, buffer, Ql_strlen(buffer));
                if (0 == ret)
                {
                    Ql_SendToModem(ql_md_port1, buffer, Ql_strlen(buffer));
                    //OUT_DEBUG(notes,buffer);
                    break;
                }

                // Get IMEI
                Ql_memset(buffer, 0x0, sizeof(buffer));
                Ql_sprintf(buffer, "imei?\r\n");
                ret = Ql_strncmp((char*)pDataEvt->data, buffer, Ql_strlen(buffer));
                if (0 == ret)
                {
                    OUT_DEBUG(notes,buffer);
                    memset(buffer, 0x0, strlen(buffer));
                    OUT_DEBUG(notes,"Start to get imei.\r\n");
                    if (Ql_GSM_GetIMEI((u8*)buffer, sizeof(buffer)) >= 0)
                    {
                        OUT_DEBUG(notes,buffer);
                    }
                    break;
                }

                // Get IMSI
                Ql_memset(buffer, 0x0, sizeof(buffer));
                Ql_sprintf(buffer, "imsi?\r\n");
                ret = Ql_strncmp((char*)pDataEvt->data, buffer, Ql_strlen(buffer));
                if (0 == ret)
                {
                    OUT_DEBUG(notes,buffer);
                    memset(buffer, 0x0, strlen(buffer));
                    if (Ql_SIM_GetIMSI((u8*)buffer, sizeof(buffer)) >= 0)
                    {
                        OUT_DEBUG(notes,buffer);
                    }
                    break;
                }

                // Get CCID
                Ql_memset(buffer, 0x0, sizeof(buffer));
                Ql_sprintf(buffer, "CCID?\r\n");
                ret = Ql_strncmp((char*)pDataEvt->data, buffer, Ql_strlen(buffer));
                if (0 == ret)
                {
                    OUT_DEBUG(notes,buffer);
                    memset(buffer, 0x0, Ql_strlen(buffer));
                    if (Ql_SIM_GetCCID((u8*)buffer, sizeof(buffer)) >= 0)
                    {
                        OUT_DEBUG(notes,"%s\r\n", buffer);
                    }
                    break;
                }

                // Get version
                Ql_memset(buffer, 0x0, sizeof(buffer));
                Ql_sprintf(buffer, "ver?\r\n");
                ret = Ql_strncmp((char*)pDataEvt->data, buffer, Ql_strlen(buffer));
                if (0 == ret)
                {
                    OUT_DEBUG(notes,buffer);
                    memset(buffer, 0x0, strlen(buffer));
                    if (Ql_GetCoreVer((u8*)buffer, sizeof(buffer)) >= 0)
                    {
                        OUT_DEBUG(notes,buffer);
                    }
                    break;
                }

                // Check SIM Card
                Ql_memset(buffer, 0x0, sizeof(buffer));
                Ql_sprintf(buffer, "sim card?\r\n");
                ret = Ql_strncmp((char*)pDataEvt->data, buffer, Ql_strlen(buffer));
                if (0 == ret)
                {
                    OUT_DEBUG(notes,buffer);
                    memset(buffer, 0x0, strlen(buffer));
                    if (Ql_IsSIMInserted())
                    {
                        OUT_DEBUG(notes,"SIM card is inserted.\r\n");
                    }
                    else
                    {
                        OUT_DEBUG(notes,"SIM card is not inserted.\r\n");
                    }
                    break;
                }
                
                // Get Call Count (call count?)
                Ql_memset(buffer, 0x0, sizeof(buffer));
                //Ql_sprintf(buffer, "Ql_GetCallCntByType(1)\r\n");
                Ql_sprintf(buffer, "call count?\r\n");
                ret = Ql_strncmp((char*)pDataEvt->data, buffer, Ql_strlen(buffer));
                if (0 == ret)
                {
                    temp = Ql_GetCallCntByType(VOICE_CALL);
                    OUT_DEBUG(notes,"voice call count: %d\r\n", temp);
                    break;
                }

                Ql_memset(buffer, 0x0, sizeof(buffer));
                Ql_sprintf(buffer, "Ql_GetCallCntByType(5)\r\n");
                ret = Ql_strncmp((char*)pDataEvt->data, buffer, Ql_strlen(buffer));
                if (0 == ret)
                {
                    temp = Ql_GetCallCntByType(CSD_CALL);
                    OUT_DEBUG(notes,"csd call count: %d\r\n", temp);

                    Ql_SendToModem(ql_md_port1, (u8*)"ATH\r\n", 17);
                    break;
                }

                // Change audio path
                Ql_memset(buffer, 0x0, sizeof(buffer));
                Ql_sprintf(buffer, "audio path=");
                pChar = Ql_strstr((char*)pDataEvt->data, buffer);
                if (pChar)
                {
                    u8 audPath;
                    char* p1;
                    char* p2;
                    OUT_DEBUG(notes,(char*)pDataEvt->data);
                    p1 = Ql_strstr((char*)pDataEvt->data, "=");
                    p2 = Ql_strstr((char*)pDataEvt->data, "\r\n");
                    Ql_memset(notes, 0x0, sizeof(notes));
                    memcpy(notes, p1 + 1, p2 - p1 -1);
                    if (strcmp(notes, "1") == 0)
                    {
                        audPath = QL_AUDIO_PATH_HEADSET;
                    }
                    else if (strcmp(notes, "2") == 0)
                    {
                        audPath = QL_AUDIO_PATH_LOUDSPEAKER;
                    }
                    else
                    {
                        audPath = QL_AUDIO_PATH_NORMAL;
                    }
                    if (!Ql_VoiceCallChangePath(audPath))
                    {
                        OUT_DEBUG(notes,"Fail to change audio path.\r\n");
                    }
                    else
                    {
                        OUT_DEBUG(notes,"Change audio path to %d.\r\n", audPath);
                    }
                    break;
                }

                // Retrive current audio path
                Ql_memset(buffer, 0x0, sizeof(buffer));
                Ql_sprintf(buffer, "audio path?\r\n");
                ret = Ql_strncmp((char*)pDataEvt->data, buffer, Ql_strlen(buffer));
                if (0 == ret)
                {
                    u8 aud_path;
                    OUT_DEBUG(notes,buffer);
                    memset(buffer, 0x0, sizeof(buffer));
                    aud_path = Ql_VoiceCallGetCurrentPath();
                    OUT_DEBUG(notes,"Current audio path: %d.\r\n", aud_path);
                    break;
                }

                // Set volume
                Ql_memset(buffer, 0x0, sizeof(buffer));
                Ql_sprintf(buffer, "volume=");
                pChar = Ql_strstr((char*)pDataEvt->data, buffer);
                if (pChar)
                {
                    pChar = Ql_strstr((char*)pDataEvt->data, "=");
                    if (pChar)
                    {
                        int i;
                        char* p2;
                        u8 vol;
                        p2 = Ql_strstr((char*)pDataEvt->data, "\r\n");
                        Ql_memcpy(buffer, pChar + 1, p2 - pChar -1);
                        vol = Ql_atoi(buffer);
                        ret = Ql_SetVolume_Ex(VOL_TYPE_SPH, QL_AUDIO_PATH_HEADSET, vol);
                        OUT_DEBUG(notes,"Ql_SetVolume_Ex(VOL_TYPE_SPH, %d) = %d;\r\n", vol, ret);
                        /*
                        ret = Ql_SetVolume(VOL_TYPE_SPH, vol);
                        OUT_DEBUG(notes,"Ql_SetVolume(VOL_TYPE_SPH, %d) = %d;\r\n", vol, ret);
                        Ql_sprintf(buffer, "AT+CLVL?\n");
                        Ql_SendToModem(ql_md_port1, (u8*)buffer, Ql_strlen(buffer));
                        */
                    }
                    break;
                }

               // Get volume
                Ql_memset(buffer, 0x0, sizeof(buffer));
                Ql_sprintf(buffer, "volume?\r\n");
                ret = Ql_strncmp((char*)pDataEvt->data, buffer, Ql_strlen(buffer));
                if (0 == ret)
                {
                    u8 i;
                    u8 vol_level;
                    OUT_DEBUG(notes,buffer);
                    memset(buffer, 0x0, strlen(buffer));
                    #if 0
                    vol_level = Ql_GetVolume(VOL_TYPE_MIC);
                    OUT_DEBUG(notes,"Ql_GetVolume(VOL_TYPE_MIC) = %d.\r\n", vol_level);
                    vol_level = Ql_GetVolume(VOL_TYPE_SPH);
                    OUT_DEBUG(notes,"Ql_GetVolume(VOL_TYPE_SPH) = %d.\r\n", vol_level);
                    #else
                    for (i = 0; i < MAX_VOL_TYPE; i++)
                    {
                        vol_level = Ql_GetVolume(i);
                        OUT_DEBUG(notes,"Ql_GetVolume(%d) = %d.\r\n", i, vol_level);
                    }
                    #endif
                    
                    break;
                }
               
                // Set MIC gain (gain=channel,gain)
                Ql_memset(buffer, 0x0, sizeof(buffer));
                Ql_sprintf(buffer, "gain=");
                pChar = Ql_strstr((char*)pDataEvt->data, buffer);
                if (pChar)
                {
                    pChar = Ql_strstr((char*)pDataEvt->data, "=");
                    if (pChar)
                    {
                        char* p2;
                        u8 channel;
                        u8 micGain;
                        
                        p2 = Ql_strstr((char*)pDataEvt->data, ",");
                        Ql_memcpy(buffer, pChar + 1, p2 - pChar -1);
                        channel = Ql_atoi(buffer);
                        
                        pChar = p2;
                        p2 = Ql_strstr((char*)pDataEvt->data, "\r\n");
                        Ql_memcpy(buffer, pChar + 1, p2 - pChar -1);
                        micGain = Ql_atoi(buffer);
                        
                        Ql_SetMicGain(channel, micGain);
                        //Ql_SetSideToneGain(channel, micGain);
                        OUT_DEBUG(notes,"Set mic gain at channel %d: %d.\r\n", channel, micGain);
                    }
                    break;
                }

                // Get MIC gain
                Ql_memset(buffer, 0x0, sizeof(buffer));
                Ql_sprintf(buffer, "mic gain?\r\n");
                ret = Ql_strncmp((char*)pDataEvt->data, buffer, Ql_strlen(buffer));
                if (0 == ret)
                {
                    u8 micGain;
                    OUT_DEBUG(notes,buffer);
                    memset(buffer, 0x0, strlen(buffer));
                    #if 1
                    micGain = Ql_GetMicGain(QL_AUDIO_PATH_NORMAL);
                    OUT_DEBUG(notes,"Mic gain at channel %d: %d.\r\n", QL_AUDIO_PATH_NORMAL, micGain);
                    micGain = Ql_GetMicGain(QL_AUDIO_PATH_HEADSET);
                    OUT_DEBUG(notes,"Mic gain at channel %d: %d.\r\n", QL_AUDIO_PATH_HEADSET, micGain);
                    micGain = Ql_GetMicGain(QL_AUDIO_PATH_LOUDSPEAKER);
                    OUT_DEBUG(notes,"Mic gain at channel %d: %d.\r\n", QL_AUDIO_PATH_LOUDSPEAKER, micGain);
                    #else
                    micGain = Ql_GetSideToneGain(QL_AUDIO_PATH_NORMAL);
                    OUT_DEBUG(notes,"Mic gain at channel %d: %d.\r\n", QL_AUDIO_PATH_NORMAL, micGain);
                    micGain = Ql_GetSideToneGain(QL_AUDIO_PATH_HEADSET);
                    OUT_DEBUG(notes,"Mic gain at channel %d: %d.\r\n", QL_AUDIO_PATH_HEADSET, micGain);
                    micGain = Ql_GetSideToneGain(QL_AUDIO_PATH_LOUDSPEAKER);
                    OUT_DEBUG(notes,"Mic gain at channel %d: %d.\r\n", QL_AUDIO_PATH_LOUDSPEAKER, micGain);
                    #endif
                    break;
                }

                // Control echo cancellation
                Ql_memset(buffer, 0x0, sizeof(buffer));
                Ql_sprintf(buffer, "echo\r\n");
                ret = Ql_strncmp((char*)pDataEvt->data, buffer, Ql_strlen(buffer));
                if (0 == ret)
                {
                    Ql_sprintf(buffer, "AT+QECHO?\n");
                    Ql_SendToModem(ql_md_port1, (u8*)buffer, Ql_strlen(buffer));

                    ret = Ql_CtrlEchoCancel(224, 2000, 30000, 374, QL_AUDIO_PATH_LOUDSPEAKER);
                    OUT_DEBUG(notes,"Ql_CtrlEchoCancel() = %d\r\n", ret);

                    Ql_sprintf(buffer, "AT+QECHO?\n");
                    Ql_SendToModem(ql_md_port1, (u8*)buffer, Ql_strlen(buffer));
                    break;
                }

                // Send DTMF
                Ql_memset(buffer, 0x0, sizeof(buffer));
                Ql_sprintf(buffer, "vts=");
                pChar = Ql_strstr((char*)pDataEvt->data, buffer);
                if (pChar)
                {
                    pChar = Ql_strstr((char*)pDataEvt->data, "=");
                    if (pChar)
                    {
                        char* p2;
                        u8 len;
                        p2 = Ql_strstr((char*)pDataEvt->data, "\r\n");
                        len = p2 - pChar -1;
                        Ql_memset(buffer, 0x0, sizeof(buffer));
                        Ql_memcpy(buffer, pChar + 1, len);
                        ret = Ql_VTS((u8*)buffer, len);
                        OUT_DEBUG(notes,"Ql_VTS() = %d\r\n", ret);
                     }
                     break;
                 }
                Ql_memset(buffer, 0x0, sizeof(buffer));
                Ql_sprintf(buffer, "vts2");
                pChar = Ql_strstr((char*)pDataEvt->data, buffer);
                if (pChar)
                {
                    ret = Ql_VTS("1234,6", 6);
                    OUT_DEBUG(notes,"Ql_VTS2() = %d\r\n", ret);
                    break;
                 }
                #if 0
                #include "ql_utility.h"
                // GB2132 => UNICODE
                Ql_memset(buffer, 0x0, sizeof(buffer));
                Ql_sprintf(buffer, "convert=");
                pChar = Ql_strstr((char*)pDataEvt->data, buffer);
                if (pChar)
                {
                    pChar = Ql_strstr((char*)pDataEvt->data, "=");
                    if (pChar)
                    {
                        char* p2;
                        u8 len;
                        p2 = Ql_strstr((char*)pDataEvt->data, "\r\n");
                        len = p2 - pChar -1;
                        Ql_memset(buffer, 0x0, sizeof(buffer));
                        Ql_memcpy(buffer, pChar + 1, len);
                        OUT_DEBUG(notes,"Ql_ConvertCodec(\"%s\")\r\n", buffer);
                        Ql_memset((char*)codec, 0x0, Ql_strlen((char*)codec));
                        //ret = Ql_ConvertCodec(QL_CODEC_GB2312, (u8*)buffer, QL_CODEC_UNICODE, codec);
                        ret = Ql_ConvertCodec(QL_CODEC_UNICODE, (u8*)buffer, QL_CODEC_GB2312, codec);
                        if (ret > 0)
                        {
                            codec[ret] = 0;
                            Ql_sprintf(buffer, "Ql_ConvertCodec(): %x\r\n", codec);
                            OUT_DEBUG(notes,buffer);
                        }
                        else
                        {
                            OUT_DEBUG(notes,"Fail to convert codec, error code: %d\r\n", ret);
                        }
                        OUT_DEBUG(notes,"Ql_ConvertCodec() = %d : %x\r\n", ret, codec);
                     }
                     break;
                 }
                 #endif

                //  For ¿Æ´óÑ¶·ÉTTS
                Ql_sprintf(buffer, "qstk=");
                ret = Ql_strncmp((char*)pDataEvt->data, buffer, Ql_strlen(buffer));
                if (0 == ret)
                {
                    char* pCh = NULL;
                    pCh = Ql_strstr((char*)pDataEvt->data, "=");
                    if (pCh != NULL)
                    {
                        Ql_memset(buffer, 0x0, sizeof(buffer));
                        Ql_strncpy(buffer, pCh + 1, pDataEvt->len - (pCh - (char*)pDataEvt->data + 1) - 2);
                        OUT_DEBUG(notes,"Ql_TTS_Play(), len=%d\r\n", Ql_strlen(buffer));
                        //ret = Ql_TTS_Play((u8*)buffer, sizeof(buffer));
                        ret = Ql_TTS_PlayInCall((u8*)buffer, sizeof(buffer), 7, 0);
                        if (ret < 0)
                        {
                            OUT_DEBUG(notes,"Fail to play TTS.\r\n");
                            break;
                        }
                        else
                        {
                            OUT_DEBUG(notes,"Playing finished.\r\n");
                        }
                    }
                    break;
                }

                // Get Operator (operator?)
                Ql_memset(buffer, 0x0, sizeof(buffer));
                Ql_sprintf(buffer, "operator?\r\n");
                ret = Ql_strncmp((char*)pDataEvt->data, buffer, Ql_strlen(buffer));
                if (0 == ret)
                {
                    char oper[20] = { 0x0 };
                    OUT_DEBUG(notes,buffer);
                    memset(buffer, 0x0, strlen(buffer));
                    ret = Ql_GetOperator((u8*)oper, sizeof(oper));
                    OUT_DEBUG(notes,"Ql_GetOperator()=%d: %s.\r\n", ret, oper);
                    break;
                }

                // Get device state (device state?)
                Ql_memset(buffer, 0x0, sizeof(buffer));
                Ql_sprintf(buffer, "device state?\r\n");
                ret = Ql_strncmp((char*)pDataEvt->data, buffer, Ql_strlen(buffer));
                if (0 == ret)
                {
                    //u8 ps_status;
                    s32 simcard;
                    s32 creg;
                    s32 cgreg;
                    u8 rssi;
                    u8 ber;
                    /*the Ql_GetDeviceCurrentRunState function can replace these AT Command:  AT+CPIN? AT+CREG? AT+CGREG? AT+CSQ*/
                    Ql_GetDeviceCurrentRunState(&simcard, &creg, &cgreg, &rssi, &ber);
                    OUT_DEBUG(notes,"Ql_GetDeviceCurrentRunState(): simcard=%d, creg=%d, cgreg=%d, rssi=%d, ber=%d\r\n", simcard, creg, cgreg, rssi, ber);
                    break;
                }

                // StartJammingDet
                Ql_memset(buffer, 0x0, sizeof(buffer));
                Ql_sprintf(buffer, "StartJammingDet\r\n");
                ret = Ql_strncmp((char*)pDataEvt->data, buffer, Ql_strlen(buffer));
                if (0 == ret)
                {
                    ret = Ql_StartJammingDet(&Callback_JammingDet);
                    OUT_DEBUG(notes,"Ql_StartJammingDet()=%d\r\n", ret);
                    break;
                }

                // StopJammingDet
                Ql_memset(buffer, 0x0, sizeof(buffer));
                Ql_sprintf(buffer, "StopJammingDet\r\n");
                ret = Ql_strncmp((char*)pDataEvt->data, buffer, Ql_strlen(buffer));
                if (0 == ret)
                {
                    ret = Ql_StopJammingDet();
                    OUT_DEBUG(notes,"Ql_StopJammingDet()=%d\r\n", ret);
                    break;
                }
                /*cmd:  Play=xxx.mp3*/
                /*cmd:  Play=xxx.wav*/
                /*cmd:  Play=xxx.amr*/
                /* Without '\r\n' */
                pChar = Ql_strstr((char*)pDataEvt->data, "Play=");
                if (pChar)
                {
                    s32 ret;
                    pChar += 5;
                    ret = Ql_StartPlayAudioFile((u8*)pChar ,0, 6, 1);
                    if(ret == QL_RET_OK)
                        ;//play ok
                    Ql_sprintf(buffer, "Ql_StartPlayAudioFile(%s)=%d\r\n", pChar, ret);
                    Ql_SendToUart(ql_uart_port1, (u8*)buffer, Ql_strlen(buffer));    
                    break;
                }

                /*cmd:  Stop*/
                pChar = Ql_strstr((char*)pDataEvt->data,"Stop");
                if (pChar)
                {
                    s32 ret;
                    ret = Ql_StopPlayAudioFile();
                    if(ret == QL_RET_OK)
                        ;//stop ok
                    Ql_sprintf(buffer,"Ql_StopPlayAudioFile()=%d\r\n", ret);
                    Ql_SendToUart(ql_uart_port1, (u8*)buffer, Ql_strlen(buffer));    
                    break;
                }    
                
                /*cmd:  play 8k audio*/
                pChar = Ql_strstr((char*)pDataEvt->data,"play 8k");
                if (pChar)
                {
                    s32 ret;
                    ret = Ql_PlayAudioFile_8k((u8*)"wav8k.wav", 0, QL_AUDIO_VOLUME_LEVEL6, QL_AUDIO_PATH_HEADSET, QL_AUDIO_VOLUME_LEVEL7);
                    Ql_sprintf(buffer,"Ql_PlayAudioFile_8k()=%d\r\n", ret);
                    Ql_SendToUart(ql_uart_port1, (u8*)buffer, Ql_strlen(buffer));    
                    break;
                }
                
                pChar = Ql_strstr((char*)pDataEvt->data,"stop 8k");
                if (pChar)
                {
                    s32 ret;
                    ret = Ql_StopAudioFile_8k();
                    Ql_sprintf(buffer,"Ql_StopAudioFile_8k()=%d\r\n", ret);
                    Ql_SendToUart(ql_uart_port1, (u8*)buffer, Ql_strlen(buffer));    
                    break;
                }    

                /*cmd:  tone detect*/
                pChar = Ql_strstr((char*)pDataEvt->data,"tonedet");
                if (pChar)
                {
                    s32 ret;
                    Ql_sprintf(buffer,"AT+QTONEDET=1\r\n");
                    Ql_SendToModem(ql_md_port1, (u8*)buffer, Ql_strlen(buffer));    
                    break;
                }   
               // Test for Ql_WDTMF() API 
                pChar = Ql_strstr(pDataEvt->data,"Ql_WDTMF=");
                if(pChar)
                {
                    char *P1;
                    char *P2;
                  s32 uploadVolume,downVolume;
                  P1 = Ql_strstr(pDataEvt->data,"=");
                  P1++;
                  uploadVolume = Ql_atoi(P1);
                  P2 = Ql_strstr(pDataEvt->data,",");
                  P2++;
                  downVolume = Ql_atoi(P2);
                  OUT_DEBUG(notes,"\r\nuploadVolume =%d,downVolume==%d\r\n" ,uploadVolume,downVolume);
                  Ql_DebugTrace("\r\nuploadVolume =%d,downVolume==%d\r\n" ,uploadVolume,downVolume);
                  ret = Ql_WDTMF(uploadVolume,downVolume,"123456789122456789954122,200,200,aaaaa,20,20",52);
                  Ql_DebugTrace("Ql_WDTMF return %d\r\n",ret);
                  break;
                }

                // Ql_Call_GetCurrCall
                Ql_memset(buffer, 0x0, sizeof(buffer));
                Ql_sprintf(buffer, "Ql_Call_GetCurrCall\r\n");
                ret = Ql_strncmp((char*)pDataEvt->data, buffer, Ql_strlen(buffer));
                if (0 == ret)
                {
                    ret = Ql_Call_GetCurrCall(Callback_Call_Info);
                    OUT_DEBUG(notes,"Ql_Call_GetCurrCall()=%d\r\n",ret);                    
                    break;
                }

                // Ql_EnableDTMF
                Ql_memset(buffer, 0x0, sizeof(buffer));
                Ql_sprintf(buffer, "Ql_EnableDTMF\r\n");
                ret = Ql_strncmp((char*)pDataEvt->data, buffer, Ql_strlen(buffer));
                if (0 == ret)
                {
                    ret = Ql_EnableDTMF(Callback_DTMF_Info);
                    OUT_DEBUG(notes,"Ql_EnableDTMF()=%d\r\n",ret);                    
                    break;
                }

                 // Ql_DisableDTMF
                Ql_memset(buffer, 0x0, sizeof(buffer));
                Ql_sprintf(buffer, "Ql_DisableDTMF\r\n");
                ret = Ql_strncmp((char*)pDataEvt->data, buffer, Ql_strlen(buffer));
                if (0 == ret)
                {
                    ret = Ql_DisableDTMF();
                    OUT_DEBUG(notes,"Ql_DisableDTMF()=%d\r\n",ret);                    
                    break;
                }

                 // Ql_GetDTMFConfig=mode
                Ql_memset(buffer, 0x0, sizeof(buffer));
                Ql_sprintf(buffer, "Ql_GetDTMFConfig=");
                ret = Ql_strncmp((char*)pDataEvt->data, buffer, Ql_strlen(buffer));
                if (0 == ret)
                {  
                    QlDTMFCONFIG dtmfconfig;
                    char* pCh = NULL;
                    pCh = Ql_strstr((char*)pDataEvt->data, "=");
                    pCh += 1;
                    dtmfconfig.mode = Ql_atoi(pCh);
                    ret = Ql_GetDTMFConfig(&dtmfconfig);
                    OUT_DEBUG(notes,"Ql_GetDTMFConfig() = %d,mode=%d,prefixpause=%d,lowthreshold=%d,highthreshold=%d\r\n",
                                                                 ret,dtmfconfig.mode,dtmfconfig.prefixpause,dtmfconfig.lowthreshold,dtmfconfig.highthreshold);

                    break;
                }

                // Ql_SetDTMFConfig=mode,prefixPause,lowThrehold,lowThrehold
                Ql_memset(buffer, 0x0, sizeof(buffer));
                Ql_sprintf(buffer, "Ql_SetDTMFConfig=");
                pChar = Ql_strstr((char*)pDataEvt->data, buffer);
                if (pChar)
                {
                    u32 i = 0;
                    char *p1;
                    char *p2;
                    u32 str[5]={0};
                    QlDTMFCONFIG dtmfconfig;
                    p1 = Ql_strstr((char*)pDataEvt->data, "=");
                    p2 = Ql_strstr((char*)pDataEvt->data, "\r\n");
                    if (p2)
                    {
                        *p2 = '\0';
                    }
                    p1 +=1;
                    
                    while((*p1) !=0)
                    {
                        p2 = Ql_strstr(p1, ",");
                        if(!p2)//NO ","
                        {
                            str[i] = Ql_atoi(p1);
                            p1 +=1;
                            break;
                        }else
                        {
                            *p2 = '\0';
                            str[i] = Ql_atoi(p1);
                            p1 = p2+1;
                            i++;
                        }
                    }
                    
                    dtmfconfig.mode = str[0];
                    dtmfconfig.prefixpause = str[1];
                    dtmfconfig.lowthreshold = str[2];
                    dtmfconfig.highthreshold = str[3];  

                    ret = Ql_SetDTMFConfig(dtmfconfig);
                    OUT_DEBUG(notes,"Ql_SetDTMFConfig() = %d,mode=%d,prefixpause=%d,lowthreshold=%d,highthreshold=%d\r\n",
                                                                 ret,dtmfconfig.mode,dtmfconfig.prefixpause,dtmfconfig.lowthreshold,dtmfconfig.highthreshold);
                     break;
                 }
                
                if (Ql_strncmp((const char *)pDataEvt->data, "$EXIT_DATA_MODE$", 16) == 0)
                {
                    ret = Ql_UartForceSendEscape(ql_md_port1);
                    OUT_DEBUG(notes, " Ql_UartForceSendEscape()=%d\r\n", ret);
                }else{
                    Ql_SendToModem(ql_md_port1, (u8*)pDataEvt->data, pDataEvt->len);
                }

                 break;
            }
            case EVENT_MODEMDATA:
            {
                //TODO: receive and hanle data from CORE through virtual modem port
                PortData_Event* pPortEvt = (PortData_Event*)&g_event.eventData.modemdata_evt;
                //OUT_DEBUG(notes,"Modem Data at vPort [%d]: %s\r\n", pPortEvt->port, pPortEvt->data);
                OUT_DEBUG(notes,"%s", pPortEvt->data);
                break;
            }
            case EVENT_MEDIA_FINISH:
            {
                Ql_sprintf(notes, "type=%d, reason=%d\r\n", g_event.eventData.mediafinish_evt.media_type, g_event.eventData.mediafinish_evt.reason);
                Ql_SendToUart(ql_uart_port1, (u8*)notes, Ql_strlen(notes));
                break;
            }
            case EVENT_TIMER:
            {
                //TODO: specify what you want to happen when the interval for timer elapes

                //OUT_DEBUG(notes,"The timer raises for %d time(s).\r\n", ++cnt);
                memset(notes, 0x0, Ql_strlen(notes));
                sprintf(notes, "The timer raises for %d time(s).\r\n", ++cnt);
                OUT_DEBUG(notes,notes);

                // Start the timer again
                if (cnt < 10)
                {
                    Ql_StartTimer(&tm);
                }
                else
                {
                    OUT_DEBUG(notes,"\r\nThe timer stops.\r\n");
                }
                break;
            }

            default:
                //OUT_DEBUG(notes,"\r\nUNKNOW EVENT(%x).\r\n", g_event.eventType);
                break;
        }
    }
}

void Callback_Dial(s32 result)
{
    OUT_DEBUG(notes,"Callback_Dial(): result = %d\r\n", result);
}

void Callback_Ring(u8* coming_call_num)
{
    OUT_DEBUG(notes,"Callback_Ring(): %s\r\n", (char*)coming_call_num);
}

void Callback_Hangup(void)
{
    OUT_DEBUG(notes,"Callback_Hangup()\r\n");
}

void Callback_TTS_Play(s32 result)
{
    OUT_DEBUG(notes,"Callback_TTS_Play(), result: %d\r\n", result);
}

void Callback_RetrieveBSInfo(QL_STNodeB_Info* bsInfo)
{
    if (bsInfo != NULL)
    {
        OUT_DEBUG(notes,"BS Info: \r\n mcc=%d, mnc=%d, lac=%d, cellId=%d, bcch=%d, bsic=%d, dbm=%d, c1=%d, c2=%d, txp=%d, rla=%d\r\n", 
            bsInfo->mcc, bsInfo->mnc, bsInfo->lac, bsInfo->cellId, bsInfo->bcch, bsInfo->bsic, bsInfo->dbm, bsInfo->c1, bsInfo->c2, bsInfo->txp, bsInfo->rla);
    }
}

void Callback_JammingDet(s32 result)
{
    OUT_DEBUG(notes,"Callback_JammingDet(), result: %d\r\n", result);
}

void Callback_Call_Info(QlCallParam *call_param)
{
    s32 i;
    OUT_DEBUG(notes, "call_count = %d\r\n",call_param->call_count);
    for(i=0;i<call_param->call_count;i++)
    {
        OUT_DEBUG(notes, "CLCC Info:call_id=%d,call_orig=%d,call_state=%d,call_mode=%d,call_mpty=%d,tel_number=%s,tel_type=%d\r\n",
                               call_param->call_info[i].call_id,call_param->call_info[i].call_orig,call_param->call_info[i].call_state,call_param->call_info[i].call_mode,
                               call_param->call_info[i].call_mpty,call_param->call_info[i].call_number,call_param->call_info[i].call_type);            
                
    }
}

void Callback_DTMF_Info(QlDTMFCode dtmfcode)
{
    OUT_DEBUG(notes,"Callback_DTMF_Info(), key= %d,persisttime=%d\r\n", dtmfcode.key,dtmfcode.persisttime);
}
void callback_PIN(s8 SIM_type)
{
     OUT_DEBUG(notes,"lock_type =%d\r\n",SIM_type);
     if(QL_ID_READY == SIM_type )
     {
        OUT_DEBUG(notes,"PIN Ready\r\n");
     }
     if(QL_ID_SIM_PIN1 == SIM_type )
     {
        OUT_DEBUG(notes,"PIN1 is need !\r\n");
     }
     if(QL_ID_SIM_PUK1 == SIM_type )
     {
        OUT_DEBUG(notes,"PIN PUK is need!\r\n");
     }
}

#endif // __EXAMPLE_CALL__

