#ifdef __EXAMPLE_LCM_UC1701__
/***************************************************************************************************
*   Example:
*       
*            LCM_UC1701 Routine
*
*   Description:
*
*           This example gives an example for LCM_UC1701 setting.
*           Through Uart port, input the special command, there will be given the response about LCD operation.
*           
*
*   Usage:
*
*           Precondition:
*
*                   Use "make/make128 LCM_UC1701" to compile, and download bin image to module.
*           
*           Through Uart port:
*
*               If input "InitLcm", that will subscribe bus function and init registers.
*               If input "bias", that will change LCD bias ratio of the voltage.
*               If input "contrast+", that will increase contrast.
*               If input "contrast+", that will decrease contrast.
*               If input "Sleep", that will let the display to sleep.
*               If input "Wakeup", that will Wakeup the sleeping display.
*               If input "ResetLCM", that will reset  LCM.
*               If input "UnInitLCM", that will unsubscribe bus function.
*               If input "Page9X12C23D123", that will display specific location information.
*               If input "SendCmd=", that will send Cmd to display.
*               If input "CmdTest", that will test Cmd is normal or not.
*               If input "DataTest", that will test Data is normal or not.
*           
****************************************************************************************************/  
#include "ql_appinit.h"
#include "ql_interface.h"
#include "ql_type.h"
#include "ql_trace.h"
#include "ql_audio.h"
#include "ql_timer.h"
#include "ql_stdlib.h"
#include "ql_pin.h"
#include "ql_bus.h"
#include "ql_error.h"
#include "ql_fcm.h"

/***************************************************************************/
/*  LCM Module xxxx use Driver IC UC1701 to work, dots is 132*65
/*  
/***************************************************************************/

char buffer[100];

QL_BUS_HANDLE busLcd = (QL_BUS_HANDLE)-1;

#define MAX_X 96
#define MAX_Y 32

//view 12:31
u8 lcm_ram_buffer[MAX_X*MAX_Y/8] = {
//8*96
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF0,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF0,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
//8*96
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0x0F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
//8*96
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0x0F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
//8*96
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0x0F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0x0F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};


#define MAX_PAGE 4

static u8 bias =0;
static u8 contrast =0x1A;
static u8 iconporinter = 0;

void DisplayInit()
{
    QlBusAccess access;
    u8 command;
    s32 iret;
    s32 i;

    //
    //Driver IC register config

    //Software Reset
    command = 0xE2;
    access.opcode = QL_BUSACCESSOPCODE_LCD_CMDWRITE;
    iret = Ql_busWrite(busLcd, &access, &command, 1);
    Ql_sprintf(buffer, "\r\nLcd Write Cmd=%x, ret=%d,busLcd=%x\r\n",command, iret,busLcd);
    Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  

    Ql_Sleep(10); 

    //Set Bias     0xA3 1/7bias,  0xA2 1/9 bias
    command = 0xA2;
    access.opcode = QL_BUSACCESSOPCODE_LCD_CMDWRITE;
    iret = Ql_busWrite(busLcd, &access, &command, 1);
    Ql_sprintf(buffer, "\r\nLcd Write bias=%d, ret=%d\r\n",bias, iret);
    Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  

    //Segment Direction Select       bit0=0,normal ,=1,reverse
    command = 0xA0;
    access.opcode = QL_BUSACCESSOPCODE_LCD_CMDWRITE;
    iret = Ql_busWrite(busLcd, &access, &command, 1);
    Ql_sprintf(buffer, "\r\nLcd Write Cmd=%x, ret=%d\r\n",command, iret);
    Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  

    //Common Direction Select        bit3=0,normal ,=1,revese
    command = 0xC8;
    access.opcode = QL_BUSACCESSOPCODE_LCD_CMDWRITE;
    iret = Ql_busWrite(busLcd, &access, &command, 1);
    Ql_sprintf(buffer, "\r\nLcd Write Cmd=%x, ret=%d,busLcd=%x\r\n",command, iret,busLcd);
    Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  


    //Set Vlcd PC[5;3]
    command = 0x22;
    access.opcode = QL_BUSACCESSOPCODE_LCD_CMDWRITE;
    iret = Ql_busWrite(busLcd, &access, &command, 1);
    Ql_sprintf(buffer, "\r\nLcd Write Cmd=%x, ret=%d,busLcd=%x\r\n",command, iret,busLcd);
    Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  

    //Set Contrast  begin
    command = 0x81;
    access.opcode = QL_BUSACCESSOPCODE_LCD_CMDWRITE;
    iret = Ql_busWrite(busLcd, &access, &command, 1);
    Ql_sprintf(buffer, "\r\nLcd Write Cmd=%x, ret=%d,busLcd=%x\r\n",command, iret,busLcd);
    Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  
    command = 0x1A;
    access.opcode = QL_BUSACCESSOPCODE_LCD_CMDWRITE;
    iret = Ql_busWrite(busLcd, &access, &command, 1);
    Ql_sprintf(buffer, "\r\nLcd Write Cmd=%x, ret=%d,busLcd=%x\r\n",command, iret,busLcd);
    Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  
    //Set Contrast  end
    
    //Set Vlcd PC[2;0]
    //Power Control 
    command = 0x2F;
    access.opcode = QL_BUSACCESSOPCODE_LCD_CMDWRITE;
    iret = Ql_busWrite(busLcd, &access, &command, 1);
    Ql_sprintf(buffer, "\r\nLcd Write Cmd=%x, ret=%d,busLcd=%x\r\n",command, iret,busLcd);
    Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  

    Ql_Sleep(40); 

    //Set Scroll Line
    command = 0x40 |0;
    access.opcode = QL_BUSACCESSOPCODE_LCD_CMDWRITE;
    iret = Ql_busWrite(busLcd, &access, &command, 1);
    Ql_sprintf(buffer, "\r\nLcd Write Cmd=%x, ret=%d,busLcd=%x\r\n",command, iret,busLcd);
    Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  

    //Set Inverse Display
    command = 0xA7;
    access.opcode = QL_BUSACCESSOPCODE_LCD_CMDWRITE;
    iret = Ql_busWrite(busLcd, &access, &command, 1);
    Ql_sprintf(buffer, "\r\nLcd Write Cmd=%x, ret=%d,busLcd=%x\r\n",command, iret,busLcd);
    Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  

#if 0
    //Set All-Pixel-OFF
    command = 0xA4 | 0;
    access.opcode = QL_BUSACCESSOPCODE_LCD_CMDWRITE;
    iret = Ql_busWrite(busLcd, &access, &command, 1);
    Ql_sprintf(buffer, "\r\nLcd Write Cmd=%x, ret=%d,busLcd=%x\r\n",command, iret,busLcd);
    Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  
#endif

    //Display ON
    command = 0xAE | 1;
    access.opcode = QL_BUSACCESSOPCODE_LCD_CMDWRITE;
    iret = Ql_busWrite(busLcd, &access, &command, 1);
    Ql_sprintf(buffer, "\r\nLcd Write Cmd=%x, ret=%d,busLcd=%x\r\n",command, iret,busLcd);
    Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));
}

//page(0-3), xstart(0-95),cols(1-96)
void DisplayPage(u8 page, u8 xstart, u8 cols, u8 datav)
{
    QlBusAccess access;
    u8 command;
    s32 iret;
    s32 i;
    
    //Y page set(0-8)
    command = 0xB0 | (page & 0x0F);
    access.opcode = QL_BUSACCESSOPCODE_LCD_CMDWRITE;
    iret = Ql_busWrite(busLcd, &access, &command, 1);

    //X address set(0-131)
    command = 0x00 | (xstart & 0x0f); // LSB
    access.opcode = QL_BUSACCESSOPCODE_LCD_CMDWRITE;
    iret = Ql_busWrite(busLcd, &access, &command, 1);

    //X address set(0-131)
    command = 0x10 | ((xstart & 0xf0)>>4); //MSB
    access.opcode = QL_BUSACCESSOPCODE_LCD_CMDWRITE;
    iret = Ql_busWrite(busLcd, &access, &command, 1);

    Ql_sprintf(buffer, "\r\nLcd page=%d, xstart=%d,cols=%d,datav=%x\r\n",page, xstart,cols,datav);
    Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  

    //data input
    Ql_memset(lcm_ram_buffer, datav, cols);

    {
        access.opcode = QL_BUSACCESSOPCODE_LCD_DATAWRITE;
        iret = Ql_busWrite(busLcd, &access, lcm_ram_buffer, (u32)cols);
        if(iret != QL_RET_OK)
        {
        }
    }
}


void DisplayFrameBufer(u8 *FrameBuffer)
{
    QlBusAccess access;
    u8 command;
    s32 iret;
    s32 i;
    u8 page;
    u8 xstart;
    u8 cols;
    u8 *ponit;

    cols = MAX_X;
    page = 0;
    xstart = 0;
    ponit = FrameBuffer + page * MAX_X + xstart;

    //Y page set(0-8)
    command = 0xB0 | (page & 0x0F);
    access.opcode = QL_BUSACCESSOPCODE_LCD_CMDWRITE;
    iret = Ql_busWrite(busLcd, &access, &command, 1);

    //X address set(0-131)
    command = 0x00 | (xstart & 0x0f); // LSB
    access.opcode = QL_BUSACCESSOPCODE_LCD_CMDWRITE;
    iret = Ql_busWrite(busLcd, &access, &command, 1);

    //X address set(0-131)
    command = 0x10 | ((xstart & 0xf0)>>4); //MSB
    access.opcode = QL_BUSACCESSOPCODE_LCD_CMDWRITE;
    iret = Ql_busWrite(busLcd, &access, &command, 1);

    {
        access.opcode = QL_BUSACCESSOPCODE_LCD_DATAWRITE;
        iret = Ql_busWrite(busLcd, &access, ponit, (u32)cols);
        if(iret != QL_RET_OK)
        {
        }
    }

    page = 1;
    xstart = 0;
    ponit = FrameBuffer + page * MAX_X + xstart;

    //Y page set(0-8)
    command = 0xB0 | (page & 0x0F);
    access.opcode = QL_BUSACCESSOPCODE_LCD_CMDWRITE;
    iret = Ql_busWrite(busLcd, &access, &command, 1);

    //X address set(0-131)
    command = 0x00 | (xstart & 0x0f); // LSB
    access.opcode = QL_BUSACCESSOPCODE_LCD_CMDWRITE;
    iret = Ql_busWrite(busLcd, &access, &command, 1);

    //X address set(0-131)
    command = 0x10 | ((xstart & 0xf0)>>4); //MSB
    access.opcode = QL_BUSACCESSOPCODE_LCD_CMDWRITE;
    iret = Ql_busWrite(busLcd, &access, &command, 1);

    {
        access.opcode = QL_BUSACCESSOPCODE_LCD_DATAWRITE;
        iret = Ql_busWrite(busLcd, &access, ponit, (u32)cols);
        if(iret != QL_RET_OK)
        {
        }
    }
    
    page = 2;
    xstart = 0;
    ponit = FrameBuffer + page * MAX_X + xstart;


    //Y page set(0-8)
    command = 0xB0 | (page & 0x0F);
    access.opcode = QL_BUSACCESSOPCODE_LCD_CMDWRITE;
    iret = Ql_busWrite(busLcd, &access, &command, 1);

    //X address set(0-131)
    command = 0x00 | (xstart & 0x0f); // LSB
    access.opcode = QL_BUSACCESSOPCODE_LCD_CMDWRITE;
    iret = Ql_busWrite(busLcd, &access, &command, 1);

    //X address set(0-131)
    command = 0x10 | ((xstart & 0xf0)>>4); //MSB
    access.opcode = QL_BUSACCESSOPCODE_LCD_CMDWRITE;
    iret = Ql_busWrite(busLcd, &access, &command, 1);

    {
        access.opcode = QL_BUSACCESSOPCODE_LCD_DATAWRITE;
        iret = Ql_busWrite(busLcd, &access, ponit, (u32)cols);
        if(iret != QL_RET_OK)
        {
        }
    }

    page = 3;
    xstart = 0;
    ponit = FrameBuffer + page * MAX_X + xstart;

    //Y page set(0-8)
    command = 0xB0 | (page & 0x0F);
    access.opcode = QL_BUSACCESSOPCODE_LCD_CMDWRITE;
    iret = Ql_busWrite(busLcd, &access, &command, 1);

    //X address set(0-131)
    command = 0x00 | (xstart & 0x0f); // LSB
    access.opcode = QL_BUSACCESSOPCODE_LCD_CMDWRITE;
    iret = Ql_busWrite(busLcd, &access, &command, 1);

    //X address set(0-131)
    command = 0x10 | ((xstart & 0xf0)>>4); //MSB
    access.opcode = QL_BUSACCESSOPCODE_LCD_CMDWRITE;
    iret = Ql_busWrite(busLcd, &access, &command, 1);

    {
        access.opcode = QL_BUSACCESSOPCODE_LCD_DATAWRITE;
        iret = Ql_busWrite(busLcd, &access, ponit, (u32)cols);
        if(iret != QL_RET_OK)
        {
        }
    }

}

void lcd_init(void)
{
    s32 iret;
    s32 period = 10;
    char *pData, *p;
    QlBusAccess access;
    u8 command;
    u8 data;
    s32 i;
                   
    {
        //
        //config LCD pin
        
        QlBusParameter busparameter;
        busparameter.busconfigversion = QL_BUS_VERSION;
        busparameter.busparameterunion.lcdparameter.brequire_cs = 1;
        busparameter.busparameterunion.lcdparameter.brequire_resetpin = 1;
        /*following selcet 3.25M clock, cs low valid, clock rising edge  sampling  data*/
        busparameter.busparameterunion.lcdparameter.lcd_serial_config = 
            QL_LCD_SERIAL_CONFIG_13MHZ_CLK | QL_LCD_SERIAL_CONFIG_CLOCK_DIVIDE_3
            /*| QL_LCD_SERIAL_CONFIG_CS_POLARITY*/
            | QL_LCD_SERIAL_CONFIG_8_BIT_MODE
            /*| QL_LCD_SERIAL_CONFIG_CLOCK_PHASE */
            /*| QL_LCD_SERIAL_CONFIG_CLOCK_POLARITY*/
            ;

        if(busLcd > 0)                                    
        {
            Ql_sprintf(buffer, "\r\nAlready Subscribe LCD\r\n",iret);
            Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  
        }
        
        busLcd = Ql_busSubscribe(QL_BUSTYPE_LCD, &busparameter);
        if(busLcd < 0)                                    
        {
            Ql_sprintf(buffer, "\r\nSubscribe LCD Failed=%d\r\n",busLcd);
            Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  
        }

        //
        //reset LCD
        
        Ql_sprintf(buffer, "\r\nSubscribe LCD succcess busLcd =%x\r\n", busLcd );
        Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  

        access.opcode = QL_BUSACCESSOPCODE_LCD_SET_RESET;
        iret = Ql_busWrite(busLcd, &access, NULL, 0);
        Ql_sprintf(buffer, "\r\nLcd Set Reset, ret=%d,busLcd=%x\r\n",iret,busLcd);
        Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  

        Ql_Sleep(1000);


        access.opcode = QL_BUSACCESSOPCODE_LCD_CLEAR_RESET;
        iret = Ql_busWrite(busLcd, &access, NULL, 0);
        Ql_sprintf(buffer, "\r\nLcd Clean Reset, ret=%d,busLcd=%x\r\n",iret,busLcd);
        Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  

        Ql_Sleep(1000); //

        access.opcode = QL_BUSACCESSOPCODE_LCD_SET_RESET;
        iret = Ql_busWrite(busLcd, &access, NULL, 0);
        Ql_sprintf(buffer, "\r\nLcd Set Reset, ret=%d,busLcd=%x\r\n",iret,busLcd);
        Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  

        Ql_Sleep(1000);
        
        DisplayInit();

        DisplayFrameBufer(lcm_ram_buffer);
    }
}


void ql_entry()
{
    bool           keepGoing = TRUE;
    QlEventBuffer  flSignalBuffer;
    s32 period = 10;
    char *pData, *p;
    QlBusAccess access;
    u8 command;
    u8 data;
    s32 i;
    s32 iret;

    Ql_DebugTrace("lcm_uc1701: ql_entry\r\n");
    
    lcd_init();
   
    while(keepGoing)
    {    
        Ql_GetEvent(&flSignalBuffer);
        switch(flSignalBuffer.eventType)
        {
            case EVENT_UARTDATA:
            {
                if (flSignalBuffer.eventData.uartdata_evt.len>0)
                {
                    s32 pin;
                    s32 mod = 0;
                    s32 pinpullenable = 0;
                    s32 pindirection = 0;
                    s32 pinlevel = 0;

                    pData = (char*)flSignalBuffer.eventData.uartdata_evt.data;
                    p = Ql_strstr(pData,"InitLcm");
                    if (p)
                    {
                        lcd_init();

                        //clear all
                        //DisplayPage(0, 0, 96, 0xFF);
                        //DisplayPage(1, 0, 96, 0xFF);
                        //DisplayPage(2, 0, 96, 0xFF);
                        //DisplayPage(3, 0, 96, 0xFF);

                        //draw block
                        //DisplayPage(0, 0, 20, 0x0);
                        //DisplayPage(1, 20, 20, 0x0);
                        //DisplayPage(2, 40, 20, 0x0);
                        //DisplayPage(3, 60, 20, 0x0);
                        break;
                    }
                    
                    p = Ql_strstr(pData,"bias");
                    if (p)
                    {
                        //bias
                        command = 0xA2 | (bias &0x01);
                        access.opcode = QL_BUSACCESSOPCODE_LCD_CMDWRITE;
                        iret = Ql_busWrite(busLcd, &access, &command, 1);

                        Ql_sprintf(buffer, "\r\nLcd Write bias=%d, ret=%d\r\n",bias, iret);
                        Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  

                        if(bias == 0)
                        bias = 1;
                        else
                        bias = 0;

                        break;
                    }  
                    
                    p = Ql_strstr(pData,"contrast+");
                    if (p)
                    {
                        contrast++;
                        if(contrast > 0x3F)
                            contrast = 0x3F;

                        //Set Contrast  begin
                        command = 0x81;
                        access.opcode = QL_BUSACCESSOPCODE_LCD_CMDWRITE;
                        iret = Ql_busWrite(busLcd, &access, &command, 1);
                        Ql_sprintf(buffer, "\r\nLcd Write Cmd=%x, ret=%d,busLcd=%x\r\n",command, iret,busLcd);
                        Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  
                        command = contrast & 0x3F;
                        access.opcode = QL_BUSACCESSOPCODE_LCD_CMDWRITE;
                        iret = Ql_busWrite(busLcd, &access, &command, 1);
                        Ql_sprintf(buffer, "\r\nLcd Write Cmd=%x, ret=%d,busLcd=%x\r\n",command, iret,busLcd);
                        Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  
                        //Set Contrast  end
                        
                        break;
                    } 
                    
                    p = Ql_strstr(pData,"contrast-");
                    if (p)
                    {
                        if(contrast <= 1)
                            contrast = 0;
                        else
                            contrast--;

                        //Set Contrast  begin
                        command = 0x81;
                        access.opcode = QL_BUSACCESSOPCODE_LCD_CMDWRITE;
                        iret = Ql_busWrite(busLcd, &access, &command, 1);
                        Ql_sprintf(buffer, "\r\nLcd Write Cmd=%x, ret=%d,busLcd=%x\r\n",command, iret,busLcd);
                        Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  
                        command = contrast & 0x3F;
                        access.opcode = QL_BUSACCESSOPCODE_LCD_CMDWRITE;
                        iret = Ql_busWrite(busLcd, &access, &command, 1);
                        Ql_sprintf(buffer, "\r\nLcd Write Cmd=%x, ret=%d,busLcd=%x\r\n",command, iret,busLcd);
                        Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  
                        //Set Contrast  end

                        break;
                    }
                    
                    p = Ql_strstr(pData,"Sleep");
                    if (p)
                    {
                        //Display OFF
                        command = 0xAE | 0;
                        access.opcode = QL_BUSACCESSOPCODE_LCD_CMDWRITE;
                        iret = Ql_busWrite(busLcd, &access, &command, 1);
                        Ql_sprintf(buffer, "\r\nLcd Write Cmd=%x, ret=%d,busLcd=%x\r\n",command, iret,busLcd);
                        Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  

#if 1
                        //Set All-Pixel-ON
                        command = 0xA4 | 1;
                        access.opcode = QL_BUSACCESSOPCODE_LCD_CMDWRITE;
                        iret = Ql_busWrite(busLcd, &access, &command, 1);
                        Ql_sprintf(buffer, "\r\nLcd Write Cmd=%x, ret=%d,busLcd=%x\r\n",command, iret,busLcd);
                        Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  
#endif                                
                        break;
                    }  
                    
                    p = Ql_strstr(pData,"Wakeup");
                    if (p)
                    {

#if 1
                        //Set All-Pixel-OFF
                        command = 0xA4 | 0;
                        access.opcode = QL_BUSACCESSOPCODE_LCD_CMDWRITE;
                        iret = Ql_busWrite(busLcd, &access, &command, 1);
                        Ql_sprintf(buffer, "\r\nLcd Write Cmd=%x, ret=%d,busLcd=%x\r\n",command, iret,busLcd);
                        Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  
#endif       
                        //Display ON
                        command = 0xAE | 1;
                        access.opcode = QL_BUSACCESSOPCODE_LCD_CMDWRITE;
                        iret = Ql_busWrite(busLcd, &access, &command, 1);
                        Ql_sprintf(buffer, "\r\nLcd Write Cmd=%x, ret=%d,busLcd=%x\r\n",command, iret,busLcd);
                        Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  
                        break;
                    }  
                    
                    p = Ql_strstr(pData,"ResetLCM");
                    if (p)
                    {
                        access.opcode = QL_BUSACCESSOPCODE_LCD_CLEAR_RESET;
                        iret = Ql_busWrite(busLcd, &access, NULL, 0);
                        Ql_sprintf(buffer, "\r\nLcd Clean Reset, ret=%d,busLcd=%x\r\n",iret,busLcd);
                        Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  
                        Ql_Sleep(1000);
                        access.opcode = QL_BUSACCESSOPCODE_LCD_SET_RESET;
                        iret = Ql_busWrite(busLcd, &access, NULL, 0);
                        Ql_sprintf(buffer, "\r\nLcd Set Reset, ret=%d,busLcd=%x\r\n",iret,busLcd);
                        Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  
                        break;
                    }  
                    
                    p = Ql_strstr(pData,"UnInitLCM");
                    if (p)
                    {
                        //
                        //please send command to LCM, power dowm LCM
                        //
                        //unsubscribe
                        iret = Ql_busUnSubscribe(busLcd);
                        busLcd = (QL_BUS_HANDLE)-1;
                        Ql_sprintf(buffer, "\r\nUnSubscribe(%d)\r\n",iret);
                        Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  
                        break;
                    }
                    
                    //Page9X12C23D123
                    p = Ql_strstr(pData,"Page");
                    if (p)
                    {
                        u8 page=0;
                        u8 xstart=0;
                        u8 cols=0;
                        u8 datav=0x00;

                        p += 4;
                        page = Ql_atoi((char*)p);
                        p = Ql_strstr(pData,"X");
                        p += 1;
                        if (p)
                        {
                            xstart = Ql_atoi((char*)p);
                        }
                        p = Ql_strstr(pData,"C");
                        p += 1;
                        if (p)
                        {
                            cols = Ql_atoi((char*)p);
                        }
                        p = Ql_strstr(pData,"D");
                        p += 1;
                        if (p)
                        {
                            datav = Ql_atoi((char*)p);
                        }                                    
                        DisplayPage(page, xstart, cols, datav);
                        break;
                    } 
                    
                    p = Ql_strstr(pData,"SendCmd=");
                    if (p)
                    {
                        u8 sendcmd;
                        p += 8;
                        sendcmd = Ql_atoi((char*)p);
                        access.opcode = QL_BUSACCESSOPCODE_LCD_CMDWRITE;
                        iret = Ql_busWrite(busLcd, &access, &sendcmd, 1);
                        Ql_sprintf(buffer, "\r\nLcd Write Cmd, ret=%d,data=%x\r\n",iret,sendcmd);
                        Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  
                        break;
                     } 
                    
                    p = Ql_strstr(pData,"CmdTest");
                    if (p)
                    {
                        QlBusAccess access;
                        u8 data = 0x33;
                        access.opcode = QL_BUSACCESSOPCODE_LCD_CMDWRITE;
                        iret = Ql_busWrite(busLcd, &access, &data, 1);
                        Ql_sprintf(buffer, "\r\nLcd Write Cmd, ret=%d,data=%x\r\n",iret,data);
                        Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  
                        break;
                    }
                    
                    p = Ql_strstr(pData,"DataTest");
                    if (p)
                    {
                        QlBusAccess access;
                        u8 data = 0xcc;
                        access.opcode = QL_BUSACCESSOPCODE_LCD_DATAWRITE;
                        iret = Ql_busWrite(busLcd, &access, &data, 1);
                        Ql_sprintf(buffer, "\r\nLcd Write Data, ret=%d,data=%x\r\n",iret,data);
                        Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  
                        break;
                    }                              
              }
                break;            
            }
            
            default:
                break;
        }
    }
}

#endif // __EXAMPLE_LCM_UC1701__

