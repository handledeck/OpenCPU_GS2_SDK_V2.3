/***************************************************************************************************
*   Example:
*       
*           GPIO Control
*
*   Description:
*
*           This example gives an example for gpio setting.
*           Through Uart port, input the special command, there will be given the response about the gpio state.
*
*   Usage:
*
*          
*
*				
*               
****************************************************************************************************/
#ifdef __TESTGPIO__
  
#include "ql_appinit.h"
#include "ql_interface.h"
#include "ql_type.h"
#include "ql_trace.h"
#include "ql_audio.h"
#include "ql_timer.h"
#include "ql_stdlib.h"
#include "ql_pin.h"

//#include <rt_misc.h>
#include <string.h>
//#include <locale.h>

#define PIN_NAME_SIZE   32

enum
{
    PARAM_TYPE_GPIO = 0,
    PARAM_TYPE_PWM,
    PARAM_TYPE_EINT,
    PARAM_TYPE_EINT_CALLBACK
};

typedef struct tag_pinDesc
{
    char    name[PIN_NAME_SIZE];
    u32     id;
}pinDesc;

const pinDesc g_pinInfoTable[] =
{
    {"netlight",        QL_PINNAME_NETLIGHT             },
    {"status",          QL_PINNAME_STATUS               },    
    {"download",        QL_PINNAME_DOWNLOAD             },
    
    {"pcmin",           QL_PINNAME_PCM_IN               },
    {"pcmclk",          QL_PINNAME_PCM_CLK              },
    {"pcmout",          QL_PINNAME_PCM_OUT              },
    {"pcmsync",         QL_PINNAME_PCM_SYNC             },        
    {"pcmrst",          QL_PINNAME_PCM_RST              },

    {"kcol0",           QL_PINNAME_KBC0                 },
    {"kcol1",           QL_PINNAME_KBC1                 },
    {"kcol2",           QL_PINNAME_KBC2                 },
    {"kcol3",           QL_PINNAME_KBC3                 },
    {"kcol4",           QL_PINNAME_KBC4                 },
    {"krow0",           QL_PINNAME_KBR0                 },
    {"krow1",           QL_PINNAME_KBR1                 },
    {"krow2",           QL_PINNAME_KBR2                 },
    {"krow3",           QL_PINNAME_KBR3                 },
    {"krow4",           QL_PINNAME_KBR4                 },
    {"dcd",             QL_PINNAME_DCD                  },
    {"ri",              QL_PINNAME_RI                   },
    {"dtr",             QL_PINNAME_DTR                  },
    {"rts",             QL_PINNAME_RTS                  },
    {"cts",             QL_PINNAME_CTS                  },
    {"cmdat3",          QL_PINNAME_CS_D3                },
    {"cmdat0",          QL_PINNAME_CS_D0                },
    {"cmvreff",         QL_PINNAME_CS_VSYNC             },
    {"cmdat4",          QL_PINNAME_CS_D4                },
    {"cmdat7",          QL_PINNAME_CS_D7                },
    {"cmdat5",          QL_PINNAME_CS_D5                },
    {"cmpdn",           QL_PINNAME_CS_PWDN              },
    {"cmdat1",          QL_PINNAME_CS_D1                },
    {"cmdat2",          QL_PINNAME_CS_D2                },
    {"cmpclk",          QL_PINNAME_CS_PCLK              },
    {"cmhref",          QL_PINNAME_CS_HSYNC             },
    {"cmmclk",          QL_PINNAME_CS_MCLK              },
    {"cmdat6",          QL_PINNAME_CS_D6                },
    {"cmrst",           QL_PINNAME_CS_RST               },
    {"sim",             QL_PINNAME_SIM_PRESENCE         },
    {"buzzer",          QL_PINNAME_BUZZER               },
    {"invalid",         QL_PINNAME_MAX                  }
};

u32 g_pin = 0, g_mode = 0, g_paramType = 0;
char g_dbg_buf[100] = {0};
QlPinParameter g_pinparameter;
QlEventBuffer  flSignalBuffer;


#define MAX_PINS    (sizeof(g_pinInfoTable)/sizeof(g_pinInfoTable[0]))
#define MAX_PARAM   10

#define OUT_DEBUG(port, ...)  \
    Ql_memset((void*)(g_dbg_buf),0,100);  \
    Ql_sprintf(g_dbg_buf,__VA_ARGS__);   \
    Ql_SendToUart(port,(u8*)(g_dbg_buf),Ql_strlen((const char*)(g_dbg_buf)));


#define INVALID_PARAM_VAL   0xff

/* return MAX_PINS if not found */
u32 find_pin_by_name(char *szName)
{
    u32 idx = 0;
    OUT_DEBUG(ql_uart_port1, "szName=[%s]\r\n", szName);
    OUT_DEBUG(ql_uart_port1, "max_pins=%d\r\n", MAX_PINS);
    for(idx = 0; idx < MAX_PINS; ++idx)
    {
        OUT_DEBUG(ql_uart_port1, "[%s]\r\n", g_pinInfoTable[idx].name);
        if(Ql_strstr(g_pinInfoTable[idx].name, szName))
        {
            return g_pinInfoTable[idx].id;
        }
    }
    return idx;
}

static void eint_callback(u8 eintno)
{
    QlPinLevel lvl;
    Ql_eintMask(eintno, 1);
    Ql_eintRead(eintno, &lvl);
    Ql_eintSetPolarity(eintno, !lvl);
    OUT_DEBUG(ql_uart_port1, "%s, eintno=%d\r\n", __func__, eintno);
}

void ql_entry()		// entry for main task
{
    bool           keepGoing = TRUE;
    char *pData, *p;
    char buffer[100];
    char *szParam = NULL;
    s32 params[MAX_PARAM];

    //__rt_stackheap_init();
    //__rt_lib_init(0, 0);

    //setlocale(LC_ALL, "C");
    
    //Ql_SetDebugMode(BASIC_MODE);	// set working mode of UART2 to BASIC_MODE
    Ql_SetDebugMode(ADVANCE_MODE);
    Ql_DebugTrace("gpiotest: ql_entry\r\n");
    Ql_OpenModemPort(ql_md_port1);  //open virtual modem serial port ql_md_port1
     
    while(keepGoing)
    {    
        Ql_GetEvent(&flSignalBuffer);		// Get event from core system. When there is no event, 
                                            // that will go into waiting state.
        switch(flSignalBuffer.eventType)
        {
            case EVENT_UARTDATA:			// When core system has received data from uart port, this event will be triggered
            {
                if (flSignalBuffer.eventData.uartdata_evt.len <= 0)
                {
                    break;
                }                
                
                s32 iret;

                pData = (char*)flSignalBuffer.eventData.uartdata_evt.data;

                /* provide the pin name */
                if(0 ==Ql_strncmp(pData, "pin=", Ql_strlen("pin=")))
                {
                    p = pData + Ql_strlen("pin=");
                    OUT_DEBUG(ql_uart_port1, "pin=[%s]\r\n", p);
                    g_pin = find_pin_by_name(p);
                    if(g_pin == MAX_PINS)
                    {
                        OUT_DEBUG(ql_uart_port1, "invalid pin name !!!\r\n");
                        break;
                    }
                    OUT_DEBUG(ql_uart_port1, "g_pin=%d\r\n", g_pin);
                    OUT_DEBUG(ql_uart_port1, "pls enter the pin mode now... use mode=... \r\n");
                    break;
                }
                
                /* modeval=m*/
                if(0 ==Ql_strncmp(pData, "mode=", Ql_strlen("mode=")))
                {
                    p = pData + Ql_strlen("mode=");
                    g_mode = Ql_atoi(p);
                    OUT_DEBUG(ql_uart_port1, "g_mode=%d, now pls use paramtype=gpio/pwm/eint/callback... \r\n", g_mode);
                    break;
                }

                /* paramtype=gpio/pwm/eint/eintcallback*/
                if(0 ==Ql_strncmp(pData, "paramtype=", Ql_strlen("paramtype=")))
                {
                    p = pData + Ql_strlen("paramtype=");
                    if(Ql_strstr(p, "gpio"))
                        g_paramType = PARAM_TYPE_GPIO;
                    else if(Ql_strstr(p, "pwm"))
                        g_paramType = PARAM_TYPE_PWM;
                    else if(Ql_strstr(p, "eint"))
                        g_paramType = PARAM_TYPE_EINT;
                    else if(Ql_strstr(p, "callback"))
                        g_paramType = PARAM_TYPE_EINT_CALLBACK;

                    OUT_DEBUG(ql_uart_port1, "g_paramType=%d, now pls use params=... \r\n", g_mode);
                    switch(g_paramType)
                    {
                    case PARAM_TYPE_GPIO:
                        OUT_DEBUG(ql_uart_port1, "pullenable,direction,level,pullsel\r\n");
                        break;
                    case PARAM_TYPE_PWM:
                        OUT_DEBUG(ql_uart_port1, "clksource,clkdiv,lowNum,highNum\r\n");
                        break;
                    case PARAM_TYPE_EINT:
                        OUT_DEBUG(ql_uart_port1, "senseType(0-edge,1-level),hwDebounce(ms),swDebounce(ms)\r\n");
                        break;
                    case PARAM_TYPE_EINT_CALLBACK:
                        OUT_DEBUG(ql_uart_port1, "senseType(0-edge,1-level),hwDebounce(ms),swDebounce(ms),automask\r\n");
                        break;
                    default:
                        break;
                    }
                    break;
                }                

                /* params=p1,p2,p3,p4 */
                if(0 ==Ql_strncmp(pData, "params=", Ql_strlen("params=")))
                {
                    char *pPos  = NULL;
                    u32 idx = 0;

                    p = pData+Ql_strlen("params=");
                    
                    Ql_memset(params, INVALID_PARAM_VAL, sizeof(params)/sizeof(s32));
                    szParam = strtok_r(p, ",", &pPos);
                    while(szParam && idx < MAX_PARAM)
                    {
                        params[idx] = Ql_atoi(szParam);
                        szParam = strtok_r(pPos, ",", &pPos);
                        idx++;
                    }
                    OUT_DEBUG(ql_uart_port1, "params set already, now pls call the functions...\r\n");
                    break;
                }
                
                /* Ql_pinSubscribe*/
                p = Ql_strstr(pData, "Ql_pinSubscribe");
                if(p)
                {
                    Ql_memset(&g_pinparameter, 0, sizeof(g_pinparameter));

                    g_pinparameter.pinconfigversion = QL_PIN_VERSION;
                    
                    if(g_paramType == PARAM_TYPE_GPIO)
                    {
                        QlGpioParameter * pParam = &g_pinparameter.pinparameterunion.gpioparameter;

                        // param[0-4]: mode, pullen, dir, lvl, pullsel
                        if(INVALID_PARAM_VAL != params[0])
                            pParam->pinpullenable = params[0];
                        if(INVALID_PARAM_VAL != params[1])
                            pParam->pindirection = params[1];
                        if(INVALID_PARAM_VAL != params[2])
                            pParam->pinlevel = params[2];
                        if(INVALID_PARAM_VAL != params[3])
                            pParam->pinpullsel = params[3];

                        iret = Ql_pinSubscribe(g_pin, g_mode, &g_pinparameter);
                        OUT_DEBUG(ql_uart_port1, "Subscribe(pin=%d,mod=%d,pul=%d,dir=%d,lvl=%d, pullsel=%d)=%d\r\n", 
                            g_pin, g_mode, pParam->pinpullenable, pParam->pindirection, pParam->pinlevel, pParam->pinpullsel, iret);
                    }
                    else if(g_paramType == PARAM_TYPE_PWM)
                    {
                        QlPwmParameter *pParam = &g_pinparameter.pinparameterunion.pwmparameter;
                        if(INVALID_PARAM_VAL != params[0])
                            pParam->pwmsource = params[0];
                        if(INVALID_PARAM_VAL != params[1])
                            pParam->pwmclkdiv = params[1];
                        if(INVALID_PARAM_VAL != params[2])
                            pParam->lowpulesnumber = params[2];
                        if(INVALID_PARAM_VAL != params[3])
                            pParam->highpulesnumber = params[3];

                        iret = Ql_pinSubscribe(g_pin, g_mode, &g_pinparameter);
                        OUT_DEBUG(ql_uart_port1, "Subscribe(pin=%d,mod=%d,src=%d,clkdiv=%d,lown=%d, highn=%d)=%d\r\n", 
                            g_pin, g_mode, pParam->pwmsource, pParam->pwmclkdiv, pParam->lowpulesnumber, pParam->highpulesnumber, iret);

                    }
                    else if(g_paramType == PARAM_TYPE_EINT)
                    {
                        QlEintParameter *pParam = &g_pinparameter.pinparameterunion.eintparameter;

                        if(INVALID_PARAM_VAL != params[0])
                            pParam->eintsensitivetype = params[0];
                        if(INVALID_PARAM_VAL != params[1])
                            pParam->hardware_de_bounce = params[1];
                        if(INVALID_PARAM_VAL != params[2])
                            pParam->software_de_bounce = params[2];

                        iret = Ql_pinSubscribe(g_pin, g_mode, &g_pinparameter);
                        OUT_DEBUG(ql_uart_port1, "Subscribe(pin=%d,mod=%d,sense=%d,hwdebounce=%d,swdebounce=%d)=%d\r\n", 
                            g_pin, g_mode, pParam->eintsensitivetype, pParam->hardware_de_bounce, pParam->software_de_bounce, iret);

                    }
                    else if(g_paramType == PARAM_TYPE_EINT_CALLBACK)
                    {
                        QlEintCallBackParameter *pParam = &g_pinparameter.pinparameterunion.eintcallbackparameter;

                        if(INVALID_PARAM_VAL != params[0])
                            pParam->eintsensitivetype = params[0];
                        if(INVALID_PARAM_VAL != params[1])
                            pParam->hardware_de_bounce = params[1];
                        if(INVALID_PARAM_VAL != params[2])
                            pParam->software_de_bounce = params[2];
                        if(INVALID_PARAM_VAL != params[3])
                            pParam->automask = params[3];

                        pParam->einttriggertype = QL_EINTTRIGGERTYPE_CALLBACK;
                        pParam->callbak_fun = eint_callback;
                        
                        iret = Ql_pinSubscribe(g_pin, g_mode, &g_pinparameter);
                        OUT_DEBUG(ql_uart_port1, "Subscribe(pin=%d,mod=%d,triggertype=%d, sense=%d,hwdebounce=%d,swdebounce=%d,automask=%d, callback=%x)=%d\r\n", 
                            g_pin, g_mode, pParam->einttriggertype, pParam->eintsensitivetype, pParam->hardware_de_bounce, pParam->software_de_bounce, pParam->automask, pParam->callbak_fun, iret);

                    }

                    break;
                }


                p = Ql_strstr(pData, "Ql_pinUnSubscribe");
                if(p)
                {
                    iret = Ql_pinUnSubscribe(g_pin);
                    OUT_DEBUG(ql_uart_port1, "Ql_pinUnSubscribe(pin=%d)=%d\r\n", 
                        g_pin, iret);
                    break;
                }

                p = Ql_strstr(pData, "Ql_pinWrite=");
                if(p)
                {
                    int lvl = 0;
                    p += Ql_strlen("Ql_pinWrite=");
                    lvl = Ql_atoi(p);
                    iret = Ql_pinWrite(g_pin, lvl);
                    OUT_DEBUG(ql_uart_port1, "Ql_pinWrite(pin=%d, lvl=%d) = %d\r\n", g_pin, lvl, iret);
                    break;
                }

                p = Ql_strstr(pData, "Ql_pinRead");
                if(p)
                {
                    QlPinLevel lvl = END_OF_QL_PINLEVEL;
                    iret = Ql_pinRead(g_pin, &lvl);
                    OUT_DEBUG(ql_uart_port1, "Ql_pinRead(pin=%d) = %d, *lvl = %d\r\n", g_pin, iret, lvl);
                    break;
                }

                p = Ql_strstr(pData, "Ql_pinControl=");
                if(p)
                {
                    p += Ql_strlen("Ql_pinControl=");
                    QlPinControl ctrl = QL_PINCONTROL_START; 
                    ctrl = Ql_atoi(p);
                    iret = Ql_pinControl(g_pin, ctrl);
                    OUT_DEBUG(ql_uart_port1, "Ql_pinControl(pin=%d, ctrl=%d) = %d\r\n", g_pin, ctrl, iret);
                    break;
                }

                p = Ql_strstr(pData, "Ql_GetGpioByName");
                if(p)
                {
                    s32 gpio = -1;
                    gpio = Ql_GetGpioByName(g_pin);
                    OUT_DEBUG(ql_uart_port1, "Ql_GetGpioByName(%d)=%d", g_pin, gpio);
                    break;
                }
                // transpas
                Ql_SendToModem(ql_md_port1, (u8*)pData, Ql_strlen((ascii*)pData));
                
                break;            
            }
            case EVENT_INTR:
            {
                Intr_Event *pIntEvt = &flSignalBuffer.eventData.intr_evt;
                OUT_DEBUG(ql_uart_port1,"EVENT_INTR: pinName=%d, pinState=%d\r\n",
                                            pIntEvt->pinName, pIntEvt->pinState);
                break;
            }
            case EVENT_MODEMDATA:
            {
                //TODO: receive and hanle data from CORE through virtual modem port
                PortData_Event* pPortEvt = (PortData_Event*)&flSignalBuffer.eventData.modemdata_evt;
                //Ql_DebugTrace("%s", pPortEvt->data);
                Ql_SendToUart(ql_uart_port1, (u8*)pPortEvt->data, pPortEvt->len);
                break;
            }

            case EVENT_KEY:
            {
                Key_Event* pKeyEvt = &flSignalBuffer.eventData.key_evt;
                OUT_DEBUG(ql_uart_port1, "\r\nEVENT_KEY: key_val(%x), isPressed=%d\r\n", pKeyEvt->key_val, pKeyEvt->isPressed);
                break;
            }

            case EVENT_POWERKEY:
            {
                Powerkey_Event* pPwrkeyEvt = &flSignalBuffer.eventData.powerkey_evt;
                char* pwrKeyOn = "POWERKEY_ON";
                char* pwrKeyOff = "POWERKEY_OFF";
                if (POWERKEY_ON == pPwrkeyEvt->powerkey)
                {
                    Ql_sprintf(buffer, "\r\nEVENT_KEY: POWERKEY_ON");
                }
                else
                {
                    Ql_sprintf(buffer, "\r\nEVENT_KEY: POWERKEY_OFF");
                }
                if (pPwrkeyEvt->isPressed)
                {
                    Ql_sprintf(buffer + Ql_strlen(buffer), ", Press Key Down\r\n");
                }
                else
                {
                    Ql_sprintf(buffer + Ql_strlen(buffer), ", Release Key\r\n");
                }
                Ql_SendToUart(ql_uart_port1, (u8*)buffer, Ql_strlen(buffer));  
                break;
            }
            
            default:
                break;
        }
    }
}

#endif 
