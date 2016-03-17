#include "events.h"
#include "crc.h"
#include "ql_type.h"
#include "ql_stdlib.h"
#include "ql_timer.h"
#include "ql_memory.h"
#include "global.h"
#include "files.h"
#include "mgpio.h"
#include "ql_interface.h"
#include "transport.h"

unsigned char __SendBuffer[LEN_BUFFER_SEND];
u16 __countMsgSend=0;
u8 __countRepeat=0;
u32 __count_num_packets=90;

unsigned short ReverseUSort(unsigned short* value){
	unsigned short val=0;
	val= (*value << 8& 0xFF00) ^ (*value>>8 & 0x00FF);
    //val=value[1]<<8^value[0];
    return val;
	//return val;
}


u32 ReverseUInt(u32* value){
	return *value << 24 ^ (*value >> 8 & 0xFF) << 16 ^ (*value >> 16 & 0xFF) << 8 ^ (*value >> 24 & 0xFF);
}

void BuildEvent(unsigned char* data,unsigned short* pos, EventData* events,unsigned short count){
	for (u32 i = 0; i < count; i++){
		unsigned short evdesc;
		EventData* evdata = &events[i];
	data[*pos] = evdata->TypeEvent;
	*pos+=1;
	evdesc = evdata->EventDesc;
	WriteShort(&data[*pos], &evdesc);
	*pos += 2;
	Ql_memcpy(&data[*pos], &evdata->TimeEvent[0], 6);
   
	*pos += 6;
	}
}

void BuildInput(unsigned char* data,unsigned short* pos){
    //состояние входов
    data[*pos]=__pins;
    *pos+=1;
    WriteShort(&data[*pos], 0);
    *pos+=2;
    WriteShort(&data[*pos], 0);
    *pos+=2;
    WriteShort(&data[*pos], 0);
    *pos+=2;
    WriteShort(&data[*pos], 0);
    *pos+=1;
    data[*pos]=0;
    *pos++;
}

void TrigerEvent(TypeEvent typeEvent, unsigned short description,EventData* eventData){
	eventData->TypeEvent = typeEvent;
	eventData->EventDesc = description;
	SetEventTime(eventData);
}

void WriteShort(unsigned char* data,unsigned short* value){
	unsigned short val = ReverseUSort(value);
	Ql_memcpy(data, &val, sizeof(unsigned short));
}

void WriteAddress(unsigned char* data, unsigned int* value){
	int val = *value << 16 ^(0x0000FF00 & *value)^(*value>>16&0x000000FF);
	Ql_memcpy(data, &val, 3);
}

int BuildEventMessage(unsigned char PowerState,EventData* events,unsigned short len_evt){
    unsigned short len = LEN_WITHOUT_DATA + (LEN_EVENT+(LEN_EVENT_DATA*len_evt));
    //unsigned char* tempBuf = (unsigned char*)Ql_GetMemory(len+LEN_WITHOUT_DATA);
    unsigned char tempBuf[512]; 
    unsigned short pos = 0;
	unsigned int address = __settings.Num;
	unsigned short lensend = 0;
	u16 crc;
	//write lenght message
    //Ql_memset(&tempBuf[0],0,LEN_EVENT_DATA*(len_evt)+LEN_WITHOUT_DATA);
    Ql_memset(&tempBuf[0],0,512);
    WriteShort(&tempBuf[pos], &len);
	pos += 2;
	//write address device
	WriteAddress(&tempBuf[pos], &address);
	pos += 3;
	//тип пакаета
	tempBuf[pos] = 0;
	//номер пакета
    WriteShort(&tempBuf[++pos], &__numPacket); 
	pos += 2;
	//источник питания
	tempBuf[pos] = PowerState;
	pos++;
	//упаковка всех сообщений
	BuildEvent(tempBuf, &pos, events, len_evt);
	//расчет изапись crc
	crc = calc_crc(&tempBuf[0], len - 2);
	WriteShort(&tempBuf[pos], &crc);
	pos += 2;
    Ql_memset(&__SendBuffer[0],0,LEN_BUFFER_SEND);
	//формирование основного пакета для передачи		
	__SendBuffer[0] = 0x17; __SendBuffer[1] = 0x13;
	//замена символов
	lensend = replace(&tempBuf[0], pos, &__SendBuffer[2]);
	lensend += 2;
	//Ql_FreeMemory(tempBuf);
	//окончание формирования сообщения
	__SendBuffer[lensend] = 0x17; __SendBuffer[++lensend] = 0x5;
    //OUTD("Build message event.Number packet:%d",__numPacket);
    __waitConfirm=1;
	return ++lensend;
}


int BuildInputStateMessage(){   
    unsigned short pos = 0;
	unsigned int address = __settings.Num;
	unsigned short lensend = 0;
	u16 crc;
    unsigned short len=LEN_WITHOUT_DATA + LEN_INPUT_STATE;
    unsigned char tempBuf[18];
    
    //len = LEN_WITHOUT_DATA + LEN_INPUT_STATE;
    
    //tempBuf = (u8*)Ql_GetMemory(len);
    
    Ql_memset(&tempBuf[0],0,len);
    
    WriteShort(&tempBuf[pos], &len);
	pos += 2;
	//write address device

    WriteAddress(&tempBuf[pos], &address);
    
	pos += 3;
	//тип пакаета
	tempBuf[pos] = 3;
	pos +=1;
    //упаковка сообщения
	//BuildInput(tempBuf,&pos);
    tempBuf[pos] = __pins;
    pos+=10;
	//расчет изапись crc
	crc = calc_crc(&tempBuf[0], len - 2);
	WriteShort(&tempBuf[pos], &crc);
	pos += 2;
	//формирование основного пакета для передачи
    //Ql_memset(&__SendBuffer[0],0,LEN_BUFFER_SEND);		
	__SendBuffer[0] = 0x17; __SendBuffer[1] = 0x13;
	//замена символов
	lensend = replace(&tempBuf[0], pos, &__SendBuffer[2]);
	lensend += 2;
	//Ql_FreeMemory(tempBuf);
	//окончание формирования сообщения
	__SendBuffer[lensend] = 0x17; __SendBuffer[++lensend] = 0x5;
    
	return ++lensend;
    
}

void SetEventTime(EventData* evdata){
	QlSysTimer cmn;
	Ql_GetLocalTime(&cmn);
	evdata->TimeEvent[5] = cmn.second;
	evdata->TimeEvent[4] = cmn.minute;
	evdata->TimeEvent[3] = cmn.hour;
	evdata->TimeEvent[2] = cmn.year;
	evdata->TimeEvent[1] = cmn.day;
	evdata->TimeEvent[0] = cmn.month;
}

int replace(unsigned char* data, int datalen, unsigned char*buffer){
	unsigned char find = 0x17;
	u32 locpos = 0;
    u32 i;
	for (i = 0; i < datalen; i++)
	{
		if (data[i] == find)
		{
			buffer[locpos] = data[i];
			buffer[++locpos] = 0x12;
		}
		else buffer[locpos] = data[i];
		locpos++;
	}
	return locpos;
}

int unreplace(unsigned char* data, int datalen){
	unsigned char find = 0x17;
	unsigned char find_1 = 0x12;
	u32 pos = 0;
    unsigned char buffer[512];
	for (u32 i = 0; i < datalen; i++)
	{
		if (data[i] == find && data[i + 1] == find_1)
		{
			buffer[pos] = data[i];
			i++;
		}
		else buffer[pos] = data[i];
		pos++;
	}
	ExtractPakets(&buffer[0], pos);
	return pos;
}

void ExtractPakets(unsigned char* pakets,u32 dlen){
	u8 ch_begin = 0x17;
	u8 ch_start = 0x13;
	u8 ch_end = 0x5;
	u16 start = 0;
	u16 end = 0;
	//u16 crc,
    u16 len;
    u32 address;
	for (u32 i = 0; i < dlen; i++)
	{
		if (pakets[i] == ch_begin && pakets[i + 1] == ch_start){
			start = i+2;
			i++;
			continue;
		}
		if (pakets[i] == ch_begin && pakets[i + 1] == ch_end){
			end = i-1;
			if (calc_crc(&pakets[start], (end-start)-1)==(pakets[end-1]<<=8)| pakets[end]){//(clc== crc){
				//длина пакета
				len =ReverseUSort((unsigned short*)&pakets[start]);
				//адрес устройства
				address = (int)((((pakets[start+2] << 0x10) | (pakets[start + 3] << 8)) | pakets[start+4]));
                //OUTD("Buffer lenght:%d. Packet length:%d. Address device:%d.",dlen,len,address);
				//тип принятого пакета
				CheckTypePacket(&pakets[start + 5],len-8);
                continue;
			}
		}		
	}	
}

void CheckTypePacket(unsigned char* packet, u16 len){
	TypePackage tpack = (TypePackage)*packet;
    
	switch (tpack)
	{
	case Events:
		break;
	case ConfirmReceipt:
	{
		packet++;
        //OUTD("char packet NUMBER",packet[1]<<8^packet[0]);
        
		unsigned short pnum =ReverseUSort((unsigned short*)packet);
        if (pnum==__numPacket) {
            __countRepeat=0;
             OUTD(">Recive confirm number:%d.", pnum);
              //LOG(">Recive confirm number:%d.", pnum);
             ConfirmMsg(); 
              if (__numPacket>=__count_num_packets) 
                  __numPacket=0;
              __numPacket++;
              __waitConfirm=0;
        }
          else{
               OUTD("!Diff number pack send:%d recive:%d ",__numPacket,pnum);
                  //TODO Перегрузка или закрытие сокета
                  __countRepeat++;
                   
                  //nSTATE=STATE_SOCKET_RECIVE;
                  if (__countRepeat>2) {
                      
                      OUTD("!Diff packet more than 3.System reboot",NULL);  
                      Ql_Sleep(200);
                      Ql_Reset(0);
                  }
         }      
		break;
	}
	case Text:{
		packet[len+1]='\0';
        OUTD(">Recive:%s",++packet);
		ParseTextCommand((char*)packet,len);
		break;
	}
	case StateInput:
		break;
	default:
		break;
	}
}


void ParseTextCommand(char* command,unsigned char len){
		command[len] = '\0';
		char* pc;
		char sym[3];
		int day=0;int month=0;int year=0;
		int hour=0;int minute=0;int second=0;
        unsigned char loctime;
		char* confing = Ql_strstr(command, "CONFIG");
		if (confing){
			pc = Ql_strstr(confing++, "DATE");
			if (pc){
				Ql_memset(&sym[0], 0, 3);
				pc += 5;
				Ql_memcpy(&sym[0],pc,2);
				day=myAtoi(&sym[0]);
				pc += 3;
				Ql_memcpy(&sym[0], pc, 2);
				month = myAtoi(&sym[0]);
				pc += 3;
				Ql_memcpy(&sym[0], pc, 2);
				year = myAtoi(&sym[0]);
				pc++;
				
			}
			pc = Ql_strstr(confing++, "TIME");
			if (pc){
				
				Ql_memset(&sym[0], 0, 3);
				pc += 5;
				Ql_memcpy(&sym[0], pc, 2);
				hour = myAtoi(&sym[0]);
				pc += 3;
				Ql_memcpy(&sym[0], pc, 2);
				minute = myAtoi(&sym[0]);
				pc += 3;
				Ql_memcpy(&sym[0], pc, 2);
				second = myAtoi(&sym[0]);
				pc++;
			}
			QlSysTimer tm;
			tm.year = year;
			tm.month = month;
			tm.day = day;
			tm.hour = hour;
			tm.minute = minute;
			tm.second = second;
            loctime =Ql_SetLocalTime(&tm);
            OUTD(">Set local time:%d-%d-%d %d:%d:%d result:%d",year,month,day,hour,minute,second,loctime);
            LOG(">Set local time:%d-%d-%d %d:%d:%d result:%d",year,month,day,hour,minute,second,loctime);
		}
	}

	int myAtoi(char *str)
	{
		int res = 0; // Initialize result
		// Iterate through all characters of input string and update result
		for (int i = 0; str[i] != '\0'; ++i)
			res = res * 10 + str[i] - '0';
		return res;
	}





