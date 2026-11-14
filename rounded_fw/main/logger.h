/**
 * @file logger.h
 *
 * Ride logger: lean, GPS position, speed and braking/accelerating to a CSV file
 * on the microSD card, 10 rows per second. Started and stopped with the record
 * button on the main screen (subject_log_request), state in subject_log_state.
 */
#ifndef LOGGER_H
#define LOGGER_H

#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Start the logger task. Nothing is mounted or written until recording starts. */
esp_err_t logger_start(void);

/** Watch the record button and show the state. Call once with the LVGL lock held. */
void logger_ui_bind(void);

/** Close the file and unmount the card, waiting for a write in progress. Call before power off. */
void logger_shutdown(void);

/**
 * Mount the card for someone else (the serial file access) and keep the logger
 * from using it until logger_card_release(). Fails while recording; *why says why.
 * Pressing record meanwhile starts the recording only after the release.
 */
bool logger_card_acquire(const char ** why);
void logger_card_release(void);

#ifdef __cplusplus
}
#endif

#endif /* LOGGER_H */
