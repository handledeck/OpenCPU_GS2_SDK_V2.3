#ifndef __UTILS_H__
#define __UTILS_H__

extern char __buf_command[];
extern unsigned char __buf_command_pos;



#define BUFFER_COMMAND_LEN 100
#define SETTINGS_FILE "settings.dat"

void removeChar(char *str, char garbage); 
char* strtok_r(char *s, const char *delim, char **last);
char * strtok(char *s, const char *delim);
void commandParce(void);
void fillBuffer(char* ctxt);
void WriteSettings(void);
void ReadSettings(void);
void PrintSettings(void);
void SetDefaultSettins(void);
void setCommand(char* command);

#endif 
