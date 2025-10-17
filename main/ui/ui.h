#ifndef EEZ_LVGL_UI_GUI_H
#define EEZ_LVGL_UI_GUI_H

#include <string.h>
#include "lvgl.h"



#if defined(EEZ_FOR_LVGL)
#include <eez/flow/lvgl_api.h>
#endif

#if !defined(EEZ_FOR_LVGL)
#include "screens.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif



void ui_init();
void ui_tick();

#if !defined(EEZ_FOR_LVGL)
void loadScreen(enum ScreensEnum screenId);
void loadScreenWithFade(enum ScreensEnum screenId);
void ui_set_pressure_value(const char *value);
#endif

#ifdef __cplusplus
}
#endif

#endif // EEZ_LVGL_UI_GUI_H