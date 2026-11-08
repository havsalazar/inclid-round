/**
 * @file main.c
 *
 * Lean angle display on the Waveshare ESP32-S3-Touch-AMOLED-1.75:
 * side lean bars, angle, GPS speed, G bar, battery, settings.
 */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_heap_caps.h"

#include "lvgl.h"
#include "bsp/esp-bsp.h"
#include "roundedui.h"      /* exported by the LVGL Pro Editor from ../roundedui */

#include "display.h"
#include "imu_task.h"
#include "pmu.h"
#include "gps_task.h"
#include "settings.h"

static const char * TAG = "main";

void app_main(void)
{
    /* Panel (still dark), touch, I2C bus and the LVGL task */
    lv_display_t * disp = display_start();
    if(disp == NULL) {
        ESP_LOGE(TAG, "Display init failed");
        return;
    }
    ESP_LOGI(TAG, "Free internal RAM after display start: %u bytes",
             (unsigned)heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
    i2c_master_bus_handle_t bus = bsp_i2c_get_handle();

    esp_err_t err = pmu_init(bus);
    if(err != ESP_OK) ESP_LOGE(TAG, "AXP2101: %s", esp_err_to_name(err));

    bsp_display_lock(-1);
    roundedui_init("");                     /* no file-based assets, everything is compiled in */
    lv_screen_load(lean_screen_create());
    settings_ui_bind();                     /* brightness (remembered until shown), G bar, power off */
    imu_ui_bind();
    pmu_ui_bind();
    gps_ui_bind();
    bsp_display_unlock();

    /* First frame into the panel, then switch it on and fade up: no white flash */
    display_show();

    /* GPS first: it keeps the bus to itself for ~1.5 s while the module boots
     * (see gps_task.c), so the IMU calibration starts after that, undisturbed */
    err = gps_task_start(bus);
    if(err != ESP_OK) ESP_LOGW(TAG, "GPS: %s", esp_err_to_name(err));

    /* Keep still and upright now: 2 s calibration */
    err = imu_task_start(bus);
    if(err != ESP_OK) ESP_LOGE(TAG, "QMI8658: %s", esp_err_to_name(err));
}
