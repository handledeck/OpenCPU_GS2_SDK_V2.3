#include "socket.h"

#ifndef __WEBSRV_H__
#define __WEBSRV_H__



extern char* __error_page;
extern char* __ok_page;
extern char* __main_page;
extern char* __download_pagmkae;
extern char __s_buf[];


typedef enum{
	GET,
	POST
}Method;

typedef struct {
	Method method;
	u32  length;
	char action[100];
	char boundary[200];
	char cookie[200];
	unsigned char data[10 * 1024];
	bool authorization;
	bool set_cookie;
}HttpHeader;

void get_main_page(const char* message);
void get_error_page(void);
void get_rebot_page(void);
/*bool read_action(char* header, char* action);*/
void page_response(HttpHeader *http_header);
/*bool read_request(HttpHeader* header);*/
bool SetLocDate(char* line);
bool parse_header(void *clients,HttpHeader *http_header);
bool vilidation(HttpHeader *http_header);
/*s32 decode_b64(const unsigned char *buftodec,unsigned char *decbuf);
s32 encode_b64(const unsigned char *buftoenc,unsigned char *encbuf);*/

#endif 
