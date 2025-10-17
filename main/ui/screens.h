#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _objects_t {
	
    lv_obj_t *main_work;
    lv_obj_t *splash;
    /*
    lv_obj_t *switch_holder3;
    lv_obj_t *switch_holder2;
    lv_obj_t *switch_holder1;
    lv_obj_t *switch_holder4;
    lv_obj_t *panel_sensor3;
    lv_obj_t *label_state_holder3;
    lv_obj_t *label_holder_name3;
    lv_obj_t *panel_sensor2;
    lv_obj_t *label_state_holder2;
    lv_obj_t *label_holder_name2;
    lv_obj_t *panel_sensor1;
    lv_obj_t *label_state_holder1;
    lv_obj_t *label_holder_name1;
    */
    lv_obj_t *keyboard_new_pressure;
    lv_obj_t *text_area_new_pressure;
    lv_obj_t *panel_screen_name;
    lv_obj_t *panel_current_pressure;
    lv_obj_t *label_current_pressure;
    /*
    lv_obj_t *panel_sensor3_1;
    lv_obj_t *label_state_holder4;
    lv_obj_t *label_holder_name4;
    lv_obj_t *button_pressure_start;
    lv_obj_t *button_pressure_stop;
    lv_obj_t *button_pressure_vent;
    */
    lv_obj_t *panel_target_pressure;
    lv_obj_t *label_target_pressure;
    lv_obj_t *splash_text;
    lv_obj_t *imagesplash;
} objects_t;

extern objects_t objects;

enum ScreensEnum {
    SCREEN_ID_MAIN_WORK = 1,
    SCREEN_ID_SPLASH = 2,
};

void create_screen_main_work();
void tick_screen_main_work();

void create_screen_splash();
void tick_screen_splash();
void ui_set_pressure_value(const char *value);

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/