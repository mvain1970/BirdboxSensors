# Birdbox Sensors

ESP32 firmware for the Birdbox Sensors project.

The initial application connects the board to Wi-Fi and stores its credentials in the ESP32's persistent storage rather than in this source tree. It reports connection status and its assigned IP address over USB every 30 seconds.

## Device

- Application serial port: `COM15`
- Bootloader serial port: `COM14`
- Chip: ESP32-S3 revision 0.2
- Memory: 16 MB flash, 8 MB PSRAM
- USB console speed: `115200`
- Default build target: ESP32-S3

## Build and upload

Use the Espressif Python installation on this computer:

```powershell
C:\Espressif\tools\python\Scripts\pio.exe run
C:\Espressif\tools\python\Scripts\pio.exe run --target erase --target upload
```

If automatic upload cannot enter download mode, hold **BOOT**, briefly press **RESET**, then release **BOOT** when the upload begins connecting.
