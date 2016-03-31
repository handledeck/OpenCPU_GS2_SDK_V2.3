#include "scheduler.h"
#include "calendar.h"
#include "ql_filesystem.h"
#include "global.h"

s8 search_index_seasson(struct schedule_config* config, u8 month, u8 day) {
	for (u8 i = 0; i < config->count_seasson; i++)
	{
		if (config->seassons[i].num_month_start<month && config->seassons[i].num_month_end > month) {
			if (config->seassons[i].num_day_start < day && config->seassons[i].num_day_end > day)
				return i;
		}
	}
	return -1;
}

//Если текущее время находится в интервале расписания возвращаем расписание, если нет возвращает NULL. Переменная Index возвращает индекс длижайшего расписания   
Schedule* search_index_schedule(struct schedule_config* config, u8 seasson_index,struct QlSysTimerTag time, struct schedule_stamp* sch_stamp,s8* index) {
	struct schedule_stamp sch_stamp_temp;
	calendar_datetime_from_systdatetime(&time);
	u32 dtnow = calendar_date_to_timestamp(&time);
	s32 first = -1;
	*index = -1;
	for (u8 i = 0; i < config->seassons[seasson_index].count_schedules; i++)
	{
		Schedule* sch = &config->seassons[seasson_index].schedules[i];
		if (sch->type == 1)
		{
			//add or remove hours to schedule
		}
		else
		{
			struct QlSysTimerTag start;
			start.day = time.day;
			start.month = time.month;
			start.year = time.year;
			start.hour = sch->hour_on;
			start.minute = sch->min_on;
			start.second = 0;
			
			u32 time_on = calendar_date_to_timestamp(&start);
			if (sch->hour_off<start.hour)
				calendar_add_day_to_date(&start);
			start.hour = sch->hour_off;
			start.minute = sch->min_off;
			u32 time_off = calendar_date_to_timestamp(&start);
			sch_stamp->start_stamp = time_on;
			sch_stamp->end_stamp = time_off;
			if (time_on<dtnow && time_off>dtnow) {
				return sch;
			}
			else
			{
				s32 x = time_on - dtnow;
				if (x > 0) {
					if (first == -1) {
						first = x;
						*index = i;
						sch_stamp_temp = *sch_stamp;
					}
					else if (first > x)
					{
						first = x;
						*index = i;
						sch_stamp_temp = *sch_stamp;
					}
					
				}
			}
		}
	}
	*sch_stamp = sch_stamp_temp;
	return NULL;
}


void parse_seassons(u8* data, struct schedule_config* config) {
	Ql_memcpy(&config->tz, data, 1);
	config->count_seasson = *(++data);
	for (size_t i = 0; i < config->count_seasson; i++)
	{
		config->seassons[i].num_month_start = *(++data);
		config->seassons[i].num_day_start = *(++data);
		config->seassons[i].num_month_end = *(++data);
		config->seassons[i].num_day_end = *(++data);
		config->seassons[i].count_schedules = *(++data);;
	}
	data++;
	for (u8 i = 0; i < config->count_seasson; i++)
	{
		for (u8 x = 0; x < config->seassons[i].count_schedules; x++)
		{
			Ql_memcpy(&config->seassons[i].schedules[x], data, 5);
			data += 5;
		}
	}
}

s8 check_data_crc(u8* data, u8 length) {
	u8 crc = 0;
	if (*data == NULL) return -1;
	for (u8 i = 0; i < length; i++)
	{
		crc ^= *data++;
	}
	if (crc != 0)
		return -1;
	return 0;
}

s8 read_schedule(u8* data, struct schedule_config* config) {
	u8 addr = 0;
	u8 i = 0;
	u8 ch[255];
	Ql_memset(&ch[0], 0, 255);
	if (data[addr++] == '$')
	{
		u8 len = data[addr++];
		for (i = 0; i<len; i++)
		{
			ch[i] = data[addr++];
		}
		ch[++i] = data[addr];
		if (check_data_crc(&ch[0], i + 1) == 0) {
			parse_seassons(&ch[0], config);
		}
		else
			return -1;
	}
	else
		return -1;
	return 0;
}

s8 write_schedule(u8* data,u32 len)
{
    s32 handle;
    u32 writen=0;
	handle = Ql_FileOpenEx(SCHEDULER_FILE, (QL_FS_READ_WRITE | QL_FS_CREATE)); 
    if (handle>0) {
       Ql_FileWrite(handle,data,len,&writen);        
       Ql_FileClose(handle);
       return 1;
    }
    else{
        OUTD("!Can't open file error:%d",handle);
        return 0;
    }
}

s8 read_file_schedule(u8* data){
    s32 handle=0;
    u32 filesize=0;
    u32 readed=0;
    Ql_FileGetSize(SCHEDULER_FILE,&filesize);
	handle = Ql_FileOpenEx(SCHEDULER_FILE, (QL_FS_READ_WRITE)); 
   if (handle>0 && filesize>0) {
       Ql_FileRead(handle,data,filesize,&readed);
        Ql_FileClose(handle);
        return 1; 
    }
    else{
        OUTD("!Can't open file error:%d",handle);
        return 0;
    }
}

