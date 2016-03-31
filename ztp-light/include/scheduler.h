#ifndef SCHEDULE_H_
#define SCHEDULE_H_

#include "ql_filesystem.h"
#include "stdlib.h"
#include "ql_stdlib.h"
#include "ql_timer.h"

#define SCHEDULER_FILE "schedule.bin"

typedef struct timezone{
	u8 hour : 4;
	u8 mark : 1;
	u8 mark_summer:1;
}t_timezone;


typedef struct schedule {
	u8 type : 1;
	u8 type_on : 2;
	u8 type_off : 5;
	u8 hour_on;
	u8 min_on;
	u8 hour_off;
	u8 min_off;
}Schedule;

typedef struct seasson {
	u8 num_month_start;
	u8 num_day_start;
	u8 num_month_end;
	u8 num_day_end;
	u8 count_schedules;
	struct schedule schedules[8];
}t_season;


typedef struct schedule_config {
	struct timezone tz;
	u8 count_seasson;
	struct seasson seassons[4];
}t_schedule_config;


typedef struct schedule_stamp{
	u32 start_stamp;
	u32 end_stamp;	
}Use_Schedule;



s8 search_index_seasson(struct schedule_config* config,u8 month,u8 day);
Schedule* search_index_schedule(struct schedule_config* config, u8 seasson_index,struct QlSysTimerTag time, struct schedule_stamp* sch_stamp,s8* index);
void parse_seassons(u8* data,struct schedule_config* config) ;
s8 check_data_crc(u8* data,u8 lenght);	
s8 read_schedule(u8* data, struct schedule_config* config);
s8 write_schedule(u8* data,u32 len);
s8 read_file_schedule(u8* data);


//status_code_t get_schedule_stamp(rmc_packet* rmc,struct schedule_config* config,struct schedule_stamp* sch_stamp);


#endif /* SCHEDULE_H_ */