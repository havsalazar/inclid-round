/**
 * @file i2c_lock.c
 */
#include "i2c_lock.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

static SemaphoreHandle_t s_lock;
static portMUX_TYPE s_init_mux = portMUX_INITIALIZER_UNLOCKED;

static SemaphoreHandle_t get_lock(void)
{
    if(s_lock == NULL) {
        SemaphoreHandle_t m = xSemaphoreCreateMutex();
        taskENTER_CRITICAL(&s_init_mux);
        if(s_lock == NULL) {
            s_lock = m;
            m = NULL;
        }
        taskEXIT_CRITICAL(&s_init_mux);
        if(m) vSemaphoreDelete(m);      /* another task created it first */
    }
    return s_lock;
}

void i2c_lock(void)
{
    xSemaphoreTake(get_lock(), portMAX_DELAY);
}

void i2c_unlock(void)
{
    xSemaphoreGive(get_lock());
}
