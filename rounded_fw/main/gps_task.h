/**
 * @file gps_task.h
 *
 * Onboard LC76G GNSS on the shared I2C bus (0x50 write / 0x54 read):
 * NMEA RMC -> subject_speed_kmh and subject_gps_fix.
 */
#ifndef GPS_TASK_H
#define GPS_TASK_H

#include "driver/i2c_master.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Start polling the LC76G every 500 ms in its own task */
esp_err_t gps_task_start(i2c_master_bus_handle_t bus);

/**
 * Copy speed and fix into the UI every 250 ms and watch subject_gps_cal_request.
 * Call once with the LVGL lock held.
 */
void gps_ui_bind(void);

#ifdef __cplusplus
}
#endif

#endif /* GPS_TASK_H */
