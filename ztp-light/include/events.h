
#include "ql_type.h"

#ifndef __EVENT_H__ 
#define __EVENT_H__

typedef enum {
	//������ ��� ������ ������ 0
	ACCEPTKEY0 = 30,
	//������ ��� ������ ������ 1
	ACCEPTKEY1 = 0x1f,
	//������ ��� ������ ������ 2
	ACCEPTKEY2 = 0x20,
	//������ ��� ������ ������ 3
	ACCEPTKEY3 = 0x21,
	//������ ��� ������ ������ 4
	ACCEPTKEY4 = 0x22,
	//������ ��� ������ ������ 5
	ACCEPTKEY5 = 0x23,
	//������ ��� ������ ������ 6
	ACCEPTKEY6 = 0x24,
	//������ ��� ������ ������ 7
	ACCEPTKEY7 = 0x25,
	//������ ��� ������ ������ 8
	ACCEPTKEY8 = 0x26,
	//������ ��� ������ ������ 9
	AcceptKey9 = 0x27,
	//���������� ���� 1 ���� �����
	AIHigh1 = 0x36,
	//���������� ���� 2 ���� �����
	AIHigh2 = 0x37,
	//���������� ���� 3 ���� �����
	AIHigh3 = 0x38,
	//���������� ���� 4 ���� �����
	AIHigh4 = 0x39,
	//���������� ���� 1 ���� �����
	AILow1 = 0x3a,
	//���������� ���� 2 ���� �����
	AILow2 = 0x3b,
	//���������� ���� 3 ���� �����
	AILow3 = 60,
	//���������� ���� 4 ���� �����
	AILow4 = 0x3d,
	//���������� ���� 1, �����
	AINorm1 = 50,
	//���������� ���� 2, �����
	AINorm2 = 0x33,
	//���������� ���� 3, �����
	AINorm3 = 0x34,
	//���������� ���� 4, �����
	AINorm4 = 0x35,
	//��������� ���������� ������ ��� �������� ������������
	CrashesWhenBattery = 0x18,
	//���� 1, ������� H-L
	DIHighToLow1 = 1,
	//���� 2, ������� H-L
	DIHighToLow2 = 3,
	//���� 3, ������� H-L
	DIHighToLow3 = 5,
	//���� 4, ������� H-L
	DIHighToLow4 = 7,
	//���� 5, ������� H-L
	DIHighToLow5 = 9,
	//���� 6, ������� H-L
	DIHighToLow6 = 11,
	//���� 7, ������� H-L
	DIHighToLow7 = 13,
	//���� 8, ������� H-L
	DIHighToLow8 = 15,
	//���� 1, ������� L-H
	DILowToHigh1 = 2,
	//���� 2, ������� L-H
	DILowToHigh2 = 4,
	//���� 3, ������� L-H
	DILowToHigh3 = 6,
	//���� 4, ������� L-H
	DILowToHigh4 = 8,
	//���� 5, ������� L-H
	DILowToHigh5 = 10,
	//���� 6, ������� L-H
	DILowToHigh6 = 12,
	//���� 7, ������� L-H
	DILowToHigh7 = 14,
	//���� 8, ������� L-H
	DILowToHigh8 = 0x10,
	//��� �������
	None = 0,
	//������� ������� ���������
	PowerOff = 0x17,
	//������� ������� ��������
	PowerOn = 0x16,
	//���� � ������ ������ 0
	ReleaseKey0 = 40,
	//���� � ������ ������ 1
	ReleaseKey1 = 0x29,
	//���� � ������ ������ 2
	ReleaseKey2 = 0x2a,
	//���� � ������ ������ 3
	ReleaseKey3 = 0x2b,
	//���� � ������ ������ 4
	ReleaseKey4 = 0x2c,
	//���� � ������ ������ 5
	ReleaseKey5 = 0x2d,
	//���� � ������ ������ 6
	ReleaseKey6 = 0x2e,
	//���� � ������ ������ 7
	ReleaseKey7 = 0x2f,
	//���� � ������ ������ 8
	ReleaseKey8 = 0x30,
	//���� � ������ ������ 9
	ReleaseKey9 = 0x31,
	//�������� ����� �������, ��������� ������
	SHLAlarm = 0x13,
	//�������� ����� �������, �����
	SHLBreak = 20,
	//�������� ����� �������, �����
	SHLNormal = 0x12,
	//�������� ����� �������, �������� ���������
	SHLShortCircut = 0x11,
	//������ �������
	TurnOn = 0x15,
	//��������� ������� ���������� �� ������ 1
	DO1Exec = 0x3E,
	//��������� ������� ���������� �� ������ 2
	DO2Exec = 0x3F,
	//��������� ������� ���������� �� ������ 3
	DO3Exec = 0x40,
	//��������� ������� ���������� �� ������ 4
	DO4Exec = 0x41,
	//��������� ������� ���������� �� ������ 5
	DO5Exec = 0x42,
	//��������� ������� ���������� �� ������ 6
	DO6Exec = 0x43,
	//��������� ������� ���������� �� ������ 7
	DO7Exec = 0x44,
	//��������� ������� ���������� �� ������ 8
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
//������� ���������� ������ ���� ������ 0 �������� ��������
extern u32 __toSend;
//����� �������� ������������� ������
extern u16 __numPacket;
//������� �������.���������� ������� ������� ������
extern s32 __timeOutEvents;
extern u32 __count_num_packets;

typedef enum{
	//����� ������� �������
	Events = 0,
	//����� ������������� ������
	ConfirmReceipt = 1,
	//��������� �����
	Text = 2,
	//����� ��������� �������
	StateInput = 3
}TypePackage;

typedef struct{
	//��� �������
	unsigned char	TypeEvent;
	//�������� ���
	u16	EventDesc;
	//����� �������
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

//���������� �������� �������
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
