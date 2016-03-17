#ifdef __EXAMPLE_SPI__
#include "ql_appinit.h"
#include "ql_interface.h"
#include "ql_type.h"
#include "ql_trace.h"
#include "ql_stdlib.h"
#include "ql_timer.h"

/***************************************************************************
* This example chooses PINs: DSP_CLK, DSP_D/C, DSP_DATA, DSP_CS
* to realize SPI interfaces. If customer needs to use other
* PINs, you can tell us, and we will reconfigure these operation interfaces
****************************************************************************/
//GPIO0 CLK (Phy pin: DSP_CLK)
#define  SPI_SCK_GPIO()         (*(volatile unsigned short *)(0x80120150)) &= 0xfffc  //GPIO_ModeSetup(SPI_SCK_PIN,0);        
#define  SPI_SCK_OUT()            (*(volatile unsigned short *)(0x80120000)) |= 0x0001  //GPIO_InitIO(OUTPUT,SPI_SCK_PIN)
#define  SPI_SCK_IN()             (*(volatile unsigned short *)(0x80120000)) &= 0xfffe  //GPIO_InitIO(INPUT,SPI_SCK_PIN)
#define  SPI_SCK_SET()            (*(volatile unsigned short *)(0x801200C0)) |= 0x0001  //GPIO_WriteIO(1,SPI_SCK_PIN)
#define  SPI_SCK_CLR()            (*(volatile unsigned short *)(0x801200C0)) &= 0xfffe  //GPIO_WriteIO(0,SPI_SCK_PIN)

//IN   GPIO2 (Phy pin: DSP_D/C)
/* MISO  */    
#define  SPI_MISO_GPIO()          (*(volatile unsigned short *)(0x80120150)) &= 0xffcf //GPIO_ModeSetup(SPI_MOSI_PIN,0);
#define  SPI_MISO_OUT()           (*(volatile unsigned short *)(0x80120000)) |= 0x0004//GPIO_InitIO(OUTPUT,SPI_MOSI_PIN)
#define  SPI_MISO_IN()            (*(volatile unsigned short *)(0x80120000)) &= 0xfffb//GPIO_InitIO(INPUT,SPI_MOSI_PIN)
#define  SPI_MISO_SET()           (*(volatile unsigned short *)(0x801200C0)) |= 0x0004//GPIO_WriteIO(1,SPI_MOSI_PIN)
#define  SPI_MISO_CLR()           (*(volatile unsigned short *)(0x801200C0)) &= 0xfffb//GPIO_WriteIO(0,SPI_MOSI_PIN)
#define  SPI_MISO_READ()          ((*(volatile unsigned short *)(0x80120100)) >> 2) & 0x0001 // GPIO_ReadIO(SPI_MISO_PIN)    


//OUT   GPIO1 (Phy pin: DSP_DATA)
/* MOSI  */    
#define  SPI_MOSI_GPIO()          (*(volatile unsigned short *)(0x80120150)) &= 0xfff3  //GPIO_ModeSetup(SPI_MISO_PIN,0);   
#define  SPI_MOSI_OUT()           (*(volatile unsigned short *)(0x80120000)) |= 0x0002//GPIO_InitIO(OUTPUT,SPI_MISO_PIN)
#define  SPI_MOSI_IN()            (*(volatile unsigned short *)(0x80120000)) &= 0xfffd//GPIO_InitIO(INPUT,SPI_MISO_PIN)
#define  SPI_MOSI_SET()           (*(volatile unsigned short *)(0x801200C0)) |= 0x0002//GPIO_WriteIO(1,SPI_MISO_PIN)
#define  SPI_MOSI_CLR()           (*(volatile unsigned short *)(0x801200C0)) &= 0xfffd//GPIO_WriteIO(0,SPI_MISO_PIN)


//CS   GPIO13 (Phy pin: DSP_CS)
/* CS  */        
#define  SPI_CS_GPIO()            (*(volatile unsigned short *)(0x80120160)) &= 0xf3ff //GPIO_InitIO(OUTPUT,SPI_CS_PIN)
#define  SPI_CS_OUT()             (*(volatile unsigned short *)(0x80120000)) |= 0x2000//GPIO_InitIO(OUTPUT,SPI_CS_PIN)
#define  SPI_CS_IN()              (*(volatile unsigned short *)(0x80120000)) &= 0xdfff//GPIO_InitIO(INPUT,SPI_CS_PIN)
#define  SPI_CS_SET()             (*(volatile unsigned short *)(0x801200C0)) |= 0x2000//GPIO_WriteIO(1,SPI_CS_PIN)
#define  SPI_CS_CLR()             (*(volatile unsigned short *)(0x801200C0)) &= 0xdfff//GPIO_WriteIO(0,SPI_CS_PIN)


QlEventBuffer g_event; // Keep this variable a global variable due to its big size
char notes[100];
#define SPI_BUF 100
#define OUTPUT_BUF 100

void ql_spi_init(void)
{
    // clock
    SPI_SCK_GPIO();
    SPI_SCK_OUT();
    SPI_SCK_CLR();

    // MISO
    SPI_MISO_GPIO();
    SPI_MISO_IN();
    SPI_MISO_CLR();

    // MOSI
    SPI_MOSI_GPIO();
    SPI_MOSI_OUT();
    SPI_MOSI_CLR();

    // CS
    SPI_CS_GPIO();
    SPI_CS_OUT();
    SPI_CS_SET();
}

u8 WriteByte_SPI(u8 WR_data)
{
    int i;
    u8 tmp=0, RD_data=0;
    
    SPI_CS_CLR();
    SPI_SCK_CLR();
    SPI_MISO_CLR();
    //Ql_Sleep(1);

    for(i=0;i<8;i++)
    {
        SPI_SCK_SET();
        if((WR_data&0x80)==0)
            SPI_MOSI_CLR();
        else
        {    
            SPI_MOSI_SET();
        }
        WR_data<<=1;             
         Ql_Sleep(1);
         SPI_SCK_CLR();
         tmp= SPI_MISO_READ();
         RD_data =(RD_data<<1)|tmp ;                   
         //Ql_Sleep(1);
         
    }
    SPI_CS_SET();
    SPI_SCK_CLR();
    SPI_MOSI_CLR();

    return RD_data;
    //Ql_Sleep(100);
    //Ql_DebugTrace("%x", RD_data);
}

void ql_entry(void)
{
    s32 ret;
    bool keepGoing = TRUE;
    QlTimer tm;
    u32 cnt = 0;
    bool bClk = FALSE;
    u16 sdi;
    u8 sdo;
    u8 sdi_val;
    u16 i = 0;
    u16 j = 0;
    bool flag = FALSE;
    u8 temp1=0, temp2=0, temp3=0;
    u8 rcvCnt = 0;
    u8 sndCnt = 0;
    char chSend[1];
    u8 read_data[SPI_BUF] = {0};
    u8 output_data[SPI_BUF] = {0};
    u8 arrMO[] = //{0,1,0,1,0,0,1};
    //{1,0,1,0,0,0,0,0,1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,1,0,0,1,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    /*A7B4*///{1,0,1,0,0,1,1,1,1};
    /*A7B4*/{1,0,1,0,0,1,1,1,1,0,1,1,0,1,0,0};
 
    Ql_SetDebugMode(ADVANCE_MODE);    /* Only after the device reboots, 
                                                                         *  the set debug-mode takes effect. 
                                                                         */
    Ql_DebugTrace("OpenCPU: Hello SPI !\r\n\r\n");    /* Print out message through DEBUG port */
    Ql_OpenModemPort(ql_md_port1);  /* or ql_md_port2, two virtual modem ports are available.*/

    ql_spi_init();
    Ql_Sleep(10000);
    SPI_CS_CLR();

#if 1    

    while (1)
    {   
        for(i = 0; i < SPI_BUF; i++)
        {
            #if 0
            read_data[i] = WriteByte_SPI(0xA7);
            read_data[++i] = WriteByte_SPI(0xB4);
            #else
            temp1 = WriteByte_SPI(0xA7);
            if(temp2 == 0xA7)
            {
                if(temp1 == 0xB4)
                {
                    i--;
                    flag = TRUE;
                    Ql_Sleep(100);
                }
            }
            temp2 = WriteByte_SPI(0xB4);
            if(!(temp1 == 0xA7 && temp2 == 0xB4))
            {
                if(flag)
                {
                    read_data[i] = temp2;
                    flag = FALSE;
                }
                else
                {
                    read_data[i] = temp1;
                    read_data[++i] = temp2;
                }
            }
            else
            {
                Ql_Sleep(100);
            }
            #endif
        }
        Ql_SendToUart(ql_uart_port1, &read_data, SPI_BUF);
       /* for(i = 0, j = 0; i < SPI_BUF; i++)
        {
            if(!(read_data[i] == 0xA7 && read_data[i + 1] == 0xB4))
            {
                output_data[j] = read_data[i];
                if(j == SPI_BUF -1)
                {
                    Ql_SendToUart(ql_uart_port1, &output_data, SPI_BUF);
                }
                else
                {
                    j++;
                }
            }
        }*/
    }

    
    
 #else   
        // 1. Clock 拉高 & 写
        SPI_SCK_SET();
        for (i = 0; i < sizeof(arrMO); i++)
        {
            if (1 == arrMO[i])
            {
                sdo = SPI_MOSI_SET();
            }
            else
            {
                sdo = SPI_MOSI_CLR();
            }
            //i = (i < (sizeof(arrMO) - 1)) ? (i + 1) : 0;
            SPI_SCK_CLR();
            SPI_SCK_SET();
        }

        // 2. Clock 再拉低 & 读
        SPI_SCK_CLR();
        
        // 读SDI
        sdi = (u8)SPI_MISO_READ();
        //Ql_DebugTrace("%d", sdi);
        sdi_val = (sdi_val << 1) | sdi;
        rcvCnt++;
        if (8 == rcvCnt)
        {
            Ql_DebugTrace("%s", (char*)(&sdi_val));
            rcvCnt = 0;
        }
    }
#endif

#if 0
    while(keepGoing)
    {
        Ql_GetEvent(&g_event);
        switch(g_event.eventType)
        {
            case EVENT_TIMER:
            {
                if (bClk)   // Clock现在是'高'，则拉'低'
                {
                    SPI_SCK_CLR();

                    // 读SDI
                    sdi = (u8)SPI_MISO_READ();
                    Ql_DebugTrace("%d", sdi);
                    sdi_val = (sdi_val << 1) | sdi;
                    rcvCnt++;
                    if (8 == rcvCnt)
                    {
                        Ql_DebugTrace("%s", (char*)(&sdi_val));
                        rcvCnt = 0;
                    }
                    //SPI_MISO_CLR();
                }
                else    // Clock现在是'低'，则拉'高'
                {
                    u8 temp;

                    // 根据输出序列写SDO
                    if (1 == arrMO[i])
                    {
                        sdo = SPI_MOSI_SET();
                    }
                    else
                    {
                        sdo = SPI_MOSI_CLR();
                    }

                    // 数据准备好后再拉clock
                    SPI_SCK_SET();
                    
                    temp = arrMO[i];
                    //Ql_memset(notes, 0x0, sizeof(notes));
                    //Ql_sprintf(notes, "%d ", temp);
                    //notes[2] = 0x0;
                    //Ql_SendToUart_2(ql_uart_port1, (u8*)notes, 2);
                    Ql_sprintf(chSend, "%d", temp);
                    Ql_SendToUart(ql_uart_port1, (u8*)chSend, 1);
                    i = (i < (sizeof(arrMO) - 1)) ? (i + 1) : 0;
                    #if 1
                    sndCnt++;
                    if (8 == sndCnt)
                    {
                        sndCnt = 0;
                        SPI_CS_SET();
                        SPI_SCK_CLR();
                        SPI_MOSI_CLR();
                        Ql_Sleep(10);
                        
                        SPI_CS_CLR();
                        //SPI_SCK_SET();
                        bClk = !bClk;
                    }
                    #endif
                }
                bClk = !bClk;
                
                // Start the timer again
                Ql_StartTimer(&tm);
                break;
            }
           
            case EVENT_UARTDATA:
            {
                //TODO: receive and handle data from UART

                /* The following code segment is written to demonstrate the usage of 'AT+QLOCPU'.
                *   If send 'AT+QLOCPU=0' through serial port tool, and reboot the device, only Core 
                *   System software is booted (the application will not be booted).
                */
                PortData_Event* pDataEvt = (PortData_Event*)&g_event.eventData.uartdata_evt;
                Ql_sprintf(notes, "AT+QLOCPU=0\r\n");
                ret = Ql_strncmp((char*)pDataEvt->data, notes, Ql_strlen(notes));
                if (0 == ret)
                {
                    Ql_SendToModem(ql_md_port1, "AT+QLOCPU=0\n", Ql_strlen(notes) + 1);
                    break;
                }
                break;
            }
           
            default:
                break;
        }
    }
}

#endif
}
#endif // __SPI_TEST__



