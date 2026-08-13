#include "mpu6050.h"
#include "driver/i2c_master.h"
#include "sdkconfig.h"
#include <esp_err.h>
#include "esp_log.h"

#define MPU6050_SENSOR_ADDR         0x68
#define MPU6050_WHO_AM_I            0x75
#define MPU6050_PWR_MGMT_1          0x6B
#define MPU6050_ACCEL_XOUT_H        0x3B
#define I2C_MASTER_TIMEOUT_MS       1000
#define ACCEL_DATA_LEN              6
#define MPU6050_WHO_AM_I_EXPECTED   0x68

static const char *TAG = "MPU6050";

static i2c_master_bus_handle_t bus_handle;
static i2c_master_dev_handle_t mpu6050_handle;

static esp_err_t mpu6050_wake_device(void) {
  uint8_t write_buf[2] = { MPU6050_PWR_MGMT_1, 0x00 };
  return i2c_master_transmit(mpu6050_handle, write_buf, sizeof(write_buf), I2C_MASTER_TIMEOUT_MS);
}

esp_err_t mpu6050_init_motion_sensor(void) {
  i2c_master_bus_config_t bus_config = {
    .i2c_port     =   I2C_NUM_0,
    .sda_io_num   =   GPIO_NUM_21,
    .scl_io_num   =   GPIO_NUM_22,
    .clk_source   =   I2C_CLK_SRC_DEFAULT,
  };

  ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, &bus_handle));

  i2c_device_config_t dev_cfg = {
    .dev_addr_length  =  I2C_ADDR_BIT_LEN_7,
    .device_address   =  MPU6050_SENSOR_ADDR,
    .scl_speed_hz     =  100000,
  };

  ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &mpu6050_handle));
  ESP_ERROR_CHECK(mpu6050_wake_device());
  ESP_LOGI(TAG, "I2C connection is successfully initialized!");

  return ESP_OK;
}

bool mpu6050_check_present(void) {
    uint8_t who_am_i = 0;
    uint8_t reg = MPU6050_WHO_AM_I;

    esp_err_t err = i2c_master_transmit_receive(
        mpu6050_handle, &reg, 1, &who_am_i, 1, I2C_MASTER_TIMEOUT_MS
    );

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "WHO_AM_I read failed: %d", err);
        return false;
    }

    ESP_LOGI(TAG, "WHO_AM_I = 0x%02X (expected 0x%02X)", who_am_i, MPU6050_WHO_AM_I_EXPECTED);
    return who_am_i == MPU6050_WHO_AM_I_EXPECTED;
}

esp_err_t mpu6050_read_motion_data(int16_t *accel_x, int16_t *accel_y, int16_t *accel_z) {
  if (!accel_x || !accel_y || !accel_z) return ESP_ERR_INVALID_ARG;
  
  uint8_t reg_addr = MPU6050_ACCEL_XOUT_H;
  uint8_t data[ACCEL_DATA_LEN];

  esp_err_t err = i2c_master_transmit_receive(mpu6050_handle, &reg_addr, 1, data, ACCEL_DATA_LEN, I2C_MASTER_TIMEOUT_MS);

  if (err != ESP_OK) return err;

  *accel_x = (int16_t)((data[0] << 8) | data[1]);
  *accel_y = (int16_t)((data[2] << 8) | data[3]);
  *accel_z = (int16_t)((data[4] << 8) | data[5]);

  return ESP_OK;
}

bool mpu6050_read_accel(mpu6050_reading_t *out) {
    if (!out) return false;
    return mpu6050_read_motion_data(&out->accel_x, &out->accel_y, &out->accel_z) == ESP_OK;
}
