@echo off
set fromelfPath=C:\Keil_v5\ARM\ARMCC\bin
set nrfutilPath="C:\Program Files\Nordic Semiconductor\Master Control Panel\3.10.0.14\nrf"
set outputPath="X:\vr\YYB\hw\nRF51822_Nordic\nRF51822-BK-FW\Source Code\nrf51_beacon\ble_app_beacon\pca20006\s110\arm\_build"
set initToolPath="X:\vr\YYB\app\Android-nRF-Connect\init packet handling"

set outputFileName=nrf51822_beacon_app

set axfFileName=%outputFileName%.axf
set hexFileName=%outputFileName%.hex
set binFileName=%outputFileName%.bin
set zipFileName=%outputFileName%.zip



::%fromelfPath%\hex2bin.exe %outputPath%\%hexFileName%
copy %fromelfPath%\%binFileName%  %outputPath%\%binFileName%
del %fromelfPath%\%binFileName% 

%nrfutilPath%\nrfutil.exe dfu genpkg --application %outputPath%\%binFileName% --application-version 0xFFFFFFFF %outputPath%\%zipFileName% --sd-req 0x4f,0x5a

::@%initToolPath%\crc.exe %outputPath%\%binFileName%
::@adb push %outputPath%\%zipFileName% \sdcard\Download\