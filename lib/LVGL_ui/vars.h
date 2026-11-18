#ifndef EEZ_LVGL_UI_VARS_H
#define EEZ_LVGL_UI_VARS_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// enum declarations

// Flow global variables

enum FlowGlobalVariables {
    FLOW_GLOBAL_VARIABLE_BATTERY_VOLTAGE = 0,
    FLOW_GLOBAL_VARIABLE_PITCH_ANGLE = 1,
    FLOW_GLOBAL_VARIABLE_ROLL_ANGLE = 2,
    FLOW_GLOBAL_VARIABLE_YAW_ANGLE = 3,
    FLOW_GLOBAL_VARIABLE_HEIGHT_CM = 4
};

// Native global variables

extern int32_t get_var_monitor_throttle();
extern void set_var_monitor_throttle(int32_t value);
extern int32_t get_var_monitor_yaw();
extern void set_var_monitor_yaw(int32_t value);
extern int32_t get_var_monitor_pitch();
extern void set_var_monitor_pitch(int32_t value);
extern int32_t get_var_monitor_roll();
extern void set_var_monitor_roll(int32_t value);
extern int32_t get_var_monitor_aux1();
extern void set_var_monitor_aux1(int32_t value);
extern int32_t get_var_monitor_aux2();
extern void set_var_monitor_aux2(int32_t value);
extern int32_t get_var_monitor_aux3();
extern void set_var_monitor_aux3(int32_t value);
extern int32_t get_var_monitor_aux4();
extern void set_var_monitor_aux4(int32_t value);

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_VARS_H*/