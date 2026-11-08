/**
 * @file pmu.c
 *
 * AXP2101: battery percentage from its fuel gauge, charging state, power off.
 */
#include "pmu.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_system.h"

#include "i2c_lock.h"
#include "lvgl.h"
#include "roundedui.h"

#define PMU_I2C_ADDR        0x34
#define PMU_I2C_HZ          400000
#define PMU_POLL_MS         2000

/* AXP2101 registers */
#define REG_STATUS1         0x00    /* bit3: battery present */
#define REG_STATUS2         0x01    /* bits 6:5: 01 = charging, bits 2:0: charger state */
#define REG_COMMON_CFG      0x10    /* bit0: power off */
#define REG_FUEL_GAUGE_CTRL 0x18    /* bit3: fuel gauge enable */
#define REG_ADC_CHAN_EN     0x30    /* bit0: battery voltage ADC */
#define REG_BAT_DET_CTRL    0x68    /* bit0: battery detection */
#define REG_BAT_PERCENT     0xA4

static const char * TAG = "pmu";

static i2c_master_dev_handle_t s_dev;

static esp_err_t reg_read(uint8_t reg, uint8_t * val)
{
    i2c_lock();
    esp_err_t err = i2c_master_transmit_receive(s_dev, &reg, 1, val, 1, 50);
    i2c_unlock();
    return err;
}

static esp_err_t reg_write(uint8_t reg, uint8_t val)
{
    uint8_t buf[2] = { reg, val };
    i2c_lock();
    esp_err_t err = i2c_master_transmit(s_dev, buf, sizeof(buf), 50);
    i2c_unlock();
    return err;
}

static esp_err_t reg_set_bits(uint8_t reg, uint8_t bits)
{
    uint8_t v;
    esp_err_t err = reg_read(reg, &v);
    if(err != ESP_OK) return err;
    return reg_write(reg, v | bits);
}

esp_err_t pmu_init(i2c_master_bus_handle_t bus)
{
    i2c_device_config_t cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = PMU_I2C_ADDR,
        .scl_speed_hz = PMU_I2C_HZ,
    };
    esp_err_t err = i2c_master_bus_add_device(bus, &cfg, &s_dev);
    if(err != ESP_OK) return err;

    uint8_t st;
    err = reg_read(REG_STATUS1, &st);
    if(err != ESP_OK) {
        ESP_LOGE(TAG, "AXP2101 not answering at 0x%02X (%s)", PMU_I2C_ADDR, esp_err_to_name(err));
        return err;
    }

    /* The percentage needs battery detection, the voltage ADC and the gauge */
    reg_set_bits(REG_BAT_DET_CTRL, 0x01);
    reg_set_bits(REG_ADC_CHAN_EN, 0x01);
    reg_set_bits(REG_FUEL_GAUGE_CTRL, 0x08);
    ESP_LOGI(TAG, "AXP2101 ready, battery %s", (st & 0x08) ? "present" : "not detected");
    return ESP_OK;
}

static void pmu_timer_cb(lv_timer_t * t)
{
    LV_UNUSED(t);
    if(s_dev == NULL) return;

    uint8_t st1, st2, pct;
    if(reg_read(REG_STATUS1, &st1) != ESP_OK || reg_read(REG_STATUS2, &st2) != ESP_OK) return;

    bool charging = ((st2 >> 5) & 0x03) == 0x01;
    lv_subject_set_int(subject_battery_charging, charging ? 1 : 0);

    if((st1 & 0x08) && reg_read(REG_BAT_PERCENT, &pct) == ESP_OK && pct <= 100) {
        lv_subject_set_int(subject_battery_pct, pct);
    }
}

void pmu_ui_bind(void)
{
    pmu_timer_cb(NULL);                         /* right away, not 2 s after boot */
    lv_timer_create(pmu_timer_cb, PMU_POLL_MS, NULL);
}

void pmu_power_off(void)
{
    ESP_LOGI(TAG, "Power off");
    vTaskDelay(pdMS_TO_TICKS(50));              /* let the log out */
    if(s_dev) reg_set_bits(REG_COMMON_CFG, 0x01);
    vTaskDelay(pdMS_TO_TICKS(500));
    /* Still running: USB power keeps the board on. Restart instead of hanging. */
    ESP_LOGW(TAG, "Still powered (USB?), restarting");
    esp_restart();
}
