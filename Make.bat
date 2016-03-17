::@echo off
::@copy qlcode\scat6252M64.txt  build\scat6252.txt
::::@echo coreSW\M10_MODEM_08A_PCB01_gprs_MT6223_S00.sym > ./build/~coresymlnk.tmp
::@echo -DQL_MEMORY_128M > .\build\~makedef.tmp
::if "%1"=="" (echo please input "make help") else tools\make.exe %1 %2 %3 

@echo off
@copy qlcode\scat6252M64.txt  build\scat6252.txt
::@echo coreSW\M10_MODEM_08A_PCB01_gprs_MT6223_S00.sym > ./build/~coresymlnk.tmp

if "%1"=="" (
	echo please input "make help"
	) else (
				if "%2"=="" (
					tools\make.exe %1 %2 %3
				) else (
							if "%2"=="128" (
								@echo -DQL_MEMORY_128M > .\build\~makedef.tmp
								tools\make.exe %1
							) else (
										if "%2"=="64" (
											@echo -DQL_MEMORY_64M > .\build\~makedef.tmp
											tools\make.exe %1
										) else (
													@echo.
													@echo.
													@echo ERROR COMPLIER CMD
													@echo.
													@echo.
													@echo.
												)
									)
						)
	          )


 




