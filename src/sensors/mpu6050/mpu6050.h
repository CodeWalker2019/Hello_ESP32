#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <esp_err.h>
#include <stdint.h>

esp_err_t mpu6050_read_motion_data(int16_t *accel_x, int16_t *accel_y, int16_t *accel_z);
esp_err_t mpu6050_init_motion_sensor(void);

#ifdef __cplusplus
}
#endif
