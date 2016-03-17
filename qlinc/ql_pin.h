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
 |  Designed by    :    Jay XIN
 |  Coded by       :    Jay XIN
 |  Tested by      :    Stanley YONG
 |
 \=========================================================================*/

#ifndef __QL_PIN_H__
#define __QL_PIN_H__

#define   QL_OPENCPU_FLAG         "QUECTEL-OPENCPU-V1.00\0"
#define   QL_OPENCPU_FLAG_MAXLEN  30

#define   QL_PIN_VERSION    0x100

typedef enum QlADCPintag
{
    QL_PIN_ADC0,
    QL_PIN_ADC1,
    QL_PIN_ADC_MAX
}QlADCPin;


/****************************************************************************
 * Pin Name Enumeration for All Supported Modules.
 * Please Refer to the document 'OpenCPU Development Guide' for Complete 
 * Pin Names for Each Module.
 ***************************************************************************/
typedef enum QlPinNameTag
{   
    /**************************************************
    *  <<<BEGIN: The fowllowing PINs for all modules
    *            (TOTAL: 19)
    ***************************************************/
    QL_PINNAME_NETLIGHT = 0,
    QL_PINNAME_STATUS,
    QL_PINNAME_PCM_SYNC,
    QL_PINNAME_KBR0,
    QL_PINNAME_KBR1,
    QL_PINNAME_KBR2,
    QL_PINNAME_KBR3,
    QL_PINNAME_KBR4,
    QL_PINNAME_KBC0,
    QL_PINNAME_KBC1,
    QL_PINNAME_KBC2,
    QL_PINNAME_KBC3,
    QL_PINNAME_KBC4,
    QL_PINNAME_DCD,
    QL_PINNAME_RI,
    QL_PINNAME_DTR,
    QL_PINNAME_CTS,
    QL_PINNAME_RTS,
    QL_PINNAME_SIM_PRESENCE,
    /******************************
    * PINs for all modules, END>>>
    *******************************/

    /**********************************************
    *  <<<BEGIN: The fowllowing PINs for M80
    *            (TOTAL: 15)
    ***********************************************/
    QL_PINNAME_DOWNLOAD,

    QL_PINNAME_CS_D3,
    QL_PINNAME_CS_D0,
    QL_PINNAME_CS_VSYNC,
    QL_PINNAME_CS_D4,
    QL_PINNAME_CS_D7,
    QL_PINNAME_CS_D5,
    QL_PINNAME_CS_PWDN,
    QL_PINNAME_CS_D1,
    QL_PINNAME_CS_D2,
    QL_PINNAME_CS_PCLK,
    QL_PINNAME_CS_HSYNC,
    QL_PINNAME_CS_MCLK,
    QL_PINNAME_CS_D6,
    QL_PINNAME_CS_RST,
    /******************************
    * PINs for M80, END>>>
    *******************************/
    
    /**********************************************
    *  <<<BEGIN: The fowllowing PINs for M50/M80
    ***********************************************/
    QL_PINNAME_PCM_IN,
    QL_PINNAME_PCM_CLK,
    QL_PINNAME_PCM_OUT,
    /******************************
    * PINs for M50/M80, END>>>
    *******************************/

    
    /**********************************************
    *  <<<BEGIN: The fowllowing PINs for M12E/M80
    ***********************************************/
    QL_PINNAME_PCM_RST,
    /******************************
    * PINs for M12E/M80, END>>>
    *******************************/

    
    /**********************************************
    *  <<<BEGIN: The fowllowing PINs for M12E/M50
    ***********************************************/
    QL_PINNAME_GPIO0,
    /******************************
    * PINs for M12E/M50, END>>>
    *******************************/


    /**********************************************
    *  <<<BEGIN: The fowllowing PINs for M12E
    ***********************************************/
    QL_PINNAME_SD_DATA,
    QL_PINNAME_SD_CLK,
    QL_PINNAME_SD_CMD,
    QL_PINNAME_GPIO2 = QL_PINNAME_PCM_SYNC,
    QL_PINNAME_GPIO3 = QL_PINNAME_PCM_RST,
    QL_PINNAME_GPIO1_KBC5 = QL_PINNAME_SD_CMD + 1,
    QL_PINNAME_GPIO4,
    /******************************
    *  PINs for M12E, END>>>
	*******************************/


    /***************************************************************
    *  <<<BEGIN: The fowllowing PINs for LCD option on M50/M80.
    *            And they're dedicated PINs, not multiplexing PINs.
    ****************************************************************/
    QL_PINNAME_LSCK,
    QL_PINNAME_LSDA,
    QL_PINNAME_LSA0,
    QL_PINNAME_LSCS,
    QL_PINNAME_LSRST,
    /******************************
    *  PINs for LCD, END>>>
	*******************************/
    
    QL_PINNAME_MAX
}QlPinName;

typedef enum QlPinSubscribeTag
{
    QL_PINSUBSCRIBE_UNSUB = 0,
    QL_PINSUBSCRIBE_SUB,
    QL_PINSUBSCRIBE_BUSSUB,     /* customer not use this parameter */
    END_OF_QL_PINSUBSCRIBE
}QlPinSubscribe;

typedef enum QlPinModeTag
{
    QL_PINMODE_1 = 0,
    QL_PINMODE_2 = 1,
    QL_PINMODE_3 = 2,
    QL_PINMODE_4 = 3,
    QL_PINMODE_UNSET = 255 /* not select function, pin status is hardware reset status */
}QlPinMode;

typedef enum QlPinDirectionTag
{
    QL_PINDIRECTION_IN= 0,
    QL_PINDIRECTION_OUT,
    END_OF_QL_PINDIRECTION
}QlPinDirection;

typedef enum QlPinLevelTag
{
    QL_PINLEVEL_LOW = 0,
    QL_PINLEVEL_HIGH,
    QL_PINLEVEL_ALTERNATE, //currently,not implemented 
    END_OF_QL_PINLEVEL
}QlPinLevel;

typedef enum QlPinPullEnableTag
{
    QL_PINPULLENABLE_DISABLE= 0,
    QL_PINPULLENABLE_ENABLE,
    END_OF_QL_PINPULLENABLE
}QlPinPullEnable;

typedef enum QlPinPullSelTag
{
    QL_PINPULLSEL_PULLDOWN = 0,
    QL_PINPULLSEL_PULLUP
}QlPinPullSel;

typedef enum QlClockSourceTag
{
    QL_CLOCKSOURCE_26M= 0,
    QL_CLOCKSOURCE_13M,
    QL_CLOCKSOURCE_6DOT5M,
    QL_CLOCKSOURCE_32K,
    END_OF_QL_CLOCKSOURCE
}QlClockSource;

typedef enum QlPwmSourceTag
{
    QL_PWMSOURCE_13M= 0,
    QL_PWMSOURCE_32K,
    END_OF_QL_PWMSOURCE
}QlPwmSource;

typedef enum QlPwmSourceDivTag
{
    QL_PWMSOURCE_DIV1= 0,
    QL_PWMSOURCE_DIV2,
    QL_PWMSOURCE_DIV4,
    QL_PWMSOURCE_DIV8,
    END_OF_QL_PWMSOURCE_DIV
}QlPwmSourceDiv;


typedef enum QlAlertClockTag
{
    QL_ALERTCLOCK_13M= 0,
    QL_ALERTCLOCK_13DIV2M,
    QL_ALERTCLOCK_13DIV4M,
    QL_ALERTCLOCK_13DIV8M,
    END_OF_QL_QL_ALERTCLOCK
}QlAlertClock;

typedef enum QlAlertModeTag
{
    QL_ALERTMODE_1= 0,
    QL_ALERTMODE_2,
    QL_ALERTMODE_3,
    END_OF_QL_ALERTMODE_
}QlAlertMode;

typedef enum QlEintTriggerTypeTag
{
    QL_EINTTRIGGERTYPE_CALLBACK= 5,
    END_OF_QL_EINTTRIGGERTYPE
}QlEintTriggerType;

typedef enum QlEintSensitiveTypeTag
{
    QL_EINTSENSITIVETYPE_EDGE= 0,
    QL_EINTSENSITIVETYPE_LEVEL,
    END_OF_QL_EINTSENSITIVETYPE
}QlEintSensitiveType;

typedef enum QlPinControlTag
{
    QL_PINCONTROL_STOP,
    QL_PINCONTROL_START,
    END_OF_QL_PINCONTROL
}QlPinControl;

typedef enum QlMicNameTag
{
    QL_MIC_MIC1 = 0,
    QL_MIC_MIC2,
    QL_MIC_MAX
}QlMicName;

typedef enum QlAdcNameTag
{
    QL_ADC_ADC0 = 0,
    QL_ADC_TEMP_BAT = 2,
    QL_ADC_MAX
}QlAdcName;

typedef enum QlUsbModeTag
{
    USB_NORMAL  = 0,
    USB_AS_VCOM = 1,   // usb as virtual serial port
    USB_AS_MS   = 2    // usb as mass storage
}QlUsbMode;

/****************************************************************************
 * User Configuration Structure
 ***************************************************************************/
typedef struct QlCustomerConfigTag
{
    /******************************************************************
    * Handfree Config
    * if user want to control PA with GPIO, user must config gpio mode and output direction in Customer_QlPinConfigTable
    *******************************************************************/
    QlPinName handfreeamplifierpin;  // this GPIO control PA when Handfree
                                        // if config QL_PINNAME_MAX, indicate do not control PA
    QlMicName handfreeinputmic;       // QL_MIC_MIC2 indicate use MIC2 when handfree, QL_MIC_MIC1 indicate use MIC1 when handfree
    
    /******************************************************************
    * Headset Config
    * if user want to detect headset plugin or  plugout with GPIO, this GPIO must have eint function, 
    * user must config this GPIO to gpio mode and input direction in Customer_QlPinConfigTable
    *******************************************************************/
    QlPinName headsetdetectpin; // this GPIO must has input function, EINT function, to detect headset plugin or plugout
                                  // if config QL_PINNAME_MAX, indicate do not detect headset, and QL_ADC_ADC0 config is invalide at below 
    bool headsetadccapture;
    u8 headsetdetectdebounce;       // headset plugin or plugout eint debounce handle, unit is 10ms, so , here is 1S
    QlAdcName headsetdetectadc;     // QL_ADC_ADC0 indicate use adc0 to detect headset, 0 adc0,QL_ADC_TEMP_BAT indicate use adc0 to detect headset, 0 TEMP_BAT
    u32 headsetadchigh;             // this is high level value,  level rang of high level value an when low level value  headset plugin
    u32 headsetadclow;              // this is low level value,  , level rang of high level value an when low level value  headset plugin
    u32 headsetadcsendkey;          // max level value when headset plugin and press sendkey
    
    /******************************************************************
    * Power Config
    *******************************************************************/
    bool powerautoon;   // FALSE indicate user to power on, TRUE indicate module core auto to power on
    bool powerautooff;  // FALSE indicate user to power off, TRUE indicate module core auto to power off
    
    /******************************************************************
    * UART Config
    *******************************************************************/
    bool uart3supportvfifo; // TRUE indicate VFIFO, to use vfifo functions, example  Ql_UartSetVfifoThreshold,Ql_UartMaxGetVfifoThresholdInfo, Ql_UartGetVfifoThresholdInfo
                              // FALSE indicate Generic uart driver and , to use Ql_UartSetGenericThreshold, Ql_UartGenericClearFEFlag, EVENT_UARTFE
    
    /******************************************************************
    * USB Config
    *******************************************************************/
    QlUsbMode usb_mode;// USB as vCOM or MASS STORAGE. By default, usb is configured as NORMAL mode, 
                       // which means USB can be only used to supply power and download firmware.
    
    /******************************************************************
    * watchdog Config, support two PINs as watchdog. If only need one
    * pin as WDT, please set the other field to 'QL_PINNAME_MAX'.
    *******************************************************************/
    QlPinName wdt_pin1; //ablaze,20120809,this GPIO is used to be external WDT
    QlPinName wdt_pin2;

    /******************************************************************
    * In the process of updating,through the specified pin output level indication.
    * 
    *******************************************************************/
    QlPinName  updt_ind_pin;//ablaze,20121126,this GPIO is used to set the specified pin.
    QlPinLevel  updt_ind_lvl; // 0, slow, 1, high, 2, alternate //2,currently,not implemented 
    u32        reserved1;
    u32        reserved2;
}QlCustomerConfig;


/****************************************************************************
 * Pin Item Struct
 ***************************************************************************/
typedef struct QlPinConfigItemTag
{
    QlPinName         pinname;
    QlPinSubscribe    subscribe;
    QlPinMode         pinmode;
    u32               parameter1;    
    u32               parameter2;    
    u32               parameter3;    
    u32               parameter4;    
}QlPinConfigItem;

typedef struct QlPinConfigTableTag
{
    s16                 pinconfigversion;   /* now this version is QL_PIN_VERSION */
    s8                  quectelflg[QL_OPENCPU_FLAG_MAXLEN];
    QlPinConfigItem     pinconfigitem[QL_PINNAME_MAX+1];   /* must end item is QL_PINNAME_MAX */
}QlPinConfigTable;


/****************************************************************************
 * Pin Parameters Definition
 ***************************************************************************/
typedef struct QlGpioParameterTag
{
    QlPinPullEnable     pinpullenable;  
    QlPinDirection      pindirection;  
    QlPinLevel          pinlevel;
    QlPinPullSel        pinpullsel;
}QlGpioParameter;

typedef struct  QlEintParameterTag
{
    QlEintSensitiveType  eintsensitivetype;
    s32                  hardware_de_bounce; /* ms */
    s32                  software_de_bounce; /* ms */
}QlEintParameter;

typedef void (*eint_callback_fun)(u8 eintno);
typedef struct  QlEintCallBackParameterTag
{
    QlEintTriggerType     einttriggertype;
    QlEintSensitiveType   eintsensitivetype;
    s32                   hardware_de_bounce; /* numbers of 32KHz clock cycles */
    s32                   software_de_bounce; /* ms */
    bool                  automask;
    eint_callback_fun     callbak_fun;  
}QlEintCallBackParameter;

typedef struct   QlClockParameterTag
{
    QlClockSource   clocksource;
}QlClockParameter;

typedef struct   QlPwmParameterTag
{
    QlPwmSource      pwmsource;
    QlPwmSourceDiv   pwmclkdiv;
    u32              lowpulesnumber;
    u32              highpulesnumber;
}QlPwmParameter;

typedef struct  QlAlertParameterTag
{
    QlAlertClock  alertclock;
    QlAlertMode   alertmode;
    u16  alertcounter1;
    u16  alertcounter2;
    u16  alertthreshold;
}QlAlertParameter;

typedef union QlPinParameterUnionTag
{
    QlGpioParameter         gpioparameter;
    QlEintParameter         eintparameter;
    QlEintCallBackParameter eintcallbackparameter;
    QlClockParameter        clockparameter;
    QlPwmParameter          pwmparameter;
    QlAlertParameter        alertparameter;
}QlPinParameterUnion;

typedef struct QlPinParameterTag
{
    s16                     pinconfigversion;   /* now this version is QL_PIN_VERSION */
    QlPinParameterUnion     pinparameterunion;     
}QlPinParameter;


typedef void (*OCPU_CB_READ_ADC)(QlADCPin adc_pin, u8 status, u16 adc_val);

/******************************************************************************
* Function:     Ql_pinSubscribe
*  
* Description:
*               This function sets pin's mode.
*
* Parameters:    
*               pinname:
*                       Pin name
*
*               pinmode:
*                       Pin mode, see QlPinMode structure
*
*               pinparameter:
*                       A pointer to QlPinParameter.
*                       It should be NULL, if pinmode is set to 'QL_PINMODE_1'.
* Return:  
*               QL_RET_OK indicates success.
*               Negative indicates failure. please see Error Code Definition.
******************************************************************************/
s32  Ql_pinSubscribe(QlPinName pinname, QlPinMode pinmode, QlPinParameter *pinparameter);


/******************************************************************************
* Function:     Ql_pinQueryMode
*  
* Description:
*               This function queries pin's mode.
*
* Parameters:    
*               pinname:
*               [in]        Pin name
*
*               pinmode:
*               [out]        Pin mode, see QlPinMode structure
*
*               pinparameter:
*               [out]        A pointer to QlPinParameter
* Return:  
*               QL_RET_OK indicates success.
*               Negative indicates failure. please see Error Code Definition.
******************************************************************************/
s32  Ql_pinQueryMode(QlPinName pinname, QlPinSubscribe *subscribe, QlPinMode *pinmode, QlPinParameter *pinparameter);


/******************************************************************************
* Function:     Ql_pinUnSubscribe
*  
* Description:
*               This function resets pin's mode to the default status.
*
* Parameters:    
*               pinname:
*               [in]        Pin name
* Return:  
*               QL_RET_OK indicates success.
*               Negative indicates failure. please see Error Code Definition.
******************************************************************************/
s32  Ql_pinUnSubscribe(QlPinName pinname);


/******************************************************************************
* Function:     Ql_pinRead
*  
* Description:
*               Reads the level value from the specified pin.
*
* Parameters:    
*               pinname:
*                       [in] Pin name
*
*               pinlevel:
*                       [out] A pointer to the pin level
* Return:  
*               QL_RET_OK indicates success.
*               Negative indicates failure. please see Error Code Definition.
******************************************************************************/
s32  Ql_pinRead(QlPinName pinname, QlPinLevel *pinlevel);


/******************************************************************************
* Function:     Ql_pinWrite
*  
* Description:
*               Writes a level value to the specified pin.
*
* Parameters:    
*               pinname:
*               [in]        Pin name
*
*               pinlevel:
*               [in]        A new pin level value
* Return:  
*               QL_RET_OK indicates success.
*               Negative indicates failure. please see Error Code Definition.
******************************************************************************/
s32  Ql_pinWrite(QlPinName pinname, QlPinLevel pinlevel);


/******************************************************************************
* Function:     Ql_pinControl
*  
* Description:
*               Sets the working status of pin. This function works for those
*               pins which pin-mode were set to CLOCK-OUT, PWM, and ALERT.
*
* Parameters:    
*               pinname:
*               [in]        Pin name
*
*               pincontrol:
*               [in]        Control parameter, see the definitin of 'QlPinControl'
* Return:  
*               QL_RET_OK indicates success.
*               Negative indicates failure. please see Error Code Definition.
******************************************************************************/
s32  Ql_pinControl(QlPinName pinname, QlPinControl pincontrol);


/******************************************************************************
* Function:     Ql_eintSetPolarity
*  
* Description:
*               Sets the polarity of the level of external interruption source.
*
* Parameters:    
*               eintno:
*               [in]        External interruption number
*
*               Polaritylevel:
*               [in]        Level polarity, see the definition of 'QlPinLevel'
* Return:  
*               None
******************************************************************************/
void Ql_eintSetPolarity(u8 eintno, QlPinLevel Polaritylevel);


/******************************************************************************
* Function:     Ql_eintMask
*  
* Description:
*               The function masks or unmasks a specified external interruption.
*
* Parameters:    
*               eintno:
*               [in]        External interruption number
*
*               mask:
*               [in]        TRUE or FALSE, mask or unmask
* Return:  
*               None
******************************************************************************/
void Ql_eintMask(u8 eintno, bool mask);


/******************************************************************************
* Function:     Ql_eintRead
*  
* Description:
*               Read levle state from a specified interruption pin.
*
* Parameters:    
*               eintno:
*                       [in] External interruption number
*
*               pinlevel:
*                       [out] A pointer to 'QlPinLevel'
* Return:  
*               None
******************************************************************************/
void Ql_eintRead(u8 eintno, QlPinLevel* pinlevel);


/******************************************************************************
* Function:     Ql_ReadADC
*  
* Description:
*               Read the level value of adc pin.
*
* Parameters:    
*               adc_pin:
*                       [in] one value of 'QlADCPin'
*
*               cb_adc:
*                       [in] callback, which will report the results.
* Return:  
*               QL_RET_OK indicates this function successes.
*               Negative indicates failure. please see Error Code Definition.
******************************************************************************/
s32 Ql_ReadADC(QlADCPin adc_pin, OCPU_CB_READ_ADC cb_adc);

/******************************************************************************
* Function:     Ql_ConfigADC
*  
* Description:
*               This function may configure the times and period of ADC sampling.
*
*               By default, the Ql_ReadADC function returns the ADC value after once sampling.
*               If the sampling time is set to be more than '1', the ADC value that Ql_ReadADC
*               returns will be the average value after several times sampling.
*
*               This function just need to be invoked once.
*
* Parameters:    
*               adc_pin:
*                       [in] one value of 'QlADCPin'
*
*               sample_count:
*                       [in] Sampling count. If the value is set to 1, then the "period" parameter will be ignored.
*                            
*               period:
*                       [in] Interval of sampling time , unit is ms. 
*                       the range is 1~100 ( ms);
* Return:  
*               QL_RET_OK indicates this function successes.
*               QL_RET_ERR_PARAM,parameter error. 
******************************************************************************/
s32 Ql_ConfigADC(QlADCPin adc_pin,u8 sample_count,u32 period);

/******************************************************************************
* Function:     QL_GetGpioByName
*  
* Description:
*               Get GPIO number by pin name.
*
* Parameters:    
*               pin:
*                       [in] pin name.
* Return:  
*               If this function successes, the gpio number is returned.
*               Negative indicates failure.
******************************************************************************/
s32 Ql_GetGpioByName(QlPinName pin);

#endif // End-of  __QL_PIN_H__

