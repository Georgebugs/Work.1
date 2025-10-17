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
    FLOW_GLOBAL_VARIABLE_B_SWITCH1_CHECKED = 0,
    FLOW_GLOBAL_VARIABLE_B_SWITCH2_CHECKED = 1,
    FLOW_GLOBAL_VARIABLE_B_SWITCH3_CHECKED = 2,
    FLOW_GLOBAL_VARIABLE_B_SWITCH4_CHECKED = 3,
    FLOW_GLOBAL_VARIABLE_CP_CURRENT_PRESSURE = 4,
    FLOW_GLOBAL_VARIABLE_CP_TARGET_PRESSURE = 5,
    FLOW_GLOBAL_VARIABLE_CP_NEW_TARGET_PRESSURE = 6
};

// Native global variables

extern bool get_var_b_switch1_checked();
extern void set_var_b_switch1_checked(bool value);
extern bool get_var_b_switch2_checked();
extern void set_var_b_switch2_checked(bool value);
extern bool get_var_b_switch3_checked();
extern void set_var_b_switch3_checked(bool value);
extern bool get_var_b_switch4_checked();
extern void set_var_b_switch4_checked(bool value);
extern const char *get_var_cp_current_pressure();
extern void set_var_cp_current_pressure(const char *value);
extern const char *get_var_cp_target_pressure();
extern void set_var_cp_target_pressure(const char *value);
extern const char *get_var_cp_new_target_pressure();
extern void set_var_cp_new_target_pressure(const char *value);


void set_var_b_switch1_checked_Directly(bool value);
void set_var_b_switch2_checked_Directly(bool value);
void set_var_b_switch3_checked_Directly(bool value);
void set_var_b_switch4_checked_Directly(bool value);

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_VARS_H*/