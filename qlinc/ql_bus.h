/*==========================================================================
 |               Quectel OpenCPU --  User API
 |
 |              Copyright (c) 2010 Quectel Ltd.
 |
 |--------------------------------------------------------------------------
 |
 | File Description
 | ----------------
 |      Definitions for bus parameters & APIs
 |
 |--------------------------------------------------------------------------
 |
 |  Designed by    :    Jay XIN
 |  Coded by       :    Jay XIN
 |  Tested by      :    Stanley YONG
 |
 \=========================================================================*/

#ifndef __QL_BUS_H__
#define __QL_BUS_H__

typedef s32  QL_BUS_HANDLE;

#define     QL_BUS_VERSION     0x100


/****************************************************************************
 *  Bus Type
 ***************************************************************************/
typedef enum QlBusTypeTag
{
    QL_BUSTYPE_LCD = 0,
    QL_BUSTYPE_I2C,
    QL_BUSTYPE_UNKOWN,
    END_OF_QL_BUSTYPE
}QlBusType;


/****************************************************************************
 *  Serial Configurations for LCD
 ***************************************************************************/
//deprecated
typedef enum QlLcdSerialConfigTag
{
    QL_LCD_SERIAL_CONFIG_13MHZ_CLK          = 0x8000,
    QL_LCD_SERIAL_CONFIG_26MHZ_CLK          = 0x4000,
    QL_LCD_SERIAL_CONFIG_CS_POLARITY        = 0x0100,
    QL_LCD_SERIAL_CONFIG_9_BIT_MODE         = 0x0010,
    QL_LCD_SERIAL_CONFIG_8_BIT_MODE         = 0x0000,
    QL_LCD_SERIAL_CONFIG_CLOCK_DIVIDE_1     = 0x0000,
    QL_LCD_SERIAL_CONFIG_CLOCK_DIVIDE_2     = 0x0004,
    QL_LCD_SERIAL_CONFIG_CLOCK_DIVIDE_3     = 0x0008,
    QL_LCD_SERIAL_CONFIG_CLOCK_DIVIDE_4     = 0x000C,
    QL_LCD_SERIAL_CONFIG_CLOCK_PHASE        = 0x0002,
    QL_LCD_SERIAL_CONFIG_CLOCK_POLARITY     = 0x0001
}QlLcdSerialConfig;

//deprecated
typedef struct QlLcdParameterTag
{
    u32       lcd_serial_config; /*QlLcdSerialConfig*/
    u8        brequire_cs;
    u8        brequire_resetpin;
}QlLcdParameter;


typedef struct QlI2cParameterTag
{
    QlPinName             pin_i2cdata;
    QlPinName             pin_i2cclk;
}QlI2cParameter;

/* index of LCD pins */
enum
{
    QL_LSCK = 0,
    QL_LSDA,
    QL_LSA0,
    QL_LSCS,
    QL_LSRST,
    QL_LCD_MAX_PINS
};

#define     QL_SERIAL_LCD_BITSIZE_8     8
#define     QL_SERIAL_LCD_BITSIZE_9     9
#define     QL_SERIAL_LCD_BITSIZE_16    16
#define     QL_SERIAL_LCD_BITSIZE_18    18
#define     QL_SERIAL_LCD_BITSIZE_24    24
#define     QL_SERIAL_LCD_BITSIZE_32    32

typedef struct QlSerialLcdParamTag
{    
    u8                  bit_size;
    u32                 wr_timing;              //low 16 bits is for high count; high 16 bits is for low count; range:1-0x3ff
    u32                 rd_timing;              //low 16 bits is for high count; high 16 bits is for low count; range:1-0x3ff
    QlPinName           pins[QL_LCD_MAX_PINS];  //array index is enumerated above. set a pin
                                                //to QL_PINNAME_MAX if you don't need it.
}QlSerialLcdParam;

typedef union QlBusParameterUnionTag
{
    QlLcdParameter      lcdparameter;
    QlI2cParameter      i2cparameter;
    QlSerialLcdParam    lcd_serial_param_ex;
}QlBusParameterUnion;


typedef struct QlBusParameterTag
{
    s16                       busconfigversion;   /*now this version is QL_BUS_VERSION*/
    QlBusParameterUnion       busparameterunion;
}QlBusParameter;


/*i2c QlBusAccess::opcode*/
typedef enum QlBusAccessI2cOpcodeTag
{
    QL_BUSACCESSOPCODE_I2C_NOTHING = 0,
    QL_BUSACCESSOPCODE_I2C_NOTSTOPBIT = 1
}QlBusAccessI2cOpcode;

/*lcd QlBusAccess::opcode*/
typedef enum QlBusAccessLcdOpcodeTag
{
    QL_BUSACCESSOPCODE_LCD_CLEAR_RESET = 0,
    QL_BUSACCESSOPCODE_LCD_SET_RESET,
    QL_BUSACCESSOPCODE_LCD_DATAWRITE,
    QL_BUSACCESSOPCODE_LCD_CMDWRITE
}QlBusAccessLcdOpcode;


typedef struct QlBusAccessTag
{
    u32 Address; /*i2c use, 7bits slave address, lcd not use*/
    u32 opcode; 
}QlBusAccess;


/*****************************************************************
* Function:     Ql_busSubscribe 
* 
* Description:
*               Configure the function of the Bus.
* Parameters:
*               bustype:
*                  [in] Bus type, QL_BUSTYPE_LCD or QL_BUSTYPE_I2C
*
*               busparameter:
*                  [in] A pointer to 'QlBusParameter'
* Return:        
*               If the function suceeds, returns the bus handle
*               <0 indicates failure
*****************************************************************/
QL_BUS_HANDLE Ql_busSubscribe(QlBusType bustype, QlBusParameter* busparameter);


/*****************************************************************
* Function:     Ql_busQuery 
* 
* Description:
*               Queries the configuation of the bus.
* Parameters:
*               bushandle:
*                  [in] Bus handle, previously returned by Ql_busSubscribe
*
*               bustype:
*                  [out] Bus type, QL_BUSTYPE_LCD or QL_BUSTYPE_I2C
*
*               busparameter:
*                  [out] A pointer to 'QlBusParameter'
* Return:        
*               If the function suceeds, returns QL_RET_OK
*               or returns an Error Code
*****************************************************************/
s32 Ql_busQuery(QL_BUS_HANDLE bushandle, QlBusType* bustype, QlBusParameter* busparameter);


/*****************************************************************
* Function:     Ql_busUnSubscribe 
* 
* Description:
*               Cancel the function of the bus.
* Parameters:
*               bushandle:
*                  [in] Bus handle, previously returned by Ql_busSubscribe
* Return:        
*               If the function suceeds, returns QL_RET_OK
*               or returns an Error Code
*****************************************************************/
s32 Ql_busUnSubscribe(QL_BUS_HANDLE bushandle);


/*****************************************************************
* Function:     Ql_busWrite 
* 
* Description:
*               Writes data to the bus.
* Parameters:
*               bushandle:
*                  [in] Bus handle, previously returned by Ql_busSubscribe
*
*               busAccess:
*                  [in] A pointer to 'QlBusAccess'
*
*               data_p:
*                  [in] A pointer to data buffer to write
*
*               datalen:
*                  [in] Length of data to write (number of bytes)
* Return:        
*               If the function suceeds, returns QL_RET_OK
*               or returns an Error Code
*****************************************************************/
s32 Ql_busWrite(QL_BUS_HANDLE bushandle, QlBusAccess* busAccess, u8* data_p, u32 datalen);


/*****************************************************************
* Function:     Ql_busRead 
* 
* Description:
*               Reads data from the bus, which is a I2C bus.
*               This function just works for I2C bus.
* Parameters:
*               bushandle:
*                  [in] Bus handle, previously returned by Ql_busSubscribe
*
*               busAccess:
*                  [in] A pointer to 'QlBusAccess'
*
*               data_p:
*                  [out] A pointer to data buffer
*
*               datalen:
*                  [in] Length of data to read (number of bytes)
* Return:        
*               If the function suceeds, returns QL_RET_OK
*               or returns an Error Code
*****************************************************************/
s32 Ql_busRead(QL_BUS_HANDLE bushandle, QlBusAccess* busAccess, u8* data_p, u32 datalen);

#endif // End-of __QL_BUS_H__

