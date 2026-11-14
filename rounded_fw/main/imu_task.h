/**
 * @file imu_task.h
 *
 * Onboard QMI8658 (I2C 0x6B) -> lean angle and longitudinal g -> the UI subjects.
 */
#ifndef IMU_TASK_H
#define IMU_TASK_H

#include <stdbool.h>
#include <stdint.h>
#include "driver/i2c_master.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Start reading the QMI8658 at ~112 Hz in its own FreeRTOS task.
 * Keep the device still and mounted upright for ~2 s after this call:
 * that is when the gyro bias and the "upright" position are calibrated.
 */
esp_err_t imu_task_start(i2c_master_bus_handle_t bus);

/**
 * Copy the latest IMU values into the UI subjects every 33 ms and watch
 * subject_imu_cal_request. Call once with the LVGL lock held.
 */
void imu_ui_bind(void);

/**
 * Latest lean (tenths of a degree, + = right) and longitudinal g (1/100 g,
 * + = accelerating). Thread safe, from any task. Returns false while calibrating.
 */
bool imu_get(int32_t * lean_x10, int32_t * accel_cg);

/** Put the QMI8658 to sleep before powering off */
void imu_sleep(void);

#ifdef __cplusplus
}
#endif

#endif /* IMU_TASK_H */
