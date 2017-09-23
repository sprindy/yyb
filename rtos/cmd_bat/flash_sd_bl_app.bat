set folder1=Y:\vr\YYB
set folder2=Y:\yyb

if exist %folder1% (
	echo "project dir: %folder1%"
	set PROJECT_DIR=%folder1%
) else (
	echo "project dir: %folder2%"
	set PROJECT_DIR=%folder2%
)

set softdevicePath="%PROJECT_DIR%\hw\nRF51822_Nordic\nRF51822-BK-FW\SoftDevice"
set applicatonPath="%PROJECT_DIR%\nrf51822\beacon\nrf51_beacon\ble_app_beacon\pca20006\s110\arm\_build"
set bootloaderPath="%PROJECT_DIR%\nrf51822\beacon\nrf51_beacon\ble_app_beacon_dfu\bootloader\pca20006\dual_bank_ble_s110\arm\_build"

nrfjprog --eraseall
nrfjprog --program %softdevicePath%\s110_nrf51822_7.1.0_softdevice.hex
nrfjprog --program %bootloaderPath%\nrf51822_beacon_bootloader.hex
nrfjprog --program %applicatonPath%\nrf51822_beacon_app.hex
nrfjprog --reset
pause

