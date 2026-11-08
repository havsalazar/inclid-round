/**
 * @file display.c
 *
 * Display start-up for the Waveshare ESP32-S3-Touch-AMOLED-1.75. It does what the
 * BSP's bsp_display_start() does, with three differences:
 *   - The panel's init sequence leaves brightness at 0 and the output off. The
 *     BSP's sets full brightness and switches the output on while the panel memory
 *     still holds garbage, which showed as a white flash until LVGL's first frame.
 *   - LVGL's draw buffers are in internal RAM (the BSP hard-codes PSRAM, which made
 *     software rendering much slower), and the LVGL task runs on core 1.
 *   - The UI can be shown rotated 180 degrees (DISPLAY_ROTATE_180).
 * Pins, panel driver and the rest of the init sequence are the BSP's.
 */
#include "display.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_check.h"
#include "driver/spi_master.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_co5300.h"

#include "lvgl_private.h"   /* lv_display_t::flush_cb, to wrap the adapter's flush */
#include "bsp/esp-bsp.h"
#include "bsp/display.h"
#include "bsp/touch.h"

static const char * TAG = "display";

/* 1: show the UI upside down compared to Waveshare's default orientation.
 * The display adapter can't rotate this panel, so each rendered chunk is turned
 * 180 degrees in place before it's sent, and the touch coordinates are mirrored. */
#define DISPLAY_ROTATE_180  1

/* Lines per LVGL draw buffer. Two buffers of 466 x 116 x 2 bytes = ~216 KB internal RAM. */
#define DRAW_BUF_LINES      116

#define FADE_IN_MS          250
#define FADE_STEPS          10

/* The BSP's init sequence, except 0x51 (brightness) = 0 instead of 0xFF and without
 * 0x29 (display on): the panel stays dark until display_show(). */
static const co5300_lcd_init_cmd_t s_init_cmds[] = {
    {0xFE, (uint8_t[]){0x20}, 1, 0},
    {0x19, (uint8_t[]){0x10}, 1, 0},
    {0x1C, (uint8_t[]){0xA0}, 1, 0},
    {0xFE, (uint8_t[]){0x00}, 1, 0},
    {0xC4, (uint8_t[]){0x80}, 1, 0},
    {0x3A, (uint8_t[]){0x55}, 1, 0},
    {0x35, (uint8_t[]){0x00}, 1, 0},
    {0x53, (uint8_t[]){0x20}, 1, 0},
    {0x51, (uint8_t[]){0x00}, 1, 0},
    {0x63, (uint8_t[]){0xFF}, 1, 0},
    {0x2A, (uint8_t[]){0x00, 0x06, 0x01, 0xD7}, 4, 0},
    {0x2B, (uint8_t[]){0x00, 0x00, 0x01, 0xD1}, 4, 0},
    {0x11, NULL, 0, 120},                           /* sleep out */
};

static lv_display_t * s_disp;
static esp_lcd_panel_handle_t s_panel;
static esp_lcd_panel_io_handle_t s_io;
static lv_display_flush_cb_t s_adapter_flush;
static int s_brightness_pct = 75;
static bool s_shown;

/*********************
 *  PANEL
 *********************/
static esp_err_t panel_write_brightness(int pct)
{
    if(pct < 0) pct = 0;
    if(pct > 100) pct = 100;
    uint8_t level = (uint8_t)(pct * 255 / 100);
    /* QSPI command framing used by the CO5300 driver: 0x02, then the command in bits 15..8 */
    uint32_t cmd = (0x02u << 24) | (0x51u << 8);
    return esp_lcd_panel_io_tx_param(s_io, cmd, &level, 1);
}

static esp_err_t panel_init(void)
{
    const spi_bus_config_t buscfg = CO5300_PANEL_BUS_QSPI_CONFIG(BSP_LCD_PCLK, BSP_LCD_DATA0, BSP_LCD_DATA1,
                                                                 BSP_LCD_DATA2, BSP_LCD_DATA3,
                                                                 BSP_LCD_H_RES * DRAW_BUF_LINES * BSP_LCD_BITS_PER_PIXEL / 8);
    ESP_RETURN_ON_ERROR(spi_bus_initialize(BSP_LCD_SPI_NUM, &buscfg, SPI_DMA_CH_AUTO), TAG, "SPI bus");

    esp_lcd_panel_io_spi_config_t io_cfg = CO5300_PANEL_IO_QSPI_CONFIG(BSP_LCD_CS, NULL, NULL);
    io_cfg.trans_queue_depth = CONFIG_BSP_LCD_TRANS_QUEUE_DEPTH;
    ESP_RETURN_ON_ERROR(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)BSP_LCD_SPI_NUM, &io_cfg, &s_io),
                        TAG, "panel IO");

    co5300_vendor_config_t vendor = {
        .init_cmds = s_init_cmds,
        .init_cmds_size = sizeof(s_init_cmds) / sizeof(s_init_cmds[0]),
        .flags = { .use_qspi_interface = 1 },
    };
    const esp_lcd_panel_dev_config_t panel_cfg = {
        .reset_gpio_num = BSP_LCD_RST,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = BSP_LCD_BITS_PER_PIXEL,
        .vendor_config = &vendor,
    };
    ESP_RETURN_ON_ERROR(esp_lcd_new_panel_co5300(s_io, &panel_cfg, &s_panel), TAG, "panel");
    esp_lcd_panel_set_gap(s_panel, 0x06, 0);
    esp_lcd_panel_reset(s_panel);
    esp_lcd_panel_init(s_panel);
    esp_lcd_panel_disp_on_off(s_panel, false);      /* output off until display_show() */
    return ESP_OK;
}

/*********************
 *  LVGL HOOKS
 *********************/
/* The CO5300 needs even start and odd end coordinates (BSP's rounder, static there) */
static void rounder_cb(lv_event_t * e)
{
    lv_area_t * a = lv_event_get_param(e);
    a->x1 &= ~1;
    a->y1 &= ~1;
    a->x2 |= 1;
    a->y2 |= 1;
}

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
 *  PUBLIC
 *********************/
lv_display_t * display_start(void)
{
    esp_lv_adapter_config_t lv_cfg = ESP_LV_ADAPTER_DEFAULT_CONFIG();
    lv_cfg.task_core_id = 1;                        /* IMU and GPS tasks stay on core 0 */
    if(esp_lv_adapter_init(&lv_cfg) != ESP_OK) return NULL;

    if(panel_init() != ESP_OK) return NULL;

    esp_lv_adapter_display_config_t disp_cfg = {
        .panel = s_panel,
        .panel_io = s_io,
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
    s_disp = esp_lv_adapter_register_display(&disp_cfg);
    if(s_disp == NULL) return NULL;
    lv_display_add_event_cb(s_disp, rounder_cb, LV_EVENT_INVALIDATE_AREA, NULL);
#if DISPLAY_ROTATE_180
    s_adapter_flush = s_disp->flush_cb;             /* wrap the adapter's flush */
    lv_display_set_flush_cb(s_disp, flush_rotate_180);
#endif

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
        const esp_lv_adapter_touch_config_t tcfg = ESP_LV_ADAPTER_TOUCH_DEFAULT_CONFIG(s_disp, tp);
        if(esp_lv_adapter_register_touch(&tcfg) == NULL) ESP_LOGE(TAG, "Touch registration failed");
    }
    else {
        ESP_LOGE(TAG, "Touch init failed");
    }

    if(esp_lv_adapter_start() != ESP_OK) return NULL;
    return s_disp;
}

/* Set when the LVGL task finishes a refresh (all chunks rendered and flushed) */
static volatile bool s_frame_done;

static void refr_ready_cb(lv_event_t * e)
{
    LV_UNUSED(e);
    s_frame_done = true;
}

void display_show(void)
{
    /* Write the whole first frame into the panel's memory while it is still off.
     * The LVGL task renders it (rendering needs its big stack, not this task's):
     * invalidate everything and wait for the end of that refresh. */
    bsp_display_lock(-1);
    s_frame_done = false;
    lv_display_add_event_cb(s_disp, refr_ready_cb, LV_EVENT_REFR_READY, NULL);
    lv_obj_invalidate(lv_screen_active());
    bsp_display_unlock();
    for(int i = 0; i < 100 && !s_frame_done; i++) vTaskDelay(pdMS_TO_TICKS(10));
    bsp_display_lock(-1);
    lv_display_remove_event_cb_with_user_data(s_disp, refr_ready_cb, NULL);
    bsp_display_unlock();
    if(!s_frame_done) ESP_LOGW(TAG, "First frame took over 1 s, switching on anyway");
    vTaskDelay(pdMS_TO_TICKS(20));                  /* last chunk's DMA */

    bsp_display_lock(-1);
    esp_lcd_panel_disp_on_off(s_panel, true);
    bsp_display_unlock();

    /* Fade up instead of popping on */
    for(int i = 1; i <= FADE_STEPS; i++) {
        bsp_display_lock(-1);
        panel_write_brightness(s_brightness_pct * i / FADE_STEPS);
        bsp_display_unlock();
        vTaskDelay(pdMS_TO_TICKS(FADE_IN_MS / FADE_STEPS));
    }
    bsp_display_lock(-1);
    s_shown = true;
    panel_write_brightness(s_brightness_pct);       /* in case it changed during the fade */
    bsp_display_unlock();
}

void display_set_brightness(int pct)
{
    s_brightness_pct = pct;
    if(s_shown || pct == 0) panel_write_brightness(pct);
}
