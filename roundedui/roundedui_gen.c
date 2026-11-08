/**
 * @file roundedui_gen.c
 */

/*********************
 *      INCLUDES
 *********************/

#include "roundedui.h"

#if defined(LV_USE_XML) && LV_USE_XML
#endif /* LV_USE_XML */

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void check_font(lv_font_t ** font, const char * name);

/**********************
 *  STATIC VARIABLES
 **********************/

static uint32_t roundedui_target = ROUNDEDUI_COMPILE_TARGET;

/*----------------
 * Translations
 *----------------*/

#ifndef LV_EDITOR_PREVIEW
    static const char * translation_languages[] = {"en", "de", NULL};
    static const char * translation_tags[] = {"dog", "cat", "house", NULL};
    static const char * translation_texts[] = {
        "This is a dog", "Das ist ein Hund", /* dog */
        "A curious little cat", "Eine neugierige kleine Katze", /* cat */
        "The house is cozy and warm", "Das Haus ist gemütlich und warm", /* house */
    };
#endif

/**********************
 *  GLOBAL VARIABLES
 **********************/

/*--------------------
 *  Permanent screens
 *-------------------*/

/*----------------
 * Fonts
 *----------------*/

lv_font_t * font_lean;
extern lv_font_t font_lean_data;
lv_font_t * font_speed;
extern lv_font_t font_speed_data;
lv_font_t * font_title;
extern lv_font_t font_title_data;
lv_font_t * font_small;
extern lv_font_t font_small_data;

/*----------------
 * Images
 *----------------*/

/* Targets: any */
const void * icon_bolt_charging = NULL;
extern const void * icon_bolt_charging_data;
const void * icon_cog = NULL;
extern const void * icon_cog_data;

/*----------------
 * Global styles
 *----------------*/

lv_style_t style_transparent;
lv_style_t style_text_muted;
lv_style_t style_value;
lv_style_t style_screen;

/*----------------
 * Subjects
 *----------------*/

lv_subject_t * subject_lean_deg;
lv_subject_t * subject_lean_abs_deg;
lv_subject_t * subject_lean_x10;
lv_subject_t * subject_lean_max_left;
lv_subject_t * subject_lean_max_right;
lv_subject_t * subject_accel_cg;
lv_subject_t * subject_g_accel_cg;
lv_subject_t * subject_g_decel_cg;
lv_subject_t * subject_speed_kmh;
lv_subject_t * subject_gps_fix;
lv_subject_t * subject_gps_sats;
lv_subject_t * subject_battery_pct;
lv_subject_t * subject_battery_charging;
lv_subject_t * subject_imu_cal_request;
lv_subject_t * subject_imu_calibrating;
lv_subject_t * subject_gps_cal_request;
lv_subject_t * subject_gps_calibrating;
lv_subject_t * subject_power_off_request;
lv_subject_t * subject_brightness_pct;
lv_subject_t * subject_show_g_bar;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void roundedui_init_gen(const char * asset_path)
{
    /* When running from the editor the theme set from the XML should overwrite this */
#if !defined(LV_EDITOR_PREVIEW)
#if LV_USE_THEME_SIMPLE
    lv_display_t * disp = lv_display_get_default();
    lv_theme_t * th = lv_theme_simple_init(disp);
    lv_display_set_theme(disp, th);
#else
    LV_LOG_WARN("Simple theme is selected in project.xml but LV_USE_THEME_SIMPLE is disabled");
#endif
#endif /*LV_EDITOR_PREVIEW*/


    /*----------------
     * Fonts
     *----------------*/

    /* Targets: any */

    #if ROUNDEDUI_CHECK_COMPILE_TARGET(ROUNDEDUI_TARGET_ALL)
    if (roundedui_check_target(ROUNDEDUI_TARGET_ALL)) {
        if (!font_lean) {
            /* font_lean */
            /* get font 'font_lean' from a C array */
            font_lean = &font_lean_data;

        }
        if (!font_speed) {
            /* font_speed */
            /* get font 'font_speed' from a C array */
            font_speed = &font_speed_data;

        }
        if (!font_title) {
            /* font_title */
            /* get font 'font_title' from a C array */
            font_title = &font_title_data;

        }
        if (!font_small) {
            /* font_small */
            /* get font 'font_small' from a C array */
            font_small = &font_small_data;

        }
    }
    #endif

    /*----------------
     * Images
     *----------------*/

    /* Targets: any */
    #if ROUNDEDUI_CHECK_COMPILE_TARGET(ROUNDEDUI_TARGET_ALL)
    if (roundedui_check_target(ROUNDEDUI_TARGET_ALL)) {
        /* icon_bolt_charging */
        if (!icon_bolt_charging) {
            icon_bolt_charging = &icon_bolt_charging_data;
        }
        /* icon_cog */
        if (!icon_cog) {
            icon_cog = &icon_cog_data;
        }
    }
    #endif

    /*----------------
     * Global styles
     *----------------*/

    static bool style_inited = false;

    if (!style_inited) {
        /*Init all styles*/
        lv_style_init(&style_transparent);
        lv_style_init(&style_text_muted);
        lv_style_init(&style_value);
        lv_style_init(&style_screen);

        lv_style_set_bg_opa(&style_transparent, 0);
        lv_style_set_border_width(&style_transparent, 0);
        lv_style_set_pad_all(&style_transparent, 0);
        lv_style_set_radius(&style_transparent, 0);
        lv_style_set_text_color(&style_text_muted, COLOR_TEXT_MUTED);
        lv_style_set_text_font(&style_text_muted, font_small);
        lv_style_set_text_color(&style_value, COLOR_TEXT);
        lv_style_set_text_font(&style_value, font_title);
        lv_style_set_bg_color(&style_screen, COLOR_BG);
        lv_style_set_bg_opa(&style_screen, (255 * 100 / 100));
        lv_style_set_text_color(&style_screen, COLOR_TEXT);
        lv_style_set_text_font(&style_screen, font_small);
        lv_style_set_pad_all(&style_screen, 0);

        style_inited = true;
    }

    /*----------------
     * Subjects
     *----------------*/
    subject_lean_deg = lv_subject_create(LV_SUBJECT_TYPE_INT);
    lv_subject_set_min_value_int(subject_lean_deg, -90);
    lv_subject_set_max_value_int(subject_lean_deg, 90);
    lv_subject_set_int(subject_lean_deg, 0);
    subject_lean_abs_deg = lv_subject_create(LV_SUBJECT_TYPE_INT);
    lv_subject_set_min_value_int(subject_lean_abs_deg, 0);
    lv_subject_set_max_value_int(subject_lean_abs_deg, 90);
    lv_subject_set_int(subject_lean_abs_deg, 0);
    subject_lean_x10 = lv_subject_create(LV_SUBJECT_TYPE_INT);
    lv_subject_set_min_value_int(subject_lean_x10, -900);
    lv_subject_set_max_value_int(subject_lean_x10, 900);
    lv_subject_set_int(subject_lean_x10, 0);
    subject_lean_max_left = lv_subject_create(LV_SUBJECT_TYPE_INT);
    lv_subject_set_min_value_int(subject_lean_max_left, 0);
    lv_subject_set_max_value_int(subject_lean_max_left, 90);
    lv_subject_set_int(subject_lean_max_left, 0);
    subject_lean_max_right = lv_subject_create(LV_SUBJECT_TYPE_INT);
    lv_subject_set_min_value_int(subject_lean_max_right, 0);
    lv_subject_set_max_value_int(subject_lean_max_right, 90);
    lv_subject_set_int(subject_lean_max_right, 0);
    subject_accel_cg = lv_subject_create(LV_SUBJECT_TYPE_INT);
    lv_subject_set_min_value_int(subject_accel_cg, -300);
    lv_subject_set_max_value_int(subject_accel_cg, 300);
    lv_subject_set_int(subject_accel_cg, 0);
    subject_g_accel_cg = lv_subject_create(LV_SUBJECT_TYPE_INT);
    lv_subject_set_min_value_int(subject_g_accel_cg, 0);
    lv_subject_set_max_value_int(subject_g_accel_cg, 300);
    lv_subject_set_int(subject_g_accel_cg, 0);
    subject_g_decel_cg = lv_subject_create(LV_SUBJECT_TYPE_INT);
    lv_subject_set_min_value_int(subject_g_decel_cg, 0);
    lv_subject_set_max_value_int(subject_g_decel_cg, 300);
    lv_subject_set_int(subject_g_decel_cg, 0);
    subject_speed_kmh = lv_subject_create(LV_SUBJECT_TYPE_INT);
    lv_subject_set_min_value_int(subject_speed_kmh, 0);
    lv_subject_set_max_value_int(subject_speed_kmh, 400);
    lv_subject_set_int(subject_speed_kmh, 0);
    subject_gps_fix = lv_subject_create(LV_SUBJECT_TYPE_INT);
    lv_subject_set_min_value_int(subject_gps_fix, 0);
    lv_subject_set_max_value_int(subject_gps_fix, 1);
    lv_subject_set_int(subject_gps_fix, 0);
    subject_gps_sats = lv_subject_create(LV_SUBJECT_TYPE_INT);
    lv_subject_set_min_value_int(subject_gps_sats, -1);
    lv_subject_set_max_value_int(subject_gps_sats, 99);
    lv_subject_set_int(subject_gps_sats, -1);
    subject_battery_pct = lv_subject_create(LV_SUBJECT_TYPE_INT);
    lv_subject_set_min_value_int(subject_battery_pct, 0);
    lv_subject_set_max_value_int(subject_battery_pct, 100);
    lv_subject_set_int(subject_battery_pct, 100);
    subject_battery_charging = lv_subject_create(LV_SUBJECT_TYPE_INT);
    lv_subject_set_min_value_int(subject_battery_charging, 0);
    lv_subject_set_max_value_int(subject_battery_charging, 1);
    lv_subject_set_int(subject_battery_charging, 0);
    subject_imu_cal_request = lv_subject_create(LV_SUBJECT_TYPE_INT);
    lv_subject_set_min_value_int(subject_imu_cal_request, 0);
    lv_subject_set_max_value_int(subject_imu_cal_request, 1);
    lv_subject_set_int(subject_imu_cal_request, 0);
    subject_imu_calibrating = lv_subject_create(LV_SUBJECT_TYPE_INT);
    lv_subject_set_min_value_int(subject_imu_calibrating, 0);
    lv_subject_set_max_value_int(subject_imu_calibrating, 1);
    lv_subject_set_int(subject_imu_calibrating, 0);
    subject_gps_cal_request = lv_subject_create(LV_SUBJECT_TYPE_INT);
    lv_subject_set_min_value_int(subject_gps_cal_request, 0);
    lv_subject_set_max_value_int(subject_gps_cal_request, 1);
    lv_subject_set_int(subject_gps_cal_request, 0);
    subject_gps_calibrating = lv_subject_create(LV_SUBJECT_TYPE_INT);
    lv_subject_set_min_value_int(subject_gps_calibrating, 0);
    lv_subject_set_max_value_int(subject_gps_calibrating, 1);
    lv_subject_set_int(subject_gps_calibrating, 0);
    subject_power_off_request = lv_subject_create(LV_SUBJECT_TYPE_INT);
    lv_subject_set_min_value_int(subject_power_off_request, 0);
    lv_subject_set_max_value_int(subject_power_off_request, 1);
    lv_subject_set_int(subject_power_off_request, 0);
    subject_brightness_pct = lv_subject_create(LV_SUBJECT_TYPE_INT);
    lv_subject_set_min_value_int(subject_brightness_pct, 25);
    lv_subject_set_max_value_int(subject_brightness_pct, 100);
    lv_subject_set_int(subject_brightness_pct, 75);
    subject_show_g_bar = lv_subject_create(LV_SUBJECT_TYPE_INT);
    lv_subject_set_min_value_int(subject_show_g_bar, 0);
    lv_subject_set_max_value_int(subject_show_g_bar, 1);
    lv_subject_set_int(subject_show_g_bar, 1);

    /*----------------
     * Translations
     *----------------*/

    #ifndef LV_EDITOR_PREVIEW
        lv_translation_add_static(translation_languages, translation_tags, translation_texts);
        lv_translation_set_language(translation_languages[0]);
    #endif

#if defined(LV_USE_XML) && LV_USE_XML
    /* Register widgets */

    /* Check all fonts / default if needed. This prevents fonts that are used in one target but
       defined in another from causing assertion failures during rendering of the Preview. */
    check_font(&font_lean, "font_lean");
    check_font(&font_speed, "font_speed");
    check_font(&font_title, "font_title");
    check_font(&font_small, "font_small");

    /* Register fonts */
    lv_xml_register_font(NULL, "font_lean", font_lean);
    lv_xml_register_font(NULL, "font_speed", font_speed);
    lv_xml_register_font(NULL, "font_title", font_title);
    lv_xml_register_font(NULL, "font_small", font_small);

    /* Register subjects */
    lv_xml_register_subject(NULL, "subject_lean_deg", subject_lean_deg);
    lv_xml_register_subject(NULL, "subject_lean_abs_deg", subject_lean_abs_deg);
    lv_xml_register_subject(NULL, "subject_lean_x10", subject_lean_x10);
    lv_xml_register_subject(NULL, "subject_lean_max_left", subject_lean_max_left);
    lv_xml_register_subject(NULL, "subject_lean_max_right", subject_lean_max_right);
    lv_xml_register_subject(NULL, "subject_accel_cg", subject_accel_cg);
    lv_xml_register_subject(NULL, "subject_g_accel_cg", subject_g_accel_cg);
    lv_xml_register_subject(NULL, "subject_g_decel_cg", subject_g_decel_cg);
    lv_xml_register_subject(NULL, "subject_speed_kmh", subject_speed_kmh);
    lv_xml_register_subject(NULL, "subject_gps_fix", subject_gps_fix);
    lv_xml_register_subject(NULL, "subject_gps_sats", subject_gps_sats);
    lv_xml_register_subject(NULL, "subject_battery_pct", subject_battery_pct);
    lv_xml_register_subject(NULL, "subject_battery_charging", subject_battery_charging);
    lv_xml_register_subject(NULL, "subject_imu_cal_request", subject_imu_cal_request);
    lv_xml_register_subject(NULL, "subject_imu_calibrating", subject_imu_calibrating);
    lv_xml_register_subject(NULL, "subject_gps_cal_request", subject_gps_cal_request);
    lv_xml_register_subject(NULL, "subject_gps_calibrating", subject_gps_calibrating);
    lv_xml_register_subject(NULL, "subject_power_off_request", subject_power_off_request);
    lv_xml_register_subject(NULL, "subject_brightness_pct", subject_brightness_pct);
    lv_xml_register_subject(NULL, "subject_show_g_bar", subject_show_g_bar);

    /* Register callbacks */

    /* Register easing callbacks */
#endif

    /* Register all the global assets so that they won't be created again when globals.xml is parsed.
     * While running in the editor skip this step to update the preview when the XML changes */
#if defined(LV_USE_XML) && LV_USE_XML && !defined(LV_EDITOR_PREVIEW)
    /* Register images */
    lv_xml_register_image(NULL, "icon_bolt_charging", icon_bolt_charging);
    lv_xml_register_image(NULL, "icon_cog", icon_cog);
#endif

#if !defined(LV_USE_XML) || LV_USE_XML == 0
    /*--------------------
     *  Permanent screens
     *-------------------*/
    /* If XML is enabled it's assumed that the permanent screens are created
     * manually from XML using lv_xml_create() */
#endif
}

void roundedui_set_target(uint32_t target)
{
    roundedui_target = target;
}

uint32_t roundedui_get_target(void)
{
    return roundedui_target;
}

bool roundedui_check_target(uint32_t target)
{
    return (roundedui_target & target) ? true : false;
}

/* Callbacks */

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void check_font(lv_font_t ** font, const char * name)
{
    if (!(*font)) {
        *font = (lv_font_t *)LV_FONT_DEFAULT;
        LV_LOG_WARN("font `%s` was not set. Using `LV_FONT_DEFAULT` instead", name);
    }
}