/**
 * @file lean_screen_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "lean_screen_gen.h"
#include "../roundedui.h"

/*********************
 *      DEFINES
 *********************/

#define LEFT_ARC_START (180 - LEAN_ARC_HALF_SPAN)
#define LEFT_ARC_END (180 + LEAN_ARC_HALF_SPAN)
#define RIGHT_ARC_START (360 - LEAN_ARC_HALF_SPAN)
#define RIGHT_ARC_END (LEAN_ARC_HALF_SPAN)
#define DANGER_SPAN (2 * LEAN_ARC_HALF_SPAN * (LEAN_LIMIT - LEAN_DANGER) / LEAN_LIMIT)
#define WARN_ANGLE (2 * LEAN_ARC_HALF_SPAN * LEAN_WARN / LEAN_LIMIT)
#define DANGER_ANGLE (2 * LEAN_ARC_HALF_SPAN * LEAN_DANGER / LEAN_LIMIT)
#define LEAN_ARC_SIZE (DISPLAY_SIZE - 2 * LEAN_ARC_INSET - 10)
#define DANGER_ARC_WIDTH (4)
#define COG_BUTTON_SIZE (60)
#define REC_RING_SIZE (34)
#define REC_DOT_SIZE (16)


/**********************
 *      TYPEDEFS
 **********************/

/***********************
 *  STATIC VARIABLES
 **********************/

/***********************
 *  STATIC PROTOTYPES
 **********************/

static lv_style_t * lean_screen_style_lean_value_warn_ref; /* set in create(), read by a binding callback */
static lv_style_t * lean_screen_style_lean_value_danger_ref; /* set in create(), read by a binding callback */
static lv_style_t * lean_screen_style_g_fill_brake_ref; /* set in create(), read by a binding callback */
static lv_style_t * lean_screen_style_rec_ring_on_ref; /* set in create(), read by a binding callback */
static lv_style_t * lean_screen_style_rec_ring_error_ref; /* set in create(), read by a binding callback */
static lv_style_t * lean_screen_style_rec_dot_on_ref; /* set in create(), read by a binding callback */
static void lean_screen_set_subject_cb(lv_event_t * e);
static void lean_screen_set_subject_cb_2(lv_event_t * e);
static void lean_screen_lv_arc_0_value_binding_cb(lv_observer_t * observer, lv_subject_t * subject);
static void lean_screen_lv_arc_1_value_binding_cb(lv_observer_t * observer, lv_subject_t * subject);
static void lean_screen_lv_arc_2_value_binding_cb(lv_observer_t * observer, lv_subject_t * subject);
static void lean_screen_lv_arc_3_value_binding_cb(lv_observer_t * observer, lv_subject_t * subject);
static void lean_screen_lv_arc_4_value_binding_cb(lv_observer_t * observer, lv_subject_t * subject);
static void lean_screen_lv_arc_5_value_binding_cb(lv_observer_t * observer, lv_subject_t * subject);
static void lean_screen_lv_label_1_text_binding_cb(lv_observer_t * observer, lv_subject_t * subject);
static void lean_screen_lv_label_3_text_binding_cb(lv_observer_t * observer, lv_subject_t * subject);
static void lean_screen_lv_label_4_hidden_binding_cb(lv_observer_t * observer, lv_subject_t * subject);
static void lean_screen_lv_label_4_text_binding_cb(lv_observer_t * observer, lv_subject_t * subject);
static void lean_screen_lv_label_5_text_binding_cb(lv_observer_t * observer, lv_subject_t * subject);
static void lean_screen_lv_label_5_style_enabled_cb(lv_observer_t * observer, lv_subject_t * subject);
static void lean_screen_lv_label_5_style_enabled_cb_2(lv_observer_t * observer, lv_subject_t * subject);
static void lean_screen_lv_label_6_hidden_binding_cb(lv_observer_t * observer, lv_subject_t * subject);
static void lean_screen_lv_label_7_text_binding_cb(lv_observer_t * observer, lv_subject_t * subject);
static void lean_screen_lv_label_7_hidden_binding_cb(lv_observer_t * observer, lv_subject_t * subject);
static void lean_screen_lv_label_8_hidden_binding_cb(lv_observer_t * observer, lv_subject_t * subject);
static void lean_screen_lv_label_9_hidden_binding_cb(lv_observer_t * observer, lv_subject_t * subject);
static void lean_screen_lv_label_10_text_binding_cb(lv_observer_t * observer, lv_subject_t * subject);
static void lean_screen_lv_label_10_hidden_binding_cb(lv_observer_t * observer, lv_subject_t * subject);
static void lean_screen_lv_label_11_hidden_binding_cb(lv_observer_t * observer, lv_subject_t * subject);
static void lean_screen_lv_obj_4_hidden_binding_cb(lv_observer_t * observer, lv_subject_t * subject);
static void lean_screen_lv_label_12_text_binding_cb(lv_observer_t * observer, lv_subject_t * subject);
static void lean_screen_lv_bar_0_style_enabled_cb(lv_observer_t * observer, lv_subject_t * subject);
static void lean_screen_lv_obj_8_style_enabled_cb(lv_observer_t * observer, lv_subject_t * subject);
static void lean_screen_lv_obj_8_style_enabled_cb_2(lv_observer_t * observer, lv_subject_t * subject);
static void lean_screen_lv_obj_9_hidden_binding_cb(lv_observer_t * observer, lv_subject_t * subject);
static void lean_screen_lv_obj_9_style_enabled_cb(lv_observer_t * observer, lv_subject_t * subject);
static void lean_screen_lv_label_15_hidden_binding_cb(lv_observer_t * observer, lv_subject_t * subject);
static void lean_screen_set_subject_cb_3(lv_event_t * e);

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lean_screen_create(void)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t style_lean_track;
    static lv_style_t style_lean_fill;
    static lv_style_t style_lean_fill_warn;
    static lv_style_t style_lean_fill_danger;
    static lv_style_t style_lean_overlay;
    static lv_style_t style_no_knob;
    static lv_style_t style_danger_zone;
    static lv_style_t style_arc_hidden_indicator;
    static lv_style_t style_lean_value;
    static lv_style_t style_lean_value_warn;
    static lv_style_t style_lean_value_danger;
    static lv_style_t style_speed_value;
    static lv_style_t style_g_track;
    static lv_style_t style_g_fill;
    static lv_style_t style_g_fill_brake;
    static lv_style_t style_g_zero;
    static lv_style_t style_cog;
    static lv_style_t style_cog_pressed;
    static lv_style_t style_rec_ring;
    static lv_style_t style_rec_ring_on;
    static lv_style_t style_rec_ring_error;
    static lv_style_t style_rec_dot;
    static lv_style_t style_rec_dot_on;

    static bool style_inited = false;

    if (!style_inited) {
        /*Init all styles*/
        lv_style_init(&style_lean_track);
        lv_style_init(&style_lean_fill);
        lv_style_init(&style_lean_fill_warn);
        lv_style_init(&style_lean_fill_danger);
        lv_style_init(&style_lean_overlay);
        lv_style_init(&style_no_knob);
        lv_style_init(&style_danger_zone);
        lv_style_init(&style_arc_hidden_indicator);
        lv_style_init(&style_lean_value);
        lv_style_init(&style_lean_value_warn);
        lv_style_init(&style_lean_value_danger);
        lv_style_init(&style_speed_value);
        lv_style_init(&style_g_track);
        lv_style_init(&style_g_fill);
        lv_style_init(&style_g_fill_brake);
        lv_style_init(&style_g_zero);
        lv_style_init(&style_cog);
        lv_style_init(&style_cog_pressed);
        lv_style_init(&style_rec_ring);
        lv_style_init(&style_rec_ring_on);
        lv_style_init(&style_rec_ring_error);
        lv_style_init(&style_rec_dot);
        lv_style_init(&style_rec_dot_on);

        lv_style_set_arc_color(&style_lean_track, COLOR_TRACK);
        lv_style_set_arc_width(&style_lean_track, LEAN_ARC_WIDTH);
        lv_style_set_arc_rounded(&style_lean_track, true);
        lv_style_set_bg_opa(&style_lean_track, 0);
        lv_style_set_border_width(&style_lean_track, 0);
        lv_style_set_pad_all(&style_lean_track, 0);
        lv_style_set_arc_color(&style_lean_fill, COLOR_ACCENT);
        lv_style_set_arc_width(&style_lean_fill, LEAN_ARC_WIDTH);
        lv_style_set_arc_rounded(&style_lean_fill, true);
        lv_style_set_arc_color(&style_lean_fill_warn, COLOR_WARN);
        lv_style_set_arc_color(&style_lean_fill_danger, COLOR_DANGER);
        lv_style_set_arc_opa(&style_lean_overlay, 0);
        lv_style_set_bg_opa(&style_no_knob, 0);
        lv_style_set_border_width(&style_no_knob, 0);
        lv_style_set_shadow_width(&style_no_knob, 0);
        lv_style_set_outline_width(&style_no_knob, 0);
        lv_style_set_pad_all(&style_no_knob, 0);
        lv_style_set_arc_color(&style_danger_zone, COLOR_DANGER);
        lv_style_set_arc_width(&style_danger_zone, DANGER_ARC_WIDTH);
        lv_style_set_arc_rounded(&style_danger_zone, false);
        lv_style_set_arc_opa(&style_danger_zone, (255 * 70 / 100));
        lv_style_set_bg_opa(&style_danger_zone, 0);
        lv_style_set_border_width(&style_danger_zone, 0);
        lv_style_set_pad_all(&style_danger_zone, 0);
        lv_style_set_arc_opa(&style_arc_hidden_indicator, 0);
        lv_style_set_text_font(&style_lean_value, font_lean);
        lv_style_set_text_color(&style_lean_value_warn, COLOR_WARN);
        lv_style_set_text_color(&style_lean_value_danger, COLOR_DANGER);
        lv_style_set_text_font(&style_speed_value, font_speed);
        lv_style_set_text_color(&style_speed_value, COLOR_TEXT);
        lv_style_set_bg_color(&style_g_track, COLOR_TRACK);
        lv_style_set_bg_opa(&style_g_track, (255 * 100 / 100));
        lv_style_set_radius(&style_g_track, 7);
        lv_style_set_pad_all(&style_g_track, 0);
        lv_style_set_bg_color(&style_g_fill, COLOR_OK);
        lv_style_set_bg_opa(&style_g_fill, (255 * 100 / 100));
        lv_style_set_radius(&style_g_fill, 7);
        lv_style_set_bg_color(&style_g_fill_brake, COLOR_DANGER);
        lv_style_set_bg_color(&style_g_zero, COLOR_TEXT_MUTED);
        lv_style_set_bg_opa(&style_g_zero, (255 * 100 / 100));
        lv_style_set_border_width(&style_g_zero, 0);
        lv_style_set_radius(&style_g_zero, 0);
        lv_style_set_pad_all(&style_g_zero, 0);
        lv_style_set_bg_opa(&style_cog, 0);
        lv_style_set_border_width(&style_cog, 0);
        lv_style_set_shadow_width(&style_cog, 0);
        lv_style_set_radius(&style_cog, COG_BUTTON_SIZE / 2);
        lv_style_set_pad_all(&style_cog, 0);
        lv_style_set_bg_color(&style_cog_pressed, COLOR_TRACK);
        lv_style_set_bg_opa(&style_cog_pressed, (255 * 100 / 100));
        lv_style_set_bg_opa(&style_rec_ring, 0);
        lv_style_set_border_width(&style_rec_ring, 3);
        lv_style_set_border_color(&style_rec_ring, COLOR_TEXT_MUTED);
        lv_style_set_radius(&style_rec_ring, REC_RING_SIZE / 2);
        lv_style_set_pad_all(&style_rec_ring, 0);
        lv_style_set_border_color(&style_rec_ring_on, COLOR_DANGER);
        lv_style_set_bg_color(&style_rec_ring_on, COLOR_DANGER);
        lv_style_set_bg_opa(&style_rec_ring_on, (255 * 25 / 100));
        lv_style_set_border_color(&style_rec_ring_error, COLOR_WARN);
        lv_style_set_bg_color(&style_rec_dot, COLOR_DANGER);
        lv_style_set_bg_opa(&style_rec_dot, (255 * 100 / 100));
        lv_style_set_border_width(&style_rec_dot, 0);
        lv_style_set_radius(&style_rec_dot, REC_DOT_SIZE / 2);
        lv_style_set_pad_all(&style_rec_dot, 0);
        lv_style_set_radius(&style_rec_dot_on, 3);

        style_inited = true;
    }


    lv_obj_t * the_root = NULL;

    #if ROUNDEDUI_CHECK_COMPILE_TARGET(ROUNDEDUI_TARGET_ALL)
    if (roundedui_check_target(ROUNDEDUI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(NULL);
        lv_obj_set_name_static(lv_obj_0, "lean_screen_#");
        lv_obj_set_scrollable(lv_obj_0, false);

        lv_obj_add_style(lv_obj_0, &style_screen, 0);
        lv_obj_add_event_cb(lv_obj_0, lean_screen_set_subject_cb, LV_EVENT_LONG_PRESSED, NULL);
        lv_obj_add_event_cb(lv_obj_0, lean_screen_set_subject_cb_2, LV_EVENT_LONG_PRESSED, NULL);
        lv_obj_t * lv_arc_0 = lv_arc_create(lv_obj_0);
        lv_obj_set_width(lv_arc_0, LEAN_ARC_SIZE);
        lv_obj_set_height(lv_arc_0, LEAN_ARC_SIZE);
        lv_obj_set_align(lv_arc_0, LV_ALIGN_CENTER);
        lv_obj_set_clickable(lv_arc_0, false);
        lv_arc_set_bg_start_angle(lv_arc_0, LEFT_ARC_START);
        lv_arc_set_bg_end_angle(lv_arc_0, LEFT_ARC_END);
        lv_arc_set_mode(lv_arc_0, LV_ARC_MODE_NORMAL);
        lv_arc_set_min_value(lv_arc_0, 0);
        lv_arc_set_max_value(lv_arc_0, LEAN_LIMIT * 10);
        lv_subject_add_observer_obj(subject_lean_x10, lean_screen_lv_arc_0_value_binding_cb, lv_arc_0, NULL);
        lv_obj_add_style(lv_arc_0, &style_lean_track, 0);
        lv_obj_add_style(lv_arc_0, &style_lean_fill, LV_PART_INDICATOR);
        lv_obj_add_style(lv_arc_0, &style_lean_fill_danger, LV_PART_INDICATOR);
        lv_obj_add_style(lv_arc_0, &style_no_knob, LV_PART_KNOB);

        lv_obj_t * lv_arc_1 = lv_arc_create(lv_obj_0);
        lv_obj_set_width(lv_arc_1, LEAN_ARC_SIZE);
        lv_obj_set_height(lv_arc_1, LEAN_ARC_SIZE);
        lv_obj_set_align(lv_arc_1, LV_ALIGN_CENTER);
        lv_obj_set_clickable(lv_arc_1, false);
        lv_arc_set_bg_start_angle(lv_arc_1, LEFT_ARC_START);
        lv_arc_set_bg_end_angle(lv_arc_1, LEFT_ARC_START + DANGER_ANGLE);
        lv_arc_set_mode(lv_arc_1, LV_ARC_MODE_NORMAL);
        lv_arc_set_min_value(lv_arc_1, 0);
        lv_arc_set_max_value(lv_arc_1, LEAN_DANGER * 10);
        lv_subject_add_observer_obj(subject_lean_x10, lean_screen_lv_arc_1_value_binding_cb, lv_arc_1, NULL);
        lv_obj_add_style(lv_arc_1, &style_lean_track, 0);
        lv_obj_add_style(lv_arc_1, &style_lean_overlay, 0);
        lv_obj_add_style(lv_arc_1, &style_lean_fill, LV_PART_INDICATOR);
        lv_obj_add_style(lv_arc_1, &style_lean_fill_warn, LV_PART_INDICATOR);
        lv_obj_add_style(lv_arc_1, &style_no_knob, LV_PART_KNOB);

        lv_obj_t * lv_arc_2 = lv_arc_create(lv_obj_0);
        lv_obj_set_width(lv_arc_2, LEAN_ARC_SIZE);
        lv_obj_set_height(lv_arc_2, LEAN_ARC_SIZE);
        lv_obj_set_align(lv_arc_2, LV_ALIGN_CENTER);
        lv_obj_set_clickable(lv_arc_2, false);
        lv_arc_set_bg_start_angle(lv_arc_2, LEFT_ARC_START);
        lv_arc_set_bg_end_angle(lv_arc_2, LEFT_ARC_START + WARN_ANGLE);
        lv_arc_set_mode(lv_arc_2, LV_ARC_MODE_NORMAL);
        lv_arc_set_min_value(lv_arc_2, 0);
        lv_arc_set_max_value(lv_arc_2, LEAN_WARN * 10);
        lv_subject_add_observer_obj(subject_lean_x10, lean_screen_lv_arc_2_value_binding_cb, lv_arc_2, NULL);
        lv_obj_add_style(lv_arc_2, &style_lean_track, 0);
        lv_obj_add_style(lv_arc_2, &style_lean_overlay, 0);
        lv_obj_add_style(lv_arc_2, &style_lean_fill, LV_PART_INDICATOR);
        lv_obj_add_style(lv_arc_2, &style_no_knob, LV_PART_KNOB);

        lv_obj_t * lv_arc_3 = lv_arc_create(lv_obj_0);
        lv_obj_set_width(lv_arc_3, LEAN_ARC_SIZE);
        lv_obj_set_height(lv_arc_3, LEAN_ARC_SIZE);
        lv_obj_set_align(lv_arc_3, LV_ALIGN_CENTER);
        lv_obj_set_clickable(lv_arc_3, false);
        lv_arc_set_bg_start_angle(lv_arc_3, RIGHT_ARC_START);
        lv_arc_set_bg_end_angle(lv_arc_3, RIGHT_ARC_END);
        lv_arc_set_mode(lv_arc_3, LV_ARC_MODE_REVERSE);
        lv_arc_set_min_value(lv_arc_3, 0);
        lv_arc_set_max_value(lv_arc_3, LEAN_LIMIT * 10);
        lv_subject_add_observer_obj(subject_lean_x10, lean_screen_lv_arc_3_value_binding_cb, lv_arc_3, NULL);
        lv_obj_add_style(lv_arc_3, &style_lean_track, 0);
        lv_obj_add_style(lv_arc_3, &style_lean_fill, LV_PART_INDICATOR);
        lv_obj_add_style(lv_arc_3, &style_lean_fill_danger, LV_PART_INDICATOR);
        lv_obj_add_style(lv_arc_3, &style_no_knob, LV_PART_KNOB);

        lv_obj_t * lv_arc_4 = lv_arc_create(lv_obj_0);
        lv_obj_set_width(lv_arc_4, LEAN_ARC_SIZE);
        lv_obj_set_height(lv_arc_4, LEAN_ARC_SIZE);
        lv_obj_set_align(lv_arc_4, LV_ALIGN_CENTER);
        lv_obj_set_clickable(lv_arc_4, false);
        lv_arc_set_bg_start_angle(lv_arc_4, RIGHT_ARC_END - DANGER_ANGLE + 360);
        lv_arc_set_bg_end_angle(lv_arc_4, RIGHT_ARC_END);
        lv_arc_set_mode(lv_arc_4, LV_ARC_MODE_REVERSE);
        lv_arc_set_min_value(lv_arc_4, 0);
        lv_arc_set_max_value(lv_arc_4, LEAN_DANGER * 10);
        lv_subject_add_observer_obj(subject_lean_x10, lean_screen_lv_arc_4_value_binding_cb, lv_arc_4, NULL);
        lv_obj_add_style(lv_arc_4, &style_lean_track, 0);
        lv_obj_add_style(lv_arc_4, &style_lean_overlay, 0);
        lv_obj_add_style(lv_arc_4, &style_lean_fill, LV_PART_INDICATOR);
        lv_obj_add_style(lv_arc_4, &style_lean_fill_warn, LV_PART_INDICATOR);
        lv_obj_add_style(lv_arc_4, &style_no_knob, LV_PART_KNOB);

        lv_obj_t * lv_arc_5 = lv_arc_create(lv_obj_0);
        lv_obj_set_width(lv_arc_5, LEAN_ARC_SIZE);
        lv_obj_set_height(lv_arc_5, LEAN_ARC_SIZE);
        lv_obj_set_align(lv_arc_5, LV_ALIGN_CENTER);
        lv_obj_set_clickable(lv_arc_5, false);
        lv_arc_set_bg_start_angle(lv_arc_5, RIGHT_ARC_END - WARN_ANGLE + 360);
        lv_arc_set_bg_end_angle(lv_arc_5, RIGHT_ARC_END);
        lv_arc_set_mode(lv_arc_5, LV_ARC_MODE_REVERSE);
        lv_arc_set_min_value(lv_arc_5, 0);
        lv_arc_set_max_value(lv_arc_5, LEAN_WARN * 10);
        lv_subject_add_observer_obj(subject_lean_x10, lean_screen_lv_arc_5_value_binding_cb, lv_arc_5, NULL);
        lv_obj_add_style(lv_arc_5, &style_lean_track, 0);
        lv_obj_add_style(lv_arc_5, &style_lean_overlay, 0);
        lv_obj_add_style(lv_arc_5, &style_lean_fill, LV_PART_INDICATOR);
        lv_obj_add_style(lv_arc_5, &style_no_knob, LV_PART_KNOB);

        lv_obj_t * lv_arc_6 = lv_arc_create(lv_obj_0);
        lv_obj_set_width(lv_arc_6, DISPLAY_SIZE - 2 * LEAN_ARC_INSET);
        lv_obj_set_height(lv_arc_6, DISPLAY_SIZE - 2 * LEAN_ARC_INSET);
        lv_obj_set_align(lv_arc_6, LV_ALIGN_CENTER);
        lv_obj_set_clickable(lv_arc_6, false);
        lv_arc_set_bg_start_angle(lv_arc_6, LEFT_ARC_END - DANGER_SPAN);
        lv_arc_set_bg_end_angle(lv_arc_6, LEFT_ARC_END);
        lv_obj_add_style(lv_arc_6, &style_danger_zone, 0);
        lv_obj_add_style(lv_arc_6, &style_arc_hidden_indicator, LV_PART_INDICATOR);
        lv_obj_add_style(lv_arc_6, &style_no_knob, LV_PART_KNOB);

        lv_obj_t * lv_arc_7 = lv_arc_create(lv_obj_0);
        lv_obj_set_width(lv_arc_7, DISPLAY_SIZE - 2 * LEAN_ARC_INSET);
        lv_obj_set_height(lv_arc_7, DISPLAY_SIZE - 2 * LEAN_ARC_INSET);
        lv_obj_set_align(lv_arc_7, LV_ALIGN_CENTER);
        lv_obj_set_clickable(lv_arc_7, false);
        lv_arc_set_bg_start_angle(lv_arc_7, RIGHT_ARC_START);
        lv_arc_set_bg_end_angle(lv_arc_7, RIGHT_ARC_START + DANGER_SPAN);
        lv_obj_add_style(lv_arc_7, &style_danger_zone, 0);
        lv_obj_add_style(lv_arc_7, &style_arc_hidden_indicator, LV_PART_INDICATOR);
        lv_obj_add_style(lv_arc_7, &style_no_knob, LV_PART_KNOB);

        lv_obj_t * lv_obj_1 = lv_obj_create(lv_obj_0);
        lv_obj_set_width(lv_obj_1, LV_SIZE_CONTENT);
        lv_obj_set_height(lv_obj_1, LV_SIZE_CONTENT);
        lv_obj_set_align(lv_obj_1, LV_ALIGN_LEFT_MID);
        lv_obj_set_x(lv_obj_1, 48);
        lv_obj_set_flex_flow(lv_obj_1, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_scrollable(lv_obj_1, false);
        lv_obj_set_clickable(lv_obj_1, false);
        lv_obj_set_style_flex_cross_place(lv_obj_1, LV_FLEX_ALIGN_CENTER, 0);
        lv_obj_add_style(lv_obj_1, &style_transparent, 0);
        lv_obj_t * lv_label_0 = lv_label_create(lv_obj_1);
        lv_label_set_text(lv_label_0, "MAX L");
        lv_obj_add_style(lv_label_0, &style_text_muted, 0);

        lv_obj_t * lv_label_1 = lv_label_create(lv_obj_1);
        lv_subject_add_observer_obj(subject_lean_max_left, lean_screen_lv_label_1_text_binding_cb, lv_label_1, NULL);
        lv_obj_add_style(lv_label_1, &style_value, 0);

        lv_obj_t * lv_obj_2 = lv_obj_create(lv_obj_0);
        lv_obj_set_width(lv_obj_2, LV_SIZE_CONTENT);
        lv_obj_set_height(lv_obj_2, LV_SIZE_CONTENT);
        lv_obj_set_align(lv_obj_2, LV_ALIGN_RIGHT_MID);
        lv_obj_set_x(lv_obj_2, -48);
        lv_obj_set_flex_flow(lv_obj_2, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_scrollable(lv_obj_2, false);
        lv_obj_set_clickable(lv_obj_2, false);
        lv_obj_set_style_flex_cross_place(lv_obj_2, LV_FLEX_ALIGN_CENTER, 0);
        lv_obj_add_style(lv_obj_2, &style_transparent, 0);
        lv_obj_t * lv_label_2 = lv_label_create(lv_obj_2);
        lv_label_set_text(lv_label_2, "MAX R");
        lv_obj_add_style(lv_label_2, &style_text_muted, 0);

        lv_obj_t * lv_label_3 = lv_label_create(lv_obj_2);
        lv_subject_add_observer_obj(subject_lean_max_right, lean_screen_lv_label_3_text_binding_cb, lv_label_3, NULL);
        lv_obj_add_style(lv_label_3, &style_value, 0);

        lv_obj_t * battery_indicator_0 = battery_indicator_create(lv_obj_0);
        lv_obj_set_align(battery_indicator_0, LV_ALIGN_TOP_MID);
        lv_obj_set_y(battery_indicator_0, 26);

        lv_obj_t * lv_label_4 = lv_label_create(lv_obj_0);
        lv_obj_set_align(lv_label_4, LV_ALIGN_TOP_MID);
        lv_obj_set_y(lv_label_4, 50);
        lv_subject_add_observer_obj(subject_clock_hour, lean_screen_lv_label_4_hidden_binding_cb, lv_label_4, NULL);
        lv_subject_add_observer_obj(subject_clock_pm, lean_screen_lv_label_4_text_binding_cb, lv_label_4, NULL);
        lv_subject_add_observer_obj(subject_clock_hour, lean_screen_lv_label_4_text_binding_cb, lv_label_4, NULL);
        lv_subject_add_observer_obj(subject_clock_min, lean_screen_lv_label_4_text_binding_cb, lv_label_4, NULL);
        lv_obj_add_style(lv_label_4, &style_text_muted, 0);

        lv_obj_t * lv_label_5 = lv_label_create(lv_obj_0);
        lv_obj_set_align(lv_label_5, LV_ALIGN_CENTER);
        lv_obj_set_y(lv_label_5, -52);
        lv_subject_add_observer_obj(subject_lean_abs_deg, lean_screen_lv_label_5_text_binding_cb, lv_label_5, NULL);
        lv_obj_add_style(lv_label_5, &style_lean_value, 0);
        lv_obj_add_style(lv_label_5, &style_lean_value_warn, 0);
        lean_screen_style_lean_value_warn_ref = &style_lean_value_warn;
        lv_subject_add_observer_obj(subject_lean_abs_deg, lean_screen_lv_label_5_style_enabled_cb, lv_label_5, NULL);
        lv_obj_add_style(lv_label_5, &style_lean_value_danger, 0);
        lean_screen_style_lean_value_danger_ref = &style_lean_value_danger;
        lv_subject_add_observer_obj(subject_lean_abs_deg, lean_screen_lv_label_5_style_enabled_cb_2, lv_label_5, NULL);

        lv_obj_t * lv_label_6 = lv_label_create(lv_obj_0);
        lv_obj_set_align(lv_label_6, LV_ALIGN_CENTER);
        lv_obj_set_y(lv_label_6, 14);
        lv_label_set_text(lv_label_6, "HOLD STILL");
        lv_subject_add_observer_obj(subject_imu_calibrating, lean_screen_lv_label_6_hidden_binding_cb, lv_label_6, NULL);
        lv_obj_set_style_text_color(lv_label_6, COLOR_WARN, 0);
        lv_obj_add_style(lv_label_6, &style_value, 0);

        lv_obj_t * lv_obj_3 = lv_obj_create(lv_obj_0);
        lv_obj_set_width(lv_obj_3, LV_SIZE_CONTENT);
        lv_obj_set_height(lv_obj_3, LV_SIZE_CONTENT);
        lv_obj_set_align(lv_obj_3, LV_ALIGN_CENTER);
        lv_obj_set_y(lv_obj_3, 56);
        lv_obj_set_flex_flow(lv_obj_3, LV_FLEX_FLOW_ROW);
        lv_obj_set_scrollable(lv_obj_3, false);
        lv_obj_set_clickable(lv_obj_3, false);
        lv_obj_set_style_flex_cross_place(lv_obj_3, LV_FLEX_ALIGN_END, 0);
        lv_obj_set_style_pad_column(lv_obj_3, SPACE_SM, 0);
        lv_obj_add_style(lv_obj_3, &style_transparent, 0);
        lv_obj_t * lv_label_7 = lv_label_create(lv_obj_3);
        lv_subject_add_observer_obj(subject_speed_kmh, lean_screen_lv_label_7_text_binding_cb, lv_label_7, NULL);
        lv_subject_add_observer_obj(subject_gps_fix, lean_screen_lv_label_7_hidden_binding_cb, lv_label_7, NULL);
        lv_obj_add_style(lv_label_7, &style_speed_value, 0);

        lv_obj_t * lv_label_8 = lv_label_create(lv_obj_3);
        lv_label_set_text(lv_label_8, "--");
        lv_subject_add_observer_obj(subject_gps_fix, lean_screen_lv_label_8_hidden_binding_cb, lv_label_8, NULL);
        lv_obj_add_style(lv_label_8, &style_speed_value, 0);

        lv_obj_t * lv_label_9 = lv_label_create(lv_obj_3);
        lv_label_set_text(lv_label_9, "km/h");
        lv_obj_set_style_pad_bottom(lv_label_9, 10, 0);
        lv_subject_add_observer_obj(subject_gps_fix, lean_screen_lv_label_9_hidden_binding_cb, lv_label_9, NULL);
        lv_obj_add_style(lv_label_9, &style_text_muted, 0);

        lv_obj_t * lv_label_10 = lv_label_create(lv_obj_3);
        lv_subject_add_observer_obj(subject_gps_sats, lean_screen_lv_label_10_text_binding_cb, lv_label_10, NULL);
        lv_obj_set_style_pad_bottom(lv_label_10, 10, 0);
        lv_subject_add_observer_obj(subject_gps_fix, lean_screen_lv_label_10_hidden_binding_cb, lv_label_10, NULL);
        lv_subject_add_observer_obj(subject_gps_sats, lean_screen_lv_label_10_hidden_binding_cb, lv_label_10, NULL);
        lv_obj_add_style(lv_label_10, &style_text_muted, 0);

        lv_obj_t * lv_label_11 = lv_label_create(lv_obj_3);
        lv_label_set_text(lv_label_11, "no GPS");
        lv_obj_set_style_pad_bottom(lv_label_11, 10, 0);
        lv_subject_add_observer_obj(subject_gps_fix, lean_screen_lv_label_11_hidden_binding_cb, lv_label_11, NULL);
        lv_subject_add_observer_obj(subject_gps_sats, lean_screen_lv_label_11_hidden_binding_cb, lv_label_11, NULL);
        lv_obj_add_style(lv_label_11, &style_text_muted, 0);

        lv_obj_t * lv_obj_4 = lv_obj_create(lv_obj_0);
        lv_obj_set_width(lv_obj_4, LV_SIZE_CONTENT);
        lv_obj_set_height(lv_obj_4, LV_SIZE_CONTENT);
        lv_obj_set_align(lv_obj_4, LV_ALIGN_CENTER);
        lv_obj_set_y(lv_obj_4, 122);
        lv_obj_set_flex_flow(lv_obj_4, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_scrollable(lv_obj_4, false);
        lv_obj_set_clickable(lv_obj_4, false);
        lv_obj_set_style_flex_cross_place(lv_obj_4, LV_FLEX_ALIGN_CENTER, 0);
        lv_obj_set_style_pad_row(lv_obj_4, SPACE_XS, 0);
        lv_subject_add_observer_obj(subject_show_g_bar, lean_screen_lv_obj_4_hidden_binding_cb, lv_obj_4, NULL);
        lv_obj_add_style(lv_obj_4, &style_transparent, 0);
        lv_obj_t * lv_label_12 = lv_label_create(lv_obj_4);
        lv_subject_add_observer_obj(subject_g_accel_cg, lean_screen_lv_label_12_text_binding_cb, lv_label_12, NULL);
        lv_subject_add_observer_obj(subject_g_decel_cg, lean_screen_lv_label_12_text_binding_cb, lv_label_12, NULL);
        lv_obj_add_style(lv_label_12, &style_value, 0);

        lv_obj_t * lv_obj_5 = lv_obj_create(lv_obj_4);
        lv_obj_set_width(lv_obj_5, LV_SIZE_CONTENT);
        lv_obj_set_height(lv_obj_5, LV_SIZE_CONTENT);
        lv_obj_set_flex_flow(lv_obj_5, LV_FLEX_FLOW_ROW);
        lv_obj_set_scrollable(lv_obj_5, false);
        lv_obj_set_clickable(lv_obj_5, false);
        lv_obj_set_style_flex_cross_place(lv_obj_5, LV_FLEX_ALIGN_CENTER, 0);
        lv_obj_set_style_pad_column(lv_obj_5, SPACE_SM, 0);
        lv_obj_add_style(lv_obj_5, &style_transparent, 0);
        lv_obj_t * lv_label_13 = lv_label_create(lv_obj_5);
        lv_label_set_text(lv_label_13, "BRK");
        lv_obj_add_style(lv_label_13, &style_text_muted, 0);

        lv_obj_t * lv_obj_6 = lv_obj_create(lv_obj_5);
        lv_obj_set_width(lv_obj_6, G_BAR_WIDTH);
        lv_obj_set_height(lv_obj_6, G_BAR_HEIGHT);
        lv_obj_set_scrollable(lv_obj_6, false);
        lv_obj_set_clickable(lv_obj_6, false);
        lv_obj_add_style(lv_obj_6, &style_transparent, 0);
        lv_obj_t * lv_bar_0 = lv_bar_create(lv_obj_6);
        lv_obj_set_width(lv_bar_0, lv_pct(100));
        lv_obj_set_height(lv_bar_0, lv_pct(100));
        lv_bar_set_orientation(lv_bar_0, LV_BAR_ORIENTATION_HORIZONTAL);
        lv_bar_set_mode(lv_bar_0, LV_BAR_MODE_SYMMETRICAL);
        lv_bar_set_min_value(lv_bar_0, -G_FULL_SCALE);
        lv_bar_set_max_value(lv_bar_0, G_FULL_SCALE);
        lv_bar_bind_value(lv_bar_0, subject_accel_cg);
        lv_obj_add_style(lv_bar_0, &style_g_track, 0);
        lv_obj_add_style(lv_bar_0, &style_g_fill, LV_PART_INDICATOR);
        lv_obj_add_style(lv_bar_0, &style_g_fill_brake, LV_PART_INDICATOR);
        lean_screen_style_g_fill_brake_ref = &style_g_fill_brake;
        lv_subject_add_observer_obj(subject_accel_cg, lean_screen_lv_bar_0_style_enabled_cb, lv_bar_0, NULL);

        lv_obj_t * lv_obj_7 = lv_obj_create(lv_obj_6);
        lv_obj_set_width(lv_obj_7, 2);
        lv_obj_set_height(lv_obj_7, lv_pct(100));
        lv_obj_set_align(lv_obj_7, LV_ALIGN_CENTER);
        lv_obj_set_clickable(lv_obj_7, false);
        lv_obj_add_style(lv_obj_7, &style_g_zero, 0);

        lv_obj_t * lv_label_14 = lv_label_create(lv_obj_5);
        lv_label_set_text(lv_label_14, "ACC");
        lv_obj_add_style(lv_label_14, &style_text_muted, 0);

        lv_obj_t * lv_button_0 = lv_button_create(lv_obj_0);
        lv_obj_set_width(lv_button_0, COG_BUTTON_SIZE);
        lv_obj_set_height(lv_button_0, COG_BUTTON_SIZE);
        lv_obj_set_align(lv_button_0, LV_ALIGN_BOTTOM_MID);
        lv_obj_set_x(lv_button_0, 0 - (COG_BUTTON_SIZE + SPACE_XS) / 2);
        lv_obj_set_y(lv_button_0, -18);
        lv_obj_add_style(lv_button_0, &style_cog, 0);
        lv_obj_add_style(lv_button_0, &style_cog_pressed, LV_STATE_PRESSED);
        lv_obj_t * lv_obj_8 = lv_obj_create(lv_button_0);
        lv_obj_set_width(lv_obj_8, REC_RING_SIZE);
        lv_obj_set_height(lv_obj_8, REC_RING_SIZE);
        lv_obj_set_align(lv_obj_8, LV_ALIGN_CENTER);
        lv_obj_set_clickable(lv_obj_8, false);
        lv_obj_set_scrollable(lv_obj_8, false);
        lv_obj_add_style(lv_obj_8, &style_rec_ring, 0);
        lv_obj_add_style(lv_obj_8, &style_rec_ring_on, 0);
        lean_screen_style_rec_ring_on_ref = &style_rec_ring_on;
        lv_subject_add_observer_obj(subject_log_state, lean_screen_lv_obj_8_style_enabled_cb, lv_obj_8, NULL);
        lv_obj_add_style(lv_obj_8, &style_rec_ring_error, 0);
        lean_screen_style_rec_ring_error_ref = &style_rec_ring_error;
        lv_subject_add_observer_obj(subject_log_state, lean_screen_lv_obj_8_style_enabled_cb_2, lv_obj_8, NULL);
        lv_obj_t * lv_obj_9 = lv_obj_create(lv_obj_8);
        lv_obj_set_width(lv_obj_9, REC_DOT_SIZE);
        lv_obj_set_height(lv_obj_9, REC_DOT_SIZE);
        lv_obj_set_align(lv_obj_9, LV_ALIGN_CENTER);
        lv_obj_set_clickable(lv_obj_9, false);
        lv_obj_set_scrollable(lv_obj_9, false);
        lv_subject_add_observer_obj(subject_log_state, lean_screen_lv_obj_9_hidden_binding_cb, lv_obj_9, NULL);
        lv_obj_add_style(lv_obj_9, &style_rec_dot, 0);
        lv_obj_add_style(lv_obj_9, &style_rec_dot_on, 0);
        lean_screen_style_rec_dot_on_ref = &style_rec_dot_on;
        lv_subject_add_observer_obj(subject_log_state, lean_screen_lv_obj_9_style_enabled_cb, lv_obj_9, NULL);

        lv_obj_t * lv_label_15 = lv_label_create(lv_obj_8);
        lv_label_set_text(lv_label_15, "SD");
        lv_obj_set_align(lv_label_15, LV_ALIGN_CENTER);
        lv_subject_add_observer_obj(subject_log_state, lean_screen_lv_label_15_hidden_binding_cb, lv_label_15, NULL);
        lv_obj_set_style_text_color(lv_label_15, COLOR_WARN, 0);
        lv_obj_add_style(lv_label_15, &style_text_muted, 0);

        lv_obj_add_event_cb(lv_button_0, lean_screen_set_subject_cb_3, LV_EVENT_CLICKED, NULL);

        lv_obj_t * lv_button_1 = lv_button_create(lv_obj_0);
        lv_obj_set_width(lv_button_1, COG_BUTTON_SIZE);
        lv_obj_set_height(lv_button_1, COG_BUTTON_SIZE);
        lv_obj_set_align(lv_button_1, LV_ALIGN_BOTTOM_MID);
        lv_obj_set_x(lv_button_1, (COG_BUTTON_SIZE + SPACE_XS) / 2);
        lv_obj_set_y(lv_button_1, -18);
        lv_obj_add_style(lv_button_1, &style_cog, 0);
        lv_obj_add_style(lv_button_1, &style_cog_pressed, LV_STATE_PRESSED);
        lv_obj_t * lv_image_0 = lv_image_create(lv_button_1);
        lv_image_set_src(lv_image_0, icon_cog);
        lv_obj_set_align(lv_image_0, LV_ALIGN_CENTER);

        lv_obj_add_screen_create_event(lv_button_1, LV_EVENT_CLICKED, settings_screen_create, LV_SCREEN_LOAD_ANIM_FADE_IN, 200, 0);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lean_screen_set_subject_cb(lv_event_t * e)
{
    LV_UNUSED(e);
    lv_subject_set_int(subject_lean_max_left, 0);
}

static void lean_screen_set_subject_cb_2(lv_event_t * e)
{
    LV_UNUSED(e);
    lv_subject_set_int(subject_lean_max_right, 0);
}

static void lean_screen_lv_arc_0_value_binding_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(subject);
    lv_obj_t * obj = lv_observer_get_target_obj(observer);
    lv_arc_set_value(obj, lv_subject_get_int(subject_lean_x10) < 0 ? 0 - lv_subject_get_int(subject_lean_x10) : 0);
}

static void lean_screen_lv_arc_1_value_binding_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(subject);
    lv_obj_t * obj = lv_observer_get_target_obj(observer);
    lv_arc_set_value(obj, lv_subject_get_int(subject_lean_x10) < 0 ? 0 - lv_subject_get_int(subject_lean_x10) : 0);
}

static void lean_screen_lv_arc_2_value_binding_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(subject);
    lv_obj_t * obj = lv_observer_get_target_obj(observer);
    lv_arc_set_value(obj, lv_subject_get_int(subject_lean_x10) < 0 ? 0 - lv_subject_get_int(subject_lean_x10) : 0);
}

static void lean_screen_lv_arc_3_value_binding_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(subject);
    lv_obj_t * obj = lv_observer_get_target_obj(observer);
    lv_arc_set_value(obj, lv_subject_get_int(subject_lean_x10) > 0 ? lv_subject_get_int(subject_lean_x10) : 0);
}

static void lean_screen_lv_arc_4_value_binding_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(subject);
    lv_obj_t * obj = lv_observer_get_target_obj(observer);
    lv_arc_set_value(obj, lv_subject_get_int(subject_lean_x10) > 0 ? lv_subject_get_int(subject_lean_x10) : 0);
}

static void lean_screen_lv_arc_5_value_binding_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(subject);
    lv_obj_t * obj = lv_observer_get_target_obj(observer);
    lv_arc_set_value(obj, lv_subject_get_int(subject_lean_x10) > 0 ? lv_subject_get_int(subject_lean_x10) : 0);
}

static void lean_screen_lv_label_1_text_binding_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(subject);
    lv_obj_t * obj = lv_observer_get_target_obj(observer);
    char eval_buf_1[LV_XML_EVAL_STRING_BUF_SIZE];
    lv_snprintf(eval_buf_1, sizeof(eval_buf_1), "%d%s", (int)(lv_subject_get_int(subject_lean_max_left)), "°");
    lv_label_set_text(obj, lv_subject_get_int(subject_lean_max_left) == 0 ? "--°" : eval_buf_1);
}

static void lean_screen_lv_label_3_text_binding_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(subject);
    lv_obj_t * obj = lv_observer_get_target_obj(observer);
    char eval_buf_1[LV_XML_EVAL_STRING_BUF_SIZE];
    lv_snprintf(eval_buf_1, sizeof(eval_buf_1), "%d%s", (int)(lv_subject_get_int(subject_lean_max_right)), "°");
    lv_label_set_text(obj, lv_subject_get_int(subject_lean_max_right) == 0 ? "--°" : eval_buf_1);
}

static void lean_screen_lv_label_4_hidden_binding_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(subject);
    lv_obj_t * obj = lv_observer_get_target_obj(observer);
    lv_obj_set_hidden(obj, lv_subject_get_int(subject_clock_hour) == 0);
}

static void lean_screen_lv_label_4_text_binding_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(subject);
    lv_obj_t * obj = lv_observer_get_target_obj(observer);
    char eval_buf_1[LV_XML_EVAL_STRING_BUF_SIZE];
    char eval_buf_2[LV_XML_EVAL_STRING_BUF_SIZE];
    lv_snprintf(eval_buf_1, sizeof(eval_buf_1), "%d%s%d%d%s", (int)(lv_subject_get_int(subject_clock_hour)), ":", (int)(lv_subject_get_int(subject_clock_min) / 10), (int)(lv_subject_get_int(subject_clock_min) % 10), " PM");
    lv_snprintf(eval_buf_2, sizeof(eval_buf_2), "%d%s%d%d%s", (int)(lv_subject_get_int(subject_clock_hour)), ":", (int)(lv_subject_get_int(subject_clock_min) / 10), (int)(lv_subject_get_int(subject_clock_min) % 10), " AM");
    lv_label_set_text(obj, lv_subject_get_int(subject_clock_pm) == 1 ? eval_buf_1 : eval_buf_2);
}

static void lean_screen_lv_label_5_text_binding_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(subject);
    lv_obj_t * obj = lv_observer_get_target_obj(observer);
    char eval_buf_1[LV_XML_EVAL_STRING_BUF_SIZE];
    lv_snprintf(eval_buf_1, sizeof(eval_buf_1), "%d%s", (int)(lv_subject_get_int(subject_lean_abs_deg)), "°");
    lv_label_set_text(obj, eval_buf_1);
}

static void lean_screen_lv_label_5_style_enabled_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(subject);
    lv_obj_t * obj = lv_observer_get_target_obj(observer);
    lv_obj_set_style_enabled(obj, lean_screen_style_lean_value_warn_ref, 0, lv_subject_get_int(subject_lean_abs_deg) >= LEAN_WARN);
}

static void lean_screen_lv_label_5_style_enabled_cb_2(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(subject);
    lv_obj_t * obj = lv_observer_get_target_obj(observer);
    lv_obj_set_style_enabled(obj, lean_screen_style_lean_value_danger_ref, 0, lv_subject_get_int(subject_lean_abs_deg) >= LEAN_DANGER);
}

static void lean_screen_lv_label_6_hidden_binding_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(subject);
    lv_obj_t * obj = lv_observer_get_target_obj(observer);
    lv_obj_set_hidden(obj, lv_subject_get_int(subject_imu_calibrating) == 0);
}

static void lean_screen_lv_label_7_text_binding_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(subject);
    lv_obj_t * obj = lv_observer_get_target_obj(observer);
    char eval_buf_1[LV_XML_EVAL_STRING_BUF_SIZE];
    lv_snprintf(eval_buf_1, sizeof(eval_buf_1), "%d", (int)(lv_subject_get_int(subject_speed_kmh)));
    lv_label_set_text(obj, eval_buf_1);
}

static void lean_screen_lv_label_7_hidden_binding_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(subject);
    lv_obj_t * obj = lv_observer_get_target_obj(observer);
    lv_obj_set_hidden(obj, lv_subject_get_int(subject_gps_fix) == 0);
}

static void lean_screen_lv_label_8_hidden_binding_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(subject);
    lv_obj_t * obj = lv_observer_get_target_obj(observer);
    lv_obj_set_hidden(obj, lv_subject_get_int(subject_gps_fix) == 1);
}

static void lean_screen_lv_label_9_hidden_binding_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(subject);
    lv_obj_t * obj = lv_observer_get_target_obj(observer);
    lv_obj_set_hidden(obj, lv_subject_get_int(subject_gps_fix) == 0);
}

static void lean_screen_lv_label_10_text_binding_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(subject);
    lv_obj_t * obj = lv_observer_get_target_obj(observer);
    char eval_buf_1[LV_XML_EVAL_STRING_BUF_SIZE];
    lv_snprintf(eval_buf_1, sizeof(eval_buf_1), "%s%d%s", "searching ", (int)(lv_subject_get_int(subject_gps_sats)), " sat");
    lv_label_set_text(obj, eval_buf_1);
}

static void lean_screen_lv_label_10_hidden_binding_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(subject);
    lv_obj_t * obj = lv_observer_get_target_obj(observer);
    lv_obj_set_hidden(obj, lv_subject_get_int(subject_gps_fix) == 1 || lv_subject_get_int(subject_gps_sats) < 0);
}

static void lean_screen_lv_label_11_hidden_binding_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(subject);
    lv_obj_t * obj = lv_observer_get_target_obj(observer);
    lv_obj_set_hidden(obj, lv_subject_get_int(subject_gps_fix) == 1 || lv_subject_get_int(subject_gps_sats) >= 0);
}

static void lean_screen_lv_obj_4_hidden_binding_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(subject);
    lv_obj_t * obj = lv_observer_get_target_obj(observer);
    lv_obj_set_hidden(obj, lv_subject_get_int(subject_show_g_bar) == 0);
}

static void lean_screen_lv_label_12_text_binding_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(subject);
    lv_obj_t * obj = lv_observer_get_target_obj(observer);
    char eval_buf_1[LV_XML_EVAL_STRING_BUF_SIZE];
    lv_snprintf(eval_buf_1, sizeof(eval_buf_1), "%d%s%d%s", (int)((lv_subject_get_int(subject_g_accel_cg) + lv_subject_get_int(subject_g_decel_cg)) / 100), ".", (int)((lv_subject_get_int(subject_g_accel_cg) + lv_subject_get_int(subject_g_decel_cg)) % 100 / 10), " G");
    lv_label_set_text(obj, eval_buf_1);
}

static void lean_screen_lv_bar_0_style_enabled_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(subject);
    lv_obj_t * obj = lv_observer_get_target_obj(observer);
    lv_obj_set_style_enabled(obj, lean_screen_style_g_fill_brake_ref, LV_PART_INDICATOR, lv_subject_get_int(subject_accel_cg) < 0);
}

static void lean_screen_lv_obj_8_style_enabled_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(subject);
    lv_obj_t * obj = lv_observer_get_target_obj(observer);
    lv_obj_set_style_enabled(obj, lean_screen_style_rec_ring_on_ref, 0, lv_subject_get_int(subject_log_state) == 1);
}

static void lean_screen_lv_obj_8_style_enabled_cb_2(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(subject);
    lv_obj_t * obj = lv_observer_get_target_obj(observer);
    lv_obj_set_style_enabled(obj, lean_screen_style_rec_ring_error_ref, 0, lv_subject_get_int(subject_log_state) == 2);
}

static void lean_screen_lv_obj_9_hidden_binding_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(subject);
    lv_obj_t * obj = lv_observer_get_target_obj(observer);
    lv_obj_set_hidden(obj, lv_subject_get_int(subject_log_state) == 2);
}

static void lean_screen_lv_obj_9_style_enabled_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(subject);
    lv_obj_t * obj = lv_observer_get_target_obj(observer);
    lv_obj_set_style_enabled(obj, lean_screen_style_rec_dot_on_ref, 0, lv_subject_get_int(subject_log_state) == 1);
}

static void lean_screen_lv_label_15_hidden_binding_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(subject);
    lv_obj_t * obj = lv_observer_get_target_obj(observer);
    lv_obj_set_hidden(obj, lv_subject_get_int(subject_log_state) != 2);
}

static void lean_screen_set_subject_cb_3(lv_event_t * e)
{
    LV_UNUSED(e);
    lv_subject_set_int(subject_log_request, 1);
}

