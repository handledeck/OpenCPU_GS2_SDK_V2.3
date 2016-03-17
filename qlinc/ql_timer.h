/*==========================================================================
 |               Quectel OpenCPU --  User API
 |
 |              Copyright (c) 2010 Quectel Ltd.
 |
 |--------------------------------------------------------------------------
 | File Description
 | ----------------
 |      Debug interface definition
 |
 |--------------------------------------------------------------------------
 |
 |  Designed by     :   Jay XIN
 |  Coded    by     :   Jay XIN
 |  Tested   by     :   Stanley YONG
 |--------------------------------------------------------------------------
 | Revision History
 | ----------------
 |  Sep. 14, 2010       Stanley Yong        Change the type of timerId to u32.
 |                                          Revise the way to generate timer Id.
 |  ------------------------------------------------------------------------
 |  Sep. 19, 2010       Jay XIN             Add APIs: 'Ql_StarLowLevelTimer'
 |                                          and 'Ql_StopLowLevelTimer'.
 |  ------------------------------------------------------------------------
 |  Sep. 26, 2010       Stanley Yong        Make the notes clear.
 |
 |  ------------------------------------------------------------------------
 |  Oct. 19, 2012       Stanley Yong        Raname Ql_StarLowLevelTimer to Ql_StartGPTimer
 |                                          and Ql_StopLowLevelTimer to Ql_StopGPTimer.
 \=========================================================================*/

#ifndef __QL_TIMER_H__
#define __QL_TIMER_H__

#include "ql_type.h"

/* Timer */
typedef struct QlTimerTag
{
    ticks		timeoutPeriod;
    u32			timerId;
}
QlTimer;

typedef struct QlSysTimerTag
{
    unsigned short year;    /* A short format of year, 0-127 */
    unsigned char month;
    unsigned char day;
    unsigned char hour;
    unsigned char minute;
    unsigned char second;
}
QlSysTimer;


/*****************************************************************
* Function:     Ql_StartTimer 
* 
* Description:
*               Start up a timer.
*
* Parameters:
*               timer_p:
*                       [in] the timer object to start
* Return:        
*               Timer Id (positive number) indicates succeed in starting the timer
*               QL_RET_ERR_INVALID_TIMER indicates invalid timer
*               QL_RET_ERR_TIMER_FULL indicates all timers are used up
*****************************************************************/
u32   Ql_StartTimer(QlTimer *timer_p);


/*****************************************************************
* Function:     Ql_StopTimer 
* 
* Description:
*               Stop a timer.
*
* Parameters:
*               timer_p:
*                       [in] The QlTimer object to stop
* Return:        
*               QL_RET_OK indicates succeed in stopping the timer
*               QL_RET_ERR_INVALID_TIMER indicates invalid timer
*****************************************************************/
s16   Ql_StopTimer(QlTimer *timer_p);


/*****************************************************************
* Function:     Ql_GetLocalTime 
* 
* Description:
*               Retrieves the current local date and time.
*
* Parameters:
*               datetime:
*                       [out] Pointer to the QlSysTimer object
* Return:        
*               TRUE if succeed in retrieving the local date and time
*               , otherwise FALSE
*****************************************************************/
bool  Ql_GetLocalTime(QlSysTimer * datetime);


/*****************************************************************
* Function:     Ql_SetLocalTime 
* 
* Description:
*               Sets the current local date and time.
*
* Parameters:
*               datetime:
*                       [in] Pointer to the QlSysTimer object
* Return:        
*               TRUE if succeed in retrieving the local date and time
*               , otherwise FALSE
*****************************************************************/
bool  Ql_SetLocalTime(QlSysTimer * datetime);


/*****************************************************************
* Function:     Ql_GetRelativeTime 
* 
* Description:
*               This function returns the number of milliseconds 
*               since the device booted.
*
* Parameters:
*               None
* Return:        
*               Number of milliseconds
*****************************************************************/
u64   Ql_GetRelativeTime(void);


/*****************************************************************
* Function:     Ql_GetRelativeTime 
* 
* Description:
*               This function returns the number of MCU counters
*               since the device booted.
*
* Parameters:
*               None
* Return:        
*               Number of MCU counters
*****************************************************************/
u32   Ql_GetRelativeTime_Counter(void);


/*****************************************************************
* Function:     Ql_SecondToTicks 
* 
* Description:
*               Converts time from seconds to ticks.
*
* Parameters:
*               seconds:
*                      [in] Seconds to convert
* Return:        
*               ticks equivalent to the seconds
*****************************************************************/
ticks Ql_SecondToTicks(u32 seconds);


/*****************************************************************
* Function:     Ql_MillisecondToTicks 
* 
* Description:
*               Converts time from milliseconds to ticks.
*
* Parameters:
*               seconds:
*                       [in] Milliseconds to convert
* Return:        
*               ticks equivalent to the milliseconds
*****************************************************************/
ticks Ql_MillisecondToTicks(u32 milliseconds);


/*****************************************************************
* Function:     Ql_Mktime 
* 
* Description:
*               This function get total seconds elapsed 
*               since 1970.01.01 00:00:00.
*
* Parameters:
*               psystime:
*                       [in] Pointer to the QlSysTimer object
* Return:        
*               The total seconds
*---------------
* Usage:
*               QlSysTimer systime;
*               Ql_GetLocalTime(&systime);
*               systime->year += your base year(such as 1900, 2000)
*               seconds = Ql_Mktime(&systime);
*****************************************************************/
u32  Ql_Mktime(QlSysTimer *psystime);


/*****************************************************************
* Function:     Ql_LocalTime2CalendarTime 
* 
* Description:
*               This function convert local broken-down time to
*               the seconds, elapsed since 1970.01.01 00:00:00.
*
* Parameters:
*               pSysTime:
*                       [out] Pointer to the QlSysTimer object
*               baseyear:
*                       [in] the base year, eg. 1900, 2000
* Return:       
*               seconds elapsed since 1970.01.01 00:00:00. 
*               return invalid value(-1) if failed.
*
* Remark:		the real year would be baseyear+pSysTime->year;
*               
*---------------
* Usage:
*               QlSysTimer t;
*               Ql_GetLocalTime(&t);
*               secs = Ql_LocalTime2CalendarTime(&t, 2000);
*               
*****************************************************************/
u32 Ql_LocalTime2CalendarTime(QlSysTimer *pSysTime, u32 baseyear);



/*****************************************************************
* Function:     Ql_CalendarTime2LocalTime 
* 
* Description:
*               This function convert the seconds, elapsed since
*               1970.01.01 00:00:00, to local broken-down time.
*
* Parameters:
*               seconds : 
                        [in] seconds elapsed since 1970.01.01 00:00:00
*               pSysTime:
*                       [out] Pointer to the QlSysTimer object
*               baseyear:
*                       [in] the base year, eg. 1900, 2000
* Return:        
*               TRUE if successful, FALSE if failed.
*
* Remark:		the real year would be baseyear+pSysTime->year;
*---------------
* Usage:
*               QlSysTimer t;
*               Ql_GetLocalTime(&t);
*               secs = Ql_LocalTime2CalendarTime(&t, 2000);
*               bool ret = Ql_CalendarTime2LocalTime(secs, &t, 2000);
*               
*****************************************************************/
bool Ql_CalendarTime2LocalTime(u32 seconds, QlSysTimer *pSysTime, u32 baseyear);

/*****************************************************************
* Function:     Ql_StartGPTimer
* 
* Description:
*               This function starts a GP timer. 
*               Notes: The GP timer is a one-shot timer, and has a 
*                      higher accuracy than QlTimer. If you want it 
*                      to be a repeating timer, you can call this
*                      function again when time out.
*                      Only one GP timer is provided in the system.
* Parameters:
*               tick10ms:  
*                   [in] The time-out value, in 10-millisecond.
*
*               onTimer:
*                   [in] A callback function to be notified when 
*                        the time-out value elapses.  
*               parameter:
*                   [in] A void type of pointer, which can be passed
*                        into the callback function as parameter.
* Return:        
*               TRUE,  indicates this function successes in starting 
*                      the GP timer.
*               FALSE, indicates this function fails to start the GP timer.
*****************************************************************/
typedef void (*callback_timerout)(void *);
#define Ql_StarLowLevelTimer Ql_StartGPTimer  // for compatibility with old name
bool Ql_StartGPTimer(u32 tick10ms, callback_timerout onTimer, void *parameter);

/*****************************************************************
* Function:     Ql_StopGPTimer
* 
* Description:
*               Stop the GP Timer.
*
* Parameters:
*               None.
* Return:        
*               None.
*****************************************************************/
#define Ql_StopLowLevelTimer Ql_StopGPTimer  // for compatibility with old name
void Ql_StopGPTimer(void);

/*****************************************************************
* Function:     Ql_Alarm_StartUp
* 
* Description:
*                   Start up module when alarm coming.
*
* Parameters:
*               datetime:  [in] Pointer to the QlSysTimer object
*               recurr:[in]
*                               0: start alarm only one time.
*                               1: repeat alarm every day.
*                               2: repeat alarm every week.
*                               3: repeat alarm every month.
* Return:        
*               QL_RET_OK indicates this function successes.
*               QL_RET_ERR_PARAM indicates parameter error.
*               QL_RET_ERR_INVALID_TIMER indicates invalid timer.
*****************************************************************/

s32 Ql_Alarm_StartUp(QlSysTimer * datetime,u8 recurr);

/*****************************************************************
* Function:     Ql_Alarm_Remove
* 
* Description:
*                   Remove alarm.
*
* Parameters:
*
* Return:        
*               QL_RET_OK indicates this function successes.
*               QL_RET_ERR_PARAM indicates this function fail.
*****************************************************************/
s32 Ql_Alarm_Remove(void);
#endif  // End-of __QL_TIMER_H__

