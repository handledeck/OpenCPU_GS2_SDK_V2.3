#include "ql_memory.h"
#include "mgpio.h"
#include "ql_pin.h"
#include "global.h"
#include "ql_stdlib.h"
#include "events.h"
#include "files.h"
#include "transport.h"

u8  __pins=0;
u32 __debong=0;   

QlPinName __pinLow[]={
    QL_PINNAME_KBR0,
    QL_PINNAME_KBR1,
    QL_PINNAME_KBR2,
    QL_PINNAME_KBR3,
    QL_PINNAME_KBR4
};

void SetModeGpio(){    
    QlPinParameter pinLight;
    QlPinParameter pinHigh;
    s32 iret;
    pinLight.pinconfigversion = QL_PIN_VERSION;
     pinLight.pinparameterunion.gpioparameter.pinpullenable = QL_PINPULLENABLE_ENABLE;
     pinLight.pinparameterunion.gpioparameter.pindirection = QL_PINDIRECTION_OUT;
     iret=Ql_pinSubscribe(QL_PINNAME_GPIO4, QL_PINMODE_2, &pinLight); 
     pinHigh.pinconfigversion = QL_PIN_VERSION;
     pinHigh.pinparameterunion.gpioparameter.pinpullenable = QL_PINPULLENABLE_DISABLE;
     pinHigh.pinparameterunion.gpioparameter.pindirection = QL_PINDIRECTION_OUT;
     pinHigh.pinparameterunion.gpioparameter.pinlevel = QL_PINLEVEL_HIGH;
     //pinHigh.pinparameterunion.eintparameter.hardware_de_bounce=60;
     //pinHigh.pinparameterunion.eintparameter.software_de_bounce=500;
     iret=Ql_pinSubscribe(QL_PINNAME_KBC0, QL_PINMODE_2, &pinHigh);
     //OUTD("Subscribe QL_PINNAME_KBC0 to  gpio mode:%d\r\n",iret); 
     QlPinParameter pinLow;
     pinLow.pinconfigversion = QL_PIN_VERSION;
     pinLow.pinparameterunion.gpioparameter.pinpullenable = QL_PINPULLENABLE_ENABLE;
     pinLow.pinparameterunion.gpioparameter.pindirection = QL_PINDIRECTION_IN;
     pinLow.pinparameterunion.gpioparameter.pinlevel = QL_PINLEVEL_LOW;
     //pinLow.pinparameterunion.eintparameter.hardware_de_bounce=60;
     //pinLow.pinparameterunion.eintparameter.software_de_bounce=500;
     for (u8 i=0;i<sizeof(__pinLow);i++) {
        iret= Ql_pinSubscribe(__pinLow[i], QL_PINMODE_2, &pinLow);
       // OUTD("Subscribe pin to gpio:%d mode:%d\r\n",i,iret);
     }
     __pins=0;
     subscribe_light();
     ReadStateGpio();
   // OUTD("Subscribe pin to gpio mode:%d\r\n",iret);
}

void subscribe_light(){
    s32 clk,dat;
    QlPinParameter pinparameter;
    pinparameter.pinconfigversion = QL_PIN_VERSION;
    pinparameter.pinparameterunion.gpioparameter.pinpullenable = QL_PINPULLENABLE_ENABLE;
    pinparameter.pinparameterunion.gpioparameter.pindirection = QL_PINDIRECTION_OUT;
    pinparameter.pinparameterunion.gpioparameter.pinlevel = QL_PINLEVEL_LOW;
    clk = Ql_pinSubscribe(QL_PINNAME_SD_CLK, QL_PINMODE_2, &pinparameter);    
    dat = Ql_pinSubscribe(QL_PINNAME_SD_DATA, QL_PINMODE_2, &pinparameter);
    if (clk!=0 || dat!=0) {
        OUTD("Light pin CLK not subscribe sd_clk:%d sd_data:%d",clk,dat);
    }
    else OUTD("Light subscribe:OK",NULL);
}

void set_light_on(){
    s32 iret;
    iret = Ql_pinWrite(QL_PINNAME_SD_DATA, QL_PINLEVEL_HIGH);
    Ql_Sleep(10);
    iret = Ql_pinWrite(QL_PINNAME_SD_CLK, QL_PINLEVEL_HIGH);
    Ql_Sleep(10);
    iret = Ql_pinWrite(QL_PINNAME_SD_CLK, QL_PINLEVEL_LOW);
}
void set_light_off(){
    s32 iret;
    iret = Ql_pinWrite(QL_PINNAME_SD_DATA, QL_PINLEVEL_LOW);
    Ql_Sleep(10);
    iret = Ql_pinWrite(QL_PINNAME_SD_CLK, QL_PINLEVEL_HIGH);
    Ql_Sleep(10);
    iret = Ql_pinWrite(QL_PINNAME_SD_CLK, QL_PINLEVEL_LOW);
}

void ReadStateGpio(){ 
    QlPinLevel pinlevel;
    QlPinLevel pinLight;
   
    Ql_pinRead(QL_PINNAME_GPIO4, &pinLight);

    for (u8 i=0;i<sizeof(__pinLow);i++) {
        Ql_pinRead(__pinLow[i], &pinlevel);
        if (pinlevel==QL_PINLEVEL_HIGH) {
            __pins =(1<<i)|__pins;
            //OUTD("Subscribe pin to gpio mode:%d\r\n",pinlevel);      
        }   
    }   
}


void CheckStateGpio(){
    QlPinLevel pinlevel;
    unsigned char pin_state;
    unsigned char val;
    EventData locData;
   __debong+=DEVICE_CYCLE;
   if (__debong>=__settings.Deboung) {
       for (u8 i = 0; i < sizeof(__pinLow); i++) {
            pin_state=0;
            Ql_pinRead(__pinLow[i], &pinlevel);
            //OUTD("pin:%d val:%d\r\n",__pinLow[i],pinlevel);
            val=((1 << i) & __pins)>>i;
            if (val!=pinlevel) {
                OUTD(">Change pin nubmer:%d val:%d",__pinLow[i]-2,pinlevel,val); 
                 if (pinlevel==QL_PINLEVEL_HIGH){
                     __pins =(1<<i)|__pins;    
                 }
                  else
                    __pins =((1<<i)^0xFF)&__pins;
                GetInputDescripon(i,pinlevel,&pin_state);            
                TrigerEvent((TypeEvent)pin_state,0,&locData);
                WriteEvents(&locData);
                
                //__toSend=BuildEventMessage(__numPacket,0,&__currentEvData,1);     
            }
        }
       __debong=0;
   }
    //OUTD("pin value %d set.deb:%d loc.deb:%d\r\n",__pins,__settings.Deboung,__debong);
    if (__pins==31) {
        Ql_pinWrite(QL_PINNAME_GPIO4, QL_PINLEVEL_LOW);
        //OUTD("result set gpio-4 mode:%d light to low:%d\r\n",iret,light);
    }
    else{
        Ql_pinWrite(QL_PINNAME_GPIO4, QL_PINLEVEL_HIGH);
        //OUTD("result set gpio-4 mode:%d light to high:%d\r\n",iret,light);
    }

}

void GetTextStateGpio(char* pin){
   //QlPinLevel pinlevel;
   u8 i=0; 
   //Ql_memset(&pin[0],0,sizeof(pin));
    for (i=0;i<5;i++) {
        //Ql_pinRead(__pinLow[i], &pinlevel);
        if (__pins & (1<<i)){ //pinlevel==QL_PINLEVEL_HIGH) {
            pin[i]='1';
        }
        else pin[i]='0';

    }
   pin[5]='\0'; 
    //OUTD(">GPIO state:%s",&pin[0]);
    //return &pin[0];
}

void GetInputDescripon(unsigned char pin,char level,unsigned char* state){
    switch (pin) {
    case 0:
         *state=level==1 ? 2:1;
      break;
     case 1:
         *state=level==1 ? 4:3;
         break;
     case 2:
         *state=level==1 ? 6:5;
         break;
     case 3:
         *state=level==1 ? 8:7;
         break;
     case 4:
         *state=level==1 ? 10:9;
         break;
     case 5:
         *state=level==1 ? 12:11;
         break;
     case 6:
         *state=level==1 ? 14:13;
         break;
     case 7:
         *state=level==1 ? 16:15;
         break;
    }
}
