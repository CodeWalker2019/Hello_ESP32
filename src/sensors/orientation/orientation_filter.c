#include "sensors/orientation/orientation_filter.h"
#include <math.h>

#define ACCEL_LSB_PER_G       16384.0f // MPU6050 default AFS_SEL=0, +/-2g
#define GYRO_LSB_PER_DEG_S    131.0f   // MPU6050 default FS_SEL=0, +/-250 deg/s
#define RAD_TO_DEG            57.29577951308232f
#define COMPLEMENTARY_ALPHA   0.98f    // weight on gyro-integrated angle vs. accel angle

static float s_roll_deg = 0.0f;
static float s_pitch_deg = 0.0f;

void orientation_filter_reset(void) {
    s_roll_deg = 0.0f;
    s_pitch_deg = 0.0f;
}

motion_orientation_t orientation_filter_update(int16_t accel_x, int16_t accel_y, int16_t accel_z,
                                                int16_t gyro_x, int16_t gyro_y,
                                                float dt_seconds) {
    float accel_g_x = accel_x / ACCEL_LSB_PER_G;
    float accel_g_y = accel_y / ACCEL_LSB_PER_G;
    float accel_g_z = accel_z / ACCEL_LSB_PER_G;

    float roll_accel_deg = atan2f(accel_g_y, accel_g_z) * RAD_TO_DEG;
    float pitch_accel_deg = atan2f(-accel_g_x, sqrtf(accel_g_y * accel_g_y + accel_g_z * accel_g_z)) * RAD_TO_DEG;

    if (dt_seconds <= 0.0f) {
        s_roll_deg = roll_accel_deg;
        s_pitch_deg = pitch_accel_deg;
    } else {
        float gyro_roll_rate_dps = gyro_x / GYRO_LSB_PER_DEG_S;
        float gyro_pitch_rate_dps = gyro_y / GYRO_LSB_PER_DEG_S;

        s_roll_deg = COMPLEMENTARY_ALPHA * (s_roll_deg + gyro_roll_rate_dps * dt_seconds)
                     + (1.0f - COMPLEMENTARY_ALPHA) * roll_accel_deg;
        s_pitch_deg = COMPLEMENTARY_ALPHA * (s_pitch_deg + gyro_pitch_rate_dps * dt_seconds)
                      + (1.0f - COMPLEMENTARY_ALPHA) * pitch_accel_deg;
    }

    motion_orientation_t result = { .pitch = s_pitch_deg, .roll = s_roll_deg };
    return result;
}
