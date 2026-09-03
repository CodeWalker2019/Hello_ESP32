#include "sensors/motion_sensor.h"
#include <esp_log.h>
#include "driver/i2c_master.h"

static const char* TAG = "MotionSensor";

static i2c_master_bus_handle_t s_bus_handle = NULL;
static i2c_master_dev_handle_t s_dev_handle = NULL;

bool motion_sensor_init(void) {
    i2c_master_bus_config_t bus_config = {
        .i2c_port = MOTION_SENSOR_I2C_PORT,
        .sda_io_num = MOTION_SENSOR_SDA_GPIO,
        .scl_io_num = MOTION_SENSOR_SCL_GPIO,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };

    if (i2c_new_master_bus(&bus_config, &s_bus_handle) != ESP_OK) {
        ESP_LOGE(TAG, "i2c_new_master_bus failed");
        return false;
    }

    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = MOTION_SENSOR_ADDR,
        .scl_speed_hz = MOTION_SENSOR_I2C_FREQ_HZ,
    };

    if (i2c_master_bus_add_device(s_bus_handle, &dev_config, &s_dev_handle) != ESP_OK) {
        ESP_LOGE(TAG, "i2c_master_bus_add_device failed");
        return false;
    }

    uint8_t wake_cmd[2] = { MPU6050_REG_PWR_MGMT_1, 0x00 };
    esp_err_t err = i2c_master_transmit(
        s_dev_handle, wake_cmd, sizeof(wake_cmd), 1000
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

    esp_err_t err = i2c_master_transmit_receive(
        s_dev_handle,
        &reg, 1,
        &who_am_i, 1,
        1000
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

    esp_err_t err = i2c_master_transmit_receive(
        s_dev_handle,
        &reg, 1,
        raw, 6,
        1000
    );

    if (err != ESP_OK) {
        return false;
    }

    out->accel_x = (raw[0] << 8) | raw[1];
    out->accel_y = (raw[2] << 8) | raw[3];
    out->accel_z = (raw[4] << 8) | raw[5];

    return true;
}
