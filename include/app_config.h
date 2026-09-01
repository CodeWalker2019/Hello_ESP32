#pragma once
#define TELEMETRY_MODE_TEXT_DEBUG   0
#define BAUD_RATE                   115200
#define DEVICE_FAMILY_ID            0x4B  // 'K'
#define CONFIG_DEFAULT_SOFTAP_SSID "HelloESP32_A1B2"

// ESPTouch V2 AES-128 key. Must be exactly 16 bytes and must match
// DEFAULT_AES_KEY in desktop-app/src/main/esptouch/constants.ts.
#define CONFIG_ESPTOUCH_V2_AES_KEY "HelloESP32Key16!"
