#ifndef __WEBSRV_H__
#define __WEBSRV_H__

extern char* __error_page;
extern char* __ok_page;
extern char* __main_page;
extern char* __download_pagmkae;

extern char __s_buf[];

void get_main_page(const char* message);
void get_error_page(void);
void get_rebot_page(void);
bool read_action(char* header, char* action);
void page_response(char* action);
bool read_request(char* header);
bool SetLocDate(char* line);
#endif 
