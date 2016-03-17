#ifdef __EXAMPLE_GPS_I2C__
#include "ql_appinit.h"
#include "ql_interface.h"
#include "ql_type.h"
#include "ql_trace.h"
#include "ql_stdlib.h"
#include "ql_timer.h"
#include "ql_bus.h"

char notes[100];

void ql_entry(void)
{
    s32 ret; 
    bool keepGoing = TRUE;
    QlBusParameter busparameter;
    QlBusAccess access;
    u16 i = 0;
    u8 data[100] = {0};
    bool flag = FALSE;
    u8 temp1=0;

    QL_BUS_HANDLE busi2c = (QL_BUS_HANDLE) - 1; 
    
    Ql_SetDebugMode(BASIC_MODE);    /* Only after the device reboots, 
                                                                         *  the set debug-mode takes effect. 
                                                                         */
    Ql_DebugTrace("OpenCPU: GPS I2C !\r\n\r\n");    /* Print out message through DEBUG port */
    Ql_OpenModemPort(ql_md_port1);  /* or ql_md_port2, two virtual modem ports are available.*/

    //ql_I2C_init();
    Ql_Sleep(10000);

    busparameter.busconfigversion = QL_BUS_VERSION;
    busparameter.busparameterunion.i2cparameter.pin_i2cdata = QL_PINNAME_M10_DISP_DC;
    busparameter.busparameterunion.i2cparameter.pin_i2cclk = QL_PINNAME_M10_DISP_DATA;

    busi2c= Ql_busSubscribe(QL_BUSTYPE_I2C, &busparameter);
    if(busi2c < 0)                                    
    {
        Ql_sprintf(notes, "\r\nSubscribe I2C Failed=%d\r\n",busi2c);
        Ql_SendToUart(ql_uart_port1,notes,Ql_strlen(notes));  
        return;
    }
 
    while (1)
    { 
        access.Address = 0x60;
        // note , this will not send stopbit
        access.opcode = QL_BUSACCESSOPCODE_I2C_NOTSTOPBIT;
        
        ret = Ql_busRead(busi2c, &access, &temp1, 1);
        if(!ret)  
        {
            if(i < 100)
            {
                data[i] = temp1;
                i++;
            }
            else
            {
                Ql_SendToUart(ql_uart_port1, &data, 100);
                i = 0;
                data[i] = temp1;
                Ql_Sleep(1);            
            }

        } 
    }
}
#endif // __SPI_TEST__



