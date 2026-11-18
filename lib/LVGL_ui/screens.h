#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

// Screens

enum ScreensEnum {
    _SCREEN_ID_FIRST = 1,
    SCREEN_ID_MAIN = 1,
    SCREEN_ID_DEBUG_INFO = 2,
    SCREEN_ID_MODE_BLE_CONTROLLER = 3,
    SCREEN_ID_MODE_4_WD_MECANUM = 4,
    SCREEN_ID_DRONE_SELECT_MENU = 5,
    SCREEN_ID_DRONE_DASHBOARD = 6,
    SCREEN_ID_DRONE_RADIO_SETUP = 7,
    SCREEN_ID_DRONE_MONITOR = 8,
    _SCREEN_ID_LAST = 8
};

typedef struct _objects_t {
    lv_obj_t *main;
    lv_obj_t *debug_info;
    lv_obj_t *mode_ble_controller;
    lv_obj_t *mode_4_wd_mecanum;
    lv_obj_t *drone_select_menu;
    lv_obj_t *drone_dashboard;
    lv_obj_t *drone_radio_setup;
    lv_obj_t *drone_monitor;
    lv_obj_t *mode_select_debug;
    lv_obj_t *mode_select_debug__obj0;
    lv_obj_t *mode_select_debug__obj1;
    lv_obj_t *mode_select_ble_ctrl;
    lv_obj_t *mode_select_ble_ctrl__obj0;
    lv_obj_t *mode_select_ble_ctrl__obj1;
    lv_obj_t *mode_select_drone;
    lv_obj_t *mode_select_drone__obj0;
    lv_obj_t *mode_select_drone__obj1;
    lv_obj_t *mode_select_4_wd_meca;
    lv_obj_t *mode_select_4_wd_meca__obj0;
    lv_obj_t *mode_select_4_wd_meca__obj1;
    lv_obj_t *obj0;
    lv_obj_t *obj0__obj3;
    lv_obj_t *obj0__obj3__obj2;
    lv_obj_t *obj0__back_ground;
    lv_obj_t *obj1;
    lv_obj_t *obj1__obj3;
    lv_obj_t *obj1__obj3__obj2;
    lv_obj_t *obj1__back_ground;
    lv_obj_t *gamepad_left;
    lv_obj_t *gamepad_left__left;
    lv_obj_t *gamepad_left__left__obj0;
    lv_obj_t *gamepad_left__left__obj1;
    lv_obj_t *gamepad_left__up;
    lv_obj_t *gamepad_left__up__obj0;
    lv_obj_t *gamepad_left__up__obj1;
    lv_obj_t *gamepad_left__center;
    lv_obj_t *gamepad_left__center__obj0;
    lv_obj_t *gamepad_left__center__obj1;
    lv_obj_t *gamepad_left__right;
    lv_obj_t *gamepad_left__right__obj0;
    lv_obj_t *gamepad_left__right__obj1;
    lv_obj_t *gamepad_left__down;
    lv_obj_t *gamepad_left__down__obj0;
    lv_obj_t *gamepad_left__down__obj1;
    lv_obj_t *gamepad_right;
    lv_obj_t *gamepad_right__left;
    lv_obj_t *gamepad_right__left__obj0;
    lv_obj_t *gamepad_right__left__obj1;
    lv_obj_t *gamepad_right__up;
    lv_obj_t *gamepad_right__up__obj0;
    lv_obj_t *gamepad_right__up__obj1;
    lv_obj_t *gamepad_right__center;
    lv_obj_t *gamepad_right__center__obj0;
    lv_obj_t *gamepad_right__center__obj1;
    lv_obj_t *gamepad_right__right;
    lv_obj_t *gamepad_right__right__obj0;
    lv_obj_t *gamepad_right__right__obj1;
    lv_obj_t *gamepad_right__down;
    lv_obj_t *gamepad_right__down__obj0;
    lv_obj_t *gamepad_right__down__obj1;
    lv_obj_t *obj2;
    lv_obj_t *obj2__obj3;
    lv_obj_t *obj2__obj3__obj2;
    lv_obj_t *obj2__back_ground;
    lv_obj_t *obj3;
    lv_obj_t *obj3__obj3;
    lv_obj_t *obj3__obj3__obj2;
    lv_obj_t *obj3__back_ground;
    lv_obj_t *dashboard;
    lv_obj_t *dashboard__obj4;
    lv_obj_t *dashboard__obj5;
    lv_obj_t *monitor;
    lv_obj_t *monitor__obj4;
    lv_obj_t *monitor__obj5;
    lv_obj_t *mapping;
    lv_obj_t *mapping__obj4;
    lv_obj_t *mapping__obj5;
    lv_obj_t *radio_setup;
    lv_obj_t *radio_setup__obj4;
    lv_obj_t *radio_setup__obj5;
    lv_obj_t *obj4;
    lv_obj_t *obj4__obj6;
    lv_obj_t *obj4__obj6__obj2;
    lv_obj_t *obj4__back_ground_1;
    lv_obj_t *obj4__obj7;
    lv_obj_t *obj4__obj8;
    lv_obj_t *obj5;
    lv_obj_t *obj5__obj6;
    lv_obj_t *obj5__obj6__obj2;
    lv_obj_t *obj5__back_ground_1;
    lv_obj_t *obj5__obj7;
    lv_obj_t *obj5__obj8;
    lv_obj_t *obj6;
    lv_obj_t *obj6__obj3;
    lv_obj_t *obj6__obj3__obj2;
    lv_obj_t *obj6__back_ground;
    lv_obj_t *throttle;
    lv_obj_t *throttle__obj9;
    lv_obj_t *throttle__obj10;
    lv_obj_t *throttle__obj11;
    lv_obj_t *yaw;
    lv_obj_t *yaw__obj9;
    lv_obj_t *yaw__obj10;
    lv_obj_t *yaw__obj11;
    lv_obj_t *pitch;
    lv_obj_t *pitch__obj9;
    lv_obj_t *pitch__obj10;
    lv_obj_t *pitch__obj11;
    lv_obj_t *roll;
    lv_obj_t *roll__obj9;
    lv_obj_t *roll__obj10;
    lv_obj_t *roll__obj11;
    lv_obj_t *obj7;
    lv_obj_t *obj7__obj12;
    lv_obj_t *obj7__obj13;
    lv_obj_t *obj8;
    lv_obj_t *obj8__obj12;
    lv_obj_t *obj8__obj13;
    lv_obj_t *obj9;
    lv_obj_t *obj9__obj12;
    lv_obj_t *obj9__obj13;
    lv_obj_t *obj10;
    lv_obj_t *obj10__obj12;
    lv_obj_t *obj10__obj13;
    lv_obj_t *text_area;
    lv_obj_t *obj11;
    lv_obj_t *battery_volt_display;
    lv_obj_t *angle_display;
    lv_obj_t *pitch_val;
    lv_obj_t *roll_val;
    lv_obj_t *yaw_val;
    lv_obj_t *height_display;
    lv_obj_t *pitch_val_1;
    lv_obj_t *armed_disp;
    lv_obj_t *obj12;
    lv_obj_t *obj13;
    lv_obj_t *scan_toggle;
    lv_obj_t *ssid_list_container;
} objects_t;

extern objects_t objects;

void create_screen_main();
void tick_screen_main();

void create_screen_debug_info();
void tick_screen_debug_info();

void create_screen_mode_ble_controller();
void tick_screen_mode_ble_controller();

void create_screen_mode_4_wd_mecanum();
void tick_screen_mode_4_wd_mecanum();

void create_screen_drone_select_menu();
void tick_screen_drone_select_menu();

void create_screen_drone_dashboard();
void tick_screen_drone_dashboard();

void create_screen_drone_radio_setup();
void tick_screen_drone_radio_setup();

void create_screen_drone_monitor();
void tick_screen_drone_monitor();

void create_user_widget_mode_selection_panel(lv_obj_t *parent_obj, void *flowState, int startWidgetIndex);
void tick_user_widget_mode_selection_panel(void *flowState, int startWidgetIndex);

void create_user_widget_back_button(lv_obj_t *parent_obj, void *flowState, int startWidgetIndex);
void tick_user_widget_back_button(void *flowState, int startWidgetIndex);

void create_user_widget_default_mode_background(lv_obj_t *parent_obj, void *flowState, int startWidgetIndex);
void tick_user_widget_default_mode_background(void *flowState, int startWidgetIndex);

void create_user_widget_gamepad(lv_obj_t *parent_obj, void *flowState, int startWidgetIndex);
void tick_user_widget_gamepad(void *flowState, int startWidgetIndex);

void create_user_widget_gamepad_single(lv_obj_t *parent_obj, void *flowState, int startWidgetIndex);
void tick_user_widget_gamepad_single(void *flowState, int startWidgetIndex);

void create_user_widget_forward_button(lv_obj_t *parent_obj, void *flowState, int startWidgetIndex);
void tick_user_widget_forward_button(void *flowState, int startWidgetIndex);

void create_user_widget_drone_select_panel(lv_obj_t *parent_obj, void *flowState, int startWidgetIndex);
void tick_user_widget_drone_select_panel(void *flowState, int startWidgetIndex);

void create_user_widget_default_mode_drone_background(lv_obj_t *parent_obj, void *flowState, int startWidgetIndex);
void tick_user_widget_default_mode_drone_background(void *flowState, int startWidgetIndex);

void create_user_widget_drone_joystick_monitor(lv_obj_t *parent_obj, void *flowState, int startWidgetIndex);
void tick_user_widget_drone_joystick_monitor(void *flowState, int startWidgetIndex);

void create_user_widget_drone_button_monitor(lv_obj_t *parent_obj, void *flowState, int startWidgetIndex);
void tick_user_widget_drone_button_monitor(void *flowState, int startWidgetIndex);

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/