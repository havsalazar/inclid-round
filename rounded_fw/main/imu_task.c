/**
 * @file imu_task.c
 *
 * QMI8658 -> lean angle + longitudinal g -> UI subjects.
 *
 * Mounting: "up" is learned during calibration (keep still, mounted as it will
 * be used). Left-right comes from the screen's horizontal axis (IMU_RIGHT), so
 * any screen tilt from flat to upright works. Recalibrate after changing the
 * mounting angle. If left/right AND accelerate/brake both read backwards, flip
 * IMU_RIGHT's sign.
 *
 * Lean: the roll gyro is integrated, and slowly pulled toward
 *   - the gravity direction when riding straight, and
 *   - atan(-yaw_lateral / yaw_vertical) while turning. In a steady turn the
 *     accelerometer only sees "down" through the vehicle, so it reads ~0 lean
 *     even at 45 deg; the split of the turn rate between the gyro axes is what
 *     reveals the real lean.
 */
#include "imu_task.h"

#include <math.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "esp_log.h"

#include "i2c_lock.h"
#include "lvgl.h"
#include "roundedui.h"

/*********************
 *  CONFIGURATION
 *********************/
#define IMU_I2C_ADDR        0x6B    /* 0x6A if SA0 is low */
#define IMU_I2C_HZ          400000

/* Board axis along the screen's horizontal (left-right across the display).
 * It stays horizontal whatever angle the screen is mounted at, from flat on the
 * tank to upright, so forward is derived from it and "up". +Y here: with the
 * screen upright the calibration measures up along -X. */
#define IMU_RIGHT           { 0.0f, 1.0f, 0.0f }

#define IMU_LEAN_SIGN       -1      /* -1 swaps left and right lean only (flipped: read inverted on the bike mount) */
#define IMU_ACCEL_SIGN      1       /* -1 swaps accelerating and braking only */

#define IMU_RATE_HZ         100
#define IMU_CAL_SAMPLES     200     /* 2 s */
#define IMU_FUSION_K        0.01f   /* pull toward the reference per sample, ~1 s time constant */
#define IMU_TURN_DPS        8.0f    /* above this turn rate the gyro split is the lean reference */
#define IMU_G_FILTER        0.15f   /* low-pass on the g value shown */
#define IMU_G_DEADBAND_CG   3       /* below 0.03 g the G bar stays at 0 */
#define UI_TEXT_PERIOD_MS   100     /* the numbers update at 10 Hz, the bars at 50 Hz */

/* QMI8658 registers */
#define REG_WHO_AM_I        0x00    /* reads 0x05 */
#define REG_CTRL1           0x02
#define REG_CTRL2           0x03    /* accel range and rate */
#define REG_CTRL3           0x04    /* gyro range and rate */
#define REG_CTRL5           0x06    /* low-pass filters */
#define REG_CTRL7           0x08    /* sensor enable */
#define REG_AX_L            0x35    /* AX, AY, AZ, GX, GY, GZ, 16 bit little endian */
#define REG_RESET           0x60

#define ACCEL_LSB_PER_G     8192.0f /* +-4 g */
#define GYRO_LSB_PER_DPS    64.0f   /* +-512 dps */
#define RAD2DEG             57.29578f

static const char * TAG = "imu";

/*********************
 *  STATE
 *********************/
static i2c_master_dev_handle_t s_dev;

static portMUX_TYPE s_mux = portMUX_INITIALIZER_UNLOCKED;
static int32_t s_lean_x10;          /* tenths of a degree */
static int32_t s_accel_cg;
static volatile bool s_cal_request = true;
static volatile bool s_calibrating = true;

typedef struct { float x, y, z; } vec3_t;

static float v_dot(vec3_t a, vec3_t b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
static vec3_t v_scale(vec3_t a, float s) { return (vec3_t){ a.x * s, a.y * s, a.z * s }; }
static vec3_t v_sub(vec3_t a, vec3_t b) { return (vec3_t){ a.x - b.x, a.y - b.y, a.z - b.z }; }
static vec3_t v_cross(vec3_t a, vec3_t b)
{
    return (vec3_t){ a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x };
}
static vec3_t v_norm(vec3_t a)
{
    float l = sqrtf(v_dot(a, a));
    return l > 1e-6f ? v_scale(a, 1.0f / l) : a;
}

/*********************
 *  QMI8658
 *********************/
static esp_err_t reg_write(uint8_t reg, uint8_t val)
{
    uint8_t buf[2] = { reg, val };
    i2c_lock();
    esp_err_t err = i2c_master_transmit(s_dev, buf, sizeof(buf), 50);
    i2c_unlock();
    return err;
}

static esp_err_t reg_read(uint8_t reg, uint8_t * buf, size_t len)
{
    i2c_lock();
    esp_err_t err = i2c_master_transmit_receive(s_dev, &reg, 1, buf, len, 50);
    i2c_unlock();
    return err;
}

static esp_err_t qmi_init(void)
{
    uint8_t who = 0;
    esp_err_t err = reg_read(REG_WHO_AM_I, &who, 1);
    if(err != ESP_OK) {
        ESP_LOGE(TAG, "QMI8658 not answering at 0x%02X (%s)", IMU_I2C_ADDR, esp_err_to_name(err));
        return err;
    }
    ESP_LOGI(TAG, "WHO_AM_I = 0x%02X%s", who, who == 0x05 ? "" : " (expected 0x05)");

    reg_write(REG_RESET, 0xB0);                     /* soft reset */
    vTaskDelay(pdMS_TO_TICKS(20));
    err  = reg_write(REG_CTRL1, 0x40);              /* address auto-increment, little endian */
    err |= reg_write(REG_CTRL2, 0x16);              /* +-4 g, ~112 Hz */
    err |= reg_write(REG_CTRL3, 0x56);              /* +-512 dps, ~112 Hz */
    err |= reg_write(REG_CTRL5, 0x11);              /* accel and gyro low-pass on */
    err |= reg_write(REG_CTRL7, 0x03);              /* accel + gyro on */
    vTaskDelay(pdMS_TO_TICKS(300));                 /* gyro start-up: early samples would skew the bias */
    return err == ESP_OK ? ESP_OK : ESP_FAIL;
}

/** Accel in g and gyro in deg/s, in the sensor frame */
static esp_err_t qmi_read(vec3_t * acc, vec3_t * gyr)
{
    uint8_t b[12];
    esp_err_t err = reg_read(REG_AX_L, b, sizeof(b));
    if(err != ESP_OK) return err;

#define RAW(i) ((int16_t)(b[i] | (b[(i) + 1] << 8)))
    acc->x = RAW(0) / ACCEL_LSB_PER_G;
    acc->y = RAW(2) / ACCEL_LSB_PER_G;
    acc->z = RAW(4) / ACCEL_LSB_PER_G;
    gyr->x = RAW(6) / GYRO_LSB_PER_DPS;
    gyr->y = RAW(8) / GYRO_LSB_PER_DPS;
    gyr->z = RAW(10) / GYRO_LSB_PER_DPS;
#undef RAW
    return ESP_OK;
}

/*********************
 *  TASK
 *********************/
static void imu_task(void * arg)
{
    LV_UNUSED(arg);

    vec3_t gyro_bias = { 0 };
    vec3_t ax_fwd = { 0 }, ax_right = { 0 }, ax_up = { 0 };
    float roll = 0.0f, accel_g = 0.0f;
    int64_t last_us = esp_timer_get_time();
    TickType_t wake = xTaskGetTickCount();

    while(1) {
        vTaskDelayUntil(&wake, pdMS_TO_TICKS(1000 / IMU_RATE_HZ));

        if(s_cal_request) {
            s_calibrating = true;
            ESP_LOGI(TAG, "Calibrating: keep still and upright for 2 s");
            vec3_t sum_a = { 0 }, sum_g = { 0 };
            int n = 0;
            while(n < IMU_CAL_SAMPLES) {
                vec3_t a, g;
                vTaskDelayUntil(&wake, pdMS_TO_TICKS(1000 / IMU_RATE_HZ));
                if(qmi_read(&a, &g) != ESP_OK) continue;
                sum_a = (vec3_t){ sum_a.x + a.x, sum_a.y + a.y, sum_a.z + a.z };
                sum_g = (vec3_t){ sum_g.x + g.x, sum_g.y + g.y, sum_g.z + g.z };
                n++;
            }
            gyro_bias = v_scale(sum_g, 1.0f / n);
            ax_up = v_norm(sum_a);                  /* at rest the accelerometer reads +1 g "up" */

            vec3_t right = IMU_RIGHT;
            if(fabsf(v_dot(v_norm(right), ax_up)) > 0.7f) {
                ESP_LOGW(TAG, "The screen is tilted sideways by more than 45 deg: mount it level left-right");
            }
            ax_right = v_norm(v_sub(right, v_scale(ax_up, v_dot(right, ax_up))));   /* level it */
            ax_fwd = v_cross(ax_up, ax_right);

            roll = accel_g = 0.0f;
            last_us = esp_timer_get_time();
            s_cal_request = false;
            s_calibrating = false;
            ESP_LOGI(TAG, "Calibrated: up=(%.2f %.2f %.2f) gyro bias=(%.2f %.2f %.2f) dps",
                     ax_up.x, ax_up.y, ax_up.z, gyro_bias.x, gyro_bias.y, gyro_bias.z);
            continue;
        }

        vec3_t a, g;
        if(qmi_read(&a, &g) != ESP_OK) continue;
        g = v_sub(g, gyro_bias);

        int64_t now_us = esp_timer_get_time();
        float dt = (now_us - last_us) / 1e6f;
        last_us = now_us;
        if(dt <= 0.0f || dt > 0.1f) dt = 1.0f / IMU_RATE_HZ;

        float a_fwd = v_dot(a, ax_fwd), a_right = v_dot(a, ax_right), a_up = v_dot(a, ax_up);
        float w_fwd = v_dot(g, ax_fwd), w_right = v_dot(g, ax_right), w_up = v_dot(g, ax_up);

        roll += w_fwd * dt;

        /* Turning: the yaw rate splits between the up and right axes by the lean.
         * Only trusted when the vertical part alone is a real turn rate; otherwise a
         * pitch movement (tilting the screen, braking dive) has a big w_right and a
         * tiny w_up, which would read as a huge lean. */
        float ref = atan2f(-a_right, a_up) * RAD2DEG;   /* gravity direction */
        if(fabsf(w_up) > IMU_TURN_DPS) {
            float turn_ref = atanf(-w_right / w_up) * RAD2DEG;
            if(fabsf(turn_ref) < 65.0f) ref = turn_ref;
        }
        roll += IMU_FUSION_K * (ref - roll);

        accel_g += IMU_G_FILTER * (a_fwd - accel_g);

        taskENTER_CRITICAL(&s_mux);
        s_lean_x10 = IMU_LEAN_SIGN * (int32_t)lroundf(roll * 10.0f);
        s_accel_cg = IMU_ACCEL_SIGN * (int32_t)lroundf(accel_g * 100.0f);
        taskEXIT_CRITICAL(&s_mux);
    }
}

esp_err_t imu_task_start(i2c_master_bus_handle_t bus)
{
    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = IMU_I2C_ADDR,
        .scl_speed_hz = IMU_I2C_HZ,
    };
    esp_err_t err = i2c_master_bus_add_device(bus, &dev_cfg, &s_dev);
    if(err != ESP_OK) return err;

    err = qmi_init();
    if(err != ESP_OK) {
        s_calibrating = false;                      /* don't show HOLD STILL forever */
        return err;
    }
    return xTaskCreatePinnedToCore(imu_task, "imu", 4096, NULL, 5, NULL, 0) == pdPASS ? ESP_OK : ESP_ERR_NO_MEM;
}

/*********************
 *  UI
 *********************/
static void ui_update_cb(lv_timer_t * t)
{
    LV_UNUSED(t);
    int32_t lean_x10, accel;
    taskENTER_CRITICAL(&s_mux);
    lean_x10 = s_lean_x10;
    accel = s_accel_cg;
    taskEXIT_CRITICAL(&s_mux);

    bool calibrating = s_calibrating;
    lv_subject_set_int(subject_imu_calibrating, calibrating ? 1 : 0);
    if(calibrating) lean_x10 = accel = 0;

    /* Bars in tenths of a degree, numbers in whole degrees */
    int32_t lean = (lean_x10 + (lean_x10 >= 0 ? 5 : -5)) / 10;
    if(accel > -IMU_G_DEADBAND_CG && accel < IMU_G_DEADBAND_CG) accel = 0;

    /* Bars every tick (50 Hz) for smooth motion */
    lv_subject_set_int(subject_lean_x10, lean_x10);
    lv_subject_set_int(subject_accel_cg, accel);

    /* Numbers at 10 Hz: easier to read than digits changing 50 times a second, and
     * redrawing the 110 px number was the biggest cost of a frame */
    static uint32_t s_last_text_ms;
    if(lv_tick_elaps(s_last_text_ms) >= UI_TEXT_PERIOD_MS) {
        s_last_text_ms = lv_tick_get();
        lv_subject_set_int(subject_lean_deg, lean);
        lv_subject_set_int(subject_lean_abs_deg, lean < 0 ? -lean : lean);
        lv_subject_set_int(subject_g_decel_cg, accel < 0 ? -accel : 0);
        lv_subject_set_int(subject_g_accel_cg, accel > 0 ? accel : 0);
    }

    /* The UI resets these to 0 on a long press, so compare against the subject itself */
    if(-lean > lv_subject_get_int(subject_lean_max_left))  lv_subject_set_int(subject_lean_max_left, -lean);
    if(lean  > lv_subject_get_int(subject_lean_max_right)) lv_subject_set_int(subject_lean_max_right, lean);
}

/* CALIBRATE INCLINOMETER sets subject_imu_cal_request to 1 */
static void cal_request_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(observer);
    if(lv_subject_get_int(subject) != 1) return;
    lv_subject_set_int(subject, 0);
    if(s_dev == NULL) return;
    ESP_LOGI(TAG, "Calibration requested from the screen");
    s_calibrating = true;                           /* show HOLD STILL right away */
    s_cal_request = true;
}

void imu_ui_bind(void)
{
    lv_subject_add_observer(subject_imu_cal_request, cal_request_cb, NULL);
    lv_timer_create(ui_update_cb, 20, NULL);   /* 50 Hz */
}

void imu_sleep(void)
{
    if(s_dev) reg_write(REG_CTRL7, 0x00);           /* all sensors off */
}
