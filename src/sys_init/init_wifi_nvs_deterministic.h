#pragma once

#include "esp_err.h"
#include "nvs_flash.h"

#define WIFI_NVS_PARTITION_NAME     NVS_DEFAULT_PART_NAME
#define WIFI_NVS_LOGS_TAG           "NVS_WIFI_INIT"


#ifdef __cplusplus
extern "C" {
#endif

esp_err_t init_wifi_nvs_deterministic(void);

#ifdef __cplusplus
}
#endif
