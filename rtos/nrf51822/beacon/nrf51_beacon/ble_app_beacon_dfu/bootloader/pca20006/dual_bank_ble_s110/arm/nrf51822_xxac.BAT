SET PATH=C:\Keil_v5\ARM\ARMCC\Bin;C:\Windows\system32;C:\Windows;C:\Windows\System32\Wbem;C:\Windows\System32\WindowsPowerShell\v1.0\;C:\Program Files\Nordic Semiconductor\nrf5x\bin\;C:\adb
SET CPU_TYPE=nRF51822_xxAC
SET CPU_VENDOR=Nordic Semiconductor
SET UV2_TARGET=nrf51822_xxac
SET CPU_CLOCK=0x00F42400
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\_build\dfu_ble_svc.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\_build\main.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\_build\led_softblink.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmAsm" --Via ".\_build\arm_startup_nrf51._ia"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\_build\system_nrf51.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\_build\ble_advdata.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\_build\ble_conn_params.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\_build\ble_srv_common.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\_build\ble_dfu.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\_build\bootloader_settings_arm.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\_build\bootloader_util_arm.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\_build\nrf_delay.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\_build\pstorage_mod.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\_build\app_error.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\_build\app_scheduler.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\_build\app_timer.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\_build\bootloader.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\_build\crc16.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\_build\dfu_init_template.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\_build\hci_mem_pool.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\_build\nrf_assert.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\_build\dfu_transport_ble_mod.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\_build\dfu_dual_bank_mod.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\_build\softdevice_handler.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmLink" --Via ".\_build\nrf51822_beacon_bootloader.lnp"
"C:\Keil_v5\ARM\ARMCC\Bin\fromelf.exe" ".\_build\nrf51822_beacon_bootloader.axf" --i32combined --output ".\_build\nrf51822_beacon_bootloader.hex"
fromelf --bin -o _build\nrf51822_beacon_bootloader.bin _build\nrf51822_beacon_bootloader.axf
