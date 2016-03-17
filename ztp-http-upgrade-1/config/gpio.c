/***************************************************************************
 *
 *            Quectel OpenCPU
 *
 *           Copyright (c) 2009 The Quectel Ltd.
 *
 ***************************************************************************
 *
 *  Designed by    :  Jay xin
 *  Coded by       :  Jay xin
 *  Tested by      :
 *
 ***************************************************************************
 ***************************************************************************/
#include "Ql_type.h"
#include "ql_pin.h"


/*========================================================================
|
| IMPORTANT NOTES:
|------------------
|
| This list is for some special requirements. Such as MIC channel selection
| for handfree mode, headset detection, audio PA control.
|
\=========================================================================*/
const QlCustomerConfig Customer_user_qlconfig =
{
    /******************************************************************
    * handfree config
    * if user want to control PA with GPIO, user must config gpio mode and output direction in Customer_QlPinConfigTable
    * example , use QL_PINNAME_GPIO1 to control PA, user must config below in Customer_QlPinConfigTable
    *{ {QL_PINNAME_GPIO1, QL_PINSUBSCRIBE_UNSUB, QL_PINMODE_1, QL_PINPULLENABLE_ENABLE,QL_PINDIRECTION_OUT,QL_PINLEVEL_LOW,0} 
    *******************************************************************/
    QL_PINNAME_MAX, // this GPIO control PA when Handfree
                                // if config QL_PINNAME_MAX, indicate do not control PA
    QL_MIC_MIC2,    // QL_MIC_MIC2 indicate use MIC2 when handfree, QL_MIC_MIC1 indicate use MIC1 when handfree

    /******************************************************************
    *headset config
    * if user want to detect headset plugin or  plugout with GPIO, this GPIO must have eint function, 
    * user must config this GPIO to gpio mode and input direction in Customer_QlPinConfigTable
    * example , use QL_PINNAME_GPIO0 to detect headset , user must config below in Customer_QlPinConfigTable
    *{QL_PINNAME_GPIO0,QL_PINSUBSCRIBE_UNSUB,QL_PINMODE_1,QL_PINPULLENABLE_ENABLE,QL_PINDIRECTION_IN,0,0} 
    *******************************************************************/
    QL_PINNAME_MAX, /* this GPIO must has input function, EINT function, to detect headset plugin or plugout
                     * if config QL_PINNAME_MAX, indicate do not detect headset, and QL_ADC_ADC0 config is invalide at below
                     */
    FALSE,          // not capture headset ADC value
    100,            // headset plugin or plugout eint debounce handle, unit is 10ms, so , here is 1S
    QL_ADC_MAX,     //QL_ADC_ADC0 indicate use adc0 to detect headset, 0 adc0,QL_ADC_TEMP_BAT indicate use adc0 to detect headset, 0 TEMP_BAT
    2800000,        //this is high level value,  level rang of high level value an when low level value  headset plugin
    500000,         // this is low level value,  , level rang of high level value an when low level value  headset plugin
    300000,         // max level value when headset plugin and press sendkey

    //<<< power config
    TRUE,           // FALSE indicate user to power on, TRUE indicate module core auto to power on
    TRUE,           // FALSE indicate user to power off, TRUE indicate module core auto to power off
    //end-of 'power config' >>>

    //<<< UART config
    TRUE,           /* bool uart3supportvfifo;// TRUE indicate VFIFO, to use vfifo functions, example  Ql_UartSetVfifoThreshold,Ql_UartMaxGetVfifoThresholdInfo, Ql_UartGetVfifoThresholdInfo
                     * FALSE indicate Generic uart driver and , to use Ql_UartSetGenericThreshold, Ql_UartGenericClearFEFlag, EVENT_UARTFE
                     */
    // end-of 'uart config' >>>
    
    /******************************************************************
    * USB Config
    *******************************************************************/
    USB_NORMAL,        // USB as vCOM or MASS STORAGE. By default, usb is configured as NORMAL mode, 
                       // which means USB can be only used to supply power and download firmware.
    
    /******************************************************************
    * Pins Config for External Watchdog
    *******************************************************************/
    QL_PINNAME_MAX,//ablaze,20120809,this GPIO use for feed external WDT
    QL_PINNAME_MAX,

    /******************************************************************
    * In the process of updating,through the specified pin output level indication.
    * 
    *******************************************************************/
    QL_PINNAME_MAX,//ablaze,20121126,this GPIO is used to set the specified pin.
    END_OF_QL_PINLEVEL,// 0, slow, 1, high, 2, alternate //2,currently,not implemented 
    0,
    0,
};


/*========================================================================
|
| IMPORTANT NOTES:
|------------------
|
| This is the initialization list for GPIOs at the early of module booting.
| Developer can do configuring here if some GPIOs need to be initialized at
| the early booting. For example, some GPIO is used to control the power 
| supply of peripheral.
|
| Besides this config list, developer may call Ql_pinSubscribe() to set
| the working mode and perameters of I/O interfaces dynamically. But it's
| later than the previous method on time sequence.
\=========================================================================*/
const QlPinConfigTable Customer_QlPinConfigTable =
{
    QL_PIN_VERSION,
    QL_OPENCPU_FLAG,
    {
        /*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        { Pin Name           |         Pin State            |     Pin Mode     |         Pull Enable           |         Direction       |       Level         |         Pull Selection         }
         *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
#if 0 // If needed, config GPIOs here
        /**************************************************
        *  <<<BEGIN: The fowllowing PINs for all modules
        *            (TOTAL: 19)
        ***************************************************/
        {QL_PINNAME_NETLIGHT,       QL_PINSUBSCRIBE_UNSUB,      QL_PINMODE_1,       QL_PINPULLENABLE_ENABLE,        QL_PINDIRECTION_OUT,    QL_PINLEVEL_LOW,        QL_PINPULLSEL_PULLDOWN      },
        {QL_PINNAME_STATUS,         QL_PINSUBSCRIBE_UNSUB,      QL_PINMODE_1,       QL_PINPULLENABLE_ENABLE,        QL_PINDIRECTION_OUT,    QL_PINLEVEL_HIGH,       QL_PINPULLSEL_PULLDOWN      },
        {QL_PINNAME_KBR0,           QL_PINSUBSCRIBE_UNSUB,      QL_PINMODE_1,       QL_PINPULLENABLE_DISABLE,       QL_PINDIRECTION_OUT,    QL_PINLEVEL_LOW,        QL_PINPULLSEL_PULLDOWN      },
        {QL_PINNAME_KBR1,           QL_PINSUBSCRIBE_UNSUB,      QL_PINMODE_1,       QL_PINPULLENABLE_ENABLE,        QL_PINDIRECTION_OUT,    QL_PINLEVEL_LOW,        QL_PINPULLSEL_PULLDOWN      },
        {QL_PINNAME_KBR2,           QL_PINSUBSCRIBE_UNSUB,      QL_PINMODE_1,       QL_PINPULLENABLE_ENABLE,        QL_PINDIRECTION_OUT,    QL_PINLEVEL_LOW,        QL_PINPULLSEL_PULLDOWN      },
        {QL_PINNAME_KBR3,           QL_PINSUBSCRIBE_UNSUB,      QL_PINMODE_1,       QL_PINPULLENABLE_ENABLE,        QL_PINDIRECTION_OUT,    QL_PINLEVEL_LOW,        QL_PINPULLSEL_PULLDOWN      },
        {QL_PINNAME_KBR4,           QL_PINSUBSCRIBE_UNSUB,      QL_PINMODE_1,       QL_PINPULLENABLE_DISABLE,       QL_PINDIRECTION_OUT,    QL_PINLEVEL_LOW,        QL_PINPULLSEL_PULLDOWN      },
        {QL_PINNAME_KBC0,           QL_PINSUBSCRIBE_UNSUB,      QL_PINMODE_1,       QL_PINPULLENABLE_ENABLE,        QL_PINDIRECTION_IN,     QL_PINLEVEL_HIGH,       QL_PINPULLSEL_PULLUP        },
        {QL_PINNAME_KBC1,           QL_PINSUBSCRIBE_UNSUB,      QL_PINMODE_1,       QL_PINPULLENABLE_ENABLE,        QL_PINDIRECTION_IN,     QL_PINLEVEL_HIGH,       QL_PINPULLSEL_PULLUP        },
        {QL_PINNAME_KBC2,           QL_PINSUBSCRIBE_UNSUB,      QL_PINMODE_1,       QL_PINPULLENABLE_ENABLE,        QL_PINDIRECTION_IN,     QL_PINLEVEL_HIGH,       QL_PINPULLSEL_PULLUP        },
        {QL_PINNAME_KBC3,           QL_PINSUBSCRIBE_UNSUB,      QL_PINMODE_1,       QL_PINPULLENABLE_ENABLE,        QL_PINDIRECTION_IN,     QL_PINLEVEL_HIGH,       QL_PINPULLSEL_PULLUP        },
        {QL_PINNAME_KBC4,           QL_PINSUBSCRIBE_UNSUB,      QL_PINMODE_1,       QL_PINPULLENABLE_ENABLE,        QL_PINDIRECTION_IN,     QL_PINLEVEL_HIGH,       QL_PINPULLSEL_PULLUP        },
        {QL_PINNAME_DCD,            QL_PINSUBSCRIBE_UNSUB,      QL_PINMODE_1,       QL_PINPULLENABLE_ENABLE,        QL_PINDIRECTION_OUT,    QL_PINLEVEL_LOW,        QL_PINPULLSEL_PULLDOWN      },
        {QL_PINNAME_RI,             QL_PINSUBSCRIBE_UNSUB,      QL_PINMODE_1,       QL_PINPULLENABLE_ENABLE,        QL_PINDIRECTION_IN,     QL_PINLEVEL_LOW,        QL_PINPULLSEL_PULLDOWN      },        
        {QL_PINNAME_DTR,            QL_PINSUBSCRIBE_UNSUB,      QL_PINMODE_1,       QL_PINPULLENABLE_ENABLE,        QL_PINDIRECTION_IN,     QL_PINLEVEL_HIGH,       QL_PINPULLSEL_PULLUP        },
        {QL_PINNAME_CTS,            QL_PINSUBSCRIBE_UNSUB,      QL_PINMODE_1,       QL_PINPULLENABLE_DISABLE,       QL_PINDIRECTION_OUT,    QL_PINLEVEL_LOW,        QL_PINPULLSEL_PULLDOWN      },
        {QL_PINNAME_RTS,            QL_PINSUBSCRIBE_UNSUB,      QL_PINMODE_1,       QL_PINPULLENABLE_ENABLE,        QL_PINDIRECTION_IN,     QL_PINLEVEL_LOW,        QL_PINPULLSEL_PULLDOWN      },
        {QL_PINNAME_SIM_PRESENCE,   QL_PINSUBSCRIBE_UNSUB,      QL_PINMODE_1,       QL_PINPULLENABLE_ENABLE,        QL_PINDIRECTION_IN,     QL_PINLEVEL_HIGH,       QL_PINPULLSEL_PULLUP        },
        /******************************
        * PINs for all modules, END>>>
        *******************************/
        
        /**********************************************
        *  <<<BEGIN: The fowllowing PINs for M80
        *            (TOTAL: 15)
        ***********************************************/
        {QL_PINNAME_DOWNLOAD,       QL_PINSUBSCRIBE_UNSUB,      QL_PINMODE_1,       QL_PINPULLENABLE_ENABLE,        QL_PINDIRECTION_IN,     QL_PINLEVEL_HIGH,       QL_PINPULLSEL_PULLUP        },
        {QL_PINNAME_CS_D0,          QL_PINSUBSCRIBE_UNSUB,      QL_PINMODE_1,       QL_PINPULLENABLE_ENABLE,        QL_PINDIRECTION_OUT,    QL_PINLEVEL_LOW,        QL_PINPULLSEL_PULLDOWN      },
        {QL_PINNAME_CS_D1,          QL_PINSUBSCRIBE_UNSUB,      QL_PINMODE_1,       QL_PINPULLENABLE_ENABLE,        QL_PINDIRECTION_OUT,    QL_PINLEVEL_LOW,        QL_PINPULLSEL_PULLDOWN      },
        {QL_PINNAME_CS_D2,          QL_PINSUBSCRIBE_UNSUB,      QL_PINMODE_1,       QL_PINPULLENABLE_ENABLE,        QL_PINDIRECTION_OUT,    QL_PINLEVEL_LOW,        QL_PINPULLSEL_PULLDOWN      },
        {QL_PINNAME_CS_D3,          QL_PINSUBSCRIBE_UNSUB,      QL_PINMODE_1,       QL_PINPULLENABLE_ENABLE,        QL_PINDIRECTION_OUT,    QL_PINLEVEL_LOW,        QL_PINPULLSEL_PULLDOWN      },
        {QL_PINNAME_CS_D4,          QL_PINSUBSCRIBE_UNSUB,      QL_PINMODE_1,       QL_PINPULLENABLE_ENABLE,        QL_PINDIRECTION_OUT,    QL_PINLEVEL_LOW,        QL_PINPULLSEL_PULLDOWN      },
        {QL_PINNAME_CS_D5,          QL_PINSUBSCRIBE_UNSUB,      QL_PINMODE_1,       QL_PINPULLENABLE_ENABLE,        QL_PINDIRECTION_OUT,    QL_PINLEVEL_LOW,        QL_PINPULLSEL_PULLDOWN      },
        {QL_PINNAME_CS_D6,          QL_PINSUBSCRIBE_UNSUB,      QL_PINMODE_1,       QL_PINPULLENABLE_ENABLE,        QL_PINDIRECTION_OUT,    QL_PINLEVEL_LOW,        QL_PINPULLSEL_PULLDOWN      },
        {QL_PINNAME_CS_D7,          QL_PINSUBSCRIBE_UNSUB,      QL_PINMODE_1,       QL_PINPULLENABLE_ENABLE,        QL_PINDIRECTION_OUT,    QL_PINLEVEL_LOW,        QL_PINPULLSEL_PULLDOWN      },
        {QL_PINNAME_CS_HSYNC,       QL_PINSUBSCRIBE_UNSUB,      QL_PINMODE_1,       QL_PINPULLENABLE_ENABLE,        QL_PINDIRECTION_OUT,    QL_PINLEVEL_LOW,        QL_PINPULLSEL_PULLDOWN      },
        {QL_PINNAME_CS_VSYNC,       QL_PINSUBSCRIBE_UNSUB,      QL_PINMODE_1,       QL_PINPULLENABLE_ENABLE,        QL_PINDIRECTION_OUT,    QL_PINLEVEL_LOW,        QL_PINPULLSEL_PULLDOWN      },
        {QL_PINNAME_CS_PWDN,        QL_PINSUBSCRIBE_UNSUB,      QL_PINMODE_1,       QL_PINPULLENABLE_ENABLE,        QL_PINDIRECTION_OUT,    QL_PINLEVEL_LOW,        QL_PINPULLSEL_PULLDOWN      },
        {QL_PINNAME_CS_MCLK,        QL_PINSUBSCRIBE_UNSUB,      QL_PINMODE_1,       QL_PINPULLENABLE_ENABLE,        QL_PINDIRECTION_OUT,    QL_PINLEVEL_LOW,        QL_PINPULLSEL_PULLDOWN      },
        {QL_PINNAME_CS_PCLK,        QL_PINSUBSCRIBE_UNSUB,      QL_PINMODE_1,       QL_PINPULLENABLE_ENABLE,        QL_PINDIRECTION_OUT,    QL_PINLEVEL_LOW,        QL_PINPULLSEL_PULLDOWN      },
        {QL_PINNAME_CS_RST,         QL_PINSUBSCRIBE_UNSUB,      QL_PINMODE_1,       QL_PINPULLENABLE_ENABLE,        QL_PINDIRECTION_OUT,    QL_PINLEVEL_LOW,        QL_PINPULLSEL_PULLDOWN      },
        {QL_PINNAME_PCM_RST,        QL_PINSUBSCRIBE_UNSUB,      QL_PINMODE_1,       QL_PINPULLENABLE_ENABLE,        QL_PINDIRECTION_OUT,    QL_PINLEVEL_LOW,        QL_PINPULLSEL_PULLDOWN      },
        /******************************
        * PINs for M80, END>>>
        *******************************/
        
        /**********************************************
        *  <<<BEGIN: The fowllowing PINs for M50/M80
        ***********************************************/
        {QL_PINNAME_PCM_IN,         QL_PINSUBSCRIBE_UNSUB,      QL_PINMODE_1,       QL_PINPULLENABLE_ENABLE,        QL_PINDIRECTION_IN,     QL_PINLEVEL_LOW,        QL_PINPULLSEL_PULLDOWN      },
        {QL_PINNAME_PCM_CLK,        QL_PINSUBSCRIBE_UNSUB,      QL_PINMODE_1,       QL_PINPULLENABLE_DISABLE,       QL_PINDIRECTION_OUT,    QL_PINLEVEL_LOW,        QL_PINPULLSEL_PULLDOWN      },
        {QL_PINNAME_PCM_OUT,        QL_PINSUBSCRIBE_UNSUB,      QL_PINMODE_1,       QL_PINPULLENABLE_ENABLE,        QL_PINDIRECTION_OUT,    QL_PINLEVEL_LOW,        QL_PINPULLSEL_PULLDOWN      },
        {QL_PINNAME_PCM_SYNC,       QL_PINSUBSCRIBE_UNSUB,      QL_PINMODE_1,       QL_PINPULLENABLE_ENABLE,        QL_PINDIRECTION_OUT,    QL_PINLEVEL_LOW,        QL_PINPULLSEL_PULLDOWN      },
        /******************************
        * PINs for M50/M80, END>>>
        *******************************/

            
        /**********************************************
        *  <<<BEGIN: The fowllowing PINs for M12E/M50
        ***********************************************/
        {QL_PINNAME_GPIO0,          QL_PINSUBSCRIBE_UNSUB,      QL_PINMODE_1,       QL_PINPULLENABLE_ENABLE,        QL_PINDIRECTION_IN,     QL_PINLEVEL_HIGH,       QL_PINPULLSEL_PULLUP        },
        /******************************
        * PINs for M12E/M50, END>>>
        *******************************/
        
        /**********************************************
        *  <<<BEGIN: The fowllowing PINs for M12E
        ***********************************************/
        {QL_PINNAME_SD_DATA,        QL_PINSUBSCRIBE_UNSUB,      QL_PINMODE_1,       QL_PINPULLENABLE_ENABLE,        QL_PINDIRECTION_IN,     QL_PINLEVEL_LOW,        QL_PINPULLSEL_PULLUP        },
        {QL_PINNAME_SD_CLK,         QL_PINSUBSCRIBE_UNSUB,      QL_PINMODE_1,       QL_PINPULLENABLE_ENABLE,        QL_PINDIRECTION_IN,     QL_PINLEVEL_LOW,        QL_PINPULLSEL_PULLUP        },
        {QL_PINNAME_SD_CMD,         QL_PINSUBSCRIBE_UNSUB,      QL_PINMODE_1,       QL_PINPULLENABLE_ENABLE,        QL_PINDIRECTION_IN,     QL_PINLEVEL_HIGH,       QL_PINPULLSEL_PULLUP        },
        {QL_PINNAME_GPIO1_KBC5,     QL_PINSUBSCRIBE_UNSUB,      QL_PINMODE_1,       QL_PINPULLENABLE_ENABLE,        QL_PINDIRECTION_IN,     QL_PINLEVEL_HIGH,       QL_PINPULLSEL_PULLUP        },
        {QL_PINNAME_GPIO2,          QL_PINSUBSCRIBE_UNSUB,      QL_PINMODE_1,       QL_PINPULLENABLE_ENABLE,        QL_PINDIRECTION_OUT,    QL_PINLEVEL_LOW,        QL_PINPULLSEL_PULLDOWN      },
        {QL_PINNAME_GPIO3,          QL_PINSUBSCRIBE_UNSUB,      QL_PINMODE_1,       QL_PINPULLENABLE_ENABLE,        QL_PINDIRECTION_OUT,    QL_PINLEVEL_LOW,        QL_PINPULLSEL_PULLDOWN      },
        {QL_PINNAME_GPIO4,          QL_PINSUBSCRIBE_UNSUB,      QL_PINMODE_1,       QL_PINPULLENABLE_ENABLE,        QL_PINDIRECTION_IN,     QL_PINLEVEL_LOW,        QL_PINPULLSEL_PULLUP        },
        /******************************
        *  PINs for M12E, END>>>
    	*******************************/
#endif

        // Please Keep the Following Line of Statement.
        {QL_PINNAME_MAX,            QL_PINSUBSCRIBE_UNSUB,      QL_PINMODE_UNSET,   0,                              0,                      0,                      0                           } /*must end with QL_PINNAME_MAX*/
    }
};

