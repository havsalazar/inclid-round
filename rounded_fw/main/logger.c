/**
 * @file logger.c
 *
 * One CSV file per recording on the microSD card: /sdcard/RIDE0001.CSV, RIDE0002.CSV, ...
 *
 *   time_s,date,utc,lat,lon,speed_kmh,lean_deg,accel_g,state,fix,sats
 *   12.3,2026-11-13,14:32:05.20,4.711234,-74.072112,87,-32.4,-0.41,BRK,1,14
 *
 * - time_s:   seconds since the recording started
 * - date/utc: from the GPS, empty until its first fix
 * - lat/lon:  decimal degrees, empty without a fix. The GPS updates them about once
 *             a second, so they repeat over ~10 rows
 * - lean_deg: + = right, - = left
 * - accel_g:  longitudinal g, + = accelerating, - = braking
 * - state:    BRK below -LOG_BRK_ACC_CG, ACC above +LOG_BRK_ACC_CG, empty in between
 *
 * The card is mounted when recording starts and unmounted when it stops, so it can
 * be pulled out whenever the button shows the round dot (not recording). Data reaches the card every
 * LOG_SYNC_ROWS rows: a power cut loses at most that much.
 *
 * The SD card has its own SDMMC pins (GPIO1-3): no traffic on the shared I2C bus.
 */
#include "logger.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_timer.h"
#include "esp_log.h"

#include "bsp/esp-bsp.h"            /* bsp_sdcard_mount(), BSP_SD_MOUNT_POINT */
#include "imu_task.h"
#include "gps_task.h"

#include "lvgl.h"
#include "roundedui.h"

#define LOG_PERIOD_MS       100     /* 10 Hz */
#define LOG_SYNC_ROWS       20      /* flush + fsync every 2 s */
#define LOG_BRK_ACC_CG      10      /* |g| above 0.10 g = BRK / ACC */
#define LOG_FILE_BUF        8192
#define LOG_MAX_FILES       9999

/* subject_log_state values, see globals.xml */
enum { LOG_STATE_OFF = 0, LOG_STATE_RECORDING = 1, LOG_STATE_ERROR = 2 };

static const char * TAG = "logger";

static SemaphoreHandle_t s_lock;    /* s_file and the mount */
static FILE * s_file;
static char * s_buf;
static bool s_mounted;
static bool s_shut_down;
static int64_t s_start_us;
static int s_rows_since_sync;

static volatile bool s_toggle_request;
static volatile int s_state = LOG_STATE_OFF;

/*********************
 *  FILE
 *********************/
/* With s_lock held */
static void close_file(void)
{
    if(s_file) {
        fflush(s_file);
        fsync(fileno(s_file));
        fclose(s_file);
        s_file = NULL;
        ESP_LOGI(TAG, "Recording stopped");
    }
    free(s_buf);
    s_buf = NULL;
    if(s_mounted) {
        bsp_sdcard_unmount();
        s_mounted = false;
    }
}

/* With s_lock held */
static bool open_file(void)
{
    if(!s_mounted) {
        esp_err_t err = bsp_sdcard_mount();
        if(err != ESP_OK) {
            ESP_LOGW(TAG, "No SD card (%s)", esp_err_to_name(err));
            return false;
        }
        s_mounted = true;
    }

    char path[40];
    struct stat st;
    int n;
    for(n = 1; n <= LOG_MAX_FILES; n++) {
        snprintf(path, sizeof(path), BSP_SD_MOUNT_POINT "/RIDE%04d.CSV", n);
        if(stat(path, &st) != 0) break;
    }
    if(n > LOG_MAX_FILES) {
        ESP_LOGW(TAG, "Card full of RIDE files");
        close_file();
        return false;
    }

    s_file = fopen(path, "w");
    if(s_file == NULL) {
        ESP_LOGW(TAG, "Can't create %s", path);
        close_file();
        return false;
    }
    s_buf = malloc(LOG_FILE_BUF);
    if(s_buf) setvbuf(s_file, s_buf, _IOFBF, LOG_FILE_BUF);

    fprintf(s_file, "time_s,date,utc,lat,lon,speed_kmh,lean_deg,accel_g,state,fix,sats\n");
    s_start_us = esp_timer_get_time();
    s_rows_since_sync = 0;
    ESP_LOGI(TAG, "Recording to %s", path);
    return true;
}

/* With s_lock held. Returns false if the card stopped taking data. */
static bool write_row(void)
{
    int32_t lean_x10, accel_cg;
    if(!imu_get(&lean_x10, &accel_cg)) lean_x10 = accel_cg = 0;    /* calibrating */
    gps_data_t gps;
    gps_get(&gps);

    const char * state = accel_cg <= -LOG_BRK_ACC_CG ? "BRK" : accel_cg >= LOG_BRK_ACC_CG ? "ACC" : "";
    int64_t t_ms = (esp_timer_get_time() - s_start_us + 50000) / 1000;   /* rounded to 0.1 s below */

    char pos[40] = ",";
    if(gps.fix) snprintf(pos, sizeof(pos), "%.6f,%.6f", gps.lat, gps.lon);

    /* Integer formatting of the tenths: no rounding surprises around 0 */
    int32_t lean_abs = lean_x10 < 0 ? -lean_x10 : lean_x10;
    int32_t g_abs = accel_cg < 0 ? -accel_cg : accel_cg;
    int r = fprintf(s_file, "%lld.%lld,%s,%s,%s,%ld,%s%ld.%ld,%s%ld.%02ld,%s,%d,%d\n",
                    (long long)(t_ms / 1000), (long long)((t_ms % 1000) / 100),
                    gps.date, gps.utc, pos, (long)gps.speed_kmh,
                    lean_x10 < 0 ? "-" : "", (long)(lean_abs / 10), (long)(lean_abs % 10),
                    accel_cg < 0 ? "-" : "", (long)(g_abs / 100), (long)(g_abs % 100),
                    state, gps.fix ? 1 : 0, gps.sats);
    if(r < 0) return false;

    if(++s_rows_since_sync >= LOG_SYNC_ROWS) {
        s_rows_since_sync = 0;
        if(fflush(s_file) != 0) return false;
        if(fsync(fileno(s_file)) != 0) return false;
    }
    return true;
}

/*********************
 *  TASK
 *********************/
static void logger_task(void * arg)
{
    (void)arg;
    TickType_t wake = xTaskGetTickCount();

    while(1) {
        vTaskDelayUntil(&wake, pdMS_TO_TICKS(LOG_PERIOD_MS));

        xSemaphoreTake(s_lock, portMAX_DELAY);
        if(s_shut_down) {
            xSemaphoreGive(s_lock);
            continue;
        }

        if(s_toggle_request) {
            s_toggle_request = false;
            if(s_file) {
                close_file();
                s_state = LOG_STATE_OFF;
            }
            else {
                s_state = open_file() ? LOG_STATE_RECORDING : LOG_STATE_ERROR;
            }
            wake = xTaskGetTickCount();             /* mounting can take a while */
        }

        if(s_file && !write_row()) {
            ESP_LOGW(TAG, "Write failed: card removed or full");
            fclose(s_file);                         /* don't fsync a card that is gone */
            s_file = NULL;
            close_file();
            s_state = LOG_STATE_ERROR;
        }
        xSemaphoreGive(s_lock);
    }
}

esp_err_t logger_start(void)
{
    s_lock = xSemaphoreCreateMutex();
    if(s_lock == NULL) return ESP_ERR_NO_MEM;
    /* Below the IMU (5) and the GPS (3): a slow card write never delays them */
    return xTaskCreatePinnedToCore(logger_task, "logger", 6144, NULL, 2, NULL, 0) == pdPASS
           ? ESP_OK : ESP_ERR_NO_MEM;
}

bool logger_card_acquire(const char ** why)
{
    if(s_lock == NULL) {
        *why = "logger not running";
        return false;
    }
    xSemaphoreTake(s_lock, portMAX_DELAY);
    if(s_file || s_shut_down) {
        *why = s_file ? "recording: stop it first" : "powering off";
        xSemaphoreGive(s_lock);
        return false;
    }
    if(!s_mounted) {
        if(bsp_sdcard_mount() != ESP_OK) {
            *why = "no SD card";
            xSemaphoreGive(s_lock);
            return false;
        }
        s_mounted = true;
    }
    return true;
}

void logger_card_release(void)
{
    close_file();                                   /* no file is open: only unmounts */
    xSemaphoreGive(s_lock);
}

void logger_shutdown(void)
{
    if(s_lock == NULL) return;
    xSemaphoreTake(s_lock, portMAX_DELAY);
    close_file();
    s_shut_down = true;
    s_state = LOG_STATE_OFF;
    xSemaphoreGive(s_lock);
}

/*********************
 *  UI
 *********************/
static void ui_update_cb(lv_timer_t * t)
{
    LV_UNUSED(t);
    lv_subject_set_int(subject_log_state, s_state);
}

/* The record button sets subject_log_request to 1 */
static void log_request_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(observer);
    if(lv_subject_get_int(subject) != 1) return;
    lv_subject_set_int(subject, 0);
    s_toggle_request = true;                        /* the task mounts and opens: not here */
}

void logger_ui_bind(void)
{
    lv_subject_add_observer(subject_log_request, log_request_cb, NULL);
    lv_timer_create(ui_update_cb, 100, NULL);
}
