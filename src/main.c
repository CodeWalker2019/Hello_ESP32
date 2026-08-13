#include "app_bridge.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sensors/mpu6050/mpu6050.h"
#include "esp_log.h"

void app_main(void) {
    int16_t accel_x = 0;
    int16_t accel_y = 0;
    int16_t accel_z = 0;

    app_bridge_init();
    mpu6050_init_motion_sensor();

    while (1) {
        app_bridge_connection_manager_worker();
        
        esp_err_t status = mpu6050_read_motion_data(&accel_x, &accel_y, &accel_z);

        if (status == ESP_OK) ESP_LOGI("Sensor data", "X: %d | Y: %d | Z: %d", accel_x, accel_y, accel_z);
        if (status != ESP_OK) ESP_LOGW("Sensor data", "Failed to read motion data!");

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}