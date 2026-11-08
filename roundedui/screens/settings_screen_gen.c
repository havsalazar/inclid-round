/**
 * @file settings_screen_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "settings_screen_gen.h"
#include "../roundedui.h"

/*********************
 *      DEFINES
 *********************/

#define MENU_BUTTON_WIDTH (270)
#define MENU_BUTTON_HEIGHT (46)


/**********************
 *      TYPEDEFS
 **********************/

/***********************
 *  STATIC VARIABLES
 **********************/

/***********************
 *  STATIC PROTOTYPES
 **********************/

static void settings_screen_pill_button_0_text_binding_cb(lv_observer_t * observer, lv_subject_t * subject);
static void settings_screen_pill_button_0_disabled_binding_cb(lv_observer_t * observer, lv_subject_t * subject);
static void settings_screen_set_subject_cb(lv_event_t * e);
static void settings_screen_pill_button_1_text_binding_cb(lv_observer_t * observer, lv_subject_t * subject);
static void settings_screen_pill_button_1_disabled_binding_cb(lv_observer_t * observer, lv_subject_t * subject);
static void settings_screen_set_subject_cb_2(lv_event_t * e);
static void settings_screen_pill_button_2_text_binding_cb(lv_observer_t * observer, lv_subject_t * subject);
static void settings_screen_set_subject_cb_3(lv_event_t * e);
static void settings_screen_pill_button_3_text_binding_cb(lv_observer_t * observer, lv_subject_t * subject);
static void settings_screen_set_subject_cb_4(lv_event_t * e);
static void settings_screen_pill_button_4_text_binding_cb(lv_observer_t * observer, lv_subject_t * subject);
static void settings_screen_set_subject_cb_5(lv_event_t * e);

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * settings_screen_create(void)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t style_title;

    static bool style_inited = false;

    if (!style_inited) {
        /*Init all styles*/
        lv_style_init(&style_title);

        lv_style_set_text_color(&style_title, COLOR_ACCENT_TEXT);
        lv_style_set_text_font(&style_title, font_title);

        style_inited = true;
    }


    lv_obj_t * the_root = NULL;

    #if ROUNDEDUI_CHECK_COMPILE_TARGET(ROUNDEDUI_TARGET_ALL)
    if (roundedui_check_target(ROUNDEDUI_TARGET_ALL)) {
        char eval_buf_1[LV_XML_EVAL_STRING_BUF_SIZE];

        lv_obj_t * lv_obj_0 = lv_obj_create(NULL);
        lv_obj_set_name_static(lv_obj_0, "settings_screen_#");
        lv_obj_set_flex_flow(lv_obj_0, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_scrollable(lv_obj_0, false);
        lv_obj_set_style_flex_main_place(lv_obj_0, LV_FLEX_ALIGN_CENTER, 0);
        lv_obj_set_style_flex_cross_place(lv_obj_0, LV_FLEX_ALIGN_CENTER, 0);
        lv_obj_set_style_flex_track_place(lv_obj_0, LV_FLEX_ALIGN_CENTER, 0);
        lv_obj_set_style_pad_row(lv_obj_0, 10, 0);

        lv_obj_add_style(lv_obj_0, &style_screen, 0);
        lv_obj_t * lv_label_0 = lv_label_create(lv_obj_0);
        lv_label_set_text(lv_label_0, "SETTINGS");
        lv_obj_add_style(lv_label_0, &style_title, 0);

        lv_obj_t * pill_button_0 = pill_button_create(lv_obj_0, lv_subject_get_int(subject_gps_calibrating) == 1 ? "SEARCHING GPS..." : "CALIBRATE GPS");
        lv_obj_set_width(pill_button_0, MENU_BUTTON_WIDTH);
        lv_obj_set_height(pill_button_0, MENU_BUTTON_HEIGHT);
        lv_subject_add_observer_obj(subject_gps_calibrating, settings_screen_pill_button_0_disabled_binding_cb, pill_button_0, NULL);
        lv_subject_add_observer_obj(subject_gps_calibrating, settings_screen_pill_button_0_text_binding_cb, pill_button_0, NULL);
        lv_obj_add_event_cb(pill_button_0, settings_screen_set_subject_cb, LV_EVENT_CLICKED, NULL);

        lv_obj_t * pill_button_1 = pill_button_create(lv_obj_0, lv_subject_get_int(subject_imu_calibrating) == 1 ? "HOLD STILL..." : "CALIBRATE INCLINOMETER");
        lv_obj_set_width(pill_button_1, MENU_BUTTON_WIDTH);
        lv_obj_set_height(pill_button_1, MENU_BUTTON_HEIGHT);
        lv_subject_add_observer_obj(subject_imu_calibrating, settings_screen_pill_button_1_disabled_binding_cb, pill_button_1, NULL);
        lv_subject_add_observer_obj(subject_imu_calibrating, settings_screen_pill_button_1_text_binding_cb, pill_button_1, NULL);
        lv_obj_add_event_cb(pill_button_1, settings_screen_set_subject_cb_2, LV_EVENT_CLICKED, NULL);

        lv_snprintf(eval_buf_1, sizeof(eval_buf_1), "%s%d%s", "BRIGHTNESS ", (int)(lv_subject_get_int(subject_brightness_pct)), "%");
        lv_obj_t * pill_button_2 = pill_button_create(lv_obj_0, eval_buf_1);
        lv_obj_set_width(pill_button_2, MENU_BUTTON_WIDTH);
        lv_obj_set_height(pill_button_2, MENU_BUTTON_HEIGHT);
        lv_subject_add_observer_obj(subject_brightness_pct, settings_screen_pill_button_2_text_binding_cb, pill_button_2, NULL);
        lv_obj_add_event_cb(pill_button_2, settings_screen_set_subject_cb_3, LV_EVENT_CLICKED, NULL);

        lv_obj_t * pill_button_3 = pill_button_create(lv_obj_0, lv_subject_get_int(subject_show_g_bar) == 1 ? "G-FORCE BAR: ON" : "G-FORCE BAR: OFF");
        lv_obj_set_width(pill_button_3, MENU_BUTTON_WIDTH);
        lv_obj_set_height(pill_button_3, MENU_BUTTON_HEIGHT);
        lv_subject_add_observer_obj(subject_show_g_bar, settings_screen_pill_button_3_text_binding_cb, pill_button_3, NULL);
        lv_obj_add_event_cb(pill_button_3, settings_screen_set_subject_cb_4, LV_EVENT_CLICKED, NULL);

        lv_obj_t * pill_button_4 = pill_button_create(lv_obj_0, lv_subject_get_int(subject_power_off_request) == 1 ? "POWERING OFF..." : "HOLD TO POWER OFF");
        lv_obj_set_width(pill_button_4, MENU_BUTTON_WIDTH);
        lv_obj_set_height(pill_button_4, MENU_BUTTON_HEIGHT);
        pill_button_set_tone(pill_button_4, PILL_BUTTON_TONE_DANGER);
        lv_subject_add_observer_obj(subject_power_off_request, settings_screen_pill_button_4_text_binding_cb, pill_button_4, NULL);
        lv_obj_add_event_cb(pill_button_4, settings_screen_set_subject_cb_5, LV_EVENT_LONG_PRESSED, NULL);

        lv_obj_t * pill_button_5 = pill_button_create(lv_obj_0, "BACK");
        lv_obj_set_width(pill_button_5, 110);
        lv_obj_set_height(pill_button_5, 40);
        lv_obj_add_screen_create_event(pill_button_5, LV_EVENT_CLICKED, lean_screen_create, LV_SCREEN_LOAD_ANIM_FADE_IN, 200, 0);

        lv_obj_t * battery_indicator_0 = battery_indicator_create(lv_obj_0);
        lv_obj_set_floating(battery_indicator_0, true);
        lv_obj_set_align(battery_indicator_0, LV_ALIGN_TOP_MID);
        lv_obj_set_y(battery_indicator_0, 18);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void settings_screen_pill_button_0_text_binding_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(subject);
    lv_obj_t * obj = lv_observer_get_target_obj(observer);
    pill_button_set_text(obj, lv_subject_get_int(subject_gps_calibrating) == 1 ? "SEARCHING GPS..." : "CALIBRATE GPS");
}

static void settings_screen_pill_button_0_disabled_binding_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(subject);
    lv_obj_t * obj = lv_observer_get_target_obj(observer);
    lv_obj_set_disabled(obj, lv_subject_get_int(subject_gps_calibrating) == 1);
}

static void settings_screen_set_subject_cb(lv_event_t * e)
{
    LV_UNUSED(e);
    lv_subject_set_int(subject_gps_cal_request, 1);
}

static void settings_screen_pill_button_1_text_binding_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(subject);
    lv_obj_t * obj = lv_observer_get_target_obj(observer);
    pill_button_set_text(obj, lv_subject_get_int(subject_imu_calibrating) == 1 ? "HOLD STILL..." : "CALIBRATE INCLINOMETER");
}

static void settings_screen_pill_button_1_disabled_binding_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(subject);
    lv_obj_t * obj = lv_observer_get_target_obj(observer);
    lv_obj_set_disabled(obj, lv_subject_get_int(subject_imu_calibrating) == 1);
}

static void settings_screen_set_subject_cb_2(lv_event_t * e)
{
    LV_UNUSED(e);
    lv_subject_set_int(subject_imu_cal_request, 1);
}

static void settings_screen_pill_button_2_text_binding_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(subject);
    lv_obj_t * obj = lv_observer_get_target_obj(observer);
    char eval_buf_1[LV_XML_EVAL_STRING_BUF_SIZE];
    lv_snprintf(eval_buf_1, sizeof(eval_buf_1), "%s%d%s", "BRIGHTNESS ", (int)(lv_subject_get_int(subject_brightness_pct)), "%");
    pill_button_set_text(obj, eval_buf_1);
}

static void settings_screen_set_subject_cb_3(lv_event_t * e)
{
    LV_UNUSED(e);
    lv_subject_set_int(subject_brightness_pct, lv_subject_get_int(subject_brightness_pct) >= 100 ? 25 : lv_subject_get_int(subject_brightness_pct) + 25);
}

static void settings_screen_pill_button_3_text_binding_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(subject);
    lv_obj_t * obj = lv_observer_get_target_obj(observer);
    pill_button_set_text(obj, lv_subject_get_int(subject_show_g_bar) == 1 ? "G-FORCE BAR: ON" : "G-FORCE BAR: OFF");
}

static void settings_screen_set_subject_cb_4(lv_event_t * e)
{
    LV_UNUSED(e);
    lv_subject_set_int(subject_show_g_bar, !lv_subject_get_int(subject_show_g_bar));
}

static void settings_screen_pill_button_4_text_binding_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(subject);
    lv_obj_t * obj = lv_observer_get_target_obj(observer);
    pill_button_set_text(obj, lv_subject_get_int(subject_power_off_request) == 1 ? "POWERING OFF..." : "HOLD TO POWER OFF");
}

static void settings_screen_set_subject_cb_5(lv_event_t * e)
{
    LV_UNUSED(e);
    lv_subject_set_int(subject_power_off_request, 1);
}

