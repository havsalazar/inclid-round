/**
 * @file gps_task.h
 *
 * Onboard LC76G GNSS on the shared I2C bus (0x50 write / 0x54 read):
 * NMEA RMC -> subject_speed_kmh and subject_gps_fix, and position and time for the logger.
 * The first valid RMC also sets the system clock (UTC, settimeofday()), and resyncs it on > 2 s drift.
 */
#ifndef GPS_TASK_H
#define GPS_TASK_H

#include <stdbool.h>
#include <stdint.h>
#include "driver/i2c_master.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    bool fix;               /* valid RMC in the last 3 s; without it lat/lon/utc are the last known values */
    double lat, lon;        /* decimal degrees, + = north / east */
    int32_t speed_kmh;      /* 0 without a fix */
    int sats;               /* satellites in view, -1 = module not answering */
    char utc[16];           /* "hh:mm:ss.ss", "" until the first fix */
    char date[12];          /* "yyyy-mm-dd", "" until the first fix */
} gps_data_t;

/** Latest GPS values. Thread safe, from any task. */
void gps_get(gps_data_t * out);

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
