#include "ql_filesystem.h"
#include "stdlib.h"
#include "ql_stdlib.h"
#include "files.h"
#include "ql_error.h"
#include "ql_trace.h"
#include "events.h"
#include "global.h"


u8* __file_events="events.dat";
u16 __confirmRecive=0;
EventData __bufEventData[COUNT_EVENTS];

void WriteEvents(EventData* evdata){
       s32 handle;
    u32 filesize=0;
    u32 readed=0;
    
    //определяем размер файла хранилища событий 
    Ql_FileGetSize(__file_events,&filesize);
    //открыв. файл
    handle = Ql_FileOpenEx(__file_events, (QL_FS_READ_WRITE | QL_FS_CREATE)); 
    if (handle>0) {
        if (__confirmRecive>0) {
            __confirmRecive--;
        }
        if (filesize >= sizeof(EventData) * COUNT_EVENTS) {
          //Ql_DebugTrace("file size:%d.Bufer:%d\r\n",filesize,(sizeof(EventData)*COUNT_EVENTS));
           Ql_FileSeek(handle,sizeof(EventData),QL_FS_FILE_BEGIN);
          //Ql_FileGetFilePosition(handle,&poss);
          //Ql_DebugTrace("set file position:%d\r\n",poss);
          Ql_FileRead(handle,(u8*)&__bufEventData[0],(COUNT_EVENTS-1)*sizeof(EventData),&readed);
          //Ql_DebugTrace("read from file:%d\r\n",readed);
          //Ql_FileTruncate(handle);
          //Ql_FileGetSize(_events,&filesize);
          //Ql_DebugTrace("tranc file size:%d\r\n",filesize);
          Ql_FileSeek(handle,0,QL_FS_FILE_BEGIN);
          Ql_FileWrite(handle,(u8*)&__bufEventData[0],(COUNT_EVENTS-1)*sizeof(EventData),&readed);
          //OUTD("write to file:%d\r\n",readed);
          Ql_FileWrite(handle,(u8*)evdata,sizeof(EventData),&readed);        
       }
       else{

            Ql_FileSeek(handle,filesize,QL_FS_FILE_BEGIN);
            Ql_FileWrite(handle,(u8*)&evdata[0],sizeof(EventData),&readed);
            Ql_FileClose(handle);
       }
       Ql_FileClose(handle);
    }
    else
        OUTD("!Can't open file error:%d",handle);
}

void PrintMessage(){
   s32 handle;
    u32 readed=0;
     u32 filesize=0;
     Ql_FileGetSize(__file_events,&filesize);
    handle = Ql_FileOpenEx(__file_events, (QL_FS_READ_ONLY)); 
    if (handle>0) {
        Ql_FileRead(handle,(u8*)&__bufEventData[0],filesize,&readed);
        Ql_FileClose(handle);
        for (int i=0;i<(filesize/sizeof(EventData));i++) {
            OUTD("read from file:%d %d %d %d %d %d", __bufEventData[i].TimeEvent[0],
                      __bufEventData[i].TimeEvent[1],__bufEventData[i].TimeEvent[2],
                      __bufEventData[i].TimeEvent[3],__bufEventData[i].TimeEvent[4],__bufEventData[i].TimeEvent[5]);
        }   
    }
}

s8 ReadEvents(void){
    u32 filesize=0;
    u32 readed=0;
    s32 handle;
    Ql_FileGetSize(__file_events,&filesize);
   // if (error!=0) {
     //   OUTD("operation get file size:%d var size:%d", error, filesize); 
   // }
    __confirmRecive=filesize/sizeof(EventData);
   //OUTD("Count to send message is:%d\r\n",__confirmRecive);
   if (__confirmRecive>0) {
   handle = Ql_FileOpenEx(__file_events, (QL_FS_READ_WRITE)); 
    if (handle>0) {
        Ql_FileRead(handle,(u8*)&__bufEventData[0],filesize,&readed);
        Ql_FileClose(handle);
        __toSend=BuildEventMessage(1,&__bufEventData[0],__confirmRecive);       
    }
    else{
        OUTD("File not present:%d",handle);
     __toSend=0;
    }
   }
   else{
        __toSend=0;
   }
   return NULL;
}

void ConfirmMsg(){
    s32 handle;
    u32 filesize=0;
    u32 readed=0;
    u32 poss=0;
    //определяем размер файла хранилища событий 
   Ql_FileGetSize(__file_events,&filesize);
       // if(error!=0)
         //   OUTD("operation get size:%d filesize:%d\r\n",filesize,error);
            if (filesize==0) {
                //OUTD("File empty.Nothing confirm",NULL);
                return;
         }
     //открыв. файл
    handle = Ql_FileOpenEx(__file_events, (QL_FS_READ_WRITE)); 
    if (handle>0) {
         
          poss=__confirmRecive*sizeof(EventData);
          //OUTD("concat from position:%d\r\n", poss); 
          Ql_FileSeek(handle,poss,QL_FS_FILE_BEGIN);
          //OUTD("read from position:%d\r\n",filesize-poss);
          Ql_FileRead(handle,(u8*)&__bufEventData[0],filesize-poss,&readed);
          //OUTD("read from file:%d\r\n",readed);
          //Ql_FileTruncate(handle);
          Ql_FileClose(handle);
          Ql_FileDelete(__file_events);
          //Ql_FileGetSize(__file_events,&filesize);
          //Ql_DebugTrace("close file.Size:%d\r\n",filesize);
          handle = Ql_FileOpenEx(__file_events, (QL_FS_READ_WRITE |QL_FS_CREATE)); 
          if (handle>0) {
              //OUTD("reopen file\r\n",NULL);
              Ql_FileSeek(handle,0,QL_FS_FILE_BEGIN);
              Ql_FileWrite(handle,(u8*)&__bufEventData[0],filesize-poss,&readed);
              Ql_FileClose(handle);
          }
    }
    else
        OUTD("!Can't open file error:%d",handle);
}

bool CheckFiles(void){
   
        Ql_FileDelete(__file_events);
   
        OUTD("Delete file true",NULL);
        return TRUE;
  
}
