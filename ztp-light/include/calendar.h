#ifndef _CALENDAR_H_INCLUDED_
#define _CALENDAR_H_INCLUDED_

#include "ql_type.h"
#include "ql_stdlib.h"
#include "ql_timer.h"

struct calendar_date {
	u8 second;        //!< 0-59
	u8 minute;        //!< 0-59
	u8 hour;          //!< 0-23
	u8 date;          //!< 0-30 \note First day of month is 0, not 1.
	u8 month;         //!< 0 January - 11 December
	u16 year;         //!< 1970-2105
	u8 dayofweek;     //!< 0 Sunday  - 6 Saturday
};

bool calendar_is_date_valid(struct QlSysTimerTag *date);

void calendar_timestamp_to_date(u32 timestamp, struct QlSysTimerTag
		*date_out);

void calendar_timestamp_to_date_tz(u32 timestamp, s8 hour,
		u8 min, struct QlSysTimerTag *date_out);

u32 calendar_date_to_timestamp(struct QlSysTimerTag *date);

u32 calendar_date_to_timestamp_tz(struct QlSysTimerTag *date, s8 hour,
		u8 min);

void calendar_time_between_dates(struct QlSysTimerTag *date_end,
		struct QlSysTimerTag *date_start, struct QlSysTimerTag *date_out);

void calendar_add_second_to_date(struct QlSysTimerTag *date);

void calendar_add_day_to_date(struct QlSysTimerTag *date);

void calendar_add_hour_to_date(struct QlSysTimerTag *date);

void calendar_date_to_tz(struct QlSysTimerTag *date,s8 hour,u8 min);

void calendar_datetime_from_systdatetime(struct QlSysTimerTag *date);

void calendar_datetime_to_sysdatetime(struct QlSysTimerTag *date);

#endif /* _CALENDAR_H_INCLUDED_ */
