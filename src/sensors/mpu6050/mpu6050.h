#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <esp_err.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    int16_t accel_x;
    int16_t accel_y;
    int16_t accel_z;
} mpu6050_reading_t;

typedef mpu6050_reading_t motion_sensor_reading_t;

esp_err_t mpu6050_init_motion_sensor(void);
bool mpu6050_check_present(void);
esp_err_t mpu6050_read_motion_data(int16_t *accel_x, int16_t *accel_y, int16_t *accel_z);
bool mpu6050_read_accel(mpu6050_reading_t *out);

#ifdef __cplusplus
}
#endif
