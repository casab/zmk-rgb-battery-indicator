/*
 * Copyright (c) 2026 Engin Al
 *
 * SPDX-License-Identifier: MIT
 *
 * Battery indicator behavior: pressing the key shows the battery level on the LEDs for a moment,
 * filling `leds` in order like a gauge: all of them at 100%, half at 50%, at least one. The color
 * says how urgent it is: `high-hue` (green) from `high-level` up, `mid-hue` (orange) from
 * `low-level` up, `low-hue` (red) below. Drawn through zmk-rgb-led-overlay.
 */

#define DT_DRV_COMPAT zmk_behavior_rgb_battery_indicator

#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <drivers/behavior.h>

#include <zmk/battery.h>
#include <zmk/behavior.h>

#include <zmk_rgb_led_overlay/led_overlay.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

BUILD_ASSERT(DT_NUM_INST_STATUS_OKAY(DT_DRV_COMPAT) == 1,
             "Exactly one zmk,behavior-rgb-battery-indicator node is supported");

#define NODE DT_DRV_INST(0)

static const uint8_t leds[] = DT_PROP(NODE, leds);
#define SLOT DT_PROP(NODE, slot)
#define DURATION_MS DT_PROP(NODE, duration_ms)
#define BRIGHTNESS DT_PROP(NODE, brightness)
#define HIGH_LEVEL DT_PROP(NODE, high_level)
#define LOW_LEVEL DT_PROP(NODE, low_level)
#define HIGH_HUE DT_PROP(NODE, high_hue)
#define MID_HUE DT_PROP(NODE, mid_hue)
#define LOW_HUE DT_PROP(NODE, low_hue)
#define FILL_MS DT_PROP(NODE, fill_ms)

static void clear_work_handler(struct k_work *work) {
    zmk_led_overlay_clear(SLOT);
    zmk_led_overlay_commit();
}

static K_WORK_DELAYABLE_DEFINE(battery_indicator_clear_work, clear_work_handler);

static int battery_indicator_pressed(struct zmk_behavior_binding *binding,
                                     struct zmk_behavior_binding_event event) {
    uint8_t level = MIN(zmk_battery_state_of_charge(), 100);
    size_t count = DIV_ROUND_CLOSEST((size_t)level * ARRAY_SIZE(leds), 100);
    count = CLAMP(count, 1, ARRAY_SIZE(leds));

    uint16_t hue = level >= HIGH_LEVEL ? HIGH_HUE : level >= LOW_LEVEL ? MID_HUE : LOW_HUE;
    struct led_rgb color = zmk_led_overlay_hsb(hue, 100, BRIGHTNESS);

    LOG_DBG("rgb battery indicator: %d%%, %d LEDs", level, count);

    // Fill up like a gauge: each LED starts fading in a little after the previous one.
    zmk_led_overlay_clear(SLOT);
    for (size_t i = 0; i < count; i++) {
        zmk_led_overlay_set_delayed(SLOT, leds[i], color, i * FILL_MS / ARRAY_SIZE(leds));
    }
    zmk_led_overlay_commit();

    k_work_reschedule(&battery_indicator_clear_work, K_MSEC(DURATION_MS));

    return ZMK_BEHAVIOR_OPAQUE;
}

static int battery_indicator_released(struct zmk_behavior_binding *binding,
                                      struct zmk_behavior_binding_event event) {
    return ZMK_BEHAVIOR_OPAQUE;
}

static const struct behavior_driver_api battery_indicator_driver_api = {
    .binding_pressed = battery_indicator_pressed,
    .binding_released = battery_indicator_released,
#if IS_ENABLED(CONFIG_ZMK_BEHAVIOR_METADATA)
    .get_parameter_metadata = zmk_behavior_get_empty_param_metadata,
#endif
};

BEHAVIOR_DT_INST_DEFINE(0, NULL, NULL, NULL, NULL, POST_KERNEL,
                        CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, &battery_indicator_driver_api);
