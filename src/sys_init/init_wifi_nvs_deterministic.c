#include "sys_init/init_wifi_nvs_deterministic.h"
#include "nvs_flash.h"
#include "esp_log.h"

esp_err_t init_wifi_nvs_deterministic(void) {
    esp_err_t err = nvs_flash_init_partition(WIFI_NVS_PARTITION_NAME);

    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(WIFI_NVS_LOGS_TAG, "Partition '%s' corrupted or version mismatch.", WIFI_NVS_PARTITION_NAME);

        // I'm fine to erase all the data in this partition and re-initialize it
        // since client application atomatically reconfigures Wi-Fi settings after ESP_TOUCH.
        // No wifi auto-connect is needed for the project purposes.
        err = nvs_flash_erase_partition(WIFI_NVS_PARTITION_NAME);

        if (err != ESP_OK) {
            ESP_LOGE(WIFI_NVS_LOGS_TAG, "Failed to erase '%s': %s", WIFI_NVS_PARTITION_NAME, esp_err_to_name(err));
            return err;
        }

        err = nvs_flash_init_partition(WIFI_NVS_PARTITION_NAME);
    }

    if (err != ESP_OK) {
        ESP_LOGE(WIFI_NVS_LOGS_TAG, "Failed to mount '%s': %s", WIFI_NVS_PARTITION_NAME, esp_err_to_name(err));
        return err;
    }

    ESP_LOGI(WIFI_NVS_LOGS_TAG, "Successfully mounted '%s'", WIFI_NVS_PARTITION_NAME);

    return ESP_OK;
}
