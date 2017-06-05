@echo off
set fromelfPath=C:\Keil_v5\ARM\ARMCC\bin
set nrfutilPath="C:\Program Files\Nordic Semiconductor\Master Control Panel\3.10.0.14\nrf"
::set outputPath="X:\vr\YYB\hw\nRF51822_Nordic\nRF51822-BK-FW\Source Code\nrf51_beacon\ble_app_beacon\pca20006\s110\arm\_build"
::set outputPath="Y:\yyb\hw\nRF51822_Nordic\nRF51822-BK-FW\Source Code\nrf51_beacon\ble_app_beacon\pca20006\s110\arm\_build"
::set outputPath="Y:\yyb\hw\nRF51822_Nordic\nRF51822-BK-FW\Source Code\nrf51_beacon\ble_app_beacon\pca20006\s110\arm\_build"
set outputPath="Y:\yyb\rtos\nrf51822\beacon\nrf51_beacon\ble_app_beacon\pca20006\s110\arm\_build"
::set outputPath="Y:\yyb\rtos\nrf51822\beacon\nrf51_beacon\ble_app_beacon_dfu\bootloader\pca20006\dual_bank_ble_s110\arm\_build"

set outputFileName=nrf51822_beacon_app

set axfFileName=%outputFileName%.axf
set binFileName=%outputFileName%.bin
set zipFileName=%outputFileName%.zip

::%fromelfPath%\fromelf.exe --bin --output %outputPath%\%binFileName% %outputPath%\%axfFileName%

%nrfutilPath%\nrfutil.exe dfu genpkg --application %outputPath%\%binFileName% --application-version 0xff %outputPath%\%zipFileName%

adb.exe push %outputPath%\%zipFileName% \sdcard\Download
