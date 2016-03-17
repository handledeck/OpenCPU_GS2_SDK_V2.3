
#-------------------------------------------------------------------------------
# config tools, tool path
#-------------------------------------------------------------------------------
ARMTOOL_PATH=C:\Progra~1\ARM\RVCT


#-------------------------------------------------------------------------------
# config version
#-------------------------------------------------------------------------------
PLATFORM=APPGS2MD
MEMORY=M64
VERSION=A01


#-------------------------------------------------------------------------------
# config build dir
#-------------------------------------------------------------------------------
BUILD_DIRS=customer example

#-------------------------------------------------------------------------------
# config link libs
#-------------------------------------------------------------------------------
#LIBS=customerlib\example_customerlib.a
LIBS=


all:
	@$(MAKE) help

help:	
	@echo **********************help*******************
	@echo  please you input following command to make example
	@echo  make newAPI
	@echo  make usb
	@echo  make LCD_S6B0724
	@echo  make powerkey
	@echo  make helloworld
	@echo  make custom
	@echo  make at
	@echo  make atc_pipe
	@echo  make gpio
	@echo  make eint
	@echo  make memory
	@echo  make pwm
	@echo  make clk
	@echo  make alert
	@echo  make keyboard
	@echo  make lcd
	@echo  make adc
	@echo  make i2c
	@echo  make file
	@echo  make file_sd
	@echo  make audio
	@echo  make record
	@echo  make transpass
	@echo  make tcpip
	@echo  make multitask
	@echo  make multitask_port
	@echo  make sms
	@echo  make call
	@echo  make headset
	@echo  make lcm_st7545t
	@echo  make lcm_uc1701
	@echo  make tcplong
	@echo  make multimemory
	@echo  make lowrateuart
	@echo  make eintcallback
	@echo  make fota
	@echo  make fota_http
	@echo  make tts
	@echo  make tcpserver
	@echo  make wrtcoderegion
	@echo  make time
	@echo  make system
	@echo  make timer
	@echo  make testgpio
	@echo  make fota_ftp
	@echo  make alarm
#	@echo  make ST_appupdate
#	@echo  make ST_wrtcoderegion
#	@echo  make ST_http2fota	
#	@echo  make st_fota_ftp
#	@echo  make st_fota_http	
	@echo  make floattest

floattest:
	@echo *******make float test *********
	@echo -D__EXAMPLE_FLOAT_TEST__ >> .\$(DEFVIATMP)
	@$(MAKE) new
	
ST_http2fota:
	@echo *******make ST_http2fota *********
	@echo -D__STRESS_TEST_HTTP2FOTA__ >> .\$(DEFVIATMP)
	@$(MAKE) new
	
ST_wrtcoderegion:
	@echo *******make ST_wrtcoderegion *********
	@echo -D__STRESS_TEST_WRTCODEREGION__ >> .\$(DEFVIATMP)
	@$(MAKE) new
	
ST_appupdate:
	@echo *******make ST_appupdate *********
	@echo -D__STRESS_TEST_APPUPDATE__ >> .\$(DEFVIATMP)
	@$(MAKE) new
	
st_fota_ftp:
	@echo *******make st_fota_ftp *********
	@echo -D__STRESS_TEST_FOTA_FTP__ >> .\$(DEFVIATMP)
	@$(MAKE) new

st_fota_http:
	@echo *******make st_fota_http *********
	@echo -D__STRESS_TEST_FOTA_HTTP__ >> .\$(DEFVIATMP)
	@$(MAKE) new

LCD_S6B0724:
	@echo *******make newAPI*********
	@echo -D__EXAMPLE_LCD_S6B0724__ >> .\$(DEFVIATMP)
	@$(MAKE) new
newAPI:
	@echo *******make newAPI*********
	@echo -D__EXAMPLE_NEWAPI__ >> .\$(DEFVIATMP)
	@$(MAKE) new
system:
	@echo *******make system *********
	@echo -D__EXAMPLE_SYSTEM__ >> .\$(DEFVIATMP)
	@$(MAKE) new

timer:
	@echo *******make timer *********
	@echo -D__EXAMPLE_TIMER__ >> .\$(DEFVIATMP)
	@$(MAKE) new
	
testgpio:
	@echo *******make testgpio *********
	@echo -D__TESTGPIO__ >> .\$(DEFVIATMP)
	@$(MAKE) new
time:
	@echo *******make time*********
	@echo -D__EXAMPLE_TIME__ >> .\$(DEFVIATMP)
	@$(MAKE) new
powerkey:
	@echo *******make helloworld*********
	@echo -D__EXAMPLE_POWERKEY__ >> .\$(DEFVIATMP)
	@$(MAKE) new	
helloworld:
	@echo *******make helloworld*********
	@echo -D__EXAMPLE_HELLWORLD__ >> .\$(DEFVIATMP)
	@$(MAKE) new

custom:
	@echo *******make custom*********
	@echo -D__CUSTOMER_CODE__ >> .\$(DEFVIATMP)
	@$(MAKE) new
usb:
	@echo *******make vcom example*********
	@echo -D__EXAMPLE_USB__ >> .\$(DEFVIATMP)
	@$(MAKE) new
gpio:
	@echo *******make gpio example*********
	@echo -D__EXAMPLE_GPIO__ >> .\$(DEFVIATMP)
	@$(MAKE) new
eint:
	@echo *******make eint example*********
	@echo -D__EXAMPLE_EINT__ >> .\$(DEFVIATMP)
	@$(MAKE) new

at:
	@echo *******make at example*********
	@echo -D__EXAMPLE_AT__ >> .\$(DEFVIATMP)
	@$(MAKE) new

atc_pipe:
	@echo *******make atc_pipe example*********
	@echo -D__ATC_PIPE__ >> .\$(DEFVIATMP)
	@$(MAKE) new

memory:
	@echo *******make memory example*********
	@echo -D__EXAMPLE_MEMORY__ >> .\$(DEFVIATMP)
	@$(MAKE) new

pwm:
	@echo *******make pwm example*********
	@echo -D__EXAMPLE_PWM__ >> .\$(DEFVIATMP)
	@$(MAKE) new


clk:
	@echo *******make clk example*********
	@echo -D__EXAMPLE_CLK__ >> .\$(DEFVIATMP)
	@$(MAKE) new

alert:
	@echo *******make alert example*********
	@echo -D__EXAMPLE_ALERT__ >> .\$(DEFVIATMP)
	@$(MAKE) new

keyboard:
	@echo *******make keyboard example*********
	@echo -D__EXAMPLE_KEYBOARD__ >> .\$(DEFVIATMP)
	@$(MAKE) new

lcd:
	@echo *******make lcd example*********
	@echo -D__EXAMPLE_LCD__ >> .\$(DEFVIATMP)
	@$(MAKE) new

adc:
	@echo *******make adc example*********
	@echo -D__EXAMPLE_ADC__ >> .\$(DEFVIATMP)
	@$(MAKE) new

i2c:
	@echo *******make i2c example*********
	@echo -D__EXAMPLE_I2C__ >> .\$(DEFVIATMP)
	@$(MAKE) new	

file:
	@echo *******make file example*********
	@echo -D__EXAMPLE_FILESYSTEM__ >> .\$(DEFVIATMP)
	@$(MAKE) new	

file_sd:
	@echo *******make file example to test SD card*********
	@echo -D__EXAMPLE_FILESYSTEM__ -D__TEST_FOR_MEMORY_CARD__ >> .\$(DEFVIATMP)
	@$(MAKE) new
	
audio:
	@echo *******make audio example*********
	@echo -D__EXAMPLE_AUDIO__ >> .\$(DEFVIATMP)
	@$(MAKE) new

record:
	@echo *******make record example*********
	@echo -D__EXAMPLE_RECORD__>> .\$(DEFVIATMP)
	@$(MAKE) new

transpass:
	@echo *******make transpass example*********
	@echo -D__EXAMPLE_TRANSPASS__>> .\$(DEFVIATMP)
	@$(MAKE) new	

tcpip:
	@echo *******make tcpip example*********
	@echo -D__EXAMPLE_TCPIP__>> .\$(DEFVIATMP)
	@$(MAKE) new	

multitask:
	@echo *******make multitask example*********
	@echo -D__EXAMPLE_MULTITASK__>> .\$(DEFVIATMP)
	@$(MAKE) new	
	
multitask_port:
	@echo *******make multitask_port example*********
	@echo -D__EXAMPLE_MULTITASK_PORT__>> .\$(DEFVIATMP)
	@$(MAKE) new	
	
sms:
	@echo *******make sms example*********
	@echo -D__EXAMPLE_SMS__ >> .\$(DEFVIATMP)
	@$(MAKE) new
	
call:
	@echo *******make call example*********
	@echo -D__EXAMPLE_CALL__ >> .\$(DEFVIATMP)
	@$(MAKE) new

headset:
	@echo *******make headset example*********
	@echo -D__EXAMPLE_HEADSET__ >> .\$(DEFVIATMP)
	@$(MAKE) new	
	
lcm_st7545t:
	@echo *******make lcm_st7545t example*********
	@echo -D__EXAMPLE_LCM_ST7545T__ >> .\$(DEFVIATMP)
	@$(MAKE) new	

lcm_uc1701:
	@echo *******make lcm_uc1701 example*********
	@echo -D__EXAMPLE_LCM_UC1701__ >> .\$(DEFVIATMP)
	@$(MAKE) new	

tcplong:
	@echo *******make tcplong example*********
	@echo -D__EXAMPLE_TCPLONG__ >> .\$(DEFVIATMP)
	@$(MAKE) new	
	
multimemory:
	@echo *******make multimemory example*********
	@echo -D__EXAMPLE_MULTIMEMORY__ >> .\$(DEFVIATMP)
	@$(MAKE) new	

lowrateuart:
	@echo *******make lowrateuart example*********
	@echo -D__EXAMPLE_LOWRATEUART__ >> .\$(DEFVIATMP)
	@$(MAKE) new	
	
eintcallback:	
	@echo *******make eintcallback example*********
	@echo -D__EXAMPLE_EINTCALLBACK__ >> .\$(DEFVIATMP)
	@$(MAKE) new	
	
fota:	
	@echo *******make fota example*********
	@echo -D__EXAMPLE_FOTA__ >> .\$(DEFVIATMP)
	@$(MAKE) new		
	
fota_http:	
	@echo *******make fota_http example*********
	@echo -D__EXAMPLE_FOTA_HTTP__ >> .\$(DEFVIATMP)
	@$(MAKE) new		
	
tts:	
	@echo *******make tts example*********
	@echo -D__EXAMPLE_TTS__ >> .\$(DEFVIATMP)
	@$(MAKE) new	
	
tcpserver:	
	@echo *******make tcpserver example*********
	@echo -D__EXAMPLE_TCPSERVER__ >> .\$(DEFVIATMP)
	@$(MAKE) new	
	
wrtcoderegion:	
	@echo *******make wrtcoderegion example*********
	@echo -D__EXAMPLE_WRTCODEREGION__ >> .\$(DEFVIATMP)
	@$(MAKE) new	
	
fota_ftp:	
	@echo *******make fota_ftp example*********
	@echo -D__EXAMPLE_FOTA_FTP__ >> .\$(DEFVIATMP)
	@$(MAKE) new		
	
alarm:	
	@echo *******make alarm example*********
	@echo -D__EXAMPLE_ALARM__ >> .\$(DEFVIATMP)
	@$(MAKE) new		
	
include .\makefiledef

export ARMTOOL_PATH
	
