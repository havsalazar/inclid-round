/**
 * @file pmu.h
 *
 * AXP2101 power management chip (I2C 0x34): battery level, charging state and
 * power off. The battery subjects of the UI are updated from here.
 */
#ifndef PMU_H
#define PMU_H

#include "driver/i2c_master.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Find the AXP2101 on the bus and enable its battery gauge */
esp_err_t pmu_init(i2c_master_bus_handle_t bus);

/**
 * Update subject_battery_pct and subject_battery_charging every 2 s.
 * Call once with the LVGL lock held, after roundedui_init().
 */
void pmu_ui_bind(void);

/** Cut the power. The PWR button turns the board back on. Does not return. */
void pmu_power_off(void);

#ifdef __cplusplus
}
#endif

#endif /* PMU_H */
