@echo off
set fromelfPath=C:\Keil_v5\ARM\ARMCC\bin
set nrfutilPath="C:\Program Files\Nordic Semiconductor\Master Control Panel\3.10.0.14\nrf"

::set projectRootDir="X:\vr\YYB\rtos\nrf51822\beacon\"
::set projectRootDir="Y:\yyb\hw\nRF51822_Nordic\nRF51822-BK-FW\Source Code\"
set projectRootDir=Y:\yyb\rtos\nrf51822\beacon\

set CUR_DIR=%cd%
echo %CUR_DIR%
	
IF %1==app (
	::default update applicaton
	set outputFileName=nrf51822_beacon_app
	set outputPath=%projectRootDir%nrf51_beacon\ble_app_beacon\pca20006\s110\arm\_build

	cd %projectRootDir%nrf51_beacon\ble_app_beacon\pca20006\s110\arm\
	call nrf51822_xxac_s110.BAT
	cd %CUR_DIR%
)
IF %1==bl (
	set outputFileName=nrf51822_beacon_bootloader
	set outputPath=%projectRootDir%nrf51_beacon\ble_app_beacon_dfu\bootloader\pca20006\dual_bank_ble_s110\arm\_build
	
	cd %projectRootDir%nrf51_beacon\ble_app_beacon_dfu\bootloader\pca20006\dual_bank_ble_s110\arm\
	call nrf51822_xxac.BAT
	cd %CUR_DIR%
)
set axfFileName=%outputFileName%.axf
set binFileName=%outputFileName%.bin
set zipFileName=%outputFileName%.zip


::echo execute file : %0
::echo first parameter : %1
echo %outputPath%\%zipFileName%
echo %outputPath%\%binFileName%

IF %1==bl (
	echo update bootloader
	%fromelfPath%\fromelf.exe --bin --output %outputPath%\%binFileName% %outputPath%\%axfFileName%
	%nrfutilPath%\nrfutil.exe dfu genpkg --bootloader %outputPath%\%binFileName% --dev-type 0xffff %outputPath%\%zipFileName%
	adb.exe push %outputPath%\%zipFileName% \sdcard\Download
)

IF %1==app (
	echo update app
	%nrfutilPath%\nrfutil.exe dfu genpkg --application %outputPath%\%binFileName% --application-version 0xff %outputPath%\%zipFileName%
)

adb.exe push %outputPath%\%zipFileName% \sdcard\Download

::fromelf --bin -o _build\nrf51822_beacon_app.bin _build\nrf51822_beacon_app.axf
::fromelf --bin -o _build\nrf51822_beacon_bootloader.bin _build\nrf51822_beacon_bootloader.axf
														