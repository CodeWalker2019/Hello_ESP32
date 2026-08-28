#ifndef MOTION_SENSOR_H
#define MOTION_SENSOR_H

#include <stdint.h>
#include <stdbool.h>
#include "driver/i2c.h"

#define MOTION_SENSOR_I2C_PORT     I2C_NUM_0
#define MOTION_SENSOR_SDA_GPIO     21
#define MOTION_SENSOR_SCL_GPIO     22
#define MOTION_SENSOR_I2C_FREQ_HZ  100000

#define MOTION_SENSOR_ADDR         0x68

#define MPU6050_REG_PWR_MGMT_1     0x6B
#define MPU6050_REG_WHO_AM_I       0x75
#define MPU6050_REG_ACCEL_XOUT_H   0x3B

#define MOTION_SENSOR_WHO_AM_I_EXPECTED  0x68

typedef struct {
    int16_t accel_x;
    int16_t accel_y;
    int16_t accel_z;
} motion_sensor_reading_t;

#ifdef __cplusplus
extern "C" {
#endif

bool motion_sensor_init(void);
bool motion_sensor_check_present(void);
bool motion_sensor_read_accel(motion_sensor_reading_t* out);

#ifdef __cplusplus
}
#endif

#endif