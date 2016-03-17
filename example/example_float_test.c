#ifdef __EXAMPLE_FLOAT_TEST__
 
#include <math.h>
#include "ql_appinit.h"
#include "ql_interface.h"
#include "ql_type.h"
#include "ql_trace.h"
#include "ql_timer.h"
#include "ql_stdlib.h"
#include "ql_fcm.h"
#include "ql_memory.h"

QlEventBuffer g_event; // Keep this variable a global variable due to its big size
char notes[100];

#define PI                      3.1415926  
#define EARTH_RADIUS            6378.137        //地球近似半径  

float radian(float d);  
float get_distance(float lat1, float lng1, float lat2, float lng2);  


// 求弧度  
float radian(float d)  
{  
     return d * PI / 180.0;   //角度1? = π / 180  
}  

float get_distance(float lat1, float lng1, float lat2, float lng2)  

{  
     float radLat1 = radian(lat1);  
     float radLat2 = radian(lat2);  
     float a = radLat1 - radLat2;  
     float b = radian(lng1) - radian(lng2);  
	 
	 //Ql_FeedWatchdog();
     float dst = 2 * asin((sqrt(pow(sin(a / 2), 2) + cos(radLat1) * cos(radLat2) * pow(sin(b / 2), 2) )));  
     //Ql_FeedWatchdog();
     dst = dst * EARTH_RADIUS;  
     dst= round(dst * 10000) / 10000;  
     return dst;  
} 

void test_dis(void)
{  
     float lat1 = 39.90744;  
     float lng1 = 116.41615;//经度,纬度1  
     float lat2 = 39.90744;  
     float lng2 = 116.30746;//经度,纬度2  
     // insert code here...  
     float dst ;
     
     dst = get_distance(lat1, lng1, lat2, lng2);  
     Ql_DebugTrace("dst = %0.3fkm\n", dst);  //dst = 9.281km  
     //return 0;  
} 

void test_float(void);

void ql_entry(void)
{
    s32 ret;
    bool keepGoing = TRUE;
    //QlTimer tm;
    u32 cnt = 0; 
    double a,b,s,pos;
    double radLat1 = 31.11;
    double radLat2 = 121.29;

    double Value;
    char Buff[10]="0.345";
    char* pStr;

    Ql_SetDebugMode(BASIC_MODE);    /* Only after the device reboots, 
                                     *  the set debug-mode takes effect. 
                                     */
    Ql_DebugTrace("OpenCPU: float test !\r\n\r\n");    /* Print out message through DEBUG port */
    Ql_OpenModemPort(ql_md_port1);  /* or ql_md_port2, two virtual modem ports are available.*/

    test_dis();
    return;
    ///////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////
    
    pStr = Ql_GetMemory(20);
    if ( pStr != NULL)
    {
        //Ql_sprintf(notes, "&pStr = %x\r\n", &pStr);
        //Ql_DebugTrace(notes);
        Ql_DebugTrace("&pStr = %x\r\n", &pStr);
        Ql_memcpy(pStr, Buff, sizeof(Buff));
    }
    else
    {
        Ql_DebugTrace("Fail to get memory for 'pStr'\r\n");
    }
    
    // Start a timer
    //tm.timeoutPeriod = Ql_SecondToTicks(2); /* Set the interval of timer */
    //Ql_StartTimer(&tm);
    //Ql_DebugTrace("The timer starts.\r\n\r\n");
/*
    ret = atoi("9");
    Ql_sprintf(notes, "atoi(\"9\")=%d\r\n", ret);
    Ql_DebugTrace(notes);

    ret = Ql_atoi_Ex("19");
    Ql_sprintf(notes, "atoi(\"19\")=%d\r\n", ret);
    Ql_DebugTrace(notes);

    ret = Ql_atoi_Ex("999");
    Ql_sprintf(notes, "atoi(\"999\")=%d\r\n", ret);
    Ql_DebugTrace(notes);
*/    
    a = sin(45.0);
    b = cos(30.0);
    s = sqrt(81);
    pos = 2 * asin(sqrt(pow(sin(a/2),2) + cos(radLat1)*cos(radLat2)*pow(sin(b/2),2)));
    Ql_DebugTrace("Float test, a=%.2f,b=%.2f,radLat1=%.3f,radLat2=%.3f, s=%.5f\r\n", a,b,radLat1,radLat2,s);
    Ql_DebugTrace("Float test, pos=%g\r\n", pos);
    Ql_sprintf(notes, "Float test, a=%.2f,b=%.2f,radLat1=%.3f,radLat2=%.3f, s=%.5f\r\n", a,b,radLat1,radLat2,s);
    Ql_DebugTrace(notes);
    
    while(keepGoing)
    {
        Ql_GetEvent(&g_event);
        switch(g_event.eventType)
        {
            case EVENT_UARTDATA:
            {
                //TODO: receive and handle data from UART
                char * pos1 = NULL;
                char * pos2 = NULL;
                PortData_Event* pDataEvt = (PortData_Event*)&g_event.eventData.uartdata_evt;
                if (Ql_strstr((char *)pDataEvt->data, "atof(") != NULL)
                {
                    pos1 = Ql_strstr((char *)pDataEvt->data, "(");
                    pos2 = Ql_strstr((char *)pDataEvt->data, ")");
                    if ( pos1 != NULL && pos2 != NULL)
                    {
                        double fTemp = 0.0;
                        char a[20] = { 0x0 };
                        Ql_memcpy(a, pos1 + 1, pos2 - pos1 - 1);
                        fTemp = Ql_atof(a);
                        Ql_sprintf(notes, "atof(%s) = %f\r\n", a, fTemp);
                        Ql_DebugTrace(notes);
                    }
                    else
                    {
                        char chTest[20] = { 0x0 };
                        Ql_memcpy(chTest, "0.12345", 7);
                        Value=Ql_atof(chTest);
                        Ql_DebugTrace("\r\nBuffer is %s.\r\n", chTest);
                        Ql_sprintf(notes, "\r\nFloat value is %f.\r\n", Value);
                        Ql_DebugTrace(notes);
                    }
                }
                else if (Ql_strstr((char *)pDataEvt->data, "isdigit(") != NULL)
                {
                    pos1 = Ql_strstr((char *)pDataEvt->data, "(");
                    pos2 = Ql_strstr((char *)pDataEvt->data, ")");
                    if ( pos1 != NULL && pos2 != NULL)
                    {
                        char ch = *(pos1 + 1);
                        bool bRet = isdigit(ch);
                        Ql_DebugTrace("isdigit(%c) = %d\r\n", ch, bRet);
                    }
                }

                break;
            }
            case EVENT_MODEMDATA:
            {
                //TODO: receive and hanle data from CORE through virtual modem port
                break;
            }
            case EVENT_TIMER:
            {
                //TODO: specify what you want to happen when the interval for timer elapes

                //Ql_DebugTrace("The timer raises for %d time(s).\r\n", ++cnt);
                //Ql_memset(notes, 0x0, Ql_strlen(notes));
                //Ql_sprintf(notes, "The timer raises for %d time(s).\r\n", ++cnt);
                //Ql_DebugTrace(notes);

                char chTest[20] = { 0x0 };
                Ql_memcpy(chTest, "0.12345", 7);
                Value=Ql_atof(chTest);
                Ql_DebugTrace("\r\nBuffer is %s.\r\n", chTest);
                Ql_DebugTrace("\r\nFloat value is %f.\r\n", Value);
                
                // Start the timer again
                if (cnt++ < 3)
                {
                    //Ql_StartTimer(&tm);
                }
                else
                {
                    Ql_DebugTrace("\r\nThe timer stops.\r\n");
                }
                break;
            }
            case EVENT_MSG:
            {
                //TODO: specify what you want to happen when the message from other task arrives
                break;
            }
            
            default:
                break;
        }
    }
    Ql_FreeMemory(pStr);
}

#endif // __EXAMPLE_FLOAT_TEST__

