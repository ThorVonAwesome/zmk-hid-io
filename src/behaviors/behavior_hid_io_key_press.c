/*
 * Copyright (c) 2021 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#define DT_DRV_COMPAT zmk_behavior_hid_io_key_press

#include <zephyr/device.h>
#include <drivers/behavior.h>
#include <zephyr/logging/log.h>

#include <zmk/behavior.h>
#include <zmk/hid.h>
#include <zephyr/input/input.h>
#include <zephyr/dt-bindings/input/input-event-codes.h>
#include <zmk/hid-io/hid_joystick.h>
#include <zmk/hid-io/endpoints.h>
#include <zmk/keys.h>
#include <dt-bindings/zmk/keys.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#if DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)

static int behavior_hid_io_key_press_init(const struct device *dev) { return 0; };

#define DPAD_LEFT  6
#define DPAD_RIGHT 7
#define DPAD_UP    8
#define DPAD_DOWN  9
#define DPAD_DEFLECTION 127

static bool dpad_left = false;
static bool dpad_right = false;
static bool dpad_up = false;
static bool dpad_down = false;

static void update_dpad_axes(void) {
    int16_t x = 0;
    int16_t y = 0;
    if (dpad_left) { x -= DPAD_DEFLECTION; }
    if (dpad_right) { x += DPAD_DEFLECTION; }
    if (dpad_up) { y -= DPAD_DEFLECTION; }
    if (dpad_down) { y += DPAD_DEFLECTION; }
    zmk_hid_joy2_movement_set(x, y);
}

static int on_keymap_binding_pressed(struct zmk_behavior_binding *binding,
                                     struct zmk_behavior_binding_event event) {
    LOG_DBG("position %d HID IO BUTTON 0x%02X", event.position, binding->param1);
    switch (binding->param1) {
    case DPAD_LEFT:
        dpad_left = true; update_dpad_axes();
        zmk_hid_keyboard_press((zmk_key_t)LEFT);
        zmk_endpoints_send_report(HID_USAGE_KEY);
        break;
    case DPAD_RIGHT:
        dpad_right = true; update_dpad_axes();
        zmk_hid_keyboard_press((zmk_key_t)RIGHT);
        zmk_endpoints_send_report(HID_USAGE_KEY);
        break;
    case DPAD_UP:
        dpad_up = true; update_dpad_axes();
        zmk_hid_keyboard_press((zmk_key_t)UP);
        zmk_endpoints_send_report(HID_USAGE_KEY);
        break;
    case DPAD_DOWN:
        dpad_down = true; update_dpad_axes();
        zmk_hid_keyboard_press((zmk_key_t)DOWN);
        zmk_endpoints_send_report(HID_USAGE_KEY);
        break;
    default:
        zmk_hid_joy2_button_press((zmk_joystick_button_t)binding->param1);
        break;
    }
    return zmk_endpoints_send_joystick_report_alt();
}

static int on_keymap_binding_released(struct zmk_behavior_binding *binding,
                                      struct zmk_behavior_binding_event event) {
    LOG_DBG("position %d HID IO BUTTON 0x%02X", event.position, binding->param1);
    switch (binding->param1) {
    case DPAD_LEFT:
        dpad_left = false; update_dpad_axes();
        zmk_hid_keyboard_release((zmk_key_t)LEFT);
        zmk_endpoints_send_report(HID_USAGE_KEY);
        break;
    case DPAD_RIGHT:
        dpad_right = false; update_dpad_axes();
        zmk_hid_keyboard_release((zmk_key_t)RIGHT);
        zmk_endpoints_send_report(HID_USAGE_KEY);
        break;
    case DPAD_UP:
        dpad_up = false; update_dpad_axes();
        zmk_hid_keyboard_release((zmk_key_t)UP);
        zmk_endpoints_send_report(HID_USAGE_KEY);
        break;
    case DPAD_DOWN:
        dpad_down = false; update_dpad_axes();
        zmk_hid_keyboard_release((zmk_key_t)DOWN);
        zmk_endpoints_send_report(HID_USAGE_KEY);
        break;
    default:
        zmk_hid_joy2_button_release((zmk_joystick_button_t)binding->param1);
        break;
    }
    return zmk_endpoints_send_joystick_report_alt();
}

static const struct behavior_driver_api behavior_hid_io_key_press_driver_api = {
    .binding_pressed = on_keymap_binding_pressed,
    .binding_released = on_keymap_binding_released,
};

#define JSKP_INST(n)                                                       \
    BEHAVIOR_DT_INST_DEFINE(n, behavior_hid_io_key_press_init, NULL,       \
                            NULL, NULL, POST_KERNEL,                       \
                            CONFIG_KERNEL_INIT_PRIORITY_DEFAULT,           \
                            &behavior_hid_io_key_press_driver_api);

DT_INST_FOREACH_STATUS_OKAY(JSKP_INST)

#endif /* DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT) */
