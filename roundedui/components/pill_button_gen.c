/**
 * @file pill_button_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "pill_button_gen.h"
#include "../roundedui.h"

/*********************
 *      DEFINES
 *********************/

/* A string subject needs a value buffer and a previous-value
 * buffer per instance. Mirrors LV_XML_INSTANCE_STRING_LEN in the runtime. */
#ifndef PILL_BUTTON_STRING_LEN
    #define PILL_BUTTON_STRING_LEN 256
#endif

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    /* <variant>: the option index, so a binding can follow it */
    lv_subject_t * tone;

    /* <prop bindable="true">: a create() parameter AND an owned subject */
    lv_subject_t * text;
    char text_buf[PILL_BUTTON_STRING_LEN];
    char text_prev_buf[PILL_BUTTON_STRING_LEN];
} pill_button_t;

/***********************
 *  STATIC VARIABLES
 **********************/

/***********************
 *  STATIC PROTOTYPES
 **********************/

static uint32_t pill_button_get_instance_event;
static void pill_button_get_instance_cb(lv_event_t * e);
static pill_button_t * pill_button_get_instance(lv_obj_t * obj);
static void pill_button_destroy_instance(pill_button_t * inst);
static void pill_button_delete_cb(lv_event_t * e);
static lv_style_t * pill_button_style_button_danger_ref; /* set in create(), read by a binding callback */
static lv_style_t * pill_button_style_button_danger_pressed_ref; /* set in create(), read by a binding callback */
static void pill_button_lv_button_0_style_enabled_cb(lv_observer_t * observer, lv_subject_t * subject);
static void pill_button_lv_button_0_style_enabled_cb_2(lv_observer_t * observer, lv_subject_t * subject);
static void pill_button_lv_label_0_text_binding_cb(lv_observer_t * observer, lv_subject_t * subject);

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * pill_button_create(lv_obj_t * parent, const char * text)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t style_button;
    static lv_style_t style_button_pressed;
    static lv_style_t style_button_disabled;
    static lv_style_t style_button_danger;
    static lv_style_t style_button_danger_pressed;

    static bool style_inited = false;

    if (!style_inited) {
        /*Init all styles*/
        lv_style_init(&style_button);
        lv_style_init(&style_button_pressed);
        lv_style_init(&style_button_disabled);
        lv_style_init(&style_button_danger);
        lv_style_init(&style_button_danger_pressed);

        lv_style_set_bg_color(&style_button, COLOR_TRACK);
        lv_style_set_bg_opa(&style_button, (255 * 100 / 100));
        lv_style_set_border_color(&style_button, COLOR_ACCENT);
        lv_style_set_border_width(&style_button, 2);
        lv_style_set_radius(&style_button, 30);
        lv_style_set_shadow_width(&style_button, 0);
        lv_style_set_pad_hor(&style_button, SPACE_MD);
        lv_style_set_pad_ver(&style_button, SPACE_XS);
        lv_style_set_text_color(&style_button, COLOR_TEXT);
        lv_style_set_text_font(&style_button, font_title);
        lv_style_set_bg_color(&style_button_pressed, COLOR_ACCENT);
        lv_style_set_border_color(&style_button_disabled, COLOR_WARN);
        lv_style_set_text_color(&style_button_disabled, COLOR_WARN);
        lv_style_set_border_color(&style_button_danger, COLOR_DANGER);
        lv_style_set_bg_color(&style_button_danger_pressed, COLOR_DANGER);

        style_inited = true;
    }


    lv_obj_t * the_root = NULL;

    if(pill_button_get_instance_event == 0) pill_button_get_instance_event = lv_event_register_id();
    pill_button_t * inst = lv_malloc(sizeof(pill_button_t));
    lv_memzero(inst, sizeof(pill_button_t));
    inst->tone = lv_subject_create(LV_SUBJECT_TYPE_INT);
    lv_subject_set_int(inst->tone, PILL_BUTTON_TONE_NORMAL);
    inst->text = lv_subject_create(LV_SUBJECT_TYPE_STRING);
    lv_subject_set_string_buffer_static(inst->text, inst->text_buf, inst->text_prev_buf, PILL_BUTTON_STRING_LEN);
    lv_subject_set_string(inst->text, text);

    #if ROUNDEDUI_CHECK_COMPILE_TARGET(ROUNDEDUI_TARGET_ALL)
    if (roundedui_check_target(ROUNDEDUI_TARGET_ALL)) {
        lv_obj_t * lv_button_0 = lv_button_create(parent);
        lv_obj_set_name_static(lv_button_0, "pill_button_#");
        lv_obj_add_event_cb(lv_button_0, pill_button_get_instance_cb, pill_button_get_instance_event, inst);
        lv_obj_set_height(lv_button_0, 60);

        lv_obj_add_style(lv_button_0, &style_button, 0);
        lv_obj_add_style(lv_button_0, &style_button_pressed, LV_STATE_PRESSED);
        lv_obj_add_style(lv_button_0, &style_button_disabled, LV_STATE_DISABLED);
        lv_obj_add_style(lv_button_0, &style_button_danger, 0);
        pill_button_style_button_danger_ref = &style_button_danger;
        lv_subject_add_observer_obj(inst->tone, pill_button_lv_button_0_style_enabled_cb, lv_button_0, inst);
        lv_obj_add_style(lv_button_0, &style_button_danger_pressed, LV_STATE_PRESSED);
        pill_button_style_button_danger_pressed_ref = &style_button_danger_pressed;
        lv_subject_add_observer_obj(inst->tone, pill_button_lv_button_0_style_enabled_cb_2, lv_button_0, inst);
        lv_obj_t * lv_label_0 = lv_label_create(lv_button_0);
        lv_obj_set_align(lv_label_0, LV_ALIGN_CENTER);
        lv_subject_add_observer_obj(inst->text, pill_button_lv_label_0_text_binding_cb, lv_label_0, inst);

        the_root = lv_button_0;
    }
    #endif

    if(the_root) lv_obj_add_event_cb(the_root, pill_button_delete_cb, LV_EVENT_DELETE, inst);
    else pill_button_destroy_instance(inst); /* no target matched, so nothing owns it */

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

void pill_button_set_tone(lv_obj_t * obj, pill_button_tone_t tone)
{
    pill_button_t * inst = pill_button_get_instance(obj);
    if(inst) lv_subject_set_int(inst->tone, tone);
}

pill_button_tone_t pill_button_get_tone(lv_obj_t * obj)
{
    pill_button_t * inst = pill_button_get_instance(obj);
    return inst ? (pill_button_tone_t)lv_subject_get_int(inst->tone) : (pill_button_tone_t)0;
}

void pill_button_set_text(lv_obj_t * obj, const char * text)
{
    pill_button_t * inst = pill_button_get_instance(obj);
    if(inst) lv_subject_set_string(inst->text, text);
}

const char * pill_button_get_text(lv_obj_t * obj)
{
    pill_button_t * inst = pill_button_get_instance(obj);
    return inst ? lv_subject_get_string(inst->text) : NULL;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void pill_button_get_instance_cb(lv_event_t * e)
{
    void ** out = lv_event_get_param(e);
    *out = lv_event_get_user_data(e);
}

static pill_button_t * pill_button_get_instance(lv_obj_t * obj)
{
    pill_button_t * inst = NULL;
    for(lv_obj_t * o = obj; o && !inst; o = lv_obj_get_parent(o)) {
        lv_obj_send_event(o, pill_button_get_instance_event, &inst);
    }
    return inst;
}

static void pill_button_destroy_instance(pill_button_t * inst)
{
    lv_subject_delete(inst->tone);
    lv_subject_delete(inst->text);
    lv_free(inst);
}

static void pill_button_delete_cb(lv_event_t * e)
{
    pill_button_destroy_instance(lv_event_get_user_data(e));
}

static void pill_button_lv_button_0_style_enabled_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(subject);
    pill_button_t * inst = lv_observer_get_user_data(observer);
    lv_obj_t * obj = lv_observer_get_target_obj(observer);
    lv_obj_set_style_enabled(obj, pill_button_style_button_danger_ref, 0, lv_subject_get_int(inst->tone) == PILL_BUTTON_TONE_DANGER);
}

static void pill_button_lv_button_0_style_enabled_cb_2(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(subject);
    pill_button_t * inst = lv_observer_get_user_data(observer);
    lv_obj_t * obj = lv_observer_get_target_obj(observer);
    lv_obj_set_style_enabled(obj, pill_button_style_button_danger_pressed_ref, LV_STATE_PRESSED, lv_subject_get_int(inst->tone) == PILL_BUTTON_TONE_DANGER);
}

static void pill_button_lv_label_0_text_binding_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(subject);
    pill_button_t * inst = lv_observer_get_user_data(observer);
    lv_obj_t * obj = lv_observer_get_target_obj(observer);
    lv_label_set_text(obj, lv_subject_get_string(inst->text));
}

