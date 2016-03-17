#include "Ql_stdlib.h"
#include "global.h"
#include "utils.h"
#include "transport.h"
#include "ql_interface.h"
#include "Ql_tcpip.h"
#include "mgpio.h"
#include "files.h"
#include "socket.h"

const char Base64Table[65] ="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
char* __error_page="HTTP/1.1 404 Not Found\nContent-Type: text/html\nContent-Length: 0\n\n";
char* __autorize="HTTP 401 Not Authorized\r\nWWW-Authenticate: Basic realm=\"insert realm\"\r\nContent-Length: %d\r\n\r\n";
char* __ok_page="HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: %d\r\n\r\n";
char* __download_page="HTTP/1.1 200 OK\r\ncontent-description: File Transfer\r\ncontent-type: application/octet-stream\r\ncontent-disposition: attachment; filename=log.txt\r\nContent-Length: %d\r\n\r\n";
char* __main_page="<html><head><style>td{border:none;font-family:Tahoma;}.al{text-align:right;}</style></head>"
					"<body>"
						"<div style='padding-top:100px' align='center'>"
							"<h1>Controller IP address %s</h1>"
							"<form action='settings'>"
							"<table style='width:800px;border-collapse:collapse'>"
                                "<tr><td>Signal strength, unit in dBm</td><td class='al'>%d</td></tr>"
								"<tr><td>Output state</td><td class='al'>%s</td></tr>"
								"<tr><td>Date Time</></td><td class='al'>%.2d.%.2d.%.2d %.2d:%.2d:%.2d</td></tr>"
								"<tr><td>APN</td><td class='al'>%s</td></tr>"
								"<tr><td>Address to connect</td><td class='al'><input type='text' name='IP' pattern=\"((^|\\.)((25[0-5])|(2[0-4]\\d)|(1\\d\\d)|([1-9]?\\d))){4}$\" value='%d.%d.%d.%d' required/></td></tr>"
								"<tr><td>Port to connect</td><td class='al'><input type='number' name='TCP' min='1025' max='65535' value='%d' required/></td></tr>"
								"<tr><td>Time to send empty packet</td><td class='al'><input type='number'  min='30' max='55' step='5' value='%d' name='TSEND' required/></td></tr>"
								"<tr><td>DEBOUNCE</td><td class='al'><input type='number' min='100' max='1000' step='50' value='%d' name='DEBOUNCE' required/></td></tr>"
								"<tr><td>DEBUG</td><td class='al'><select name='DEBUG'>%s</select></td></tr>"
								"<tr><td>LOG</td><td class='al'><select name='LOG'>%s</select></td></tr>"
								"<tr><td style='border-bottom:double'>&nbsp</td><td style='border-bottom:double'>&nbsp</td></tr>"
								"<tr><td>&nbsp</td><td>&nbsp</td></tr>" 
								"<tr><td>&nbsp</td><td class='al'><code>%s</code></td></tr>"    
								"<tr><td>&nbsp</td><td>&nbsp</td></tr>" 
								"<tr><td><input type='submit'  value='Save settings' />\r\n"
								"<input type='button' value='Set Date time' onclick=\"javascript:var d=new Date();location.href='date?'+(d.getFullYear()-2000)+'-'+(d.getMonth()+1)+'-'+d.getDate()+'T'+d.getHours()+':'+d.getMinutes()+':'+d.getSeconds()\" />\r\n"
								"<input type='button' value='%s' onclick=\"javascript:location.href='%s'\" />\r\n"
								"<input type='button' value='Get Log' onclick=\"javascript:location.href='log'\" />\r\n"
								"<input type='button' value='Refresh' onclick=\"javascript:location.href='/'\" />\r\n"
								"</td>"
								"<td align='right'>"
								"<input type='button' value='Reboot' onclick=\"javascript: location.href = 'reboot'\" />"
								"</td>"
								"</tr>"
							"</table>"
							"</form>"
						"</div>"
					"</body></html>\0";	
char* __reboot_page="<html><body><div style='padding-top:100px' align='center'><h1>Controller ip:%s reboot</h1></div></body></html>";
char __data_buf[4096]="";
char __s_buf[DATA_LEN]="";

bool SetLocDate(char* line) {
	char* pch;
	char* last;
	int inx = 0;
	QlSysTimer ql;
	pch = strtok_r(line, "T", &last);
	pch = strtok(pch, "-");
	while (pch != NULL)
	{
		if (inx == 0)
			ql.year = Ql_atoi(pch);
		if (inx == 1)
			ql.month = Ql_atoi(pch);
		if (inx == 2)
			ql.day = Ql_atoi(pch);
		pch = strtok(NULL, "-");
		inx++;
	}
	inx = 0;
	pch = strtok(last, ":");
	while (pch != NULL)
	{
		if (inx == 0)
			ql.hour = Ql_atoi(pch);
		if (inx == 1)
			ql.minute = Ql_atoi(pch);
		if (inx == 2)
			ql.second = Ql_atoi(pch);
		pch = strtok(NULL, ":");
		inx++;
	}
	bool ret= Ql_SetLocalTime(&ql);
	return ret;
}

void get_error_page(void){

}

void get_rebot_page(void){
	Ql_sprintf(__data_buf,__reboot_page,__str_loc_ip_addr);
}

void get_main_page(const char* message){
	 char pp[8];
	QlSysTimer loct;
    s32 simcard;
        s32 creg=0;
        s32 cgreg=0;
        u8 rssi=0;
        u8 ber=0;
        Ql_GetDeviceCurrentRunState(&simcard, &creg, &cgreg, &rssi, &ber);
	 Ql_GetLocalTime(&loct);
	 GetTextStateGpio(&pp[0]);
	Ql_sprintf(__data_buf,__main_page,
			   __str_loc_ip_addr,
                rssi,
			   &pp[0],
			   loct.day,loct.month,loct.year,loct.hour,loct.minute,loct.second,
               __settings.APN,
               __settings.IP[0],__settings.IP[1],__settings.IP[2],__settings.IP[3],
               __settings.TCP,
			   __settings.TSend,
			   __settings.Deboung,
			   __debug ? "<option selected>TRUE</option><option>FALSE</option>":"<option>TRUE</option><option selected>FALSE</option>",
			   __log ? "<option selected>TRUE</option><option>FALSE</option>":"<option>TRUE</option><option selected>FALSE</option>",
			    message,
			   __est_connection ? "Stop":"Start",
			   __est_connection ? "stop":"run"
               );
}

/*bool read_action(char* header,char* action) {
	char* index = Ql_strstr(header, "GET ");
	if (index != NULL)
	{
		if (Ql_strstr(index, "GET /"))
		{
			char* in = Ql_strstr(index, "HTTP");
			int len_action = in - (index + 5);
			if (len_action > 1) {
				Ql_strncat(action, index + 5, len_action - 1);
			}
			else
			  Ql_strncat(action, "/", 1);
		}
        return TRUE;
	}
    else{
        return FALSE;
    }
}*/

bool vilidation(HttpHeader *http_header){
    OUTD("cookie value is:%s",http_header->cookie);
    if (Ql_strstr(http_header->cookie,"NONE")) 
    {
        return FALSE;
    }
    else return TRUE;
}

void page_response(HttpHeader *header){
	bool reset=FALSE;
	bool prepare=TRUE;
    char* action=header->action;
    OUTD("action=%s",action);
	Ql_memset(__data_buf,0,4096);
	Ql_memset(__s_buf,0,10240);
    
    if (Ql_strstr(header->action, "/")) {
		get_main_page( __est_connection ? "System state is start":"System state stop");
        OUTD("main page",NULL);
	}
	else if (Ql_strstr(action,"reboot")) {
		get_rebot_page();
		reset=TRUE;
	}
	else if (Ql_strstr(action,"settings?")) {
		char* pch=NULL;
		char* last=NULL;
		//OUTD("parse setting action:%s",pch);
		pch=strtok_r(action+9,"&",&last);
	  //OUTD("parse command:%s",pch);
     while (pch!=NULL) {
        setCommand(pch);
        pch=strtok_r(NULL,"&",&last);
		//OUTD("parse command:%s",pch);
    }
	
     WriteSettings();
	 ReadSettings();
	 get_main_page("Save settings controller is OK");
	}
	else if (Ql_strstr(action,"date?")) {
		bool sdt=SetLocDate(action+5);
		get_main_page(sdt ? "Set date and time is OK":"Error set date and time");
	}
	else if (Ql_strstr(action,"stop")) {
		StopEstConnection();
		get_main_page("Recive command Stop");
	}
	else if (Ql_strstr(action,"run")) {
		nSTATE=STATE_EST_SOCKET_CREATE;
		get_main_page("Recive command Start");
	}
	else if (Ql_strstr(action,"log")) {
		u16 size=0;
		ReadLogFile(&size);
		Ql_sprintf(__s_buf,__download_page,size);
		//Ql_strcpy(&__s_buf[Ql_strlen(__s_buf)],__buf_log);
		
		Ql_memcpy(&__s_buf[Ql_strlen(__s_buf)],__buf_log,size);
		OUTD("__download_page:%d file size:%d buffer-len:%d",Ql_strlen(__download_page),size,Ql_strlen(__s_buf));
		
		prepare=FALSE;
	}
    else if (Ql_strstr(action, "autorize")) {
        Ql_sprintf(__s_buf,__autorize,0);
        prepare=FALSE;
        OUTD("set autorize",NULL);
    }
    else {
		get_error_page();
		Ql_sprintf(__s_buf,__error_page,0);
		prepare=FALSE;
	}
	if (prepare) {
        //if (header->set_cookie) {
          //   Ql_sprintf(__s_buf, __ok_page, Ql_strlen(__data_buf),"\r\nSet-Cookie: __ztp=987654321\r\n"); 
        //}
        Ql_sprintf(__s_buf, __ok_page, Ql_strlen(__data_buf)); 
		Ql_strcpy(&__s_buf[Ql_strlen(__s_buf)],__data_buf);
        OUTD("sbuf:%d dbuf:%d",Ql_strlen(__s_buf),Ql_strlen(__data_buf));
        
	}
	if (reset) {
		Ql_Sleep(10000);
		Ql_Reset(0);
	}
}



/*bool read_request(HttpHeader* header){
	char action[100]="";
    if(read_action(header->action))
        {
            OUTD("read action:%s",action);
            page_response(header); 
            return TRUE;
        }
	else return FALSE;
}*/

bool parse_header(void *clients,HttpHeader *http_header){
	char *param;
	char *value;
	char *last;
	char *ph;
	char* bon_val = NULL;
	char header[1024 * 2];
    char*lens=NULL;
    QlClient *client=(QlClient*)clients;
	char* head = Ql_strstr((char*)client->recvBuffer, "\r\n\r\n");
	if (head) {
       
        http_header->set_cookie=FALSE;
        http_header->authorization=FALSE;
        Ql_memset(http_header->action,0,100);
        Ql_memset(http_header->boundary,0,200);
        Ql_memset(http_header->cookie,0,200);
        Ql_memset(http_header->data,0,10 * 1024);
        http_header->length=0;
         Ql_strcpy(http_header->cookie, "NONE"); 
		int head_len = (char*)head - ((char*)client->recvBuffer);
		int body_len = (int)(client->pRecvCurrentPos) - head_len - 4;
		Ql_memcpy(header, client->recvBuffer, head_len);
		ph = strtok_r(header, "\r\n", &last);
		char* lst;
		if (Ql_strstr(ph, "GET")!=NULL)
		{
			char* act = strtok_r(ph+4, " ", &lst);
			if (Ql_strlen(act)>1) 
                Ql_strcpy(http_header->action, act+1); 
            else
                Ql_strcpy(http_header->action, act); 
			http_header->method = GET;
            OUTD("action:%s",http_header->action);
		}
		if (Ql_strstr(ph, "POST")!=NULL) {
			http_header->method = POST;
			char* act = strtok_r(ph+5, " ", &lst);
            if (Ql_strlen(act)>1) 
                Ql_strcpy(http_header->action, act+1); 
            else
                Ql_strcpy(http_header->action, act); 
		}
		while (1)
		{
			ph = strtok_r(NULL, "\r", &last);
			if (ph == NULL) {
				return TRUE;
			}
			param = strtok_r(ph + 1, ":", &value);
			if (Ql_strstr(param, "Content-Type")) {
				char* bon = Ql_strstr(value, "boundary");
				bon_val = bon + 9;
				Ql_strcpy(http_header->boundary, bon_val);
			}
			else if ((lens = Ql_strstr(param, "Cookie"))!=NULL)
			{
                char* cookie;
                if ((cookie=Ql_strstr(value,"__ztp"))!=NULL) {
                    Ql_strcpy(http_header->cookie, cookie+6); 
                    http_header->authorization=TRUE;
                    http_header->set_cookie=FALSE;
                }
			}
			else if ((lens = Ql_strstr(param, "Content-Length"))!=NULL) {
				char* slen = lens + 16;
				int len_head = Ql_atoi(slen);
				http_header->length = len_head;
				if (len_head == body_len){
					OUTD("data full",NULL);
                }
                else
                {
                    OUTD("not full data.wait:%d have:%d", len_head, body_len);
                    return FALSE;
                }
              }
            else if ((lens = Ql_strstr(param, "Authorization"))!=NULL) {
                /*char* slen = lens + 21;
                unsigned char dbuf[50];
                Ql_memset(dbuf,0,50);
                decode_b64((unsigned char*)slen,dbuf);
                OUTD("decode buf:%s",dbuf);
                param = strtok_r((char*)dbuf, ":", &value);
                if (Ql_strstr(param,"admin") && Ql_strstr(value,"admin")) {
                    OUTD("Autorization user:%s pswd:%s",param,value);
                    http_header->authorization=TRUE;
                    http_header->set_cookie=TRUE;
                }*/
            }
        }
		}
	else
	{
        return FALSE;
	}
}

bool get_http_data(QlClient *client,HttpHeader *header) {
	char* fota = Ql_strstr((char*)client->recvBuffer, "QUECTEL_FOTA_UPGRADE");
	if (fota) {
		unsigned char* end = &client->recvBuffer[client->pRecvCurrentPos - (Ql_strlen(header->boundary) + 8)];
		if (Ql_strstr((char*)end, header->boundary)) {
			int lens = (char*)end - fota;
			Ql_memcpy(header->data, fota, lens);
		}
        return TRUE;
	}
    else
        return FALSE;
	
}


/*
s32 encode_b64(const unsigned char *buftoenc,unsigned char *encbuf)
{
	// выделяем память под временный буфер
	//unsigned char *buftemp = (unsigned char*)Ql_GetMemory(bufsize + 3);
    u8 bufsize=50;
    unsigned char buftemp[50];
	Ql_memset(buftemp, '\0', bufsize + 3);
	Ql_memcpy(buftemp, buftoenc, bufsize);

	s32 i = 0;
	s32 b64byte[5];

	while (i < bufsize)
	{
		b64byte[0] = buftemp[i] >> 2;
		b64byte[1] = ((buftemp[i] & 3) << 4) | (buftemp[i + 1] >> 4);
		b64byte[2] = ((buftemp[i + 1] & 0x0F) << 2) | (buftemp[i + 2] >> 6);
		b64byte[3] = buftemp[i + 2] & 0x3F;
		if (b64byte[0] == 0)
			encbuf[i + (i / 3)] = '=';
		else
			encbuf[i + (i / 3)] = Base64Table[b64byte[0]];

		if (b64byte[1] == 0)
			encbuf[i + (i / 3) + 1] = '=';
		else
			encbuf[i + (i / 3) + 1] = Base64Table[b64byte[1]];

		if (b64byte[2] == 0)
			encbuf[i + (i / 3) + 2] = '=';
		else
			encbuf[i + (i / 3) + 2] = Base64Table[b64byte[2]];

		if (b64byte[3] == 0)
			encbuf[i + (i / 3) + 3] = '=';
		else
			encbuf[i + (i / 3) + 3] = Base64Table[b64byte[3]];
		i += 3;
	}

	//Ql_FreeMemory(buftemp);
	return Ql_strlen((char*)encbuf);
}

s32 decode_b64(const unsigned char *buftodec,unsigned char *decbuf)
{
	// выделяем память под временный буфер
	//unsigned char *buftemp =(unsigned char*)Ql_GetMemory(bufsize);
    u8 bufsize=50;
    unsigned char buftemp[50];
	Ql_memset(buftemp, '\0', bufsize);
	Ql_memcpy(buftemp, buftodec, bufsize);

	int i = 0;
	int cpos[5];
	unsigned char binbyte[4];

	while (i < bufsize)
	{
		if (buftemp[i] == '=')
			cpos[0] = 0;
		else
			cpos[0] = Ql_strchr(Base64Table, buftemp[i]) - Base64Table;

		if (buftemp[i + 1] == '=')
			cpos[1] = 0;
		else
			cpos[1] = Ql_strchr(Base64Table, buftemp[i + 1]) - Base64Table;

		if (buftemp[i + 2] == '=')
			cpos[2] = 0;
		else
			cpos[2] = Ql_strchr(Base64Table, buftemp[i + 2]) - Base64Table;

		if (buftemp[i + 3] == '=')
			cpos[3] = 0;
		else
			cpos[3] = Ql_strchr(Base64Table, buftemp[i + 3]) - Base64Table;

		binbyte[0] = ((cpos[0] << 2) | (cpos[1] >> 4));
		binbyte[1] = ((cpos[1] << 4) | (cpos[2] >> 2));
		binbyte[2] = (((cpos[2] & 0x03) << 6) | (cpos[3] & 0x3f));
		decbuf[i - (i / 4)] = binbyte[0];
		decbuf[i - (i / 4) + 1] = binbyte[1];
		decbuf[i - (i / 4) + 2] = binbyte[2];
		i += 4;
	}

	//Ql_FreeMemory(buftemp);
	return Ql_strlen((char*)decbuf);
}*/







