# ESP-IDF firmware for the lean angle UI on the Waveshare ESP32-S3-Touch-AMOLED-1.75
# (round 466x466 CO5300 AMOLED over QSPI, CST9217 touch, QMI8658 IMU, AXP2101 PMU,
# LC76G GPS on the shared I2C bus).
#
# The UI is exported by the LVGL Pro Editor (Ctrl+E) into ../roundedui;
# main/CMakeLists.txt compiles it in.
cmake_minimum_required(VERSION 3.22)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(rounded_fw)
