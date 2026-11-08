# The UI exported by the LVGL Pro Editor (Ctrl+E) from ../../roundedui.
# file_list_gen.cmake lists every generated .c file in LV_EDITOR_PROJECT_SOURCES.
set(UI_DIR "${CMAKE_CURRENT_LIST_DIR}/../../roundedui")
if(NOT EXISTS "${UI_DIR}/file_list_gen.cmake")
    message(FATAL_ERROR "No exported UI in ${UI_DIR}: open it in the LVGL Pro Editor and press Ctrl+E")
endif()
set(LV_EDITOR_PROJECT_SOURCES)
include("${UI_DIR}/file_list_gen.cmake")

idf_component_register(
    SRCS "main.c" "display.c" "imu_task.c" "pmu.c" "gps_task.c" "settings.c" "i2c_lock.c" ${LV_EDITOR_PROJECT_SOURCES}
    INCLUDE_DIRS "." "${UI_DIR}"
    REQUIRES esp_driver_i2c esp_driver_spi esp_lcd esp_timer nvs_flash)

# The exported code includes "lvgl.h" instead of "lvgl/lvgl.h" with this
target_compile_definitions(${COMPONENT_LIB} PRIVATE LV_LVGL_H_INCLUDE_SIMPLE)
