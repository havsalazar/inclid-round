/**
 * @file settings.c
 */
#include "settings.h"
#include "imu_task.h"
#include "pmu.h"

#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"

#include "lvgl.h"
#include "display.h"
#include "roundedui.h"

static const char * TAG = "settings";

#define POWER_OFF_DELAY_MS  600     /* time to show "POWERING OFF..." */
#define SAVE_DELAY_MS       2000    /* save this long after the last change */
#define NVS_NAMESPACE       "settings"
#define NVS_KEY_BRIGHTNESS  "bright"
#define NVS_KEY_G_BAR       "g_bar"

static lv_timer_t * s_save_timer;

/*********************
 *  POWER OFF
 *********************/
static void power_off_timer_cb(lv_timer_t * t)
{
    LV_UNUSED(t);
    display_set_brightness(0);
    imu_sleep();
    pmu_power_off();                /* does not return */
}

static void power_off_request_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(observer);
    if(lv_subject_get_int(subject) != 1) return;
    ESP_LOGI(TAG, "Power off requested from the screen");
    lv_timer_t * t = lv_timer_create(power_off_timer_cb, POWER_OFF_DELAY_MS, NULL);
    lv_timer_set_repeat_count(t, 1);
}

/*********************
 *  SAVED SETTINGS
 *********************/
static void save_cb(lv_timer_t * t)
{
    nvs_handle_t h;
    if(nvs_open(NVS_NAMESPACE, NVS_READWRITE, &h) != ESP_OK) {
        lv_timer_pause(t);
        return;
    }
    nvs_set_u8(h, NVS_KEY_BRIGHTNESS, (uint8_t)lv_subject_get_int(subject_brightness_pct));
    nvs_set_u8(h, NVS_KEY_G_BAR, (uint8_t)lv_subject_get_int(subject_show_g_bar));
    nvs_commit(h);
    nvs_close(h);
    lv_timer_pause(t);              /* until the next change */
}

static void schedule_save(void)
{
    if(s_save_timer == NULL) return;
    lv_timer_reset(s_save_timer);
    lv_timer_resume(s_save_timer);
}

static void brightness_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(observer);
    display_set_brightness(lv_subject_get_int(subject));
    schedule_save();
}

static void g_bar_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(observer);
    LV_UNUSED(subject);
    schedule_save();
}

static bool settings_load(void)
{
    esp_err_t err = nvs_flash_init();
    if(err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        nvs_flash_erase();
        err = nvs_flash_init();
    }
    if(err != ESP_OK) {
        ESP_LOGE(TAG, "NVS init failed (%s): settings won't be remembered", esp_err_to_name(err));
        return false;
    }

    nvs_handle_t h;
    uint8_t v;
    if(nvs_open(NVS_NAMESPACE, NVS_READONLY, &h) != ESP_OK) return true;    /* nothing saved yet */
    if(nvs_get_u8(h, NVS_KEY_BRIGHTNESS, &v) == ESP_OK) lv_subject_set_int(subject_brightness_pct, v);
    if(nvs_get_u8(h, NVS_KEY_G_BAR, &v) == ESP_OK) lv_subject_set_int(subject_show_g_bar, v);
    nvs_close(h);
    return true;
}

void settings_ui_bind(void)
{
    lv_subject_add_observer(subject_power_off_request, power_off_request_cb, NULL);

    if(settings_load()) {
        /* One-shot, paused until something changes */
        s_save_timer = lv_timer_create(save_cb, SAVE_DELAY_MS, NULL);
        lv_timer_set_auto_delete(s_save_timer, false);
        lv_timer_set_repeat_count(s_save_timer, -1);
        lv_timer_pause(s_save_timer);
    }
    /* Observers also fire once right away: that applies the loaded brightness */
    lv_subject_add_observer(subject_brightness_pct, brightness_cb, NULL);
    lv_subject_add_observer(subject_show_g_bar, g_bar_cb, NULL);
    if(s_save_timer) lv_timer_pause(s_save_timer);  /* nothing changed yet */
}
