#ifndef ORIENTATION_FILTER_H
#define ORIENTATION_FILTER_H

#include <stdint.h>

typedef struct {
    float pitch;
    float roll;
} motion_orientation_t;

#ifdef __cplusplus
extern "C" {
#endif

void orientation_filter_reset(void);

motion_orientation_t orientation_filter_update(int16_t accel_x, int16_t accel_y, int16_t accel_z,
                                                int16_t gyro_x, int16_t gyro_y,
                                                float dt_seconds);

#ifdef __cplusplus
}
#endif

#endif
