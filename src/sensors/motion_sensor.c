#include "sensors/motion_sensor.h"
#include <esp_log.h>

static const char* TAG = "MotionSensor";

bool motion_sensor_init(void) {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = MOTION_SENSOR_SDA_GPIO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = MOTION_SENSOR_SCL_GPIO,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = MOTION_SENSOR_I2C_FREQ_HZ,
    };

    if (i2c_param_config(MOTION_SENSOR_I2C_PORT, &conf) != ESP_OK) {
        ESP_LOGE(TAG, "i2c_param_config failed");
        return false;
    }

    if (i2c_driver_install(MOTION_SENSOR_I2C_PORT, conf.mode, 0, 0, 0) != ESP_OK) {
        ESP_LOGE(TAG, "i2c_driver_install failed");
        return false;
    }

    uint8_t wake_cmd[2] = { MPU6050_REG_PWR_MGMT_1, 0x00 };
    esp_err_t err = i2c_master_write_to_device(
        MOTION_SENSOR_I2C_PORT, MOTION_SENSOR_ADDR,
        wake_cmd, sizeof(wake_cmd),
        1000 / portTICK_PERIOD_MS
    );

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to wake sensor: %d", err);
        return false;
    }

    return true;
}

bool motion_sensor_check_present(void) {
    uint8_t who_am_i = 0;
    uint8_t reg = MPU6050_REG_WHO_AM_I;

    esp_err_t err = i2c_master_write_read_device(
        MOTION_SENSOR_I2C_PORT, MOTION_SENSOR_ADDR,
        &reg, 1,
        &who_am_i, 1,
        1000 / portTICK_PERIOD_MS
    );

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "WHO_AM_I read failed: %d", err);
        return false;
    }

    ESP_LOGI(TAG, "WHO_AM_I = 0x%02X (expected 0x%02X)", who_am_i, MOTION_SENSOR_WHO_AM_I_EXPECTED);
    return who_am_i == MOTION_SENSOR_WHO_AM_I_EXPECTED;
}

bool motion_sensor_read_accel(motion_sensor_reading_t* out) {
    uint8_t raw[6];
    uint8_t reg = MPU6050_REG_ACCEL_XOUT_H;

    esp_err_t err = i2c_master_write_read_device(
        MOTION_SENSOR_I2C_PORT, MOTION_SENSOR_ADDR,
        &reg, 1,
        raw, 6,
        1000 / portTICK_PERIOD_MS
    );

    if (err != ESP_OK) {
        return false;
    }

    out->accel_x = (raw[0] << 8) | raw[1];
    out->accel_y = (raw[2] << 8) | raw[3];
    out->accel_z = (raw[4] << 8) | raw[5];

    return true;
}