@echo off
@echo LIB_DIR=%1 > ./build/~libname.tmp
@echo LIB_NAME=%1 >> ./build/~libname.tmp
tools\make.exe --file=makefilelib %1 %2

 




