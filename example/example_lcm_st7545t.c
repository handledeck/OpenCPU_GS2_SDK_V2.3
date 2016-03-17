#ifdef __EXAMPLE_LCM_ST7545T__
/***************************************************************************************************
*   Example:
*       
*            LCM_ST7545T Routine
*
*   Description:
*
*           This example gives an example for LCM_ST7545T setting.
*           Through Uart port, input the special command, there will be given the response about LCD operation.
*           
*
*   Usage:
*
*           Precondition:
*
*                   Use "make/make128 LCM_ST7545T" to compile, and download bin image to module.
*           
*           Through Uart port:
*
*               If input "InitLcm", that will subscribe bus function and init registers.
*               If input "bias+", that will increase LCD bias ratio of the voltage required for driving the LCD.
*               If input "bias-", that will decrease LCD bias ratio of the voltage required for driving the LCD.
*               If input "vop+", that will increase output voltage.
*               If input "vop-", that will decrease output voltage.
*               If input "Sleep", that will let the display to sleep.
*               If input "Wakeup", that will Wakeup the sleeping display.
*               If input "ResetLCM", that will reset  LCM.
*               If input "UnInitLCM", that will unsubscribe bus function.
*               If input "icon+", that will move forward icon.
*               If input "icon-", that will move back icon.
*               If input "DisplaycontrolX", that will set the control Mode of display.
*               If input "Page9X12C23D123", that will display specific location information.
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
/*  LCM Module DXDSJ9664-01 use Driver IC ST7545T to work, dots is 96*64
/*  
/***************************************************************************/


char buffer[100];

QL_BUS_HANDLE busLcd = (QL_BUS_HANDLE)-1;

#define MAX_X 96
#define MAX_Y 64
u8 lcm_ram_buffer[MAX_X*MAX_Y/8] = {0};

#define MAX_PAGE 8

static u8 bias =0;
static u8 vop =63;
static u8 iconporinter = 0;

void DisplayInit()
{
    QlBusAccess access;
    u8 command;
    s32 iret;
    s32 i;

    //
    //Driver IC register config
    //PD=0, V=0, H=1
    command = 0x21;
    access.opcode = QL_BUSACCESSOPCODE_LCD_CMDWRITE;
    iret = Ql_busWrite(busLcd, &access, &command, 1);
    Ql_sprintf(buffer, "\r\nLcd Write Cmd=%x, ret=%d,busLcd=%x\r\n",command, iret,busLcd);
    Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  
    
    //bias=4;
    command = 0x10 | bias;
    access.opcode = QL_BUSACCESSOPCODE_LCD_CMDWRITE;
    iret = Ql_busWrite(busLcd, &access, &command, 1);
    Ql_sprintf(buffer, "\r\nLcd Write bias=%d, ret=%d\r\n",bias, iret);
    Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  

    //Vop = BF;
    command = 0x80 | vop;
    access.opcode = QL_BUSACCESSOPCODE_LCD_CMDWRITE;
    iret = Ql_busWrite(busLcd, &access, &command, 1);
    Ql_sprintf(buffer, "\r\nLcd Write vop=%x, ret=%d\r\n",vop, iret);
    Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  


    //PD=0, V=0, H=0
    command = 0x20;
    access.opcode = QL_BUSACCESSOPCODE_LCD_CMDWRITE;
    iret = Ql_busWrite(busLcd, &access, &command, 1);
    Ql_sprintf(buffer, "\r\nLcd Write Cmd=%x, ret=%d,busLcd=%x\r\n",command, iret,busLcd);
    Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  

    //D=1, E=0
    command = 0x0C;
    access.opcode = QL_BUSACCESSOPCODE_LCD_CMDWRITE;
    iret = Ql_busWrite(busLcd, &access, &command, 1);
    Ql_sprintf(buffer, "\r\nLcd Write Cmd=%x, ret=%d,busLcd=%x\r\n",command, iret,busLcd);
    Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));
}


#if 0
u8 bitcolo[] = 
{
    0x1,
    0x3,
    0x7,
    0xF,
    0x1F,
    0x3F,
    0x7F,
    0xFF
};

//x(0-95), y(0-63)
void DisplayArea(u8 xstart, u8 ystart, u8 xend, u8 yend, bool black)
{

    QlBusAccess access;
    u8 command;
    s32 iret;
    s32 i;
    u8 *pdraw;
    u32 bytesinioneling;
    u32 poniter;
    u8 boundaryvalue;
        
    u8 pagestart,pageend;
    u8 xphystart, xphyend;

    u8 yline;
    u8 xcols;

    if(xstart>=MAX_X)
        return;
    if(xend>=MAX_X)
        xend = (MAX_X-1);

    if(ystart>=MAX_Y)
        return;
    if(yend>=MAX_Y)
        yend = (MAX_Y-1);

    pagestart = ystart / 8; // page align
    pageend = yend / 8;
    xphystart = xstart;
    xphyend = xend;

    bytesinioneling = (MAX_X/8);

    //fill area data
    for(yline=ystart; yline<=yend;yline++)
    {
        //fill head data in line, align 8 bit
        xcols = xstart;
        poniter = yline * bytesinioneling + (xcols/8);
        if(xcols%8)
        {
            u8 temp=0;
            boundaryvalue = lcm_ram_buffer[poniter];
            boundaryvalue &= bitcolo[(xcols%8)];
            if(black)
                boundaryvalue |= ~(bitcolo[(xcols%8));
            lcm_ram_buffer[poniter] = boundaryvalue;
            poniter++;
            xcols = (((xcols+7)/8)*8);
        }

        //file data
        for(i=(xcols/8);i<=(xend/8);i++)
        {
            
            if(black)
                lcm_ram_buffer[poniter] = 0xFF;
            else 
                lcm_ram_buffer[poniter] = 0x00;
            poniter++;
        }

        //fill tail data in line, align 8 bit
        xcols = xend;
        if(xcols%8)
        {
            u8 temp=0;
            boundaryvalue = lcm_ram_buffer[poniter];
            boundaryvalue &= bitcolo[(xcols%8)];
            if(black)
                boundaryvalue |= ~(bitcolo[(xcols%8)]);
            lcm_ram_buffer[poniter] = boundaryvalue;
            poniter++;
        }

        
    }
    
    
    //PD=0, V=0, H=0
    command = 0x20;
    access.opcode = QL_BUSACCESSOPCODE_LCD_CMDWRITE;
    iret = Ql_busWrite(busLcd, &access, &command, 1);
    Ql_sprintf(buffer, "\r\nLcd Write Cmd=%x, ret=%d,busLcd=%x\r\n",command, iret,busLcd);
    Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  


    //Y page set(0-9)
    command = 0x40 | (page & 0x0F);
    access.opcode = QL_BUSACCESSOPCODE_LCD_CMDWRITE;
    iret = Ql_busWrite(busLcd, &access, &command, 1);
    Ql_sprintf(buffer, "\r\nLcd Write Page=%x, ret=%d,busLcd=%x\r\n",command&0x0F, iret,busLcd);
    Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  

    //X address set(0-101)
    command = 0x80 | (xaddress & 0x7f);
    access.opcode = QL_BUSACCESSOPCODE_LCD_CMDWRITE;
    iret = Ql_busWrite(busLcd, &access, &command, 1);
    Ql_sprintf(buffer, "\r\nLcd Write X address=%x, ret=%d,busLcd=%x\r\n",command&0x7F, iret,busLcd);
    Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  

    //data input
    Ql_memset(lcm_ram_buffer, datav, cols);
    Ql_sprintf(buffer, "\r\nLcd Write data=%x, cols=%d\r\n",datav, cols);
    Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  
    for(i=pagestart; i<=pageend;i++)
    {
        access.opcode = QL_BUSACCESSOPCODE_LCD_DATAWRITE;
        iret = Ql_busWrite(busLcd, &access, lcm_ram_buffer, cols);
        if(iret != QL_RET_OK)
        {
            Ql_sprintf(buffer, "\r\nLcd Write data error ret=%d\r\n",iret);
            Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  
        }
    }

}

#endif

//page(0-9), xstart(0-101),cols(0-120)
void DisplayPage(u8 page, u8 xstart, u8 cols, u8 datav)
{
    QlBusAccess access;
    u8 command;
    s32 iret;
    s32 i;
    
    //PD=0, V=0, H=0
    command = 0x20;
    access.opcode = QL_BUSACCESSOPCODE_LCD_CMDWRITE;
    iret = Ql_busWrite(busLcd, &access, &command, 1);


    //Y page set(0-9)
    command = 0x40 | (page & 0x0F);
    access.opcode = QL_BUSACCESSOPCODE_LCD_CMDWRITE;
    iret = Ql_busWrite(busLcd, &access, &command, 1);

    //X address set(0-101)
    command = 0x80 | (xstart & 0x7f);
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

    Ql_SetDebugMode(BASIC_MODE);   
    Ql_DebugTrace("lcm_st7545t: ql_entry\r\n");
    Ql_OpenModemPort(ql_md_port1);       

    Ql_memset(lcm_ram_buffer, 0xFF, sizeof(lcm_ram_buffer));
 
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
                    s32 iret;

                    pData = (char*)flSignalBuffer.eventData.uartdata_evt.data;
                    p = Ql_strstr(pData,"InitLcm");
                    if (p)
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
                            break;
                        }
                        
                        busLcd = Ql_busSubscribe(QL_BUSTYPE_LCD, &busparameter);
                        if(busLcd < 0)                                    
                        {
                            Ql_sprintf(buffer, "\r\nSubscribe LCD Failed=%d\r\n",busLcd);
                            Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  
                            break;
                        }

                        //
                        //reset LCD
                        Ql_sprintf(buffer, "\r\nSubscribe LCD succcess busLcd =%x\r\n", busLcd );
                        Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  

                        access.opcode = QL_BUSACCESSOPCODE_LCD_CLEAR_RESET;
                        iret = Ql_busWrite(busLcd, &access, NULL, 0);
                        Ql_sprintf(buffer, "\r\nLcd Clean Reset, ret=%d,busLcd=%x\r\n",iret,busLcd);
                        Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  

                        Ql_Sleep(2); //

                        access.opcode = QL_BUSACCESSOPCODE_LCD_SET_RESET;
                        iret = Ql_busWrite(busLcd, &access, NULL, 0);
                        Ql_sprintf(buffer, "\r\nLcd Set Reset, ret=%d,busLcd=%x\r\n",iret,busLcd);
                        Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  

                         DisplayInit();

                         DisplayPage(0, 0, 102, 0x00);
                         DisplayPage(1, 0, 102, 0x00);
                         DisplayPage(2, 0, 102, 0x00);
                         DisplayPage(3, 0, 102, 0x00);
                         DisplayPage(4, 0, 102, 0x00);
                         DisplayPage(5, 0, 102, 0x00);
                         DisplayPage(6, 0, 102, 0x00);
                         DisplayPage(7, 0, 102, 0x00);
                         DisplayPage(8, 0, 102, 0x00);
                         DisplayPage(9, 0, 102, 0x00);
                        
                        break;
                    }
                    
                    p = Ql_strstr(pData,"bias+");
                    if (p)
                    {
                        //PD=0, V=0, H=1
                        command = 0x21;
                        access.opcode = QL_BUSACCESSOPCODE_LCD_CMDWRITE;
                        iret = Ql_busWrite(busLcd, &access, &command, 1);

                        bias++;
                        bias &= 0x07;
                        command = 0x10 | bias;
                        access.opcode = QL_BUSACCESSOPCODE_LCD_CMDWRITE;
                        iret = Ql_busWrite(busLcd, &access, &command, 1);
                        Ql_sprintf(buffer, "\r\nLcd Write bias=%d, ret=%d\r\n",bias, iret);
                        Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  
                        
                        //PD=0, V=0, H=0
                        command = 0x20;
                        access.opcode = QL_BUSACCESSOPCODE_LCD_CMDWRITE;
                        iret = Ql_busWrite(busLcd, &access, &command, 1);

                        break;
                    }  
                    
                    p = Ql_strstr(pData,"bias-");
                    if (p)
                    {
                        //PD=0, V=0, H=1
                        command = 0x21;
                        access.opcode = QL_BUSACCESSOPCODE_LCD_CMDWRITE;
                        iret = Ql_busWrite(busLcd, &access, &command, 1);

                        bias--;
                        bias &= 0x07;
                        command = 0x10 | bias;
                        access.opcode = QL_BUSACCESSOPCODE_LCD_CMDWRITE;
                        iret = Ql_busWrite(busLcd, &access, &command, 1);
                        Ql_sprintf(buffer, "\r\nLcd Write bias=%d, ret=%d\r\n",bias, iret);
                        Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  
                        
                        //PD=0, V=0, H=0
                        command = 0x20;
                        access.opcode = QL_BUSACCESSOPCODE_LCD_CMDWRITE;
                        iret = Ql_busWrite(busLcd, &access, &command, 1);

                        break;
                    } 
                    
                    p = Ql_strstr(pData,"vop+");
                    if (p)
                    {
                        //PD=0, V=0, H=1
                        command = 0x21;
                        access.opcode = QL_BUSACCESSOPCODE_LCD_CMDWRITE;
                        iret = Ql_busWrite(busLcd, &access, &command, 1);

                        vop++;
                        vop &= 0x7F;
                        command = 0x80 | vop;
                        access.opcode = QL_BUSACCESSOPCODE_LCD_CMDWRITE;
                        iret = Ql_busWrite(busLcd, &access, &command, 1);
                        Ql_sprintf(buffer, "\r\nLcd Write vop=%d, ret=%d\r\n",vop, iret);
                        Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  
                        
                        //PD=0, V=0, H=0
                        command = 0x20;
                        access.opcode = QL_BUSACCESSOPCODE_LCD_CMDWRITE;
                        iret = Ql_busWrite(busLcd, &access, &command, 1);

                        break;
                    }  
                    
                    p = Ql_strstr(pData,"vop-");
                    if (p)
                    {
                        //PD=0, V=0, H=1
                        command = 0x21;
                        access.opcode = QL_BUSACCESSOPCODE_LCD_CMDWRITE;
                        iret = Ql_busWrite(busLcd, &access, &command, 1);

                        vop--;
                        vop &= 0x7F;
                        command = 0x80 | vop;
                        access.opcode = QL_BUSACCESSOPCODE_LCD_CMDWRITE;
                        iret = Ql_busWrite(busLcd, &access, &command, 1);
                        Ql_sprintf(buffer, "\r\nLcd Write vop=%d, ret=%d\r\n",vop, iret);
                        Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  
                        
                        //PD=0, V=0, H=0
                        command = 0x20;
                        access.opcode = QL_BUSACCESSOPCODE_LCD_CMDWRITE;
                        iret = Ql_busWrite(busLcd, &access, &command, 1);

                        break;
                    }                             
                    p = Ql_strstr(pData,"Sleep");
                    if (p)
                    {
                        //PD=1, V=0, H=0
                        command = 0x24;
                        access.opcode = QL_BUSACCESSOPCODE_LCD_CMDWRITE;
                        iret = Ql_busWrite(busLcd, &access, &command, 1);
                        Ql_sprintf(buffer, "\r\nLcd Write Cmd=%x, ret=%d,busLcd=%x\r\n",command, iret,busLcd);
                        Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  

                        break;
                    }  
                    
                    p = Ql_strstr(pData,"Wakeup");
                    if (p)
                    {
                        //PD=0, V=0, H=0
                        command = 0x20;
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
                        Ql_Sleep(2);
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

                    p = Ql_strstr(pData,"icon+");
                    if (p)
                    {
                        DisplayPage(8, iconporinter, 1, 0xFF);
                        iconporinter++;
                        if(iconporinter > 101)
                            iconporinter = 101;
                        break;
                    }
                    
                    p = Ql_strstr(pData,"icon-");
                    if (p)
                    {
                        DisplayPage(8, iconporinter, 1, 0x00);
                        iconporinter--;
                        if(iconporinter > 101)
                            iconporinter = 0;
                        break;
                    }
                    
                    p = Ql_strstr(pData,"displaycontrol");
                    if (p)
                    {
                        u8 de=0x08;
                        if(Ql_strstr(pData,"0"))
                        {
                            de |= 0;
                        }
                        else if(Ql_strstr(pData,"2"))
                        {
                            de |= 4;
                        }
                        else if (Ql_strstr(pData,"1"))
                        {
                            de |= 1;
                        }
                        else if (Ql_strstr(pData,"3"))
                        {
                            de |= 5;
                        }
                        else
                        {
                            de |= 4;
                        }

                        //PD=0, V=0, H=0
                        command = 0x20;
                        access.opcode = QL_BUSACCESSOPCODE_LCD_CMDWRITE;
                        iret = Ql_busWrite(busLcd, &access, &command, 1);
                        Ql_sprintf(buffer, "\r\nLcd Write Cmd=%x, ret=%d,busLcd=%x\r\n",command, iret,busLcd);
                        Ql_SendToUart(ql_uart_port1,(u8*)buffer,Ql_strlen(buffer));  


                        command = de;
                        access.opcode = QL_BUSACCESSOPCODE_LCD_CMDWRITE;
                        iret = Ql_busWrite(busLcd, &access, &command, 1);
                        Ql_sprintf(buffer, "\r\nLcd Write Cmd=%x, ret=%d,busLcd=%x\r\n",command, iret,busLcd);
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

#endif // __EXAMPLE_LCM_ST7545T__

