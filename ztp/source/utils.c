#include "ql_stdlib.h"
#include "utils.h"
#include "ql_trace.h"
#include  "global.h"
#include "ql_filesystem.h"
#include "ql_interface.h"


char __buf_command[BUFFER_COMMAND_LEN];
unsigned char __buf_command_pos;

void WriteSettings(void){
    s32 handle;
    u32 writed;
    s32 result;
    handle=Ql_FileOpenEx(SETTINGS_FILE,QL_FS_CREATE_ALWAYS);
    //handle=Ql_FileOpen(SETTINGS_FILE,TRUE,FALSE);
    if (handle<0) {
        OUTD("Can't open file settings for writing...:%d",handle);
        return;
    }
    else
    {
        result=Ql_FileWrite(handle,(u8*)&__settings,sizeof(settings),&writed);
        if (result!=0) {
            OUTD("Error write file settings:%d",result);
        }
        else{
            Ql_FileFlush(handle);
            OUTD("Write file settings is OK",NULL);
            Ql_FileClose(handle);
        }
    }
}

void SetDefaultSettins(void){
    __settings.IP[0]=127;
    __settings.IP[1]=0;
    __settings.IP[2]=0;
    __settings.IP[3]=1;
    __settings.TCP=10245;
    __settings.Num=1;
    __settings.TSend=DEVICE_CONTROL_EMPTY;
    Ql_memcpy(__settings.Debug,"FALSE",5);
    Ql_memcpy(__settings.Pass,"undefine",8);
    Ql_memcpy(__settings.Pwd,"undefine",8);
    Ql_memcpy(__settings.APN,"vpn2.mts.by",11);
    Ql_memcpy(__settings.User,"vpn",3);  
}

void ReadSettings(void){
    s32 result;
    u32 readed;
    s32 handle;
    result=Ql_FileCheck(SETTINGS_FILE);
    if (result!=0) {
        OUTD("File settings not found. Create new file:%d",result);
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
                OUTD("Error read file settings.Error:%d",result);
            }
            OUTD("Read file settings OK. Read:%d bytes",readed);
            Ql_FileClose(handle);
         //PrintSettings();
        }
}

void PrintSettings(){
    QlSysTimer loct;
    ReadSettings();
    Ql_GetLocalTime(&loct);
        OUTD("APN=%s IP=%d.%d.%d.%d TCP=%d USER=%s PASS=%s DEBUG=%s NUM=%d PWD=%s TSEND=%d DATE=%.2d.%.2d.%.2d TIME=%.2d:%.2d:%.2d\r\n",
                      __settings.APN,__settings.IP[0],__settings.IP[1],__settings.IP[2],__settings.IP[3],
                      __settings.TCP,__settings.User,__settings.Pass,__settings.Debug,__settings.Num,__settings.Pwd,__settings.TSend,
                     loct.day,loct.month,loct.year,loct.hour,loct.minute,loct.second);
}


void setCommand(char* command){
    char* cmd = NULL;
    char idx=0;
    char* pch=NULL;
	cmd = Ql_strstr(command, "DEBUG=");
	if (cmd){
		cmd += 6;
		if (Ql_strstr(cmd, "TRUE")){
			//Ql_DebugTrace("Set DEBUG:TRUE\r\n",NULL);
            __debug=TRUE;
        }
        else{
            //Ql_DebugTrace("Set DEBUG:FALSE\r\n",NULL);
            __debug=FALSE;
        }
        Ql_strcpy(&__settings.Debug[0],cmd);
	}
	cmd = Ql_strstr(command, "TCP=");
	if (cmd){
		cmd += 4;
        __settings.TCP=Ql_atoi(cmd);
		//Ql_DebugTrace("Set TCP:%d\r\n",__settings.TCP);
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
        OUTD("Set IP:%d.%d.%d.%d\r\n",__settings.IP[0],__settings.IP[1],__settings.IP[2],__settings.IP[3]);
		
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
    cmd = Ql_strstr(command, "PWD=");
	if (cmd){
		cmd += 4;
		Ql_strcpy(&__settings.Pwd[0],cmd);
        
	}
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
        Ql_DebugTrace("!!!!-----Get command to reboot..-----!!!!\r\n",NULL);
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
