# Battery level indicator for ZMK.
if (CONFIG_ZMK_RGB_BATTERY_INDICATOR)
  zephyr_library()
  zephyr_library_sources(src/behavior_rgb_battery_indicator.c)
  zephyr_include_directories(${APPLICATION_SOURCE_DIR}/include)
endif()
