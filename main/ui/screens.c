#include <string.h>

#include "screens.h"
#include "images.h"
#include "fonts.h"
#include "actions.h"
#include "vars.h"
#include "styles.h"
#include "ui.h"
#include "lvgl.h"
#include <string.h>

objects_t objects;
lv_obj_t *tick_value_change_obj;
uint32_t active_theme_index = 0;

/*
static void event_handler_cb_main_work_switch_holder3(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    if (event == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t *ta = lv_event_get_target(e);
        if (tick_value_change_obj != ta) {
            bool value = lv_obj_has_state(ta, LV_STATE_CHECKED);
            set_var_b_switch3_checked(value);
        }
    }
}

static void event_handler_cb_main_work_switch_holder2(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    if (event == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t *ta = lv_event_get_target(e);
        if (tick_value_change_obj != ta) {
            bool value = lv_obj_has_state(ta, LV_STATE_CHECKED);
            set_var_b_switch2_checked(value);
        }
    }
}

static void event_handler_cb_main_work_switch_holder1(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    if (event == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t *ta = lv_event_get_target(e);
        if (tick_value_change_obj != ta) {
            bool value = lv_obj_has_state(ta, LV_STATE_CHECKED);
            set_var_b_switch1_checked(value);
        }
    }
}
*/
static void event_handler_cb_main_work_text_area_new_pressure(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    if (event == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t *ta = lv_event_get_target(e);
        if (tick_value_change_obj != ta) {
            const char *value = lv_textarea_get_text(ta);
            set_var_cp_new_target_pressure(value);
        }
    }
}
/*
static void event_handler_cb_main_work_switch_holder4(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    if (event == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t *ta = lv_event_get_target(e);
        if (tick_value_change_obj != ta) {
            bool value = lv_obj_has_state(ta, LV_STATE_CHECKED);
            set_var_b_switch4_checked(value);
        }
    }
}
*/
// Переменная для хранения введённого значения
//static char new_pressure_str[16];   // Можно потом преобразовать в число
//static float new_pressure_value = 0;
extern char last_entered_pressure[32];

void create_screen_main_work() {
	lv_obj_t *layer = lv_obj_create(lv_scr_act()); // создаём "панель" или слой
		objects.main_work = layer;
		lv_obj_set_size(layer, 480, 320);
		lv_obj_clear_flag(layer, LV_OBJ_FLAG_SCROLLABLE); // панель статическая

		// Label заголовок
		lv_obj_t *label_title = lv_label_create(layer);
		lv_label_set_text(label_title, "Текущее давление");
		lv_obj_align(label_title, LV_ALIGN_TOP_MID, 0, 10);

		// Label текущее давление
		lv_obj_t *label_value = lv_label_create(layer);
		lv_label_set_text(label_value, last_entered_pressure);
		lv_obj_align(label_value, LV_ALIGN_CENTER, 0, 0);
		
    lv_obj_t *obj = lv_obj_create(0);
    objects.main_work = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 800, 480);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SNAPPABLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER);
    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    {
		
		
		
        lv_obj_t *parent_obj = obj;
       
        {
            // KeyboardNewPressure
            lv_obj_t *obj = lv_keyboard_create(parent_obj);
            objects.keyboard_new_pressure = obj;
            lv_obj_set_pos(obj, 0, 190);
            lv_obj_set_size(obj, 800, 290);
            lv_keyboard_set_mode(obj, LV_KEYBOARD_MODE_NUMBER);
            lv_obj_add_event_cb(obj, action_a_keyboard_clicked, LV_EVENT_CLICKED, (void *)0);
            lv_obj_set_style_align(obj, LV_ALIGN_DEFAULT, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // TextAreaNewPressure
            lv_obj_t *obj = lv_textarea_create(parent_obj);
            objects.text_area_new_pressure = obj;
            lv_obj_set_pos(obj, 10, 152);
            lv_obj_set_size(obj, 395, 42);
            lv_textarea_set_max_length(obj, 10);
            lv_textarea_set_placeholder_text(obj, "новое целевое давление");
            lv_textarea_set_one_line(obj, true);
            lv_textarea_set_password_mode(obj, false);
            lv_obj_add_event_cb(obj, event_handler_cb_main_work_text_area_new_pressure, LV_EVENT_ALL, 0);
            lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_TEXTAREA_PLACEHOLDER | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &ui_font_main_rus_14, LV_PART_TEXTAREA_PLACEHOLDER | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_keyboard_set_textarea(objects.keyboard_new_pressure, objects.text_area_new_pressure);
        }
        
       
        
        {
    // panelTargetPressure
    lv_obj_t *obj = lv_obj_create(parent_obj);
    objects.panel_target_pressure = obj;
    lv_obj_set_pos(obj, 10, 10);
    lv_obj_set_size(obj, 395, 140);
    lv_obj_set_style_text_font(obj, &ui_font_main_rus_14, LV_PART_TEXTAREA_PLACEHOLDER | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    // Отключаем все лишнее
    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
    //lv_obj_set_scroll_dir(obj, LV_DIR_NONE); // Запрещаем прокрутку
    //lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE); // Убираем возможность скролла
    //lv_obj_set_style_radius(obj, 0, 0); // Убираем скругления (опционально)
    //lv_obj_set_style_border_width(obj, 0, 0); // Убираем границу (опционально)
    //lv_obj_set_style_bg_opa(obj, LV_OPA_TRANSP, 0); // Прозрачный фон (опционально)
    lv_obj_set_style_bg_color(obj, lv_color_white(), 0); // Белый фон
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0); // Полностью непрозрачный фон
    
    
    
    lv_obj_t *label = lv_label_create(obj);
    //objects.text_area_new_pressure = label;
    lv_obj_set_pos(label, 10, 45);
	lv_obj_set_size(label, 395, 45);
    lv_obj_set_style_text_font(obj, &ui_font_main_rus_14, LV_PART_TEXTAREA_PLACEHOLDER | LV_STATE_DEFAULT);
	//lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    //lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_label_set_text(label, "Itog shim:");
	lv_obj_set_style_text_color(obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_center(label);
    {
        lv_obj_t *parent_obj = obj;
        
            // labelTargetPressure
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.label_target_pressure = obj;
            lv_obj_set_pos(obj, 74, 1);
            lv_obj_set_size(obj, 204, 45);
            //lv_obj_set_style_text_font(obj, &ui_font_main_rus_14, LV_PART_MAIN | LV_STATE_DEFAULT);
            //lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            //lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
            //lv_label_set_text(lv_label_create, "Текущий шим");
       
       
    }
}


{
            
            lv_obj_t *label_value = lv_label_create(parent_obj);
			objects.label_current_pressure = label_value;
			lv_obj_set_pos(label_value, -22, 45);
			lv_obj_set_size(label_value, 395, 45);
			lv_obj_set_style_text_font(label_value, &ui_font_main_rus_36, LV_PART_MAIN | LV_STATE_DEFAULT);
			lv_obj_set_style_text_align(label_value, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
			lv_obj_set_style_text_color(label_value, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
			lv_label_set_text(label_value, "10");
            
        }
        
        
    }
    /*
        {
    // PanelCurrentPressure
    lv_obj_t *obj = lv_obj_create(parent_obj);
    objects.panel_current_pressure = obj;
    lv_obj_set_pos(obj, 10, 30);
    lv_obj_set_size(obj, 500, 124);
    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_event_cb(obj, event_handler_cb_main_work_text_area_new_pressure, LV_EVENT_ALL, 0);

    {
        lv_obj_t *parent_obj = obj;

        // LabelCurrentPressure — крупное число (значение)
        lv_obj_t *label_value = lv_label_create(parent_obj);
        objects.label_current_pressure = label_value;
        lv_obj_set_pos(label_value, -22, -5);
        lv_obj_set_size(label_value, 395, LV_SIZE_CONTENT);
        lv_obj_set_style_text_font(label_value, &ui_font_main_rus_36, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_align(label_value, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
        //lv_label_set_text(label_value, ""); // изначально пусто
        extern char last_entered_pressure[32];  // чтобы видеть переменную из action.c
		lv_label_set_text(objects.label_current_pressure, last_entered_pressure);

        // LabelTitle — подпись "Текущее давление"
        lv_obj_t *label_title = lv_label_create(parent_obj);
        //objects.label_current_pressure = lv_label_create(parent_obj);
        lv_obj_set_pos(label_title, 68, -22);
        lv_obj_set_size(label_title, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_set_style_text_font(label_title, &ui_font_main_rus_14, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_label_set_text(label_title, "Текущее давление");
    }
}
      */  
    lv_keyboard_set_textarea(objects.keyboard_new_pressure, objects.text_area_new_pressure);
    
    tick_screen_main_work();
    lv_scr_load(objects.main_work);
}

void ui_set_pressure_value(const char *value)
{
    if (objects.label_current_pressure) {
        lv_label_set_text(objects.label_current_pressure, value);
    }
}

void tick_screen_main_work() {
	/*
    {
        bool new_val = get_var_b_switch3_checked();
        bool cur_val = lv_obj_has_state(objects.switch_holder3, LV_STATE_CHECKED);
        if (new_val != cur_val) {
            tick_value_change_obj = objects.switch_holder3;
            if (new_val) lv_obj_add_state(objects.switch_holder3, LV_STATE_CHECKED);
            else lv_obj_clear_state(objects.switch_holder3, LV_STATE_CHECKED);
            tick_value_change_obj = NULL;
        }
    }
    {
        bool new_val = get_var_b_switch2_checked();
        bool cur_val = lv_obj_has_state(objects.switch_holder2, LV_STATE_CHECKED);
        if (new_val != cur_val) {
            tick_value_change_obj = objects.switch_holder2;
            if (new_val) lv_obj_add_state(objects.switch_holder2, LV_STATE_CHECKED);
            else lv_obj_clear_state(objects.switch_holder2, LV_STATE_CHECKED);
            tick_value_change_obj = NULL;
        }
    }
    
    {
        bool new_val = get_var_b_switch1_checked();
        bool cur_val = lv_obj_has_state(objects.switch_holder1, LV_STATE_CHECKED);
        if (new_val != cur_val) {
            tick_value_change_obj = objects.switch_holder1;
            if (new_val) lv_obj_add_state(objects.switch_holder1, LV_STATE_CHECKED);
            else lv_obj_clear_state(objects.switch_holder1, LV_STATE_CHECKED);
            tick_value_change_obj = NULL;
        }
    }
    */
    {
        const char *new_val = get_var_cp_new_target_pressure();
        const char *cur_val = lv_textarea_get_text(objects.text_area_new_pressure);
        uint32_t max_length = lv_textarea_get_max_length(objects.text_area_new_pressure);
        if (strncmp(new_val, cur_val, max_length) != 0) {
            tick_value_change_obj = objects.text_area_new_pressure;
            lv_textarea_set_text(objects.text_area_new_pressure, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = get_var_cp_current_pressure();
        const char *cur_val = lv_label_get_text(objects.label_current_pressure);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.label_current_pressure;
            lv_label_set_text(objects.label_current_pressure, new_val);
            tick_value_change_obj = NULL;
        }
    }
    /*
    {
        bool new_val = get_var_b_switch4_checked();
        bool cur_val = lv_obj_has_state(objects.switch_holder4, LV_STATE_CHECKED);
        if (new_val != cur_val) {
            tick_value_change_obj = objects.switch_holder4;
            if (new_val) lv_obj_add_state(objects.switch_holder4, LV_STATE_CHECKED);
            else lv_obj_clear_state(objects.switch_holder4, LV_STATE_CHECKED);
            tick_value_change_obj = NULL;
        }
    }
    */
    {
        const char *new_val = get_var_cp_target_pressure();
        const char *cur_val = lv_label_get_text(objects.label_target_pressure);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.label_target_pressure;
            lv_label_set_text(objects.label_target_pressure, new_val);
            tick_value_change_obj = NULL;
        }
    }
}

void create_screen_splash() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.splash = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 800, 480);
    lv_obj_add_event_cb(obj, action_splash_screen_loaded_cb, LV_EVENT_SCREEN_LOADED, (void *)0);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            // splashText
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.splash_text = obj;
            lv_obj_set_pos(obj, 178, 315);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &ui_font_main_rus_36, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Контроллер тонометров\nv 0.3a");
        }
        {
            // Imagesplash
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.imagesplash = obj;
            lv_obj_set_pos(obj, 100, 120);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_image_set_src(obj, &img_splash_metran_logo);
        }
    }
    
    tick_screen_splash();
}

void tick_screen_splash() {
}



typedef void (*tick_screen_func_t)();
tick_screen_func_t tick_screen_funcs[] = {
    tick_screen_main_work,
    tick_screen_splash,
};
void tick_screen(int screen_index) {
    tick_screen_funcs[screen_index]();
}
void tick_screen_by_id(enum ScreensEnum screenId) {
    tick_screen_funcs[screenId - 1]();
}

void create_screens() {
    lv_disp_t *dispp = lv_disp_get_default();
    lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), true, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    
    create_screen_main_work();
    create_screen_splash();
}
