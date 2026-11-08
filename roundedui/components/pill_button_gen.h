/**
 * @file pill_button_gen.h
 */

#ifndef LVGL_PRO_PILL_BUTTON_GEN_H
#define LVGL_PRO_PILL_BUTTON_GEN_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
    #include "lvgl.h"
#else
    #include "lvgl/lvgl.h"
#endif

#if defined(LV_USE_XML) && LV_USE_XML
    #include "lv_xml/lv_xml.h"
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
    PILL_BUTTON_TONE_NORMAL = 0,
    PILL_BUTTON_TONE_DANGER = 1
} pill_button_tone_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_obj_t * pill_button_create(lv_obj_t * parent, const char * text);
/**
 * Set the `tone` variant of a pill_button.
 * @param obj       pointer to a pill_button, or to any widget inside one
 * @param tone  the option to switch to
 */
void pill_button_set_tone(lv_obj_t * obj, pill_button_tone_t tone);

/**
 * Get the current `tone` variant of a pill_button.
 * @param obj       pointer to a pill_button, or to any widget inside one
 * @return          the current option, or the first one if `obj` is not in a pill_button
 */
pill_button_tone_t pill_button_get_tone(lv_obj_t * obj);

/**
 * Set the `text` property of a pill_button, and notify everything bound to it.
 * @param obj       pointer to a pill_button, or to any widget inside one
 * @param text  the new value. It is copied, so it need not stay alive
 */
void pill_button_set_text(lv_obj_t * obj, const char * text);

/**
 * Get the current `text` property of a pill_button.
 * @param obj       pointer to a pill_button, or to any widget inside one
 * @return          the current value, or NULL if `obj` is not in a pill_button
 */
const char * pill_button_get_text(lv_obj_t * obj);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PRO_PILL_BUTTON_GEN_H*/