| Supported Targets | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | -------- | -------- | -------- |

# ThymeWatchRound firmware with Mass Storage

```
***ERROR*** A stack overflow in task main has been detected.


Backtrace: 0x40375df2:0x3fccabf0 0x4037df29:0x3fccac10 0x4037ef5a:0x3fccac30 0x4038084f:0x3fccacb0 0x4037f020:0x3fccacd0 0x4037f016:0x3fccad20 0x0006041d:0x3fcce510 |<-CORRUPTED
--- 0x40375df2: panic_abort at /Users/eggfly/esp-idf-v5.3.2/components/esp_system/panic.c:463
0x4037df29: esp_system_abort at /Users/eggfly/esp-idf-v5.3.2/components/esp_system/port/esp_system_chip.c:92
0x4037ef5a: vApplicationStackOverflowHook at /Users/eggfly/esp-idf-v5.3.2/components/freertos/FreeRTOS-Kernel/portable/xtensa/port.c:563
0x4038084f: vTaskSwitchContext at /Users/eggfly/esp-idf-v5.3.2/components/freertos/FreeRTOS-Kernel/tasks.c:3701 (discriminator 7)
0x4037f020: _frxt_dispatch at /Users/eggfly/esp-idf-v5.3.2/components/freertos/FreeRTOS-Kernel/portable/xtensa/portasm.S:451
0x4037f016: _frxt_int_exit at /Users/eggfly/esp-idf-v5.3.2/components/freertos/FreeRTOS-Kernel/portable/xtensa/portasm.S:246

```