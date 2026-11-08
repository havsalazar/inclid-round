/**
 * @file gps_task.c
 *
 * LC76G over I2C, using the handshake of Waveshare's 09_LC76G_I2C demo:
 *   1. write {08 00 51 AA 04 00 00 00} to 0x50, read 4 bytes from 0x54: bytes waiting (LE)
 *   2. write {00 20 51 AA} + those 4 bytes to 0x50, read that many bytes from 0x54: NMEA text
 *
 * The module's I2C side gets stuck for good (0x54 never answers again until the
 * next reset) if it sees traffic for other devices between its reset and its first
 * transaction. So the bus lock is held from the reset pulse until the first
 * successful read (gps_boot()). Once it is talking, other traffic is fine.
 *
 * Only $GxRMC is used: fix status and speed over ground.
 *
 * CALIBRATE GPS drops the current fix and shows SEARCHING GPS... until a fresh
 * valid RMC arrives (or GPS_SEARCH_TIMEOUT_MS passes). It does not send commands
 * to the receiver.
 */
#include "gps_task.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "esp_log.h"

#include "bsp/esp-bsp.h"            /* bsp_io_expander_init(): the TCA9554 */
#include "i2c_lock.h"

#include "lvgl.h"
#include "roundedui.h"

#define GPS_RST_EXIO            IO_EXPANDER_PIN_NUM_7
#define GPS_BOOT_MS             1500    /* reset released -> first transaction attempt */
#define GPS_BOOT_TRIES          30      /* then one attempt every 100 ms, bus still locked */

#define GPS_ADDR_W              0x50
#define GPS_ADDR_R              0x54
#define GPS_I2C_HZ              100000
#define GPS_POLL_MS             500
#define GPS_MAX_READ            2048
#define GPS_CHUNK               512     /* bytes per transaction: ~46 ms at 100 kHz */
#define GPS_STEP_MS             10      /* Quectel: >= 10 ms between the steps */
#define GPS_FAILS_BEFORE_RESET  10      /* consecutive failed polls (~5 s) -> reset the module */
#define GPS_SCL_WAIT_US         300000  /* clock stretching allowance, see gps_task_start() */
#define GPS_FIX_TIMEOUT_US      (3 * 1000000LL)     /* no valid RMC for 3 s = no fix */
#define GPS_SEARCH_TIMEOUT_MS   120000
#define GPS_SPEED_DEADBAND_KMH  3                   /* GPS jitter when standing still */

static const char * TAG = "gps";

static i2c_master_dev_handle_t s_w, s_r;            /* 0x50 command, 0x54 read */
static esp_io_expander_handle_t s_expander;

static portMUX_TYPE s_mux = portMUX_INITIALIZER_UNLOCKED;
static int32_t s_speed_kmh;
static int64_t s_last_fix_us;                       /* 0 = never */
static volatile bool s_searching;
static int64_t s_search_start_us;
static int64_t s_last_ok_us;                        /* last transaction the module answered, 0 = never */

/* Satellites in view, from the GSV sentences: one count per constellation
 * ($GPGSV GPS, $GLGSV GLONASS, $GAGSV Galileo, $GBGSV BeiDou, $GQGSV QZSS) */
#define GPS_MAX_TALKERS         6
static struct {
    char talker[2];
    int count;
    int64_t time_us;
} s_gsv[GPS_MAX_TALKERS];

/*********************
 *  NMEA
 *********************/
static bool nmea_checksum_ok(const char * s)
{
    if(s[0] != '$') return false;
    const char * star = strchr(s, '*');
    if(star == NULL || strlen(star) < 3) return false;
    uint8_t sum = 0;
    for(const char * p = s + 1; p < star; p++) sum ^= (uint8_t)*p;
    return sum == (uint8_t)strtol(star + 1, NULL, 16);
}

/** Field n (0 = sentence id) of a comma separated sentence, copied into out */
static void nmea_field(const char * s, int n, char * out, size_t out_len)
{
    while(n > 0 && *s) {
        if(*s == ',') n--;
        s++;
    }
    size_t i = 0;
    while(*s && *s != ',' && *s != '*' && i + 1 < out_len) out[i++] = *s++;
    out[i] = '\0';
}

static void mark_answered(void)
{
    int64_t now = esp_timer_get_time();
    taskENTER_CRITICAL(&s_mux);
    s_last_ok_us = now;
    taskEXIT_CRITICAL(&s_mux);
}

/* $xxGSV,<messages>,<message number>,<satellites in view>,... */
static void nmea_gsv(const char * line)
{
    char field[8];
    nmea_field(line, 3, field, sizeof(field));
    int count = atoi(field);
    int64_t now = esp_timer_get_time();

    int slot = -1, oldest = 0;
    for(int i = 0; i < GPS_MAX_TALKERS; i++) {
        if(s_gsv[i].talker[0] == line[1] && s_gsv[i].talker[1] == line[2]) { slot = i; break; }
        if(s_gsv[i].time_us < s_gsv[oldest].time_us) oldest = i;
    }
    if(slot < 0) slot = oldest;

    taskENTER_CRITICAL(&s_mux);
    s_gsv[slot].talker[0] = line[1];
    s_gsv[slot].talker[1] = line[2];
    s_gsv[slot].count = count;
    s_gsv[slot].time_us = now;
    taskEXIT_CRITICAL(&s_mux);
}

static void nmea_line(const char * line)
{
    if(strlen(line) < 7 || !nmea_checksum_ok(line)) return;
    if(strncmp(line + 3, "GSV", 3) == 0) {
        nmea_gsv(line);
        return;
    }
    if(strncmp(line + 3, "RMC", 3) != 0) return;   /* $GPRMC, $GNRMC ... */

    char status[4], knots[16];
    nmea_field(line, 2, status, sizeof(status));
    nmea_field(line, 7, knots, sizeof(knots));
    if(status[0] != 'A') return;                    /* V = no valid fix */

    int32_t kmh = (int32_t)lroundf(strtof(knots, NULL) * 1.852f);
    if(kmh < GPS_SPEED_DEADBAND_KMH) kmh = 0;

    taskENTER_CRITICAL(&s_mux);
    s_speed_kmh = kmh;
    s_last_fix_us = esp_timer_get_time();
    taskEXIT_CRITICAL(&s_mux);
    s_searching = false;
}


/* Split the byte stream into NMEA lines */
static void nmea_feed(const uint8_t * buf, int n)
{
    static char line[128];
    static size_t line_len;

    for(int i = 0; i < n; i++) {
        char c = (char)buf[i];
        if(c == '$') line_len = 0;
        if(c == '\r' || c == '\n') {
            if(line_len > 0) {
                line[line_len] = '\0';
                nmea_line(line);
                line_len = 0;
            }
        }
        else if(line_len + 1 < sizeof(line)) {
            line[line_len++] = c;
        }
    }
}

/*********************
 *  LC76G
 *********************/
/* One length-request + data-read transaction: separate frames per step, >= 10 ms
 * between steps, and the bus lock held by the caller so no other device talks in
 * between. Returns bytes read, 0 if none waiting, or -step on failure. */
static int gps_transaction(uint8_t * buf, size_t max)
{
    static const uint8_t req_len[] = { 0x08, 0x00, 0x51, 0xAA, 0x04, 0x00, 0x00, 0x00 };
    uint8_t len_le[4];

    if(i2c_master_transmit(s_w, req_len, sizeof(req_len), 100) != ESP_OK) return -1;
    vTaskDelay(pdMS_TO_TICKS(GPS_STEP_MS));
    if(i2c_master_receive(s_r, len_le, sizeof(len_le), 500) != ESP_OK) return -2;

    uint32_t len = len_le[0] | (len_le[1] << 8) | (len_le[2] << 16) | ((uint32_t)len_le[3] << 24);
    if(len == 0) return 0;
    if(len > max) len = max;

    uint8_t req_data[8] = { 0x00, 0x20, 0x51, 0xAA,
                            len & 0xFF, (len >> 8) & 0xFF, (len >> 16) & 0xFF, (len >> 24) & 0xFF };
    vTaskDelay(pdMS_TO_TICKS(GPS_STEP_MS));
    if(i2c_master_transmit(s_w, req_data, sizeof(req_data), 100) != ESP_OK) return -3;
    vTaskDelay(pdMS_TO_TICKS(GPS_STEP_MS));
    if(i2c_master_receive(s_r, buf, len, 500) != ESP_OK) return -4;
    return (int)len;
}

static int gps_read(uint8_t * buf, size_t max)
{
    if(max > GPS_CHUNK) max = GPS_CHUNK;    /* keep the bus lock short for the IMU */
    i2c_lock();
    int n = gps_transaction(buf, max);
    i2c_unlock();
    return n;
}

/* Reset the module (EXIO7 of the TCA9554 = GPS_RST, active low) and keep the bus
 * to ourselves until its first successful transaction: see the top of the file.
 * The IMU waits meanwhile (about 1.5 s). Returns true when the module answers. */
static bool gps_boot(uint8_t * buf, size_t max)
{
    if(s_expander == NULL) s_expander = bsp_io_expander_init();
    if(s_expander == NULL) {
        ESP_LOGW(TAG, "TCA9554 not found: can't reset the GPS");
        return false;
    }

    i2c_lock();
    esp_io_expander_set_dir(s_expander, GPS_RST_EXIO, IO_EXPANDER_OUTPUT);
    esp_io_expander_set_level(s_expander, GPS_RST_EXIO, 0);
    vTaskDelay(pdMS_TO_TICKS(50));
    esp_io_expander_set_level(s_expander, GPS_RST_EXIO, 1);
    vTaskDelay(pdMS_TO_TICKS(GPS_BOOT_MS));

    int n = -1;
    for(int i = 0; i < GPS_BOOT_TRIES && n < 0; i++) {
        n = gps_transaction(buf, max > GPS_CHUNK ? GPS_CHUNK : max);
        if(n < 0) vTaskDelay(pdMS_TO_TICKS(100));
    }
    i2c_unlock();

    if(n > 0) nmea_feed(buf, n);
    if(n < 0) ESP_LOGW(TAG, "LC76G not answering after reset (step %d)", -n);
    else ESP_LOGI(TAG, "LC76G answering");
    if(n >= 0) mark_answered();
    return n >= 0;
}

static void gps_task(void * arg)
{
    LV_UNUSED(arg);
    static uint8_t rx[GPS_MAX_READ];
    int fails = 0;

    gps_boot(rx, sizeof(rx));

    while(1) {
        vTaskDelay(pdMS_TO_TICKS(GPS_POLL_MS));
        /* Drain what is waiting in chunks, releasing the bus lock between them so
         * the module's buffer doesn't fill up (it goes to sleep if not read in time) */
        int n, chunks = 0;
        do {
            n = gps_read(rx, sizeof(rx));
            if(n > 0) nmea_feed(rx, n);
            if(n > 0) vTaskDelay(pdMS_TO_TICKS(GPS_STEP_MS));
        } while(n == GPS_CHUNK && ++chunks < 8);

        if(n >= 0) {
            mark_answered();
            fails = 0;
            continue;
        }
        if(++fails >= GPS_FAILS_BEFORE_RESET) {
            ESP_LOGW(TAG, "LC76G stopped answering (step %d): resetting it", -n);
            gps_boot(rx, sizeof(rx));
            fails = 0;
        }
    }
}

esp_err_t gps_task_start(i2c_master_bus_handle_t bus)
{
    i2c_device_config_t cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = GPS_ADDR_W,
        .scl_speed_hz = GPS_I2C_HZ,
        /* The LC76G may hold SCL low while it prepares data. ESP-IDF's default
         * hardware SCL timeout is 2 ms, after which the transfer is aborted and
         * reported as ESP_ERR_INVALID_RESPONSE, exactly like a NACK. */
        .scl_wait_us = GPS_SCL_WAIT_US,
    };
    esp_err_t err = i2c_master_bus_add_device(bus, &cfg, &s_w);
    if(err != ESP_OK) return err;
    cfg.device_address = GPS_ADDR_R;
    err = i2c_master_bus_add_device(bus, &cfg, &s_r);
    if(err != ESP_OK) return err;

    return xTaskCreatePinnedToCore(gps_task, "gps", 4096, NULL, 3, NULL, 0) == pdPASS ? ESP_OK : ESP_ERR_NO_MEM;
}


/*********************
 *  UI
 *********************/
static void ui_update_cb(lv_timer_t * t)
{
    LV_UNUSED(t);
    int32_t kmh;
    int64_t last, last_ok;
    int sats = 0;
    int64_t now = esp_timer_get_time();
    taskENTER_CRITICAL(&s_mux);
    kmh = s_speed_kmh;
    last = s_last_fix_us;
    last_ok = s_last_ok_us;
    for(int i = 0; i < GPS_MAX_TALKERS; i++) {
        if(s_gsv[i].time_us != 0 && now - s_gsv[i].time_us < GPS_FIX_TIMEOUT_US) sats += s_gsv[i].count;
    }
    taskEXIT_CRITICAL(&s_mux);

    bool fix = !s_searching && last != 0 && now - last < GPS_FIX_TIMEOUT_US;
    bool answering = last_ok != 0 && now - last_ok < GPS_FIX_TIMEOUT_US;
    lv_subject_set_int(subject_gps_sats, answering ? sats : -1);

    if(s_searching && (now - s_search_start_us) / 1000 > GPS_SEARCH_TIMEOUT_MS) s_searching = false;

    lv_subject_set_int(subject_gps_fix, fix ? 1 : 0);
    lv_subject_set_int(subject_speed_kmh, fix ? kmh : 0);
    lv_subject_set_int(subject_gps_calibrating, s_searching ? 1 : 0);
}

/* CALIBRATE GPS sets subject_gps_cal_request to 1 */
static void cal_request_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(observer);
    if(lv_subject_get_int(subject) != 1) return;
    lv_subject_set_int(subject, 0);
    ESP_LOGI(TAG, "GPS calibration requested: waiting for a fresh fix");
    s_search_start_us = esp_timer_get_time();
    s_searching = true;
    lv_subject_set_int(subject_gps_calibrating, 1);
}

void gps_ui_bind(void)
{
    lv_subject_add_observer(subject_gps_cal_request, cal_request_cb, NULL);
    lv_timer_create(ui_update_cb, 250, NULL);
}
