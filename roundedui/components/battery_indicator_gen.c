/**
 * @file battery_indicator_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "battery_indicator_gen.h"
#include "../roundedui.h"

/*********************
 *      DEFINES
 *********************/

#define BATTERY_BODY_WIDTH (34)
#define BATTERY_BODY_HEIGHT (17)
#define BATTERY_TIP_WIDTH (3)
#define BATTERY_TIP_HEIGHT (8)
#define BATTERY_LOW (30)
#define BATTERY_CRITICAL (15)


/**********************
 *      TYPEDEFS
 **********************/

/***********************
 *  STATIC VARIABLES
 **********************/

/***********************
 *  STATIC PROTOTYPES
 **********************/

static void battery_indicator_lv_image_0_hidden_binding_cb(lv_observer_t * observer, lv_subject_t * subject);
static void battery_indicator_lv_label_0_text_binding_cb(lv_observer_t * observer, lv_subject_t * subject);
static void battery_indicator_lv_bar_0_style_bg_color_binding_cb(lv_observer_t * observer, lv_subject_t * subject);

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * battery_indicator_create(lv_obj_t * parent)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t style_battery_body;
    static lv_style_t style_battery_tip;
    static lv_style_t style_battery_track;
    static lv_style_t style_battery_fill;

    static bool style_inited = false;

    if (!style_inited) {
        /*Init all styles*/
        lv_style_init(&style_battery_body);
        lv_style_init(&style_battery_tip);
        lv_style_init(&style_battery_track);
        lv_style_init(&style_battery_fill);

        lv_style_set_bg_opa(&style_battery_body, 0);
        lv_style_set_border_color(&style_battery_body, COLOR_TEXT);
        lv_style_set_border_width(&style_battery_body, 2);
        lv_style_set_radius(&style_battery_body, 4);
        lv_style_set_pad_all(&style_battery_body, 2);
        lv_style_set_bg_color(&style_battery_tip, COLOR_TEXT);
        lv_style_set_bg_opa(&style_battery_tip, (255 * 100 / 100));
        lv_style_set_border_width(&style_battery_tip, 0);
        lv_style_set_radius(&style_battery_tip, 1);
        lv_style_set_pad_all(&style_battery_tip, 0);
        lv_style_set_bg_opa(&style_battery_track, 0);
        lv_style_set_radius(&style_battery_track, 1);
        lv_style_set_pad_all(&style_battery_track, 0);
        lv_style_set_bg_color(&style_battery_fill, COLOR_OK);
        lv_style_set_bg_opa(&style_battery_fill, (255 * 100 / 100));
        lv_style_set_radius(&style_battery_fill, 1);

        style_inited = true;
    }


    lv_obj_t * the_root = NULL;

    #if ROUNDEDUI_CHECK_COMPILE_TARGET(ROUNDEDUI_TARGET_ALL)
    if (roundedui_check_target(ROUNDEDUI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(parent);
        lv_obj_set_name_static(lv_obj_0, "battery_indicator_#");
        lv_obj_set_width(lv_obj_0, LV_SIZE_CONTENT);
        lv_obj_set_height(lv_obj_0, LV_SIZE_CONTENT);
        lv_obj_set_flex_flow(lv_obj_0, LV_FLEX_FLOW_ROW);
        lv_obj_set_scrollable(lv_obj_0, false);
        lv_obj_set_clickable(lv_obj_0, false);
        lv_obj_set_style_flex_cross_place(lv_obj_0, LV_FLEX_ALIGN_CENTER, 0);
        lv_obj_set_style_pad_column(lv_obj_0, SPACE_XS, 0);

        lv_obj_add_style(lv_obj_0, &style_transparent, 0);
        lv_obj_t * lv_image_0 = lv_image_create(lv_obj_0);
        lv_image_set_src(lv_image_0, icon_bolt_charging);
        lv_subject_add_observer_obj(subject_battery_charging, battery_indicator_lv_image_0_hidden_binding_cb, lv_image_0, NULL);

        lv_obj_t * lv_label_0 = lv_label_create(lv_obj_0);
        lv_subject_add_observer_obj(subject_battery_pct, battery_indicator_lv_label_0_text_binding_cb, lv_label_0, NULL);
        lv_obj_add_style(lv_label_0, &style_text_muted, 0);

        lv_obj_t * lv_obj_1 = lv_obj_create(lv_obj_0);
        lv_obj_set_width(lv_obj_1, LV_SIZE_CONTENT);
        lv_obj_set_height(lv_obj_1, LV_SIZE_CONTENT);
        lv_obj_set_flex_flow(lv_obj_1, LV_FLEX_FLOW_ROW);
        lv_obj_set_scrollable(lv_obj_1, false);
        lv_obj_set_clickable(lv_obj_1, false);
        lv_obj_set_style_flex_cross_place(lv_obj_1, LV_FLEX_ALIGN_CENTER, 0);
        lv_obj_add_style(lv_obj_1, &style_transparent, 0);
        lv_obj_t * lv_obj_2 = lv_obj_create(lv_obj_1);
        lv_obj_set_width(lv_obj_2, BATTERY_BODY_WIDTH);
        lv_obj_set_height(lv_obj_2, BATTERY_BODY_HEIGHT);
        lv_obj_set_scrollable(lv_obj_2, false);
        lv_obj_set_clickable(lv_obj_2, false);
        lv_obj_add_style(lv_obj_2, &style_battery_body, 0);
        lv_obj_t * lv_bar_0 = lv_bar_create(lv_obj_2);
        lv_obj_set_width(lv_bar_0, lv_pct(100));
        lv_obj_set_height(lv_bar_0, lv_pct(100));
        lv_bar_set_min_value(lv_bar_0, 0);
        lv_bar_set_max_value(lv_bar_0, 100);
        lv_bar_bind_value(lv_bar_0, subject_battery_pct);
        lv_subject_add_observer_obj(subject_battery_pct, battery_indicator_lv_bar_0_style_bg_color_binding_cb, lv_bar_0, NULL);
        lv_obj_add_style(lv_bar_0, &style_battery_track, 0);
        lv_obj_add_style(lv_bar_0, &style_battery_fill, LV_PART_INDICATOR);

        lv_obj_t * lv_obj_3 = lv_obj_create(lv_obj_1);
        lv_obj_set_width(lv_obj_3, BATTERY_TIP_WIDTH);
        lv_obj_set_height(lv_obj_3, BATTERY_TIP_HEIGHT);
        lv_obj_set_clickable(lv_obj_3, false);
        lv_obj_add_style(lv_obj_3, &style_battery_tip, 0);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void battery_indicator_lv_image_0_hidden_binding_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(subject);
    lv_obj_t * obj = lv_observer_get_target_obj(observer);
    lv_obj_set_hidden(obj, lv_subject_get_int(subject_battery_charging) == 0);
}

static void battery_indicator_lv_label_0_text_binding_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(subject);
    lv_obj_t * obj = lv_observer_get_target_obj(observer);
    char eval_buf_1[LV_XML_EVAL_STRING_BUF_SIZE];
    lv_snprintf(eval_buf_1, sizeof(eval_buf_1), "%d%s", (int)(lv_subject_get_int(subject_battery_pct)), "%");
    lv_label_set_text(obj, eval_buf_1);
}

static void battery_indicator_lv_bar_0_style_bg_color_binding_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(subject);
    lv_obj_t * obj = lv_observer_get_target_obj(observer);
    lv_obj_set_style_bg_color(obj, lv_subject_get_int(subject_battery_pct) <= BATTERY_CRITICAL ? COLOR_DANGER : lv_subject_get_int(subject_battery_pct) <= BATTERY_LOW ? COLOR_WARN : COLOR_OK, LV_PART_INDICATOR);
}

