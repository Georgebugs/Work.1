#ifndef EEZ_LVGL_UI_EVENTS_H
#define EEZ_LVGL_UI_EVENTS_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

extern void action_splash_screen_loaded_cb(lv_event_t * e);
extern void action_a_keyboard_clicked(lv_event_t * e);
extern void action_b_pressure_start(lv_event_t * e);
extern void action_b_pressure_stop(lv_event_t * e);
extern void action_b_pressure_vent(lv_event_t * e);


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_EVENTS_H*/