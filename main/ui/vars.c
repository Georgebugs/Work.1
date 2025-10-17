#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "../mainboardcommands.h"
#include "vars.h"

static bool b_switch1_checked = false;
static bool b_switch2_checked = false;
static bool b_switch3_checked = false;
static bool b_switch4_checked = false;
char cp_current_pressure[16] = { 0 };
char cp_target_pressure[16] = { 0 };
char cp_new_target_pressure[100] = { 0 };

const char *get_var_cp_new_target_pressure() {
    return cp_new_target_pressure;
}

void set_var_cp_new_target_pressure(const char *value) {
    strncpy(cp_new_target_pressure, value, sizeof(cp_new_target_pressure) / sizeof(char));
    cp_new_target_pressure[sizeof(cp_new_target_pressure) / sizeof(char) - 1] = 0;
}

const char *get_var_cp_target_pressure() {
    return cp_target_pressure;
}

void set_var_cp_target_pressure(const char *value) {
    strncpy(cp_target_pressure, value, sizeof(cp_target_pressure) / sizeof(char));
    cp_target_pressure[sizeof(cp_target_pressure) / sizeof(char) - 1] = 0;
}

bool get_var_b_switch1_checked() {
    return b_switch1_checked;
}
void set_var_b_switch1_checked(bool value) {
    vMainBoardCommandSetSendorClosedState(0, value);
}
void set_var_b_switch1_checked_Directly(bool value) {
    b_switch1_checked = value;
}

bool get_var_b_switch2_checked() {
    return b_switch2_checked;
}
void set_var_b_switch2_checked(bool value) {
    vMainBoardCommandSetSendorClosedState(1, value);
}
void set_var_b_switch2_checked_Directly(bool value) {
    b_switch2_checked = value;
}

bool get_var_b_switch3_checked() {
    return b_switch3_checked;
}
void set_var_b_switch3_checked(bool value) {
    vMainBoardCommandSetSendorClosedState(2, value);
}
void set_var_b_switch3_checked_Directly(bool value) {
    b_switch3_checked = value;
}

bool get_var_b_switch4_checked() {
    return b_switch4_checked;
}
void set_var_b_switch4_checked(bool value) {
    vMainBoardCommandSetSendorClosedState(3, value);
}
void set_var_b_switch4_checked_Directly(bool value) {
    b_switch4_checked = value;
}

const char *get_var_cp_current_pressure() {
    return cp_current_pressure;
}

void set_var_cp_current_pressure(const char *value) {
    strncpy(cp_current_pressure, value, sizeof(cp_current_pressure) / sizeof(char));
    cp_current_pressure[sizeof(cp_current_pressure) / sizeof(char) - 1] = 0;
}