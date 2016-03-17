#include "Ql_stdlib.h"
#include "global.h"
#include "utils.h"
#include "transport.h"
#include "ql_interface.h"
#include "Ql_tcpip.h"
#include "mgpio.h"
#include "files.h"
#include "socket.h"

char* __error_page="HTTP/1.1 404 Not Found\nContent-Type: text/html\nContent-Length: 0\n\n";
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

bool read_action(char* header,char* action) {
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
        OUTD("recive simple bytes stream",NULL);
    }
}

void page_response(char* action){
	bool reset=FALSE;
	bool prepare=TRUE;
	Ql_memset(__data_buf,0,2048);
	Ql_memset(__s_buf,0,10240);
	if (Ql_strstr(action,"/")) {
		get_main_page( __est_connection ? "System state is start":"System state stop");
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
     else {
		get_error_page();
		Ql_sprintf(__s_buf,__error_page,0);
		prepare=FALSE;
	}
	if (prepare) {
		Ql_sprintf(__s_buf,__ok_page,Ql_strlen(__data_buf));
		Ql_strcpy(&__s_buf[Ql_strlen(__s_buf)],__data_buf);
	}
	
	//len_send = Ql_SocketSend(socket, (u8 *)__s_buf,Ql_strlen(__s_buf)); 
	//s32 index=findClientBySockid(socket);
	//OUTD("send text:%s",__s_buf);
	//func_send_handle(socket,__s_buf);
	/*OUTD("need to send:%d.", len_buf_send);
	s32 len_send=512;
	
    while (len_send != 0) {
        if (len_send>0) {
        
        if (len_send >= 512) 
			len_send = Ql_SocketSend(socket,(u8*)__s_buf,512); 
        else
			len_send = Ql_SocketSend(socket, (u8 *)__s_buf,len_send); 
			OUTD("we send:%d.", len_send);
			len_send = len_buf_send - len_send; 
			OUTD("we need to send:%d.", len_send);
		}
		else
		{
			OUTD("send is negotive:%d.", len_send);
			Ql_Sleep(100);
		}
	}*/

	if (reset) {
		Ql_Sleep(10000);
		Ql_Reset(0);
	}
}



bool read_request(char* header){
	//char r_buf[1024];
	char action[100]="";
    
	//char head[100]="";
	//Ql_memcpy(head,header,100);
    //s32 len;
	
    //OUTD("Soccket read:%d",socket);
    //len=Ql_SocketRecv(socket,(u8*)&r_buf[0],1024);
	//if (len>0) {
        //r_buf[len - 1] = '\0'; 
        //OUTD("recive %d text:%s",len,&r_buf);
			if(read_action(header,action))
            {
                OUTD("read action:%s",action);
                page_response(action); 
                return TRUE;
            }
			else return FALSE;
	//}
}








