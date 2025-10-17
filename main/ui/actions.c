#include <stdio.h>
#include <math.h>
#include "ui.h"
#include "screens.h"
#include "images.h"
#include "actions.h"
#include "esp_log.h"
#include "vars.h"
#include "../PressureUnits.h"
#include "../mainboardcommands.h"
#include "driver/gpio.h"
#include "esp_rom_sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#define PERIOD_US 1000
#define PWM_PIN   6

//#define UART_PORT_NUM      UART_NUM_1   // если у тебя RS485 на UART1
//#define UART_TX_PIN        25           // новый TX
//#define UART_RX_PIN        26           // новый RX (пример)
//#define UART_RTS_PIN       UART_PIN_NO_CHANGE
//#define UART_CTS_PIN       UART_PIN_NO_CHANGE

/*
void rs485_init(void)
{
    const uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };
    uart_param_config(UART_PORT_NUM, &uart_config);

    // Переназначаем пины UART
    uart_set_pin(UART_PORT_NUM, UART_TX_PIN, UART_RX_PIN, UART_RTS_PIN, UART_CTS_PIN);

    // Инициализируем драйвер UART
    uart_driver_install(UART_PORT_NUM, 256, 0, 0, NULL, 0);
}
*/

static const char *TAG = "ACTION";
char last_entered_pressure[32] = ""; // хранит последнее введённое значение



// === Глобальные переменные для управления PWM ===
static int dutyCycle = 0;
static bool pwm_running = false;
static TaskHandle_t pwm_task_handle = NULL;

// --- Простая функция формирования импульса ---
void pwm_write(int duty) {
    gpio_set_level(PWM_PIN, 1);
    esp_rom_delay_us(duty * PERIOD_US / 100);
    gpio_set_level(PWM_PIN, 0);
    esp_rom_delay_us(PERIOD_US - (duty * PERIOD_US / 100));
}

// --- Отдельная FreeRTOS-задача для PWM ---
void pwm_task(void *pvParameters) {
    ESP_LOGI(TAG, "PWM task started");
    gpio_reset_pin(PWM_PIN);
    gpio_set_direction(PWM_PIN, GPIO_MODE_OUTPUT);

    while (pwm_running) {
        pwm_write(dutyCycle);
        
    }

    ESP_LOGI(TAG, "PWM task stopped");
    vTaskDelete(NULL);
}

// === Обработчики событий ===

void action_splash_screen_loaded_cb(lv_event_t *e) {
    loadScreenWithFade(SCREEN_ID_MAIN_WORK);
}

void action_a_keyboard_clicked(lv_event_t *e) {
    uint32_t ulSwitchNumber = lv_keyboard_get_selected_btn(lv_event_get_target_obj(e));
    lv_obj_t * lvTextArea = lv_keyboard_get_textarea(lv_event_get_target_obj(e));
    
    const char * txt = lv_textarea_get_text(lvTextArea);

    if (ulSwitchNumber == 3) { // Clear
        set_var_cp_new_target_pressure("");
    }

    if (ulSwitchNumber == 7) { // Set
        set_var_cp_new_target_pressure("");
        set_var_cp_current_pressure(txt);
        printf("Number (Set): %s\n", txt);

        float fResult = fConvertFromString(txt, 10);
        
        strncpy(last_entered_pressure, txt, sizeof(last_entered_pressure));

        if (!isnan(fResult)) {
            if (fResult < 0) fResult = 0;
            if (fResult > 100) fResult = 100;
            dutyCycle = (int)fResult;

            // Если задача PWM не запущена — запускаем
            if (!pwm_running) {
                pwm_running = true;
                xTaskCreate(pwm_task, "pwm_task", 4096, NULL, 5, &pwm_task_handle);
            }
            
            lv_label_set_text(objects.label_current_pressure, last_entered_pressure);

            ESP_LOGI(TAG, "Duty cycle set to %d%%", dutyCycle);
        }
        //lv_label_set_text(objects.label_current_pressure, txt);
    }
    
    
}
void event_handler_cb_main_work_text_area_new_pressure(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);

    // Когда пользователь завершает ввод (нажал OK или Enter)
    if (code == LV_EVENT_READY || code == LV_EVENT_DEFOCUSED) {
        const char *txt = lv_textarea_get_text(obj);
        
        strncpy(last_entered_pressure, txt, sizeof(last_entered_pressure));

        // Показываем введённое давление в label
        lv_label_set_text(objects.label_current_pressure, txt);
        
    }
}





void action_b_pressure_start(lv_event_t *e) {
    vMainBoardCommandStartGoingToPressure();
}

void action_b_pressure_stop(lv_event_t *e) {
    vMainBoardCommandStopGoingToPressure();
}

void action_b_pressure_vent(lv_event_t *e) {
    vMainBoardCommandVentilation();
}