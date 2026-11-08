/**
 * @file main.c
 *
 * Lean angle display on the Waveshare ESP32-S3-Touch-AMOLED-1.75:
 * side lean bars, angle, GPS speed, G bar, battery, settings.
 */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "lvgl.h"
#include "lvgl_private.h"   /* lv_display_t::flush_cb, to wrap the adapter's flush */
#include "bsp/esp-bsp.h"
#include "bsp/display.h"
#include "bsp/touch.h"
#include "esp_heap_caps.h"
#include "roundedui.h"      /* exported by the LVGL Pro Editor from ../roundedui */

#include "imu_task.h"
#include "pmu.h"
#include "gps_task.h"
#include "settings.h"

static const char * TAG = "main";

/* 1: show the UI upside down compared to Waveshare's default orientation.
 * The display adapter can't rotate this panel, so each rendered chunk is turned
 * 180 degrees in place before it's sent (see flush_rotate_180), and the touch
 * coordinates are mirrored to match. */
#define DISPLAY_ROTATE_180  1

/*********************
 *  180 DEGREE ROTATION
 *********************/
static lv_display_flush_cb_t s_adapter_flush;

/* Turning a rectangle 180 degrees = reversing its pixel order and mirroring its
 * position in the screen. Works in place, so no extra buffer is needed. */
static void flush_rotate_180(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
    int32_t w = lv_area_get_width(area);
    int32_t h = lv_area_get_height(area);
    uint32_t stride = lv_draw_buf_width_to_stride(w, lv_display_get_color_format(disp));
    int32_t hor = lv_display_get_horizontal_resolution(disp);
    int32_t ver = lv_display_get_vertical_resolution(disp);

    /* Pixel (r, c) swaps with (h-1-r, w-1-c). 16 bit colors (RGB565). */
    for(int32_t r = 0; r < (h + 1) / 2; r++) {
        uint16_t * top = (uint16_t *)(px_map + r * stride);
        uint16_t * bot = (uint16_t *)(px_map + (h - 1 - r) * stride);
        int32_t cols = (r == h - 1 - r) ? w / 2 : w;    /* middle row: only half of it */
        for(int32_t c = 0; c < cols; c++) {
            uint16_t t = top[c];
            top[c] = bot[w - 1 - c];
            bot[w - 1 - c] = t;
        }
    }

    lv_area_t rot = {
        .x1 = hor - 1 - area->x2, .x2 = hor - 1 - area->x1,
        .y1 = ver - 1 - area->y2, .y2 = ver - 1 - area->y1,
    };
    s_adapter_flush(disp, &rot, px_map);
}

/*********************
 *  DISPLAY START
 *********************/
/* Lines per LVGL draw buffer. Two buffers of 466 x 116 x 2 bytes = ~216 KB internal RAM. */
#define DRAW_BUF_LINES      116

/* The CO5300 needs even start and odd end coordinates (BSP's rounder, which is static there) */
static void rounder_cb(lv_event_t * e)
{
    lv_area_t * a = lv_event_get_param(e);
    a->x1 &= ~1;
    a->y1 &= ~1;
    a->x2 |= 1;
    a->y2 |= 1;
}

/* What bsp_display_start_with_config() does, except that the draw buffers are in
 * internal RAM instead of PSRAM (the BSP hard-codes PSRAM, which makes software
 * rendering much slower) and the LVGL task is pinned to core 1. */
static lv_display_t * display_start(void)
{
    esp_lv_adapter_config_t lv_cfg = ESP_LV_ADAPTER_DEFAULT_CONFIG();
    lv_cfg.task_core_id = 1;                    /* IMU and GPS tasks stay on core 0 */
    if(esp_lv_adapter_init(&lv_cfg) != ESP_OK) return NULL;

    esp_lcd_panel_handle_t panel;
    esp_lcd_panel_io_handle_t io;
    const bsp_display_config_t panel_cfg = {
        .max_transfer_sz = BSP_LCD_H_RES * DRAW_BUF_LINES * BSP_LCD_BITS_PER_PIXEL / 8,
    };
    if(bsp_display_new(&panel_cfg, &panel, &io) != ESP_OK) return NULL;

    esp_lv_adapter_display_config_t disp_cfg = {
        .panel = panel,
        .panel_io = io,
        .profile = {
            .interface = ESP_LV_ADAPTER_PANEL_IF_OTHER,
            .rotation = ESP_LV_ADAPTER_ROTATE_0,
            .hor_res = BSP_LCD_H_RES,
            .ver_res = BSP_LCD_V_RES,
            .buffer_height = DRAW_BUF_LINES,
            .use_psram = false,
            .enable_ppa_accel = false,
            .require_double_buffer = true,
        },
        .tear_avoid_mode = ESP_LV_ADAPTER_TEAR_AVOID_MODE_NONE,
    };
    lv_display_t * disp = esp_lv_adapter_register_display(&disp_cfg);
    if(disp == NULL) return NULL;
    lv_display_add_event_cb(disp, rounder_cb, LV_EVENT_INVALIDATE_AREA, NULL);

    /* Touch: mirroring inverted when the UI is turned around */
    bsp_display_cfg_t touch_cfg = {
        .touch_flags = {
            .swap_xy = 0,
            .mirror_x = DISPLAY_ROTATE_180 ? 0 : 1,
            .mirror_y = DISPLAY_ROTATE_180 ? 0 : 1,
        },
    };
    esp_lcd_touch_handle_t tp;
    if(bsp_touch_new(&touch_cfg, &tp) == ESP_OK) {
        const esp_lv_adapter_touch_config_t tcfg = ESP_LV_ADAPTER_TOUCH_DEFAULT_CONFIG(disp, tp);
        if(esp_lv_adapter_register_touch(&tcfg) == NULL) ESP_LOGE(TAG, "Touch registration failed");
    }
    else {
        ESP_LOGE(TAG, "Touch init failed");
    }

    bsp_display_brightness_init();
    if(esp_lv_adapter_start() != ESP_OK) return NULL;
    return disp;
}

void app_main(void)
{
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
#if DISPLAY_ROTATE_180
    s_adapter_flush = disp->flush_cb;       /* wrap the adapter's flush */
    lv_display_set_flush_cb(disp, flush_rotate_180);
#endif
    roundedui_init("");                     /* no file-based assets, everything is compiled in */
    lv_screen_load(lean_screen_create());
    settings_ui_bind();                     /* brightness, G bar, power off */
    imu_ui_bind();
    pmu_ui_bind();
    gps_ui_bind();
    bsp_display_unlock();

    /* Keep still and upright now: 2 s calibration */
    err = imu_task_start(bus);
    if(err != ESP_OK) ESP_LOGE(TAG, "QMI8658: %s", esp_err_to_name(err));

    err = gps_task_start(bus);
    if(err != ESP_OK) ESP_LOGW(TAG, "GPS: %s", esp_err_to_name(err));
}
