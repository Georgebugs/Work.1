#include <stdbool.h>
#include "esp_err.h"
#include "esp_system.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "gui.h"
#include "communication.h"
#include "driver/ledc.h"
#include "esp_rom_sys.h"
#include "lvgl.h"
#include "ui.h"
static const char* TAG = "MAIN";

#define PWM_PIN 6   // пин для подключения к Sensor AD (GPIO4 свободный ADC)
#define PERIOD_US 1000

/*void gui_task(void *pvParameter)
{
    // Создание экранов должно происходить в GUI-потоке
    create_screens();
    ESP_LOGI(TAG, "LVGL screens created successfully");

    while (1)
    {
        lv_timer_handler();                // обновление LVGL
        vTaskDelay(pdMS_TO_TICKS(10));     // каждые 10 мс
    }
}
*/
void app_main(void)
{
	
    gpio_reset_pin(PWM_PIN);
    gpio_set_direction(PWM_PIN, GPIO_MODE_OUTPUT);

    int dutyCycle = 30; // скважность (%) — можно менять

    ESP_ERROR_CHECK_WITHOUT_ABORT(eGuiInitDisplay(7));
    ESP_ERROR_CHECK_WITHOUT_ABORT(eCommunicationInit(3));

    //while (1) {
		
		
        // цикл ШИМ
        /*for (int i = 0; i < 1; i++) {
            gpio_set_level(PWM_PIN, 1);
            esp_rom_delay_us(dutyCycle * PERIOD_US / 100);

            gpio_set_level(PWM_PIN, 0);
            esp_rom_delay_us(PERIOD_US - (dutyCycle * PERIOD_US / 100));
        }
        
        */
    //}
    //lv_init();
    //xTaskCreate(gui_task, "gui_task", 8192, NULL, 2, NULL); // выдели побольше стека (8К)

    //ESP_LOGI(TAG, "Main finished init, running...");

// Настройка GPIO16 как выход
}