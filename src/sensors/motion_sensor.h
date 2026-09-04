#ifndef MOTION_SENSOR_H
#define MOTION_SENSOR_H

#include <stdint.h>
#include <stdbool.h>
#include "driver/i2c_master.h"
#include "sensors/orientation/orientation_filter.h"

#define MOTION_SENSOR_I2C_PORT     I2C_NUM_0
#define MOTION_SENSOR_SDA_GPIO     21
#define MOTION_SENSOR_SCL_GPIO     22
#define MOTION_SENSOR_I2C_FREQ_HZ  100000

#define MOTION_SENSOR_ADDR         0x68

#define MPU6050_REG_PWR_MGMT_1     0x6B
#define MPU6050_REG_WHO_AM_I       0x75
#define MPU6050_REG_ACCEL_XOUT_H   0x3B
#define MPU6050_REG_GYRO_XOUT_H    0x43
#define MPU6050_REG_CONFIG         0x1A

// DLPF_CFG=3 in CONFIG[2:0]: 44Hz accel / 42Hz gyro bandwidth. Cuts
// high-frequency vibration/electrical noise while staying well under the
// 10ms sensor_task poll interval (~4.9ms/4.8ms filter delay).
#define MPU6050_CONFIG_DLPF_CFG_3  0x03

#define MOTION_SENSOR_WHO_AM_I_EXPECTED  0x68

typedef struct {
    int16_t accel_x;
    int16_t accel_y;
    int16_t accel_z;
    int16_t gyro_x;
    int16_t gyro_y;
    int16_t gyro_z;
} motion_sensor_reading_t;

#ifdef __cplusplus
extern "C" {
#endif

bool motion_sensor_init(void);
bool motion_sensor_check_present(void);

// Burst-reads accel + temp + gyro in one I2C transaction (ACCEL_XOUT_H through
// GYRO_ZOUT_L are contiguous registers), so both readings come from the same
// sample instant instead of two separate transactions.
bool motion_sensor_read(motion_sensor_reading_t* out);

// Reads accel+gyro and fuses them into a pitch/roll estimate via
// orientation_filter_update(). Yaw is intentionally not produced (see the
// comment in read_motion_sensor_data()'s definition). Not thread-safe: the
// filter keeps internal state across calls, so only call this from one task.
bool read_motion_sensor_data(motion_orientation_t* out);

#ifdef __cplusplus
}
#endif

#endif