/**
 * @file roundedui_gen.h
 */

#ifndef LVGL_PRO_ROUNDEDUI_GEN_H
#define LVGL_PRO_ROUNDEDUI_GEN_H

#ifndef UI_SUBJECT_STRING_LENGTH
#define UI_SUBJECT_STRING_LENGTH 256
#endif

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



/* Prototypes for target functions, needed by responsive const definitions */

void roundedui_set_target(uint32_t target);
uint32_t roundedui_get_target(void);
bool roundedui_check_target(uint32_t target);

/*********************
 *      DEFINES
 *********************/

#define ROUNDEDUI_TARGET_UNDEFINED  (0 << 1)
#define ROUNDEDUI_TARGET_TARGET1    (1 << 1)
#define ROUNDEDUI_TARGET_ALL        0x0FFFFFFF

/* By default compile for all targets, allowing to switch to any targets at runtime */
#ifndef ROUNDEDUI_COMPILE_TARGET
#define ROUNDEDUI_COMPILE_TARGET ROUNDEDUI_TARGET_ALL
#endif

#define ROUNDEDUI_CHECK_COMPILE_TARGET(target) (ROUNDEDUI_COMPILE_TARGET & (target) ? 1 : 0)

#define COLOR_BG (lv_color_hex(0x000000))
#define COLOR_TRACK (lv_color_hex(0x1e2438))
#define COLOR_ACCENT (lv_color_hex(0x2f6bff))
#define COLOR_ACCENT_TEXT (lv_color_hex(0x5c8cff))
#define COLOR_TEXT (lv_color_hex(0xf2f4f8))
#define COLOR_TEXT_MUTED (lv_color_hex(0x9aa4c0))
#define COLOR_OK (lv_color_hex(0x3ad070))
#define COLOR_WARN (lv_color_hex(0xffb020))
#define COLOR_DANGER (lv_color_hex(0xff4a40))
#define SPACE_XS (4)
#define SPACE_SM (8)
#define SPACE_MD (12)
#define SPACE_LG (20)
#define DISPLAY_SIZE (466)
#define LEAN_LIMIT (60)
#define LEAN_WARN (35)
#define LEAN_DANGER (45)
#define LEAN_ARC_INSET (6)
#define LEAN_ARC_WIDTH (26)
#define LEAN_ARC_HALF_SPAN (58)
#define G_FULL_SCALE (150)
#define G_BAR_WIDTH (170)
#define G_BAR_HEIGHT (14)


#ifndef LV_XML_EVAL_STRING_BUF_SIZE
    #define LV_XML_EVAL_STRING_BUF_SIZE 256
#endif

/**
 * Animation path of a style transition with a `bezier(…)` easing. The control
 * points come from the transition descriptor's `user_data`, as a transition
 * descriptor cannot store animation parameters itself.
 */
static inline int32_t roundedui_style_transition_bezier_path_helper(const lv_anim_t * a)
{
    const lv_anim_bezier3_para_t * para = (const lv_anim_bezier3_para_t *)lv_anim_get_user_data(a);
    if(para == NULL) return lv_anim_path_linear(a);

    lv_anim_t a_tmp = *a;
    lv_anim_set_bezier3_param(&a_tmp, para->x1, para->y1, para->x2, para->y2);
    return lv_anim_path_custom_bezier3(&a_tmp);
}

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL VARIABLES
 **********************/

/*-------------------
 * Permanent screens
 *------------------*/

/*----------------
 * Global styles
 *----------------*/

extern lv_style_t style_transparent;
extern lv_style_t style_text_muted;
extern lv_style_t style_value;
extern lv_style_t style_screen;

/*----------------
 * Fonts
 *----------------*/

/* Targets: any */
extern lv_font_t * font_lean;
extern lv_font_t * font_speed;
extern lv_font_t * font_title;
extern lv_font_t * font_small;


/*----------------
 * Images
 *----------------*/

/* Targets: any */
extern const void * icon_bolt_charging;
extern const void * icon_cog;

/*----------------
 * Subjects
 *----------------*/

extern lv_subject_t * subject_lean_deg;
extern lv_subject_t * subject_lean_abs_deg;
extern lv_subject_t * subject_lean_x10;
extern lv_subject_t * subject_lean_max_left;
extern lv_subject_t * subject_lean_max_right;
extern lv_subject_t * subject_accel_cg;
extern lv_subject_t * subject_g_accel_cg;
extern lv_subject_t * subject_g_decel_cg;
extern lv_subject_t * subject_speed_kmh;
extern lv_subject_t * subject_gps_fix;
extern lv_subject_t * subject_gps_sats;
extern lv_subject_t * subject_battery_pct;
extern lv_subject_t * subject_battery_charging;
extern lv_subject_t * subject_imu_cal_request;
extern lv_subject_t * subject_imu_calibrating;
extern lv_subject_t * subject_gps_cal_request;
extern lv_subject_t * subject_gps_calibrating;
extern lv_subject_t * subject_power_off_request;
extern lv_subject_t * subject_brightness_pct;
extern lv_subject_t * subject_show_g_bar;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/*----------------
 * Event Callbacks
 *----------------*/

/*----------------
 * Easing Callbacks
 *----------------*/

/**
 * Initialize the component library
 */

void roundedui_init_gen(const char * asset_path);

/**********************
 *      MACROS
 **********************/

/**********************
 *   POST INCLUDES
 **********************/

/*Include all the widgets, components and screens of this library*/
#include "components/battery_indicator_gen.h"
#include "components/pill_button_gen.h"
#include "screens/lean_screen_gen.h"
#include "screens/settings_screen_gen.h"

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PRO_ROUNDEDUI_GEN_H*/