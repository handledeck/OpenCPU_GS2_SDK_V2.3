
#include "ql_type.h"

#ifndef __EVENT_H__ 
#define __EVENT_H__

typedef enum {
	//Принят под охрану ключом 0
	ACCEPTKEY0 = 30,
	//Принят под охрану ключом 1
	ACCEPTKEY1 = 0x1f,
	//Принят под охрану ключом 2
	ACCEPTKEY2 = 0x20,
	//Принят под охрану ключом 3
	ACCEPTKEY3 = 0x21,
	//Принят под охрану ключом 4
	ACCEPTKEY4 = 0x22,
	//Принят под охрану ключом 5
	ACCEPTKEY5 = 0x23,
	//Принят под охрану ключом 6
	ACCEPTKEY6 = 0x24,
	//Принят под охрану ключом 7
	ACCEPTKEY7 = 0x25,
	//Принят под охрану ключом 8
	ACCEPTKEY8 = 0x26,
	//Принят под охрану ключом 9
	AcceptKey9 = 0x27,
	//Аналоговый вход 1 выше нормы
	AIHigh1 = 0x36,
	//Аналоговый вход 2 выше нормы
	AIHigh2 = 0x37,
	//Аналоговый вход 3 выше нормы
	AIHigh3 = 0x38,
	//Аналоговый вход 4 выше нормы
	AIHigh4 = 0x39,
	//Аналоговый вход 1 выше нормы
	AILow1 = 0x3a,
	//Аналоговый вход 2 выше нормы
	AILow2 = 0x3b,
	//Аналоговый вход 3 выше нормы
	AILow3 = 60,
	//Аналоговый вход 4 выше нормы
	AILow4 = 0x3d,
	//Аналоговый вход 1, норма
	AINorm1 = 50,
	//Аналоговый вход 2, норма
	AINorm2 = 0x33,
	//Аналоговый вход 3, норма
	AINorm3 = 0x34,
	//Аналоговый вход 4, норма
	AINorm4 = 0x35,
	//Аварийное завершение работы при разрядке аккумулятора
	CrashesWhenBattery = 0x18,
	//Вход 1, переход H-L
	DIHighToLow1 = 1,
	//Вход 2, переход H-L
	DIHighToLow2 = 3,
	//Вход 3, переход H-L
	DIHighToLow3 = 5,
	//Вход 4, переход H-L
	DIHighToLow4 = 7,
	//Вход 5, переход H-L
	DIHighToLow5 = 9,
	//Вход 6, переход H-L
	DIHighToLow6 = 11,
	//Вход 7, переход H-L
	DIHighToLow7 = 13,
	//Вход 8, переход H-L
	DIHighToLow8 = 15,
	//Вход 1, переход L-H
	DILowToHigh1 = 2,
	//Вход 2, переход L-H
	DILowToHigh2 = 4,
	//Вход 3, переход L-H
	DILowToHigh3 = 6,
	//Вход 4, переход L-H
	DILowToHigh4 = 8,
	//Вход 5, переход L-H
	DILowToHigh5 = 10,
	//Вход 6, переход L-H
	DILowToHigh6 = 12,
	//Вход 7, переход L-H
	DILowToHigh7 = 14,
	//Вход 8, переход L-H
	DILowToHigh8 = 0x10,
	//Нет событий
	None = 0,
	//Сетевое питание выключено
	PowerOff = 0x17,
	//Сетевое питание включено
	PowerOn = 0x16,
	//Снят с охраны ключом 0
	ReleaseKey0 = 40,
	//Снят с охраны ключом 1
	ReleaseKey1 = 0x29,
	//Снят с охраны ключом 2
	ReleaseKey2 = 0x2a,
	//Снят с охраны ключом 3
	ReleaseKey3 = 0x2b,
	//Снят с охраны ключом 4
	ReleaseKey4 = 0x2c,
	//Снят с охраны ключом 5
	ReleaseKey5 = 0x2d,
	//Снят с охраны ключом 6
	ReleaseKey6 = 0x2e,
	//Снят с охраны ключом 7
	ReleaseKey7 = 0x2f,
	//Снят с охраны ключом 8
	ReleaseKey8 = 0x30,
	//Снят с охраны ключом 9
	ReleaseKey9 = 0x31,
	//Охранный шлейф включен, нарушение шлейфа
	SHLAlarm = 0x13,
	//Охранный шлейф включен, обрыв
	SHLBreak = 20,
	//Охранный шлейф включен, норма
	SHLNormal = 0x12,
	//Охранный шлейф включен, короткое замыкание
	SHLShortCircut = 0x11,
	//Прибор включен
	TurnOn = 0x15,
	//Выполнена команда управления по выходу 1
	DO1Exec = 0x3E,
	//Выполнена команда управления по выходу 2
	DO2Exec = 0x3F,
	//Выполнена команда управления по выходу 3
	DO3Exec = 0x40,
	//Выполнена команда управления по выходу 4
	DO4Exec = 0x41,
	//Выполнена команда управления по выходу 5
	DO5Exec = 0x42,
	//Выполнена команда управления по выходу 6
	DO6Exec = 0x43,
	//Выполнена команда управления по выходу 7
	DO7Exec = 0x44,
	//Выполнена команда управления по выходу 8
	DO8Exec = 0x45

}TypeEvent;

#define LEN_EVENT 3
#define LEN_EVENT_DATA 9
#define LEN_WITHOUT_DATA 8
#define LEN_BUFFER_SEND 8192
#define EVENT_SEND_TIMEOUT 60000;
#define LEN_INPUT_STATE 10;
#define LEN_TEMP_BUFFER 18;

extern unsigned char __SendBuffer[];
//признак наполнения буфера если больше 0 начинаем передачу
extern u32 __toSend;
//номер текущего передаваемого пакета
extern u16 __numPacket;
//таймаут событий.необходима посылка пустого пакета
extern s32 __timeOutEvents;
extern u32 __count_num_packets;

typedef enum{
	//Пакет событий прибора
	Events = 0,
	//Пакет подтверждения приема
	ConfirmReceipt = 1,
	//Текстовый пакет
	Text = 2,
	//Пакет состояния выходов
	StateInput = 3
}TypePackage;

typedef struct{
	//тип события
	unsigned char	TypeEvent;
	//описание его
	u16	EventDesc;
	//время события
	unsigned char	TimeEvent[6];
}EventData;

typedef struct{
    unsigned char din;
    unsigned short dout_1;
    unsigned short dout_2;
    unsigned short dout_3;
    unsigned short dout_4;
    unsigned char guard_tape;  
}StateInputData;

//переменная текущего события
extern EventData __currentEvData;  
extern StateInputData __inputState;
extern u16 __countMsgSend;

unsigned short ReverseUSort(unsigned short* value);
u32 ReverseUInt(u32* value);
void WriteShort(unsigned char* data, unsigned short* value);
void WriteAddress(unsigned char* data, unsigned int* value);
int BuildEventMessage(unsigned char PowerState, EventData* events, unsigned short count);
void BuildEvent(unsigned char* data, unsigned short* pos, EventData* events, unsigned short count);
void BuildInput(unsigned char* data,unsigned short* pos);
int BuildInputStateMessage(void);
void TrigerEvent(TypeEvent typeEvent,unsigned short description,EventData* eventData);
void SetEventTime(EventData* evdata);
int replace(unsigned char* data, int datalen, unsigned char*buffer);
int unreplace(unsigned char* data, int datalen);
void ExtractPakets(unsigned char* pakets, u32 dlen);
void CheckTypePacket(unsigned char* packet,unsigned short len);
void ParseTextCommand(char* command,unsigned char len);
int myAtoi(char *str);


#endif
