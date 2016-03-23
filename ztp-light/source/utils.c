#include "ql_stdlib.h"
#include "utils.h"
#include "ql_trace.h"
#include  "global.h"
#include "ql_filesystem.h"
#include "ql_interface.h"
#include "mgpio.h"
#include "transport.h"
#include "files.h"



#define OUTER(frm,...)\
    Ql_memset(&__debug_buffer[0],0,LEN_DEBUG_BUFFER);\
    GetTextTime();\
    Ql_sprintf(&__debug_buffer[10],frm,__VA_ARGS__);\
    Ql_SendToUart(ql_uart_port1,(u8*)__debug_buffer,Ql_strlen(__debug_buffer));\
    Ql_SendToUart(ql_uart_port1,(u8*)"\r\n",2);

char __buf_command[BUFFER_COMMAND_LEN];
unsigned char __buf_command_pos;


void WriteSettings(void){
    s32 handle;
    u32 writed;
    s32 result;
    handle=Ql_FileOpenEx(SETTINGS_FILE,QL_FS_CREATE_ALWAYS);
    //handle=Ql_FileOpen(SETTINGS_FILE,TRUE,FALSE);
    if (handle<0) {
        OUTD("!Open file settings error:%d",handle);
        return;
    }
    else
    {
        result=Ql_FileWrite(handle,(u8*)&__settings,sizeof(settings),&writed);
        if (result!=0) {
            OUTD("!Write file settings error:%d",result);
        }
        else{
            Ql_FileFlush(handle);
            //OUTD("Write file settings is OK",NULL);
            Ql_FileClose(handle);
        }
    }
}

void SetDefaultSettins(void){
    __settings.IP[0]=172;
    __settings.IP[1]=23;
    __settings.IP[2]=0;
    __settings.IP[3]=9;
    __settings.TCP=10245;
    __settings.Num=1;
    __settings.Deboung=500;
    
    __settings.TSend=(DEVICE_CONTROL_EMPTY/1000);
    Ql_memcpy(__settings.Log,"FALSE",5);
    Ql_memcpy(__settings.Debug,"FALSE",5);
    Ql_memcpy(__settings.Pass,"gsd9drekj5",10);
    Ql_memcpy(__settings.Pwd,"NONE",8);
    Ql_memcpy(__settings.APN,"vpn2.mts.by",11);
    Ql_memcpy(__settings.User,"vpn",3);
    Ql_memcpy(__settings.WUser,"admin",5);
    Ql_memcpy(__settings.WPass,"admin",5);
}

void ReadSettings(void){
    s32 result;
    u32 readed;
    s32 handle;
    result=Ql_FileCheck(SETTINGS_FILE);
    if (result!=0) {
        //OUTD("!File settings not found:%d",result);
        //handle=Ql_FileOpenEx(SETTINGS_FILE,QL_FS_CREATE);
        //if (handle<0) {
          //  OUTD("Error create file settings.Error:%d",handle);
            //return;
        WriteSettings();
        }
        else{
           handle=Ql_FileOpenEx(SETTINGS_FILE,QL_FS_READ_ONLY);
           result=Ql_FileRead(handle,(u8*)&__settings,sizeof(settings),&readed);
            if (result!=0) {
                OUTD("!Error read file settings.Error:%d",result);
            }
            
            Ql_FileClose(handle);
            //PrintSettings();
        }
}

void PrintSettings(){
    QlSysTimer loct;
    ReadSettings();
    Ql_GetLocalTime(&loct);
     s32 simcard;
     s32 creg;
     s32 cgreg;
     u8 rssi;
     u8 ber;
     char pp[8];
     Ql_GetDeviceCurrentRunState(&simcard, &creg, &cgreg, &rssi, &ber);
     GetTextStateGpio(&pp[0]);
     OUTER("\r\nAPN=%s IP=%d.%d.%d.%d TCP=%d USER=%s PASS=%s DEBUG=%s LOG=%s NUM=%d TSEND=%d DATE=%.2d.%.2d.%.2d TIME=%.2d:%.2d:%.2d DEBOUNCE=%d LocalIp=%s WUser=%s WPass=%s\r\n\r\n ***** SIM:%d GSM:%d GPRS:%d PINS=%s SIGNAL:%ddBm ******\r\n",
                      __settings.APN,__settings.IP[0],__settings.IP[1],__settings.IP[2],__settings.IP[3],
                      __settings.TCP,__settings.User,__settings.Pass,__settings.Debug,__settings.Log,__settings.Num,__settings.TSend,
                     loct.day,loct.month,loct.year,loct.hour,loct.minute,loct.second,__settings.Deboung,
                     __str_loc_ip_addr,__settings.WUser,__settings.WPass,simcard,creg,cgreg,&pp[0],rssi);
     
}


void setCommand(char* command){
    char* cmd = NULL;
    char idx=0;
    char* pch=NULL;
    cmd = Ql_strstr(command, "LIGHTS=");
	if (cmd){
		cmd += 7;
		if (Ql_strstr(cmd, "ON")){
            set_light_on();
        }
        else{
            set_light_off();
        }
        Ql_strcpy(&__settings.Debug[0],cmd);
	}
	cmd = Ql_strstr(command, "DEBUG=");
	if (cmd){
		cmd += 6;
		if (Ql_strstr(cmd, "TRUE")){
            __debug=TRUE;
        }
        else{
            __debug=FALSE;
        }
        Ql_strcpy(&__settings.Debug[0],cmd);
	}
    cmd = Ql_strstr(command, "LOG=");
	if (cmd){
		cmd += 4;
		if (Ql_strstr(cmd, "TRUE")){
            __log=TRUE;
        }
        else{
            __log=FALSE;
        }
        Ql_strcpy(&__settings.Log[0],cmd);
	}
	cmd = Ql_strstr(command, "TCP=");
	if (cmd){
		cmd += 4;
        __settings.TCP=Ql_atoi(cmd);
	}
	cmd = Ql_strstr(command, "IP=");
	if (cmd){
		cmd += 3;
       
        pch = strtok(cmd, ".");
		while (pch != NULL)
		{
			__settings.IP[idx] = Ql_atoi(pch);   
			pch = strtok(NULL, ".");
			idx++;
		}
        //OUTD("Set IP:%d.%d.%d.%d\r\n",__settings.IP[0],__settings.IP[1],__settings.IP[2],__settings.IP[3]);
		
	}
	cmd = Ql_strstr(command, "APN=");
	if (cmd){
		cmd += 4;
		Ql_strcpy(&__settings.APN[0],cmd);
        
	}
	cmd = Ql_strstr(command, "PASS=");
	if (cmd){
		cmd += 5;
		Ql_strcpy(&__settings.Pass[0],cmd);
        
	}
    cmd = Ql_strstr(command, "NUM=");
	if (cmd){
		cmd += 4;
		__settings.Num=Ql_atoi(cmd);
	}
    /*cmd = Ql_strstr(command, "PWD=");
	if (cmd){
		cmd += 4;
		Ql_strcpy(&__settings.Pwd[0],cmd);
        
	}*/
     cmd = Ql_strstr(command, "USER=");
	if (cmd){
		cmd += 5;
		Ql_strcpy(&__settings.User[0],cmd);
        
	}
    cmd = Ql_strstr(command, "TIME=");
	if (cmd){
        char* last;
        QlSysTimer tm;
        Ql_GetLocalTime(&tm);
        cmd+=5;
        if (Ql_strlen(cmd)==8) {
        tm.hour = Ql_atoi(strtok_r(cmd, ":",&last));
		tm.minute = Ql_atoi(strtok_r(last, ":",&last));
        tm.second = Ql_atoi(strtok_r(last, ":",&last));
        Ql_SetLocalTime(&tm);
     }
	}
    cmd = Ql_strstr(command, "DATE=");
	if (cmd){
		char* last;
        QlSysTimer tm;
        Ql_GetLocalTime(&tm);
        cmd+=5;
        if (Ql_strlen(cmd)==8) {
        tm.day = Ql_atoi(strtok_r(cmd, ".",&last));
		tm.month = Ql_atoi(strtok_r(last, ".",&last));
        tm.year = Ql_atoi(strtok_r(last, ".",&last));
        Ql_SetLocalTime(&tm);
        }
	}
    cmd = Ql_strstr(command, "REBOOT");
	if (cmd){
        OUTD("!-----Get command to reboot..-----",NULL);
        Ql_Sleep(100);
		Ql_Reset(0);
	}

     cmd = Ql_strstr(command, "TSEND=");
	if (cmd){
        s32 tsend;
        cmd+=6;
        tsend=Ql_atoi(cmd);
        if (tsend>10 && tsend<60) {
            __settings.TSend = tsend;
        }
        
    }

    cmd = Ql_strstr(command, "DEBOUNCE=");
	if (cmd){
        s32 deboung;
        cmd+=9;
        deboung=Ql_atoi(cmd);
        //OUTD("set debounce ok:%d",deboung);
        if (deboung>=100 && deboung<=1000) {
            //OUTD("set debounce ok:%d",deboung);
            __settings.Deboung = deboung;
        }
        
    }
    cmd = Ql_strstr(command, "WUser=");
	if (cmd){
		cmd += 6;
		Ql_strcpy(&__settings.WUser[0],cmd);
	}
    cmd = Ql_strstr(command, "WPass=");
	if (cmd){
		cmd += 6;
		Ql_strcpy(&__settings.WPass[0],cmd);
	}
    cmd = Ql_strstr(command, "?");
	if (cmd){
        PrintSettings();
	}
}


void commandParce(){
    char* pch=NULL;
    char* last=NULL;
     __buf_command[__buf_command_pos]='\0';
     removeChar(__buf_command,'\r');
     pch=strtok_r(__buf_command," ",&last);
     while (pch!=NULL) {
         //Ql_DebugTrace("Text:%s\r\n",pch);
         setCommand(pch);
        pch=strtok_r(NULL," ",&last);
    }
     __buf_command_pos=0;
     WriteSettings();
     //PrintSettings();
}

void fillBuffer(char* ctxt){  
    if ((__buf_command_pos+Ql_strlen(ctxt))>BUFFER_COMMAND_LEN)
        __buf_command_pos=0;
    Ql_sprintf(&__buf_command[__buf_command_pos], "%s", ctxt);
     __buf_command_pos+=Ql_strlen(ctxt);
}

void removeChar(char *str, char garbage) {

	char *src, *dst;
	for (src = dst = str; *src != '\0'; src++) {
		*dst = *src;
		if (*dst != garbage) dst++;
	}
	*dst = '\0';
}

char* strtok_r(char *s, const char *delim, char **last)
{
	char *spanp;
	int c, sc;
	char *tok;

	if (s == NULL && (s = *last) == NULL)
	{
		return NULL;
	}
cont:
	c = *s++;
	for (spanp = (char *)delim; (sc = *spanp++) != 0;)
	{
		if (c == sc)
		{
			goto cont;
		}
	}

	if (c == 0)		
	{
		*last = NULL;
		return NULL;
	}
	tok = s - 1;
	for (;;)
	{
		c = *s++;
		spanp = (char *)delim;
		do
		{
			if ((sc = *spanp++) == c)
			{
				if (c == 0)
				{
					s = NULL;
				}
				else
				{
					char *w = s - 1;
					*w = '\0';
				}
				*last = s;
				return tok;
			}
		} while (sc != 0);
	}
}

char * strtok(char *s, const char *delim)
{
	static char *last;

	return strtok_r(s, delim, &last);
}
