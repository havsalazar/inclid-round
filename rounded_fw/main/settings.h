/**
 * @file settings.h
 *
 * Settings screen actions that need C: brightness, the G bar on/off (both
 * remembered in NVS) and power off. The calibrations live in imu_task.c and
 * gps_task.c.
 */
#ifndef SETTINGS_H
#define SETTINGS_H

#ifdef __cplusplus
extern "C" {
#endif

/** Load the saved settings and watch the subjects. Call once with the LVGL lock held. */
void settings_ui_bind(void);

#ifdef __cplusplus
}
#endif

#endif /* SETTINGS_H */
