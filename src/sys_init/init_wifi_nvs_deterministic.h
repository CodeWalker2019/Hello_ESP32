#pragma once

#include "esp_err.h"

#define WIFI_NVS_PARTITION_NAME     "nvs_wifi"
#define WIFI_NVS_LOGS_TAG           "NVS_WIFI_INIT"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t init_wifi_nvs_deterministic(void);

#ifdef __cplusplus
}
#endif
