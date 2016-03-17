#include "ql_filesystem.h"
#include "global.h"
#include "store.h"
#include "ql_stdlib.h"

void WriteSettings(settings* sf){
    s32 handle;
    u32 writed;
    
    Ql_FileDelete(FILE_SETTINGS);
    handle = Ql_FileOpenEx(FILE_SETTINGS, QL_FS_CREATE); 
    if (handle>0) {
        
        //OUTD("open file for write settings OK\r\n",NULL);
        Ql_FileWrite(handle,(u8*)sf,sizeof(settings),&writed);
        //OUTD("result write settings:%d\r\n",result);
        Ql_FileClose(handle);
        //OUTD("Try APN:%s Login:%s Pass:%s\r\n",sf->APN,sf->User,sf->Pass);
         /*Ql_DebugTrace("APN=%s IP=%d.%d.%d.%d TCP=%d USER=%s PASS=%s DEBUG=%d NUM=%d PWD=%s\r\n",
                      sf->APN,sf->IP[0],sf->IP[1],sf->IP[2],sf->IP[3],
                      sf->TCP,sf->User,sf->Pass,sf->Debug,sf->Num,sf->Pwd);*/
                     
    }
    else OUTD("open file for write settings Error:%d",handle);
}

void ReadSettings(settings* sf){
    s32 handle;
    u32 readed;
    //Ql_FileGetSize(FILE_SETTINGS,&readed);
    //OUTD("try open file %s.File size:%d\r\n",FILE_SETTINGS,readed);
    handle = Ql_FileOpenEx(FILE_SETTINGS, QL_FS_READ_ONLY); 
    if (handle>0) {
        //OUTD("open file for read settings OK\r\n",NULL);
        Ql_memset(sf,0,sizeof(settings));
        Ql_FileRead(handle,(u8*)sf,readed,&readed);
        //OUTD("result read settings:%d\r\n",result);
        Ql_FileClose(handle);
        /*Ql_DebugTrace("APN=%s IP=%d.%d.%d.%d TCP=%d USER=%s PASS=%s DEBUG=%d NUM=%d PWD=%s\r\n",
                      sf->APN,sf->IP[0],sf->IP[1],sf->IP[2],sf->IP[3],
                      sf->TCP,sf->User,sf->Pass,sf->Debug,sf->Num,sf->Pwd);*/
    }
    else OUTD("open file for read settings Error:%d",handle);
}
