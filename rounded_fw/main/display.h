/**
 * @file display.h
 *
 * Round CO5300 AMOLED (466x466, QSPI), CST9217 touch and the LVGL task.
 *
 * Start-up without the white flash: the panel is initialised dark and with its
 * output off, the UI's first frame is written into its memory, and only then is
 * it switched on and faded up (display_show()).
 */
#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Panel (dark, output off), LVGL, touch. Returns NULL on failure. */
lv_display_t * display_start(void);

/**
 * Render a complete first frame, switch the panel on and fade up to the
 * brightness set with display_set_brightness(). Call once, after the first
 * screen is loaded, from any task except the LVGL one, without the LVGL lock.
 */
void display_show(void);

/**
 * Brightness in percent (0..100). Before display_show() it is only remembered,
 * so loading a saved setting at start-up doesn't light the panel early.
 * Call with the LVGL lock held (or from the LVGL task).
 */
void display_set_brightness(int pct);

#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_H */
